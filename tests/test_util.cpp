#include "../dory/dory.h"
#include <assert.h>


dory::Logger::ptr g_logger = DORY_LOG_ROOT();

void test() {
    //DORY_LOG_INFO(g_logger) << dory::BacktraceToString(10);
    DORY_ASSERT2(0 == 1, "abcdef xxx");
}

int main(int argc, char const *argv[])
{
    test();
    return 0;
}
