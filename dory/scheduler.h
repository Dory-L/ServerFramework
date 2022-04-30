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

    //threads 线程数量，use_caller 是否使用当前调用线程，name 协程调度器名称
    Scheduler(size_t threads = 1, bool use_caller = true, const std::string& name = "");
    virtual ~Scheduler();

    //返回协程调度器名称
    const std::string& getName() const { return m_name; }
    //返回当前协程调度器
    static Scheduler* GetThis();
    //获取当前协程调度器的调度协程
    static Fiber* GetMainFiber();
    //启动协程调度器
    void start();
    //停止协程调度器
    void stop();

    //调度协程
    //fc 协程或函数
    //thread 协程执行的线程id，-1标识任意线程
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

    //批量调度协程
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
    //通知协程调度器有任务了
    virtual void tickle();
    //协程调度函数
    void run();
    //返回是否可以停止
    virtual bool stopping();//子类需要清理任务的机会
    //协程无任务可调度时执行idle协程
    virtual void idle();
    //设置当前协程的调度器
    void setThis();
private:
    //协程调度器启动
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
    //协程/函数/线程组
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

        //重置数据
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
    //use_call有效时，调度携程
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
    //是否正在停止
    bool m_stopping = true;
    //是否自动停止
    bool m_autoStop = false;
    //主线程id(use_call)
    int m_rootThread = 0;
};

} // namespace dory
