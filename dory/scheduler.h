#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <list>
#include <map>
#include "fiber.h"
#include "Thread.h"

namespace dory {

class Scheduler {
public:
    typedef std::shared_ptr<Scheduler> ptr;
    typedef Mutex MutexType;

    //threads-线程数，usecaller-是否加入线程调度器，name-名称
    Scheduler(size_t threads = 1, bool use_caller = true, const std::string& name = "");
    virtual ~Scheduler();

    const std::string& getName() const { return m_name; }

    static Scheduler* GetThis();
    //获取调度器的主协程
    static Fiber* GetMainFiber();

    void start();
    void stop();

    template<class FiberOrCb>
    void schedule(FiberOrCb fc, int thread = -1) {
        bool need_tickle = false;
        {
            MutexType::Lock lock(m_mutex);
            need_tickle = scheduleNoLock(fc, thread);
        }

        if (need_tickle) {
            tickle();
        }
    }

    template<class InputIterator>
    void schedule(InputIterator begin, InputIterator end) {
        bool need_tickle = false;
        {
            MutexType::Lock lock(m_mutex);
            while (begin != end) {
                need_tickle = scheduleNoLock(&*begin) || need_tickle; //用的指针，会被swap
                ++begin;
            }
        }
        if (need_tickle) {
            tickle();
        }
    }
protected:
    //唤醒线程
    virtual void tickle();
    void run();
    virtual bool stoppping();//子类需要清理任务的机会
    //什么都不做，站着任务或者sleep让出执行时间
    virtual void idle();

    void setThis();
private:
    template<class FiberOrCb>
    bool scheduleNoLock(FiberOrCb fc, int thread) {
        bool need_tickle = m_fibers.empty(); //若为true，表示目前没有任务，可能所有线程陷入到内核态，放进去了就唤醒
        FiberAndThread ft(fc, thread);
        if (ft.fiber || ft.cb) {
            m_fibers.push_back(ft);
        }
        return need_tickle;
    }
private:
    struct FiberAndThread {
        Fiber::ptr fiber;
        std::function<void()> cb;
        //线程id，协程所在的线程
        int thread;

        FiberAndThread(Fiber::ptr f, int thr)
            :fiber(f), thread(thr) {
        }

        FiberAndThread(Fiber::ptr* f, int thr)
            :thread(thr) {
            fiber.swap(*f); //将原来的f指向的智能指针置空，确保能引用释放
        }

        FiberAndThread(std::function<void()> f, int thr)
            :cb(f), thread(thr) {
        }

        FiberAndThread(std::function<void()>* f, int thr)
            :thread(thr) {
            cb.swap(*f);
        }
        
        FiberAndThread()  
            :thread(-1) {
        }

        void reset() {
            fiber = nullptr;
            cb = nullptr;
            thread = -1;
        }
    };
private:
    MutexType m_mutex;
    //线程池
    std::vector<Thread::ptr> m_threads;
    //即将要执行或计划要执行的协程
    std::list<FiberAndThread> m_fibers;
    //主协程
    Fiber::ptr m_rootFiber;
    //调度器名
    std::string m_name;
protected:
    //线程id
    std::vector<int> m_threadIds;
    //线程数量
    size_t m_threadCount = 0;
    //活跃线程数量
    std::atomic<size_t> m_activeThreadCount = {0};
    //空闲线程数量
    std::atomic<size_t> m_idleThreadCount = {0};
    //停止
    bool m_stopping = true;
    //自动停止
    bool m_autoStop = false;
    //主线程id
    int m_rootThread = 0;
};

} // namespace dory
