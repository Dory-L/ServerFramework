#include "../dory/dory.h"
#include <unistd.h>

dory::Logger::ptr g_logger = DORY_LOG_ROOT();

int count = 0;
// dory::RWMutex s_mutex;
dory::Mutex s_mutex;

void fun1() {
    DORY_LOG_INFO(g_logger) << "name: " << dory::Thread::GetName()
                            << " this.name: " << dory::Thread::GetThis()->getName()
                            << " id: " << dory::GetThreadId()
                            << " this.id: " << dory::Thread::GetThis()->getId();
    for (int i = 0; i < 1000000; ++i) {
        // dory::RWMutex::WriteLock lock(s_mutex);
        dory::Mutex::Lock lock(s_mutex);
        ++count;
    }
}

void fun2() {

}

int main(int argc, char** argv) {
    DORY_LOG_INFO(g_logger) << "thread test begin";
    std::vector<dory::Thread::ptr> thrs;
    for (int i = 0; i < 5; ++i) {
        dory::Thread::ptr thr(new dory::Thread(&fun1, "name_" + std::to_string(i)));
        thrs.push_back(thr);
    }

    for (int i = 0; i < 5; ++i) {
        thrs[i]->join();
    }
    DORY_LOG_INFO(g_logger) << "thread test end";
    DORY_LOG_INFO(g_logger) << "count=" << count;
    return 0;
}