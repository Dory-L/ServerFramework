#include "hook.h"
#include <dlfcn.h>

#include "fiber.h"
#include "iomanager.h"
#include "fd_manager.h"
#include "log.h"


static dory::Logger::ptr g_logger = DORY_LOG_NAME("system");
namespace dory {

static thread_local bool t_hook_enable = false;

#define HOOK_FUN(XX) \
    XX(sleep) \
    XX(usleep) \
    XX(nanosleep) \
    XX(socket) \
    XX(connect) \
    XX(accept) \
    XX(read) \
    XX(readv) \
    XX(recv) \
    XX(recvfrom) \
    XX(recvmsg) \
    XX(write) \
    XX(writev) \
    XX(send) \
    XX(sendto) \
    XX(sendmsg) \
    XX(close) \
    XX(fcntl) \
    XX(ioctl) \
    XX(getsockopt) \
    XX(setsockopt)


void hook_init() {
    static bool is_inited = false;
    if (is_inited) {
        return;
    }
#define XX(name) name ## _f = (name ## _fun)dlsym(RTLD_NEXT, #name); //返回第一个搜索到的函数名对应的符号地址，默认是系统库函数
    HOOK_FUN(XX);
#undef XX
}

struct _HookIniter {
    _HookIniter() {
        hook_init();
    }
};

static _HookIniter s_hook_initer;//在main函数之前初始化

bool is_hook_enable() {
    return t_hook_enable;
}

void set_hook_enable(bool flag) {
    t_hook_enable = flag;
}

}

struct timer_info {
    //是否取消
    int cancelled = 0;
};

template<typename OriginFun, typename... Args>
static ssize_t do_io(int fd, OriginFun fun, const char* hook_fun_name,
            uint32_t event, int timeout_so, Args&&... args) {
    if (!dory::t_hook_enable) {
        return fun(fd, std::forward<Args>(args)...);
    }

    dory::FdCtx::ptr ctx = dory::FdMgr::GetInstance()->get(fd);
    if (!ctx) { //不存在说明不是socket
        return fun(fd, std::forward<Args>(args)...);
    }

    if (ctx->isClosed()) { //句柄关闭了
        errno = EBADF;
        return -1;
    }

    //不是socket，或者用户设置了非阻塞，直接执行原函数
    if (!ctx->isSocket() || ctx->getUserNonblock()) {
        return fun(fd, std::forward<Args>(args)...);
    }

    uint64_t to = ctx->getTimeout(timeout_so); //取出超时时间
    std::shared_ptr<timer_info> tinfo(new timer_info); //设置超时的条件

retry: //失败了要重新读
    ssize_t n = fun(fd, std::forward<Args>(args)...);
    while (n == -1 && errno == EINTR) { //被中断，重试
        n = fun(fd, std::forward<Args>(args)...);
    }
    if (n == -1 && errno == EAGAIN) { //被阻塞
        dory::IOManager* iom = dory::IOManager::GetThis();
        dory::Timer::ptr timer;
        std::weak_ptr<timer_info> winfo(tinfo);

        if (to != (uint64_t)-1) {   //有超时参数，设置超时定时器
            timer = iom->addConditionTiemr(to, [winfo, fd, iom, event](){
                auto t = winfo.lock();
                if (!t || t->cancelled) { //条件不存在，或者设置错误直接返回
                    return;
                }
                t->cancelled = ETIMEDOUT;   //设置超时
                iom->cancelEvent(fd, (dory::IOManager::Event)(event)); //超时了就不需要了
            }, winfo);
        }

        int rt = iom->addEvent(fd, (dory::IOManager::Event)(event)); //把这个协程加入event事件
        if (rt) { //添加失败
            DORY_LOG_ERROR(g_logger) << hook_fun_name << " addEvent("
                                         << fd << ", " << event << ")";
            if (timer) {
                timer->cancel();
            }
            return -1;
        } else {
            dory::Fiber::YieldToHold(); //添加成功，切换到后台,两个返回点1、从上面的回调中返回，2、event触发有数据了
            if (timer) {    //唤醒回来，timer还存在，就取消掉
                timer->cancel();  //不用在调用上面的回调了
            }
            if (tinfo->cancelled) { //有值说明超时了，是从定时任务唤醒的
                errno = tinfo->cancelled;
                return -1;
            }

            goto retry;
        }
    }
    return n;
}

extern "C" {
#define XX(name) name ## _fun name ## _f = nullptr;
    HOOK_FUN(XX);
#undef XX

unsigned int sleep(unsigned int seconds) {
    if (!dory::t_hook_enable) { //没有hook住，就返回原函数的结果
        return sleep_f(seconds);
    }
    dory::Fiber::ptr fiber = dory::Fiber::GetThis();
    dory::IOManager* iom = dory::IOManager::GetThis();
    iom->addTimer(seconds * 1000, std::bind((void(dory::Scheduler::*)
            (dory::Fiber::ptr, int thread))&dory::IOManager::schedule, iom, fiber, -1));//在指定的时间之后schedule
    // iom->addTimer(seconds * 1000, [iom, fiber](){   //这个协程1s之后再从epoll唤醒
    //     iom->schedule(fiber);
    // });//在指定的时间之后schedule 
    dory::Fiber::YieldToHold();
    return 0;
}

int usleep(useconds_t usec) {
    if (!dory::t_hook_enable) { //没有hook住，就返回原函数的结果
        return usleep_f(usec);
    }
    dory::Fiber::ptr fiber = dory::Fiber::GetThis();
    dory::IOManager* iom = dory::IOManager::GetThis();
    // iom->addTimer(usec / 1000, std::bind(&dory::IOManager::schedule, iom, fiber));//在指定的时间之后schedule
    iom->addTimer(usec / 1000, [iom, fiber](){
        iom->schedule(fiber);
    });//在指定的时间之后schedule
    dory::Fiber::YieldToHold();
    return 0;
}

int nanosleep(const struct timespec *req, struct timespec *rem) {
    if (!dory::t_hook_enable) { //没有hook住，就返回原函数的结果
        return nanosleep_f(req, rem);
    }

    int timeout_ms = req->tv_sec * 1000 + req->tv_nsec / 1000 / 1000;
    dory::Fiber::ptr fiber = dory::Fiber::GetThis();
    dory::IOManager* iom = dory::IOManager::GetThis();
    iom->addTimer(timeout_ms, [iom, fiber](){
        iom->schedule(fiber);
    });//在指定的时间之后schedule
    dory::Fiber::YieldToHold();
    return 0;
}

int socket(int domain, int type, int protocol) {
    if (!dory::t_hook_enable) { //没有hook住，就返回原函数的结果
        return socket_f(domain, type, protocol);
    }
    int fd = socket_f(domain, type, domain);
    if (fd == -1) {
        return fd;
    }
    dory::FdMgr::GetInstance()->get(fd, true);
    return fd;
}

int connect_with_timeout(int fd, const struct sockaddr *addr, socklen_t addrlen, uint64_t timeout_ms) {
    if (!dory::t_hook_enable) { //没有hook住，就返回原函数的结果
        return connect_f(fd, addr, addrlen);
    }
    dory::FdCtx::ptr ctx = dory::FdMgr::GetInstance()->get(fd);
    if (!ctx || ctx->isClosed()) {
        errno = EBADF;
        return -1;
    }

    if (!ctx->isSocket()) {
        return connect_f(fd, addr, addrlen);
    }

    if (ctx->getUserNonblock()) {   //非阻塞调用原函数并返回
        return connect_f(fd, addr, addrlen);
    }

    int n = connect_f(fd, addr, addrlen);
    if (n == 0) {
        return 0;
    } else if (n != -1 || errno != EINPROGRESS) {
        return n;
    }

    //n == -1 && errno = EINPROGRESS  被阻塞了
    dory::IOManager* iom = dory::IOManager::GetThis();
    dory::Timer::ptr timer;
    std::shared_ptr<timer_info> tinfo(new timer_info);
    std::weak_ptr<timer_info> winfo(tinfo);

    if (timeout_ms != (uint64_t)-1) {
        timer = iom->addConditionTiemr(timeout_ms, [winfo, fd, iom](){
            auto t = winfo.lock();
            if (!t || t->cancelled) {
                return;
            }
            t->cancelled = ETIMEDOUT;
            iom->cancelEvent(fd, dory::IOManager::WRITE);
        }, winfo);
    }

    int rt = iom->addEvent(fd, dory::IOManager::WRITE);//connect成功之后，这个事件马上就被触发
    if (rt == 0) { //添加成功
        dory::Fiber::YieldToHold(); //此处唤醒有两种情况，1、超时；2、连接成功
        if (timer) {
            timer->cancel();
        }
        if (tinfo->cancelled) { //被超时事件唤醒的，返回错误
            errno = tinfo->cancelled;
            return -1;
        }
    } else {
        if (timer) {
            timer->cancel();
        }
        DORY_LOG_ERROR(g_logger) << "connect addEvent(" << fd << ", WRITE) error";
    }

    int error = 0;
    socklen_t len = sizeof(int);
    if (-1 == getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len)) {
        return -1;
    }
    if (!error) {
        return 0;
    } else {
        errno = error;
        return -1;
    }
}

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    return connect_f(sockfd, addr, addrlen);
}

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    int fd = do_io(sockfd, accept_f, "accept", dory::IOManager::READ, SO_RCVTIMEO, addr, addrlen);
    if (fd >= 0) {
        dory::FdMgr::GetInstance()->get(fd, true);
    }
    return fd;
}

ssize_t read(int fd, void *buf, size_t count) {
    return do_io(fd, read_f, "read", dory::IOManager::READ, SO_RCVTIMEO, buf, count);
}

ssize_t readv(int fd, const struct iovec *iov, int iovcnt) {
    return do_io(fd, readv_f, "read", dory::IOManager::READ, SO_RCVTIMEO, iov, iovcnt);
}

ssize_t recv(int sockfd, void *buf, size_t len, int flags) {
    return do_io(sockfd, recv_f, "recv", dory::IOManager::READ, SO_RCVTIMEO, buf, len, flags);
}

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen) {
    return do_io(sockfd, recvfrom_f, "recvfrom", dory::IOManager::READ, SO_RCVTIMEO, buf, len, flags, src_addr, addrlen);
}

ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags) {
    return do_io(sockfd, recvmsg_f, "recvmsg", dory::IOManager::READ, SO_RCVTIMEO, msg, flags);
}

ssize_t write(int fd, const void *buf, size_t count) {
    return do_io(fd, write_f, "write", dory::IOManager::WRITE, SO_SNDTIMEO, buf, count);
}

ssize_t writev(int fd, const struct iovec *iov, int iovcnt) {
    return do_io(fd, writev_f, "writev", dory::IOManager::WRITE, SO_SNDTIMEO, iov, iovcnt);
}

ssize_t send(int sockfd, const void *buf, size_t len, int flags) {
    return do_io(sockfd, send_f, "send", dory::IOManager::WRITE, SO_SNDTIMEO, buf, len, flags);
}

ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen) {
    return do_io(sockfd, sendto_f, "sendto", dory::IOManager::WRITE, SO_SNDTIMEO, buf, len, flags, dest_addr, addrlen);
}

ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags) {
    return do_io(sockfd, sendmsg_f, "sendmsg", dory::IOManager::WRITE, SO_SNDTIMEO, msg, flags);
}

//先取消fd的事件再关闭
int close(int fd) {
    if (!dory::t_hook_enable) {
        return close_f(fd);
    }
    
    dory::FdCtx::ptr ctx = dory::FdMgr::GetInstance()->get(fd);
    if (ctx) {
        auto iom = dory::IOManager::GetThis();
        if (iom) {
            iom->cancelAll(fd); //取消fd的所有事件在关闭
        }
        dory::FdMgr::GetInstance()->del(fd);
    }
    return close_f(fd);
}

//hook是为了拿到nonblock信息，是否用户设置的nonblock
int fcntl(int fd, int cmd, ... /* arg */ ) {
    va_list va;
    va_start(va, cmd);
    switch(cmd) {
        case F_SETFL:
            {
                int arg = va_arg(va, int);
                va_end(va);
                dory::FdCtx::ptr ctx = dory::FdMgr::GetInstance()->get(fd);
                if (!ctx || ctx->isClosed() || !ctx->isSocket()) {//不存在，关闭或不是socket
                    return fcntl_f(fd, cmd, arg);
                }
                ctx->setUserNonblock(arg & O_NONBLOCK);
                if (ctx->getSysNonblock()) {
                    arg |= O_NONBLOCK;
                } else {
                    arg &= ~O_NONBLOCK;
                }
                return fcntl_f(fd, cmd, arg);
            }
            break;
        case F_DUPFD:
        case F_DUPFD_CLOEXEC:
        case F_SETFD:
        case F_SETOWN:
        case F_SETSIG:
        case F_SETLEASE:
        case F_NOTIFY:
        case F_SETPIPE_SZ:
            {
                int arg = va_arg(va, int);
                va_end(va);
                return fcntl_f(fd, cmd, arg);
            }
            break;

        case F_GETFL:
            {
                va_end(va);
                int arg = fcntl(fd, cmd);
                dory::FdCtx::ptr ctx = dory::FdMgr::GetInstance()->get(fd);
                if (!ctx || ctx->isClosed() || !ctx->isSocket()) {
                    return arg;
                }
                if (ctx->getUserNonblock()) {
                    return arg | O_NONBLOCK;    //用户设置了nonblock
                } else {
                    return arg & ~O_NONBLOCK;   //用户没有设置nonblock
                }
            }
            break;
        case F_GETFD:
        case F_GETOWN:
        case F_GETSIG:
        case F_GETLEASE:
        case F_GETPIPE_SZ:
            {
                va_end(va);
                return fcntl(fd, cmd);
            }
            break;

        case F_SETLK:
        case F_SETLKW:
        case F_GETLK:
            {
                struct flock* arg = va_arg(va, struct flock*);
                va_end(va);
                return fcntl_f(fd, cmd, arg);
            }
            break;
        
        case F_GETOWN_EX:
        case F_SETOWN_EX:
            {
                struct f_owner_exlock* arg = va_arg(va, struct f_owner_exlock*);
                va_end(va);
                return fcntl(fd, cmd, arg);
            }
            break;
        default:
            va_end(va);
            return fcntl(fd, cmd);
    }
}

int ioctl(int fd, unsigned long request, ...) {
    va_list va;
    va_start(va, request);
    void* arg = va_arg(va, void*);
    va_end(va);

    if (FIONBIO == request) {
        bool user_nonblock = !!*(int*)arg;
        dory::FdCtx::ptr ctx = dory::FdMgr::GetInstance()->get(fd);
        if (!ctx ||ctx->isClosed() || !ctx->isSocket()) {
            return ioctl_f(fd, request, arg);
        }
        ctx->setUserNonblock(user_nonblock);
    }
    return ioctl_f(fd, request, arg);
}

int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen) {
    return getsockopt_f(sockfd, level, optname, optval, optlen);
}

int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen) {
    if (!dory::t_hook_enable) {
        return setsockopt_f(sockfd, level, optname, optval, optlen);
    }

    if (level == SOL_SOCKET) {
        if (optname == SO_RCVTIMEO || optname == SO_SNDTIMEO) {             //设置了套接字的超时时间
            dory::FdCtx::ptr ctx = dory::FdMgr::GetInstance()->get(sockfd);
            if (ctx) {
                const timeval* tv = (const timeval*)optval;
                ctx->setTimeout(optname, tv->tv_sec * 1000 + tv->tv_usec / 1000);
            }
        }
    }
    return setsockopt_f(sockfd, level, optname, optval, optlen);
}

}

