#include "fiber.h"
#include "config.h"
#include "macro.h"
#include "scheduler.h"
#include <atomic>

namespace dory {

static Logger::ptr g_logger = DORY_LOG_NAME("system");

//协程id
static std::atomic<uint64_t> s_fiber_id {0};
//协程数量
static std::atomic<uint64_t> s_fiber_count {0};
//当前协程
static thread_local Fiber* t_fiber = nullptr;
//main协程
static thread_local Fiber::ptr t_threadFiber = nullptr;

static ConfigVar<uint32_t>::ptr g_fiber_stack_size =
    Config::Lookup<uint32_t>("fiber.stack_size", 128 * 1024, "fiber stack size");

class MallocStackAllocator {
public:
    static void* Alloc(size_t size) {
        return malloc(size);
    }

    static void Dealloc(void* vp, size_t size) {
        return free(vp);
    }
};

using StackAllocator = MallocStackAllocator;

uint64_t Fiber::GetFiberId() {
    if (t_fiber) {
        return t_fiber->getId();
    }
    return 0;
}

//当前线程的上下文赋给协程
Fiber::Fiber() {
    m_state = EXEC;
    SetThis(this);//设置自己为当前协程

    if (getcontext(&m_ctx)) { //失败返回非零
        DORY_ASSERT2(false, "getcontext");
    }

    ++s_fiber_count;

    DORY_LOG_DEBUG(g_logger) << "Fiber::Fiber main";
}

//正真的创建协程
Fiber::Fiber(std::function<void()> cb, size_t stacksize, bool use_caller) 
    :m_id(++s_fiber_id)
    ,m_cb(cb){
    ++s_fiber_count;
    m_stacksize = stacksize ? stacksize : g_fiber_stack_size->getValue();

    m_stack = StackAllocator::Alloc(m_stacksize);
    if (getcontext(&m_ctx)) {
        DORY_ASSERT2(false, "getcontext");
    }
    m_ctx.uc_link = nullptr;//关联的上下文，指定为nullptr，则协程执行第二个参数的函数之后退出线程
    m_ctx.uc_stack.ss_sp = m_stack;//stack指针
    m_ctx.uc_stack.ss_size = m_stacksize;//栈大小

    if (!use_caller) {
        makecontext(&m_ctx, &Fiber::MainFunc, 0);
    } else {
        makecontext(&m_ctx, &Fiber::CallerMainFunc, 0);
    }
    DORY_LOG_DEBUG(g_logger) << "Fiber::Fiber id=" << m_id;
}

Fiber::~Fiber() {
    --s_fiber_count;
    if (m_stack) {
        DORY_ASSERT(m_state == TERM
                || m_state == EXCEPT
                || m_state == INIT);
        StackAllocator::Dealloc(m_stack, m_stacksize);
    } else { //住协程没有栈
        DORY_ASSERT(!m_cb);
        DORY_ASSERT(m_state == EXEC);

        Fiber* cur = t_fiber;
        if (cur == this) {
            SetThis(nullptr);
        }
    }
    DORY_LOG_DEBUG(g_logger) << "Fiber::~Fiber id=" << m_id;
}

//重置协程函数，并重置状态
//能重置的有INIT、TERM、EXCEPT
void Fiber::reset(std::function<void()> cb) {
    DORY_ASSERT(m_stack);
    DORY_ASSERT(m_state == TERM
            || m_state == EXCEPT
            || m_state == INIT);
    m_cb = cb;
    if (getcontext(&m_ctx)) {
        DORY_ASSERT2(false, "getcontext");
    }

    m_ctx.uc_link = nullptr;//关联的上下文，指定为nullptr，则协程执行第二个参数的函数之后退出线程
    m_ctx.uc_stack.ss_sp = m_stack;//stack指针
    m_ctx.uc_stack.ss_size = m_stacksize;//栈大小

    makecontext(&m_ctx, &Fiber::MainFunc, 0);
    m_state = INIT;
}

//将当前协程强行切换到目标执行协程
void Fiber::call() {
    SetThis(this);
    m_state = EXEC;
    DORY_LOG_INFO(g_logger) << getId();
    if (swapcontext(&t_threadFiber->m_ctx, &m_ctx)) {
        DORY_ASSERT2(false, "swapcontext");
    }
}

void Fiber::back() {
    SetThis(t_threadFiber.get());
    if (swapcontext(&m_ctx, &t_threadFiber->m_ctx)) {
        DORY_ASSERT2(false, "swapcontext");
    }
}

//切换到当前
void Fiber::swapIn() {
    SetThis(this);
    DORY_ASSERT(m_state != EXEC);
    m_state = EXEC;
    if (swapcontext(&Scheduler::GetMainFiber()->m_ctx, &m_ctx)) {
        DORY_ASSERT2(false, "swapcontext");
    }
}

//切换到后台
void Fiber::swapOut() {
    SetThis(Scheduler::GetMainFiber());
    if (swapcontext(&m_ctx, &Scheduler::GetMainFiber()->m_ctx)) {
        DORY_ASSERT2(false, "swapcontext");
    }
}

//设置当前协程
void Fiber::SetThis(Fiber* f) {
    t_fiber = f;
}

//返回当前协程
Fiber::ptr Fiber::GetThis() {
    if (t_fiber) {
        return t_fiber->shared_from_this();
    }
    Fiber::ptr main_fiber(new Fiber);
    DORY_ASSERT(t_fiber == main_fiber.get());
    t_threadFiber = main_fiber;
    return t_fiber->shared_from_this();
}

//协程切换到后台，并设置为Ready状态
void Fiber::YieldToReady() {
    Fiber::ptr cur = GetThis();
    cur->m_state = READY;
    cur->swapOut();//当前协程切出去
}

//协程切换到后台，并设置为Hold状态
void Fiber::YieldToHold() {
    Fiber::ptr cur = GetThis();
    //cur->m_state = HOLD;
    cur->swapOut();
}
//当前总协程数
uint64_t Fiber::TotalFibers() {
    return s_fiber_count;
}

void Fiber::MainFunc() {
    Fiber::ptr cur = GetThis();
    DORY_ASSERT(cur);
    try {
        cur->m_cb();
        cur->m_cb = nullptr;//fctional中有可能bind了智能指针参数，导致引用计数加1
        cur->m_state = TERM;
    } catch (std::exception& ex) {
        cur->m_state = EXCEPT;
        DORY_LOG_ERROR(g_logger) << "Fiber Except:" << ex.what()
            << " fiber_id=" << cur->getId()
            << std::endl
            << dory::BacktraceToString();
    } catch (...) {
        cur->m_state = EXCEPT;
        DORY_LOG_ERROR(g_logger) << "Fiber Except"
            << " fiber_id=" << cur->getId()
            << std::endl
            << dory::BacktraceToString();
    }

    auto raw_ptr = cur.get();
    cur.reset();
    raw_ptr->swapOut();//子协程执行完退出之后回到主协程，不存在内存泄漏，内存会被智能指针销毁，只是存在野指针
    //cur->swapOut();//执行swapOut之后，cur不被销毁，当前子协程的引用计数永远不为零，不能执行析构函数

    DORY_ASSERT2(false, "never reach fiber_id=" + std::to_string(raw_ptr->getId()));
}

void Fiber::CallerMainFunc() {
    Fiber::ptr cur = GetThis();
    DORY_ASSERT(cur);
    try {
        cur->m_cb();
        cur->m_cb = nullptr;//fctional中有可能bind了智能指针参数，导致引用计数加1
        cur->m_state = TERM;
    } catch (std::exception& ex) {
        cur->m_state = EXCEPT;
        DORY_LOG_ERROR(g_logger) << "Fiber Except:" << ex.what()
            << " fiber_id=" << cur->getId()
            << std::endl
            << dory::BacktraceToString();
    } catch (...) {
        cur->m_state = EXCEPT;
        DORY_LOG_ERROR(g_logger) << "Fiber Except"
            << " fiber_id=" << cur->getId()
            << std::endl
            << dory::BacktraceToString();
    }

    auto raw_ptr = cur.get();
    cur.reset();
    raw_ptr->back();//子协程执行完退出之后回到主协程，不存在内存泄漏，内存会被智能指针销毁，只是存在野指针
    //cur->swapOut();//执行swapOut之后，cur不被销毁，当前子协程的引用计数永远不为零，不能执行析构函数

    DORY_ASSERT2(false, "never reach fiber_id=" + std::to_string(raw_ptr->getId()));
}

}