#pragma once

#include <memory>
#include <set>
#include <vector>
#include "Thread.h"

namespace dory {

class TimerManager;
class Timer : public std::enable_shared_from_this<Timer> {
friend class TimerManager;
public:
    typedef std::shared_ptr<Timer> ptr;

    //取消定时器
    bool cancel();
    //重新定时
    bool refresh();
    //更改执行周期
    //form_now是否从现在开始定时
    bool reset(uint64_t ms, bool from_now);
private:
    //必须通过TimerManager创建，所以构造函数私有
    Timer(uint64_t ms, std::function<void()> cb,
            bool recurring, TimerManager* manager);
    Timer(uint64_t next);

private:
    //是恢复循环定时器
    bool m_recurring = false;
    //执行周期
    uint64_t m_ms = 0;
    //精确的执行时间(某个时刻)
    uint64_t m_next = 0;
    std::function<void()> m_cb;
    TimerManager* m_manager = nullptr;

private:
    struct Comparator {
        bool operator()(const Timer::ptr& lhs, const Timer::ptr& rhs) const;
    };
    

};

class TimerManager {
friend class Timer;
public:
    typedef RWMutex RWMutexType;

    TimerManager();
    virtual ~TimerManager();

    //添加timer
    Timer::ptr addTimer(uint64_t ms, std::function<void()> cb
                        ,bool recurring = false);

    //条件定时器
    //只有当条件存在时才触发定时器
    Timer::ptr addConditionTiemr(uint64_t ms, std::function<void()> cb
                        ,std::weak_ptr<void> weak_cond
                        ,bool recurring = false);

    uint64_t getNextTimer();
    //获取超时timer的回调函数
    void listExpiredCb(std::vector<std::function<void()> >& cbs);
    bool hasTimer();
protected:
    virtual void onTimerInsertedAtFront() = 0;
    //添加timer，可能到队首
    void addTimer(Timer::ptr val, RWMutexType::WriteLock& lock);
private:
    //系统时间是否有问题
    //true 有问题， false 无问题
    bool detectClockRollover(uint64_t now_ms);
private:
    RWMutexType m_mutex;
    std::set<Timer::ptr, Timer::Comparator> m_timers;
    bool m_tickled = false;
    uint64_t m_previousTime = 0;
};

}