#include "fiber.h"
#include "config.h"
#include "macro.h"
#include <atomic>

namespace dory {

//协程id
static std::atomic<uint64_t> s_fiber_id {0};
//协程数量
static std::atomic<uint64_t> s_fiber_count {0};
//当前协程
static thread_local Fiber* t_fiber = nullptr;
//main协程
static thread_local std::shared_ptr<Fiber::ptr> t_threadFiber = nullptr;

static ConfigVar<uint32_t>::ptr g_fiber_stack_size =
    Config::Lookup<uint32_t>("fiber.stack_size", 1024 * 1024, "fiber stack size");

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

//当前线程的上下文赋给协程
Fiber::Fiber() {
    m_state = EXEC;
    SetThis(this);//设置自己为当前协程

    if (getcontext(&m_ctx)) { //失败返回非零
        DORY_ASSERT2(false, "getcontext");
    }

    ++s_fiber_count;
}

//正真的创建协程
Fiber::Fiber(std::function<void()> cb, size_t stacksize = 0) 
    :m_id(++s_fiber_id)
    ,m_cb(cb){
    ++s_fiber_count;
    m_stacksize = stacksize ? stacksize : g_fiber_stack_size->getValue();

    m_stack = StackAllocator::Alloc(m_stacksize);
    if (getcontext(&m_ctx)) {
        DORY_ASSERT2(false, "getcontext");
    }
    m_ctx.uc_link = nullptr;//关联的上下文
    m_ctx.uc_stack.ss_sp = m_stack;//stack指针
    m_ctx.uc_stack.ss_size = m_stacksize;//栈大小

    makecontext(&m_ctx, &Fiber::MainFunc, 0);

}

Fiber::~Fiber() {
    --s_fiber_count;
    if (m_stack) {
        DORY_ASSERT(m_state == TERM
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
}

//重置协程函数，并重置状态
//能重置的有INIT、TERM
void Fiber::reset(std::function<void()> cb) {

}
//切换到当前协程执行
void Fiber::swapIn() {

}
//切换到后台
void Fiber::swapOut() {

}

//设置当前协程
void Fiber::SetThis(Fiber* f) {

}

//返回当前协程
Fiber::ptr Fiber::GetThis() {

}
//协程切换到后台，并设置为Ready状态
void Fiber::YieldToReady() {

}
//协程切换到后台，并设置为Hold状态
void Fiber::YieldToHold() {

}
//当前总协程数
uint64_t Fiber::TotalFibers() {

}

void Fiber::MainFunc() {

}
}