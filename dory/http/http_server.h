#pragma once
#include "../tcp_server.h"
#include "http_session.h"

namespace dory {
namespace http {

class HttpServer : public TcpServer {
public:
    typedef std::shared_ptr<HttpServer> ptr;
    HttpServer(bool kepalive = false
                ,dory::IOManager* worker = dory::IOManager::GetThis()
                ,dory::IOManager* accept = dory::IOManager::GetThis());
protected:
    void handleClient(Socket::ptr client) override;

private:
    bool m_isKeepalive;
};

}
}