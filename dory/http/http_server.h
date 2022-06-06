#pragma once
#include "../tcp_server.h"
#include "http_session.h"
#include "servlet.h"

namespace dory {
namespace http {

class HttpServer : public TcpServer {
public:
    typedef std::shared_ptr<HttpServer> ptr;
    HttpServer(bool kepalive = false
                ,dory::IOManager* worker = dory::IOManager::GetThis()
                ,dory::IOManager* accept = dory::IOManager::GetThis());

    ServletDispatch::ptr getServletDispatch() const { return m_dispatch; }
    void setServletDispatch(ServletDispatch::ptr v) { m_dispatch = v; }
protected:
    void handleClient(Socket::ptr client) override;

private:
    bool m_isKeepalive;
    ServletDispatch::ptr m_dispatch;
};

}
}