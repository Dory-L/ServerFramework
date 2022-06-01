#include "../dory/http/http_parser.h"
#include "../dory/log.h"

static dory::Logger::ptr g_logger = DORY_LOG_ROOT();

const char test_request_data[] = "POST / HTTP/1.1\r\n"
                                "Host: www.baidu.com\r\n"
                                "Content-Length: 10\r\n\r\n"
                                "1234567890";

void test_request() {
    dory::http::HttpRequestParser parser;
    std::string tmp = test_request_data;
    size_t s = parser.excute(&tmp[0], tmp.size());
    DORY_LOG_INFO(g_logger) << "execute rt=" << s
        << " has_error=" << parser.hasError()
        << " is_finished=" << parser.isFinished()
        << " total=" << tmp.size()
        << " content-length=" << parser.getContentLength();
    tmp.resize(tmp.size() - s);

    DORY_LOG_INFO(g_logger) << parser.getData()->toString();
    DORY_LOG_INFO(g_logger) << tmp;
}

const char test_response_data[] = "HTTP/1.0 200 OK\r\n"
                                "Accept-Ranges: bytes\r\n"
                                "Cache-Control: no-cache\r\n"
                                "Content-Length: 9508\r\n"
                                "Content-Type: text/html\r\n"
                                "Date: Mon, 30 May 2022 14:20:45 GMT\"\r\n"
                                "P3p: CP=\" OTI DSP COR IVA OUR IND COM \"\r\n"
                                "P3p: CP=\" OTI DSP COR IVA OUR IND COM \"\r\n"
                                "Pragma: no-cache\r\n"
                                "Server: BWS/1.1\r\n"
                                "Set-Cookie: BAIDUID=D3105EE3605A0E426EC554314DF5B81F:FG=1; expires=Thu, 31-Dec-37 23:55:55 GMT; max-age=2147483647; path=/; domain=.baidu.com\r\n"
                                "Set-Cookie: BIDUPSID=D3105EE3605A0E426EC554314DF5B81F; expires=Thu, 31-Dec-37 23:55:55 GMT; max-age=2147483647; path=/; domain=.baidu.com\r\n"
                                "Set-Cookie: PSTM=1653920445; expires=Thu, 31-Dec-37 23:55:55 GMT; max-age=2147483647; path=/; domain=.baidu.com\r\n"
                                "Set-Cookie: BAIDUID=D3105EE3605A0E4214DF325111BEE229:FG=1; max-age=31536000; expires=Tue, 30-May-23 14:20:45 GMT; domain=.baidu.com; path=/; version=1; comment=bd\r\n"
                                "Traceid: 165392044527941068905465133681634467394\r\n"
                                "Vary: Accept-Encoding\r\n"
                                "X-Frame-Options: sameorigin\r\n"
                                "X-Ua-Compatible: IE=Edge,chrome=1\r\n\r\n"
                                "1267987978-980-909=\r\n"
                                "wsgsaerfghsd\r\n";

void test_response() {
    dory::http::HttpResponseParser parser;
    std::string tmp = test_response_data;
    size_t s = parser.excute(&tmp[0], tmp.size());
    DORY_LOG_INFO(g_logger) << "execute rt=" << s
        << " has_error=" << parser.hasError()
        << " is_finished=" << parser.isFinished()
        << " total=" << tmp.size()
        << " content-length=" << parser.getContentLength()
        << " tmp[s]=" << tmp[s];
    tmp.resize(tmp.size() - s);

    DORY_LOG_INFO(g_logger) << parser.getData()->toString();
    DORY_LOG_INFO(g_logger) << tmp;

}

int main(int argc, char const *argv[])
{
    test_request();
    DORY_LOG_INFO(g_logger) << "--------------------------------";
    test_response();
    return 0;
}
