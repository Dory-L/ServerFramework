#pragma once

#include "scheduler.h"
#include "timer.h"

namespace dory {

class IOManager : public Scheduler, public TimerManager {
public:
    typedef std::shared_ptr<IOManager> ptr;
    typedef RWMutex RWMutexType;

    enum Event {
        //无事件
        NONE    = 0x0,
        //读事件
        READ    = 0x1, //等于EPOLLIN
        //写事件
        WRITE   = 0x4  //等于EPOLLOUT
    };
private:
    //Socket事件上下文类
    struct FdContext {
        typedef Mutex MutexType;
        //事件上下文类
        struct EventContext {
            //待执行的scheduler
            Scheduler* scheduler = nullptr;
            //事件协程
            Fiber::ptr fiber;
            //事件回调函数            
            std::function<void()> cb;         
        };

        //获取事件对应的上下文
        //event 事件类型
        //return 对应事件的上下文
        EventContext& getContext(Event event);
        //重置事件上下文
        //ctx待重置的上下文类
        void resetContext(EventContext& ctx);
        //触发事件
        //event 事件类型
        void triggerEvent(Event event);
        
        //读事件上下文
        EventContext read;      //读事件
        //写事件上下文
        EventContext write;     //写事件
        //事件相关的句柄
        int fd = 0;             //事件关联的句柄
        //当前的事件
        Event events = NONE;    //已注册的事件
        //事件的Mutex
        MutexType mutex;

    };
    
public:
    IOManager(size_t threads = 1, bool use_caller = true, const std::string& name = "");
    ~IOManager();

    //加入事件
    //return: 1 success， 0 retry，-1 error
    int addEvent(int fd, Event event, std::function<void()> cb = nullptr);
    //删除事件
    //不会触发事件
    bool delEvent(int fd, Event event);
    //取消事件
    //如过event事件存在则触发
    bool cancelEvent(int fd, Event event);
    //取消所有事件
    bool cancelAll(int fd);

    static IOManager* GetThis();

protected:
    void tickle() override;
    bool stopping() override;
    void idle() override;
    //有新的定时器被加到了前面要处理的事情
    void onTimerInsertedAtFront() override;

    //重置句柄上下文的容器大小
    //size 容器大小
    void contextResize(size_t size);
    bool stopping(uint64_t& timeout);
private:
    int m_epfd = 0;
    int m_tickleFds[2];

    //等待执行的事件数量
    std::atomic<size_t> m_pendingEventCount = {0};
    //IOManager的Mutex
    RWMutexType m_mutex;
    //socket事件上下文的容器
    std::vector<FdContext*> m_fdContexts;
};

}