#include "../dory/dory.h"

dory::Logger::ptr g_logger = DORY_LOG_ROOT();

void test_fiber() {
    static int s_count = 5;
    DORY_LOG_INFO(g_logger) << "test in fiber s_count=" << s_count;

    sleep(1);
    if (--s_count >= 0) {
        dory::Scheduler::GetThis()->schedule(&test_fiber, dory::GetThreadId());
    }
}

int main(int argc, char const *argv[])
{
    DORY_LOG_INFO(g_logger) << "main";
    dory::Scheduler sc(3, false , "test");
    sc.start();
    sleep(1);
    DORY_LOG_INFO(g_logger) << "schedule";
    sc.schedule(&test_fiber);
    sleep(10);
    sc.stop();
    DORY_LOG_INFO(g_logger) << "over";
    return 0;
}
