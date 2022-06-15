#include "http_session.h"
#include "http_parser.h"

namespace dory {
namespace http {

HttpSession::HttpSession(Socket::ptr sock, bool owner)
    :SocketStream(sock, owner){
}

HttpRequest::ptr HttpSession::recvRequest() {
    HttpRequestParser::ptr parser(new HttpRequestParser);

    uint64_t buffer_size = HttpRequestParser::GetHttpRequestBufferSize();//不分段
    // uint64_t buffer_size = 100;//分段
    //为了防止内存不释放，用的时候还是用裸指针
    std::shared_ptr<char> buffer(
            new char[buffer_size], [](char* ptr){
                delete[] ptr;
            });
    char* data = buffer.get();
    int offset = 0;
    do {
        int len = read(data + offset, buffer_size - offset);
        if (len <= 0) { //发生错误
            return nullptr;
        }
        len += offset;
        size_t nparse = parser->excute(data, len);
        if (parser->hasError()) {
            return nullptr;
        }
        offset = len - nparse;
        if (offset == (int)buffer_size) {
            return nullptr;
        }
        if (parser->isFinished()) {
            break;
        }
    } while (true);
    int64_t length =  parser->getContentLength();
    if (length > 0) {
        std::string body;
        body.resize(length);

        int len = 0;
        if (length >= offset) {
            memcpy(&body[0], data, offset);
            len = offset;
        } else {
            memcpy(&body[0], data, length);
            len = length;
        }
        length -= offset;
        if (length > 0) {
            if (readFixSize(&body[len], length) <= 0) {
                return nullptr;
            }
        }
        parser->getData()->setBody(body);
    }
    return parser->getData();

}

int HttpSession::sendResponse(HttpResponse::ptr rsp) {
    std::stringstream ss;
    ss << *rsp;
    std::string data = ss.str();
    return writeFixSize(data.c_str(), data.size());
}

}
}