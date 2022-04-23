#include "../dory/dory.h"
#include <unistd.h>

dory::Logger::ptr g_logger = DORY_LOG_ROOT();

void fun1() {
    DORY_LOG_INFO(g_logger) << "name: " << dory::Thread::GetName()
                            << " this.name: " << dory::Thread::GetThis()->getName()
                            << " id: " << dory::GetThreadId()
                            << " this.id: " << dory::Thread::GetThis()->getId();
    sleep(60);
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
    return 0;
}