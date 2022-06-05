#pragma once

#include "http.h"
#include "http11_parser.h"
#include "httpclient_parser.h"
#include <memory>

namespace dory {
namespace http {

/**
 * @brief http请求解析类
 * 
 */
class HttpRequestParser {
public:
    typedef std::shared_ptr<HttpRequestParser> ptr;
    HttpRequestParser();

    /**
     * @brief 解析协议
     * 
     * @param data 协议文本内存
     * @param len 协议文本内存长度
     * @param off 
     * @return size_t 
     */
    size_t excute(char* data, size_t len);
    int isFinished();
    int hasError(); 

    HttpRequest::ptr getData()  { return m_data; }
    void setError(int v) { m_error = v; }

    uint64_t getContentLength();
public:
    static uint64_t GetHttpRequestBufferSize();
    static uint64_t GetHttpRequestMaxBodySize();
private:
    http_parser m_parser;
    HttpRequest::ptr m_data;
    //1000:invalid method
    //1001:invalid version
    //1002:invalid field
    int m_error;
};

/**
 * @brief http响应解析结构体
 * 
 */
class HttpResponseParser {
public:
    typedef std::shared_ptr<HttpResponseParser> ptr;
    HttpResponseParser();

    size_t excute(char* data, size_t len);
    int isFinished();
    int hasError();

    HttpResponse::ptr getData()  { return m_data; }
    void setError(int v) { m_error = v; }
    
    uint64_t getContentLength();
private:
    httpclient_parser m_parser;
    HttpResponse::ptr m_data;
    //1001：invalid version
    //1002:invalid field
    int m_error;
};

}
}