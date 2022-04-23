#pragma once
//pthread_xxx
//std::thread, pthread
//写少读多，使用读写锁能提高效率
//选用c++11的线程库，和pthread的互斥量

#include <thread>
#include <pthread.h>
#include <functional>
#include <memory>

namespace dory
{
class Thread {
public:
    typedef std::shared_ptr<Thread> ptr;
    Thread(std::function<void()> cb, const std::string& name);
    ~Thread();

    pid_t getId() const {return m_id; }
    const std::string& getName() const { return m_name; }

    void join();

    //get current thread's reference
    static Thread* GetThis();
    //给log使用，直接获取当前线程名称
    static const std::string& GetName();
    //主线程不是自己创建，需要这个方法改主线程的名字
    static void SetName(const std::string& name);
private:
    Thread(const Thread&) = delete;
    Thread(const Thread&&) = delete;
    Thread& operator=(const Thread&) = delete;

    static void* run(void* arg);
private:
    pid_t m_id = -1;
    pthread_t m_thread = 0;
    std::function<void()> m_cb;
    std::string m_name;
};

} // namespace dory
