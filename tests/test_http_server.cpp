#include "../dory/http/http_server.h"
#include "../dory/log.h"

static dory::Logger::ptr g_logger = DORY_LOG_ROOT();

void run() {
    dory::http::HttpServer::ptr server(new dory::http::HttpServer);
    dory::Address::ptr addr = dory::Address::LookupAnyIPAddress("0.0.0.0:8020");
    while (!server->bind(addr)) {
        sleep(2);
    }
    auto sd = server->getServletDispatch();
    sd->addServlet("/dory/xx", [](dory::http::HttpRequest::ptr req
                ,dory::http::HttpResponse::ptr rsp
                ,dory::http::HttpSession::ptr session){
        rsp->setBody(req->toString());
        return 0;
    });

    sd->addGlobServlet("/dory/*", [](dory::http::HttpRequest::ptr req
                ,dory::http::HttpResponse::ptr rsp
                ,dory::http::HttpSession::ptr session){
        rsp->setBody("Glob:\r\n" + req->toString());
        return 0;
    });
    server->start();
}

int main(int argc, char const *argv[]) {
    dory::IOManager iom(2);
    iom.schedule(run);
    return 0;
}
