#include "timer.h"
#include "util.h"

namespace dory {

bool Timer::Comparator::operator()(const Timer::ptr& lhs
                        ,const Timer::ptr& rhs) const {
    if (!lhs && !rhs) {
        return false;
    }
    if (!lhs) {
        return true;
    }
    if (!rhs) {
        return false;
    }
    if (lhs->m_next < rhs->m_next) {
        return true;
    }
    if (rhs->m_next < lhs->m_next) {
        return false;
    }
    return lhs.get() < rhs.get();
}

Timer::Timer(uint64_t ms, std::function<void()> cb,
            bool recurring, TimerManager* manager)
    :m_recurring(recurring)
    ,m_ms(ms)
    ,m_cb(cb)
    ,m_manager(manager){
    m_next = dory::GetCurrentMS() + m_ms;
}

Timer::Timer(uint64_t next)
    :m_next(next) {
}

bool Timer::cancel() {
    TimerManager::RWMutexType::WriteLock lock(m_manager->m_mutex);
    if (m_cb) {
        m_cb = nullptr;
        auto it = m_manager->m_timers.find(shared_from_this());
        m_manager->m_timers.erase(it);
        return true;
    }
    return false;
}

bool Timer::refresh() {
    TimerManager::RWMutexType::WriteLock lock(m_manager->m_mutex);
    if (!m_cb) {
        return false;
    }
    auto it = m_manager->m_timers.find(shared_from_this());
    if (it == m_manager->m_timers.end()) {
        return false;
    }
    //先删除再添加回去，红黑树不能原地修改key
    m_manager->m_timers.erase(it);
    m_next = dory::GetCurrentMS() + m_ms;
    m_manager->m_timers.insert(shared_from_this());
    return false;
}

bool Timer::reset(uint64_t ms, bool from_now) {
    if (ms == m_ms && !from_now) {
        return true;
    }
    TimerManager::RWMutexType::WriteLock lock(m_manager->m_mutex);
    if (!m_cb) {
        return false;
    }
    
    auto it = m_manager->m_timers.find(shared_from_this());
    if (it == m_manager->m_timers.end()) {
        return false;
    }
    //先删除再添加回去，红黑树不能原地修改key
    m_manager->m_timers.erase(it);
    uint64_t start = 0;
    if (from_now) {
        start = dory::GetCurrentMS();
    } else {
        start = m_next - m_ms;
    }
    m_ms = ms;
    m_next = start + m_ms;
    // m_manager->addTimer(shared_from_this(), lock);
    m_manager->addTimer(shared_from_this(), lock);
    return true;
}

TimerManager::TimerManager() {
    m_previousTime = dory::GetCurrentMS();
}

TimerManager::~TimerManager() {

}

Timer::ptr TimerManager::addTimer(uint64_t ms, std::function<void()> cb
                        ,bool recurring) {
    Timer::ptr timer(new Timer(ms, cb, recurring, this));
    RWMutexType::WriteLock lock(m_mutex);
    addTimer(timer, lock);
    return timer;
}

static void OnTimer(std::weak_ptr<void> weak_cond, std::function<void()> cb) {
    std::shared_ptr<void> tmp = weak_cond.lock();
    if (tmp) {
        cb();
    }
}

//条件定时器
//只有当条件存在时才触发定时器
Timer::ptr TimerManager::addConditionTiemr(uint64_t ms, std::function<void()> cb
                                        ,std::weak_ptr<void> weak_cond
                                        ,bool recurring) {
    return addTimer(ms, std::bind(&OnTimer, weak_cond, cb), recurring);
}

uint64_t TimerManager::getNextTimer() {
    RWMutexType::ReadLock lock(m_mutex);
    m_tickled = true;
    if (m_timers.empty()) {
        return ~0ull;//零取反成为最大值
    }

    const Timer::ptr& next = *m_timers.begin();
    uint64_t now_ms = dory::GetCurrentMS();
    if (now_ms >= next->m_next) {   //已经超时就返回0，立即执行
        return 0;
    } else {
        return next->m_next - now_ms;   //返回剩余时间
    }
}

void TimerManager::listExpiredCb(std::vector<std::function<void()> >& cbs) {
    uint64_t now_ms = dory::GetCurrentMS();
    std::vector<Timer::ptr> expired;
    {
        RWMutexType::ReadLock lock(m_mutex);
        if (m_timers.empty()) {
            return;
        }
    }
    RWMutexType::WriteLock lock(m_mutex);
    if (m_timers.empty()) {
        return;
    }

    bool rollover = detectClockRollover(now_ms);
    if (!rollover && ((*m_timers.begin())->m_next > now_ms)) { //系统时钟没有问题并且也没有timer超时
        return;
    }

    Timer::ptr now_timer(new Timer(now_ms));
    //如果系统时间有问题，就把整个m_timers清理掉
    auto it = rollover ? m_timers.end() : m_timers.lower_bound(now_timer);
    while (it != m_timers.end() && (*it)->m_next == now_ms) {
        ++it;
    }
    expired.insert(expired.begin(), m_timers.begin(), it);
    m_timers.erase(m_timers.begin(), it);
    cbs.reserve(expired.size());

    for (auto& timer : expired) {
        cbs.push_back(timer->m_cb);
        if (timer->m_recurring) {
            timer->m_next = now_ms + timer->m_ms;
            m_timers.insert(timer);
        } else {
            timer->m_cb = nullptr; //置空，防止智能指针计数不能置零
        }
    }
}

void TimerManager::addTimer(Timer::ptr val, RWMutexType::WriteLock& lock) {
    auto it = m_timers.insert(val).first;
    bool at_front = (it == m_timers.begin()) && !m_tickled;
    if (at_front) { //只修改一次，在getNextTiemr的时候重置m_tickle
        m_tickled = true;
    }
    lock.unlock();

    //如果新加入的timer在最前面，则新timer的定时器最短，需要重新设定时间
    if (at_front) {
        onTimerInsertedAtFront();//唤醒epoll_wait
    }
}

bool TimerManager::detectClockRollover(uint64_t now_ms) {
    bool rollover = false;
    if (now_ms < m_previousTime && now_ms < (m_previousTime - 60 * 60 * 1000)) {
        rollover = true;
    }
    m_previousTime = now_ms;
    return rollover;
}

bool TimerManager::hasTimer() {
    RWMutexType::ReadLock lock(m_mutex);
    return m_timers.empty();
}

}