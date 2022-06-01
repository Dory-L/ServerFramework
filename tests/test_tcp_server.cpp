#include "../dory/tcp_server.h"
#include "../dory/iomanager.h"
#include "../dory/log.h"
#include <unistd.h>

static dory::Logger::ptr g_logger = DORY_LOG_ROOT();

void run() {
    auto addr = dory::Address::LookupAny("0.0.0.0:8033");
    // auto addr2 = dory::UnixAddress::ptr(new dory::UnixAddress("/tmp/unix_addr"));
    std::vector<dory::Address::ptr> addrs;
    std::vector<dory::Address::ptr> fails;
    addrs.push_back(addr);
    // addrs.push_back(addr2);

    dory::TcpServer::ptr tcp_server(new dory::TcpServer);
    while (!tcp_server->bind(addrs, fails)) {
        sleep(2);
    }
    tcp_server->start();
}

int main(int argc, char const *argv[])
{
    dory::IOManager iom(2);
    iom.schedule(run);
    return 0;
}
