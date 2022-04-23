#include "Thread.h"
#include "log.h"
#include "util.h"

namespace dory {

//线程局部变量，记录当前线程，thread_local修饰的变量具有线程周期，这些变量在线程开始时生成，在线程结束时被销毁，并且每一个线程都拥有一个独立的变量实例
static thread_local Thread* t_thread = nullptr;
//线程局部变量，线程名称
static thread_local std::string t_thread_name = "UNKNOW";
//get current thread's reference

static dory::Logger::ptr g_logger = DORY_LOG_NAME("system");


Thread* Thread::GetThis() {
    return t_thread;
}

//给log使用，直接获取当前线程名称
const std::string& Thread::GetName() {
    return t_thread_name;
}

//主线程不是自己创建，需要这个方法改主线程的名字
void Thread::SetName(const std::string& name) {
    if (t_thread) {
        t_thread->m_name = name;
    }
    t_thread_name = name;
}

Thread::Thread(std::function<void()> cb, const std::string& name) {
    if (name.empty()) {
        m_name = "UNKNOW";
    } else {
        m_name = name;
    }
    m_cb = cb;
    int rt = pthread_create(&m_thread, nullptr, &Thread::run, this);
    if (rt) {
        DORY_LOG_ERROR(g_logger) << "pthread_create thread fail, rt=" << rt
                << " name=" << m_name;
        throw std::logic_error("pthread_create error");
    }
}

Thread::~Thread() {
    if (m_thread) {
        pthread_detach(m_thread);
    }
}

void Thread::join() {
    if (m_thread) {
        int rt = pthread_join(m_thread, nullptr);
        if (rt) {
            DORY_LOG_ERROR(g_logger) << "pthread_join thread fail, rt=" << rt
                    << " name=" << m_name;
            throw std::logic_error("pthread_join error");
        }
        m_thread = 0;
    }
}

//启动线程，arg为创建的线程指针
void* Thread::run(void* arg) {
    Thread* thread = (Thread*)arg;
    t_thread = thread;
    t_thread_name = thread->m_name;
    thread->m_id = dory::GetThreadId();
    pthread_setname_np(pthread_self(), thread->m_name.substr(0, 15).c_str());

    std::function<void()> cb;
    cb.swap(thread->m_cb);

    cb();
    return 0;
}




} // namespace dory
