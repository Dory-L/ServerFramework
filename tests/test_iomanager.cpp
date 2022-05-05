#include "../dory/dory.h"
#include "../dory/iomanager.h"
#include "../dory/timer.h"

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <errno.h>

dory::Logger::ptr g_logger = DORY_LOG_ROOT();

void test_fiber() {
    DORY_LOG_INFO(g_logger) << "test_fiber";

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(fd, F_SETFL, O_NONBLOCK);

    sockaddr_in add;
    memset(&add, 0, sizeof(sockaddr_in));
    add.sin_family = AF_INET;
    add.sin_port = htons(80);
    inet_pton(AF_INET, "180.101.49.11", &add.sin_addr.s_addr);
    
    if (!connect(fd, (const sockaddr*)&add, sizeof(add))) {
    } else if (errno == EINPROGRESS) {
        DORY_LOG_INFO(g_logger) << "add event errno=" << errno << " " << strerror(errno);
        dory::IOManager::GetThis()->addEvent(fd, dory::IOManager::READ, [](){
            DORY_LOG_INFO(g_logger) << "read callback";
        });
        dory::IOManager::GetThis()->addEvent(fd, dory::IOManager::WRITE, [=](){
            DORY_LOG_INFO(g_logger) << "write callback";
            dory::IOManager::GetThis()->cancelEvent(fd, dory::IOManager::READ);
            close(fd);
        });
    } else {
        DORY_LOG_INFO(g_logger) << "else" << errno << " " << strerror(errno);
    }
}

void test1() {
    dory::IOManager iom;
    iom.schedule(&test_fiber);
}

dory::Timer::ptr timer;

void test_timer() {
    dory::IOManager iom(2);
    timer = iom.addTimer(1000, [](){ //不能用值传递，不然会自动释放timer,也不能用引用，
        static int i = 0;
        DORY_LOG_INFO(g_logger) << "hello timer i=" << i;
        if (++i == 5) {
            timer->reset(2000, true);
            // timer->cancel();
        }
    }, true);
}

int main(int argc, char const *argv[])
{
    // test1();
    test_timer();
    return 0;
}
