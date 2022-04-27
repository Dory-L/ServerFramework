#include "../dory/dory.h"

dory::Logger::ptr g_logger = DORY_LOG_ROOT();
void run_int_fiber() {
    DORY_LOG_INFO(g_logger) << "run_int_fiber begin";
    dory::Fiber::YieldToHold();
    DORY_LOG_INFO(g_logger) << "run_int_fiber end";
    dory::Fiber::YieldToHold();
}

void test_fiber() {
    DORY_LOG_INFO(g_logger) << "main begin -1";
    {
        dory::Fiber::GetThis();
        DORY_LOG_INFO(g_logger) << "main begin";
        dory::Fiber::ptr fiber(new dory::Fiber(run_int_fiber));
        fiber->swapIn();
        DORY_LOG_INFO(g_logger) << "main after swapIn";
        fiber->swapIn();
        DORY_LOG_INFO(g_logger) << "main after end";
        fiber->swapIn();
    }
    DORY_LOG_INFO(g_logger) << "main after end2";
}

int main(int argc, char const *argv[]) {
    dory::Thread::SetName("main");
    // test_fiber();

    std::vector<dory::Thread::ptr> thrs;
    for (int i = 0; i < 3; ++i) {
        thrs.push_back(dory::Thread::ptr(
                new dory::Thread(&test_fiber, "name_" + std::to_string(i))));
    }
    for (auto i : thrs) {
        i->join();
    }

    return 0;
}
