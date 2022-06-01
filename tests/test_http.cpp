#include "../dory/http/http.h"
#include "../dory/log.h"

void test_request() {
    dory::http::HttpRequest::ptr req(new dory::http::HttpRequest);
    req->setHeader("host", "www.baidu.com");
    req->setBody("hello baidu");

    req->dump(std::cout) << std::endl;
}

void test_response() {
    dory::http::HttpResponse::ptr rsp(new dory::http::HttpResponse);
    rsp->setHeader("X-X", "dory");
    rsp->setBody("hello dory");
    rsp->setStatus((dory::http::HttpStatus)400);
    rsp->setClose(false);

    rsp->dump(std::cout) << std::endl;
}

int main(int argc, char const *argv[])
{
    test_response();
    return 0;
}
