#include <iostream>
#include "../dory/http/http_connection.h"
#include "../dory/log.h"
#include "../dory/iomanager.h"

static dory::Logger::ptr g_logger = DORY_LOG_ROOT();

void test_pool() {
    dory::http::HttpConnectionPool::ptr pool(new dory::http::HttpConnectionPool(
                    "www.baidu.com", "", 80, 10, 1000 * 30, 5));
    dory::IOManager::GetThis()->addTimer(1000, [pool](){
        auto r = pool->doGet("/", 300);
        DORY_LOG_INFO(g_logger) << r->toString();
    }, true);
}

void run() {
    dory::Address::ptr addr = dory::Address::LookupAnyIPAddress("www.baidu.com:80");
    if (!addr) {
        DORY_LOG_INFO(g_logger) << "get addr error";
        return;
    }

    dory::Socket::ptr sock = dory::Socket::CreateTCP(addr);
    bool rt = sock->connect(addr);
    if (!rt) {
        DORY_LOG_INFO(g_logger) << "connection " << addr << " failed";
        return;
    }

    dory::http::HttpConnection::ptr conn(new dory::http::HttpConnection(sock));
    dory::http::HttpRequest::ptr req(new dory::http::HttpRequest);
    DORY_LOG_INFO(g_logger) << "req:" << std::endl
        << *req;

    conn->sendRequest(req);
    auto rsp = conn->recvResponse();

    if (!rsp) {
        DORY_LOG_INFO(g_logger) << "recv response error";
        return;
    }
    DORY_LOG_INFO(g_logger) << "rsp:" << std::endl
        << *rsp;

    DORY_LOG_INFO(g_logger) << "=======================================================";

    auto res = dory::http::HttpConnection::DoGet("http://www.baidu.com/", 300);
    DORY_LOG_INFO(g_logger) << "result=" << res->result
            << " error=" << res->error
            << " rsp=" << (rsp ? rsp->toString() : "");

    DORY_LOG_INFO(g_logger) << "=======================================================";
    test_pool();
}

int main(int argc, char const *argv[])
{
    dory::IOManager iom(2);
    iom.schedule(run);
    return 0;
}
