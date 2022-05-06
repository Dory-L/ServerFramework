#include "hook.h"
#include "fiber.h"
#include "iomanager.h"
#include "fd_manager.h"
#include <dlfcn.h>
#include <functional>

namespace dory {

static thread_local bool t_hook_enable = false;

#define HOOK_FUN(XX) \
    XX(sleep) \
    XX(usleep) \
    XX(nanosleep) \
    XX(socket) \
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


template<typename OriginFun, typename ... Args>
static ssize_t do_io(int fd, OriginFun fun, const char* hook_fun_name,
            uint32_t event, int timeout_so, ssize_t buflen, Args&&... args) {
    if (!dory::is_hook_enable) {
        return fun(fd, std::forward<Args>(args)...);
    }

    dory::FdCtx::ptr ctx = dory::FdMgr::GetInstance()->get(fd);
    if (!ctx) {
        return fun(fd, std::forward<Args>(args)...);
    }

    if (ctx->isClosed) {
        errno = EBADF;
        return -1;
    }

    //不是socket，或者已经设置非阻塞，直接执行原函数
    if (!ctx->isSocket() || ctx->getUserNonblock()) {
        return fun(fd, std::forward<Args>(args)...);
    }

    uint64_t to = ctx->getTimeout(timeout_so);
    std::shared_ptr<timer_info> tinfo(new timer_info);

    ssize_t n = fun(fd, std::forward(Args)(args)...);
    while (n == -1 && errno == EINTR) { //被中断，重试
        n = fun(fd, std::forward<Args>(args)...);
    }
    if (n == -1 && errno == EAGAIN) { //被阻塞
        dory::IOManager* iom = dory
    }

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
    // iom->addTimer(seconds * 1000, std::bind(&dory::IOManager::schedule iom, fiber));//在指定的时间之后schedule
    iom->addTimer(seconds * 1000, [iom, fiber](){   //这个协程1s之后再从epoll唤醒
        iom->schedule(fiber);
    });//在指定的时间之后schedule
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

    }
}

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {

}

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {

}

ssize_t read(int fd, void *buf, size_t count) {

}

ssize_t readv(int fd, const struct iovec *iov, int iovcnt) {

}

ssize_t recv(int sockfd, void *buf, size_t len, int flags) {

}

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen) {

}

ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags) {

}

ssize_t write(int fd, const void *buf, size_t count) {

}

ssize_t writev(int fd, const struct iovec *iov, int iovcnt) {

}

ssize_t send(int sockfd, const void *buf, size_t len, int flags);

ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);

ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags);

int close(int fd) {

}

int fcntl(int fd, int cmd, ... /* arg */ ) {

}

int ioctl(int fd, unsigned long request, ...) {

}

int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen) {
    
}

int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen) {

}

}

