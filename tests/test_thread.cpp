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
    while (true) {
        DORY_LOG_INFO(g_logger) << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
    }
}

void fun3() {
    while (true) {
        DORY_LOG_INFO(g_logger) << "==============================================================";
    }
}

int main(int argc, char** argv) {
    DORY_LOG_INFO(g_logger) << "thread test begin";
    YAML::Node root = YAML::LoadFile("/home/lly/projectfile/dory/workspace/bin/conf/test2.yml");
    dory::Config::LoadFromYaml(root);

    std::vector<dory::Thread::ptr> thrs;
    for (int i = 0; i < 2; ++i) {
        dory::Thread::ptr thr(new dory::Thread(&fun2, "name_" + std::to_string(i * 2)));
        dory::Thread::ptr thr2(new dory::Thread(&fun3, "name_" + std::to_string(i * 2)));
        thrs.push_back(thr);
        thrs.push_back(thr2);
    }

    for (size_t i = 0; i < thrs.size(); ++i) {
        thrs[i]->join();
    }
    DORY_LOG_INFO(g_logger) << "thread test end";
    DORY_LOG_INFO(g_logger) << "count=" << count;
    return 0;
}