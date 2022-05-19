#include "../dory/dory.h"
#include "../dory/socket.h"
#include "../dory/iomanager.h"
#include "../dory/address.h"

static dory::Logger::ptr g_logger = DORY_LOG_ROOT();

void test_socket() {
    dory::IPAddress::ptr addr = dory::Address::LookupAnyIPAddress("www.baidu.com");
    if (addr) {
        DORY_LOG_INFO(g_logger) << "get address: " << addr->toString();
    } else {
        DORY_LOG_ERROR(g_logger) << "get address fail";
        return;
    }

    dory::Socket::ptr sock = dory::Socket::CreateTCP(addr);
    addr->setPort(80);
    DORY_LOG_INFO(g_logger) << "addr=" << addr->toString();
    if(!sock->connect(addr)) {
        DORY_LOG_ERROR(g_logger) << "connect " << addr->toString() << " fail";
    } else {
        DORY_LOG_INFO(g_logger) << "connect " << addr->toString() << " connected";
    }

    const char buff[] = "GET / HTTP/1.0\r\n\r\n";
    int rt = sock->send(buff, sizeof(buff));
    if (rt <= 0) {
        DORY_LOG_INFO(g_logger) << "send fail rt=" << rt;
        return;
    }

    std::string buffs;
    buffs.resize(4096);
    rt = sock->recv(&buffs[0], buffs.size());

    if (rt <= 0) {
        DORY_LOG_INFO(g_logger) << "recv fail rt=" << rt;
        return;
    }
    buffs.resize(rt);
    DORY_LOG_INFO(g_logger) << buffs;
}

int main(int argc, char const *argv[])
{
    dory::IOManager iom;
    iom.schedule(&test_socket);
    return 0;
}
