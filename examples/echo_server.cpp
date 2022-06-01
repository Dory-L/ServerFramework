#include "../dory/tcp_server.h"
#include "../dory/log.h"
#include "../dory/iomanager.h"
#include "../dory/bytearray.h"
#include <unistd.h>

static dory::Logger::ptr g_logger = DORY_LOG_ROOT();

class EchoServer : public dory::TcpServer {
public:
    EchoServer(int type);
    void handleClient(dory::Socket::ptr client);
private:
    //0:文本
    //1:二进制
    int m_type;
};

EchoServer::EchoServer(int type)
    :m_type(type){
}

void EchoServer::handleClient(dory::Socket::ptr client) {
    DORY_LOG_INFO(g_logger) << "handleClient " << *client;
    dory::ByteArray::ptr ba(new dory::ByteArray);
    while (true) {
        ba->clear();
        std::vector<iovec> iovs;
        ba->getWriteBuffers(iovs, 1024);

        int rt = client->recv(&iovs[0], iovs.size());
        if (rt == 0) {
            DORY_LOG_INFO(g_logger) << "client close: " << *client;
            break;
        } else if (rt < 0) {
            DORY_LOG_INFO(g_logger) << "client error rt=" << rt
                << " errno=" << errno << " errstr=" << strerror(errno);
            break;
        }
        ba->setPosition(ba->getPosition() + rt);
        ba->setPosition(0);
        if (m_type == 1) {//text
            std::cout << ba->toString();
        } else {
            std::cout << ba->toHexString();
        }
        std::cout.flush();
    }
}

int type = 1;

void run() {
    EchoServer::ptr es(new EchoServer(type));
    auto addr = dory::Address::LookupAny("0.0.0.0:8888");
    while (!es->bind(addr)) {
        sleep(2);
    }
    es->start();
}

int main(int argc, char const *argv[]) {
    if (argc < 2) {
        DORY_LOG_INFO(g_logger) << "use as[" << argv[0] << " -t] or [" << argv[0] << " -b]";
        return 0;
    }

    if (!strcmp(argv[1], "-b")) {
        type = 2;
    }

    dory::IOManager iom(2);
    iom.schedule(run);
    return 0;
}
