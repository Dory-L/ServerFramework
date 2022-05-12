#include "../dory/hook.h"
#include "../dory/iomanager.h"
#include "../dory/dory.h"
#include <arpa/inet.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

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

void test_sock() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    
    sockaddr_in add;
    memset(&add, 0, sizeof(sockaddr_in));
    add.sin_family = AF_INET;
    add.sin_port = htons(80);
    inet_pton(AF_INET, "180.101.49.11", &add.sin_addr.s_addr);

    DORY_LOG_INFO(g_logger) << "begin connect";
    int rt = connect(fd, (const sockaddr*)&add, sizeof(add));
    DORY_LOG_INFO(g_logger) << "connect rt=" << rt << " errno=" << errno;

    if (rt) {
        return;
    }

    const char data[] = "GET / HTTP/1.0\r\n\r\n";
    rt = send(fd, data, sizeof(data), 0);
    DORY_LOG_INFO(g_logger) << "send rt=" << rt << " errno=" << errno;

    if (rt <= 0) {
        return;
    }

    std::string buff;
    buff.resize(4096);

    rt = recv(fd, &buff[0], buff.size(), 0);
    DORY_LOG_INFO(g_logger) << "recv rt=" << rt << " errno=" << errno;

    if (rt <= 0) {
        return;
    }

    buff.resize(rt);
    DORY_LOG_INFO(g_logger) << buff;
}

int main(int argc, char const *argv[])
{
    // test_sleep();
    dory::IOManager iom;
    iom.schedule(test_sock);
    return 0;
}

