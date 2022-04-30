#pragma once
#include <memory>
#include <functional>
#include <ucontext.h>
#include "Thread.h"

namespace dory {
class Scheduler;
class Fiber : public std::enable_shared_from_this<Fiber> {
friend class Scheduler;
public:
    typedef std::shared_ptr<Fiber> ptr;

    enum State {
        INIT, //初始化
        HOLD, //暂停中
        EXEC, //执行中
        TERM, //终止
        READY, //可执行
        EXCEPT  //异常
    };
private:
    //每个线程第一个协程的构造
    Fiber();

public:
    Fiber(std::function<void()> cb, size_t stacksize = 0, bool use_caller = false);
    ~Fiber();

    //重置协程函数，并重置状态
    //能重置的有INIT、TERM
    void reset(std::function<void()> cb);   
    //切换到当前协程执行
    void swapIn();
    //切换到后台
    void swapOut();

    void call();

    void back();

    uint64_t getId() const { return m_id; }

    State getState() const { return m_state; }
public:
    //设置当前协程
    static void SetThis(Fiber* f);
    //返回当前协程
    static Fiber::ptr GetThis();
    //协程切换到后台，并设置为Ready状态
    static void YieldToReady();
    //协程切换到后台，并设置为Hold状态
    static void YieldToHold();
    //当前总协程数
    static uint64_t TotalFibers();

    static void MainFunc();
    static void CallerMainFunc();
    //返回协程号
    static uint64_t GetFiberId();
private:
    //协程id
    uint64_t m_id = 0;
    //栈大小
    uint32_t m_stacksize = 0;
    State m_state = INIT;

    //当前协程
    ucontext_t m_ctx;
    //栈内存空间
    void* m_stack = nullptr;

    std::function<void()> m_cb;
};


}