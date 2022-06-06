#include "http_server.h"
#include "../log.h"

namespace dory {
namespace http {

static dory::Logger::ptr g_logger = DORY_LOG_NAME("system");

HttpServer::HttpServer(bool keepalive
            ,dory::IOManager* worker
            ,dory::IOManager* accept)
    :TcpServer(worker, accept)
    ,m_isKeepalive(keepalive) {
    m_dispatch.reset(new ServletDispatch);
}

void HttpServer::handleClient(Socket::ptr client) {
    HttpSession::ptr session(new HttpSession(client));
    do {
        auto req = session->recvRequest();
        if (!req) {
            DORY_LOG_WARN(g_logger) << "recv http request fail, error=" 
                << errno << " errstr=" << strerror(errno)
                << " client" << *client;
            break;
        }


        HttpResponse::ptr rsp(new HttpResponse(req->getVersion(), req->isClose() || !m_isKeepalive));
        m_dispatch->handle(req, rsp, session);

        // rsp->setBody("hello dory");

        // DORY_LOG_INFO(g_logger) << "request:" << std::endl
        //     << *req;
        // DORY_LOG_INFO(g_logger) << "response:" << std::endl
        //     << *rsp;

        session->sendResponse(rsp);
    } while (m_isKeepalive);
    session->close();
}

}
}