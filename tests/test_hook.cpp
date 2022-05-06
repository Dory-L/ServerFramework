#include "../dory/hook.h"
#include "../dory/iomanager.h"
#include "../dory/dory.h"

dory::Logger::ptr g_logger = DORY_LOG_ROOT();

void test_sleep() {
    dory::IOManager iom(1);
    iom.schedule([](){
        sleep(2);
        DORY_LOG_INFO(g_logger) << "sleep 2";
    });
    iom.schedule([](){
        sleep(3);
        DORY_LOG_INFO(g_logger) << "sleep 3";
    });
    DORY_LOG_INFO(g_logger) << "test_sleep";
}

int main(int argc, char const *argv[])
{
    test_sleep();
    return 0;
}

