#include "scheduler.h"
#include "log.h"
#include "macro.h"

namespace dory {

static dory::Logger::ptr g_logger = DORY_LOG_NAME("system");

//主调度器
static thread_local Scheduler* t_scheduler = nullptr; 
//执行run方法的主协程
static thread_local Fiber* t_fiber = nullptr;

Scheduler::Scheduler(size_t threads, bool use_caller, const std::string& name) 
    :m_name(name) {
    DORY_ASSERT(threads > 0);

    if (use_caller) {
        dory::Fiber::GetThis();
        --threads; //当前线程被消耗作为主协程

        DORY_ASSERT(GetThis() == nullptr); //只能有一个调度器
        t_scheduler = this;

        m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, true));
        dory::Thread::SetName(m_name);

        //主协程不再是thread的主协程了，而是run方法的主协程
        t_fiber = m_rootFiber.get();
        m_rootThread = dory::GetThreadId();
        m_threadIds.push_back(m_rootThread);
    } else {
        m_rootThread = -1;  //这个线程不参与调度
    }
    m_threadCount = threads;
}

Scheduler::~Scheduler() {
    DORY_ASSERT(m_stopping);
    if (GetThis() == this) {
        t_scheduler = nullptr;
    }
}

Scheduler* Scheduler::GetThis() {
    return t_scheduler;
}

Fiber* Scheduler::GetMainFiber() {
    return t_fiber;
}

void Scheduler::start() {
    MutexType::Lock lock(m_mutex);
    if (!m_stopping) {
        return;
    }

    m_stopping = false;
    DORY_ASSERT(m_threads.empty());

    //启动线程池，都执行调度器的run方法
    m_threads.resize(m_threadCount);
    for (size_t i = 0; i < m_threadCount; ++i) {
        m_threads[i].reset(new Thread(std::bind(&Scheduler::run, this)
                            , m_name + "_" + std::to_string(i)));
        m_threadIds.push_back(m_threads[i]->getId());
    }
    // lock.unlock();
    //use_caller
    // if (m_rootFiber) {
    //     // m_rootFiber->swapIn();
    //     m_rootFiber->call();
    //     DORY_LOG_INFO(g_logger) << "call out" << m_rootFiber->getState();
    // }
}

void Scheduler::stop() {
    m_autoStop = true;
    if (m_rootFiber //调度协程
            && m_threadCount == 0
            && (m_rootFiber->getState() == Fiber::TERM
                || m_rootFiber->getState() == Fiber::INIT)) {
        DORY_LOG_INFO(g_logger) << this <<" stopped";
        m_stopping = true;

        //只有一个线程的情况
        if (stopping()) {
            return;
        }
    }

    // bool exit_on_this_fiber = false;
    if (m_rootThread != -1) {   //是use_caler的线程，一定要在他自己的线程去stop
        DORY_ASSERT(GetThis() == this);
    } else {        //其他的在任意线程都可以stop
        DORY_ASSERT(GetThis() != this);
    }

    m_stopping = true; //通知每一个线程结束
    for (size_t i = 0; i < m_threadCount; ++i) {
        tickle();
    }

    if (m_rootFiber) {
        tickle();
    }

    if (m_rootFiber) {
        // while (!stopping()) {
        //     if (m_rootFiber->getState() == Fiber::TERM
        //             || m_rootFiber->getState() == Fiber::EXCEPT) {
        //         m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, true));
        //         DORY_LOG_INFO(g_logger) << "root fiber is term, reset";
        //         t_fiber = m_rootFiber.get();
        //     }
        //     m_rootFiber->call();
        // }
        if (!stopping()) {
            m_rootFiber->call();
        }
    }

    std::vector<Thread::ptr> thrs;
    {
        MutexType::Lock lock(m_mutex);
        thrs.swap(m_threads);
    }

    for (auto& i : thrs) {
        i->join();
    }

    // if (exit_on_this_fiber) {
    // }
}

void Scheduler::setThis() {
    t_scheduler = this;
}

//执行协程调度的方法
void Scheduler::run() {
    DORY_LOG_INFO(g_logger) << "run";
    setThis();

    if (dory::GetThreadId() != m_rootThread) { //子线程也要创建自己的主协程
        t_fiber = Fiber::GetThis().get();
    }

    Fiber::ptr idle_fiber(new Fiber(std::bind(&Scheduler::idle, this))); //什么都不做的协程，衍生类绑定自己的idle
    Fiber::ptr cb_fiber;

    FiberAndThread ft;
    while (true) {
        ft.reset();
        bool tickle_me = false;
        bool is_active = false;
        {//从消息队列中取出要执行的消息
            MutexType::Lock lock(m_mutex);
            auto it = m_fibers.begin();
            while (it != m_fibers.end()) {
                //正在执行run或者不等于当前线程，跳过
                if (it->thread != -1 && it->thread != dory::GetThreadId()) {
                    ++it;
                    tickle_me = true;
                    continue;
                }

                //可以处理的情况
                DORY_ASSERT(it->fiber || it->cb);
                if (it->fiber && it->fiber->getState() == Fiber::EXEC) {
                    ++it;
                    continue;
                }

                ft = *it;
                m_fibers.erase(it); //取出需要执行的任务
                ++m_activeThreadCount;//任务取出来就加1，防止idle线程误判没有任务提前退出
                is_active = true;
                break;
            }
        }

        if (tickle_me) {
            tickle();
        }

        if (ft.fiber && (ft.fiber->getState() != Fiber::TERM
                            && ft.fiber->getState() != Fiber::EXCEPT)) {
            // ++m_activeThreadCount；
            ft.fiber->swapIn();
            --m_activeThreadCount;

            if (ft.fiber->getState() == Fiber::READY) { //YielToReady需要再次执行
                schedule(ft.fiber);
            } else if (ft.fiber->getState() != Fiber::TERM
                    && ft.fiber->getState() != Fiber::EXCEPT) {
                ft.fiber->m_state = Fiber::HOLD; //让出了执行时间状态变为HOLD
            }
            ft.reset();
        } else if (ft.cb) {
            if (cb_fiber) {
                cb_fiber->reset(ft.cb);
            } else {
                cb_fiber.reset(new Fiber(ft.cb));
            }
            ft.reset();
            // ++m_activeThreadCount；
            cb_fiber->swapIn();
            --m_activeThreadCount;
            if (cb_fiber->getState() == Fiber::READY) {
                schedule(cb_fiber);
                cb_fiber.reset();
            } else if (cb_fiber->getState() == Fiber::EXCEPT
                    || cb_fiber->getState() == Fiber::TERM) {
                cb_fiber->reset(nullptr);
            } else {
                cb_fiber->m_state = Fiber::HOLD;
                cb_fiber.reset();
            }
        } else { //没有任务，执行idle
            if (is_active) {
                --m_activeThreadCount;
                continue;
            }
            if (idle_fiber->getState() == Fiber::TERM) {
                DORY_LOG_INFO(g_logger) << "idle fiber term";
                break; //整个线程任务完成退出
            }

            ++m_idleThreadCount;
            idle_fiber->swapIn();
            --m_idleThreadCount;
            if (idle_fiber->getState() != Fiber::TERM
                    && idle_fiber->getState() != Fiber::EXCEPT) {
                idle_fiber->m_state = Fiber::HOLD;
            }
        }
    }
}

void Scheduler::tickle() {
    DORY_LOG_INFO(g_logger) << "tickle";
}

bool Scheduler::stopping() {
    MutexType::Lock lcok(m_mutex);//使用到m_fibers
    return m_autoStop && m_stopping 
            && m_fibers.empty() && m_activeThreadCount == 0;
}

void Scheduler::idle() {
    DORY_LOG_INFO(g_logger) << "idle";
    while (!stopping()) {
        dory::Fiber::YieldToHold();
    }
}

} // namespace dory
