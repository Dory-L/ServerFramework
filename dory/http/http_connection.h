#pragma once

#include "http.h"
#include "../socket_stream.h"
#include "../uri.h"
#include "../Thread.h"
#include <memory>
#include <list>
#include <stdint.h>

namespace dory {
namespace http {

struct HttpResult {
    typedef std::shared_ptr<HttpResult> ptr;
    
    enum class Error {
        //正常
        OK = 0,
        //非法URL
        INVALID_URL = 1,
        //无法解析HOST
        INVALID_HOST = 2,
        //连接失败
        CONNECT_FAIL = 3,
        //连接被对端关闭
        SEND_CLOSE_BY_PEER = 4,
        //发送连接请求产生Socket错误
        SEND_SOCK_ERROR = 5,
        //超时
        TIMEOUT = 6,
        //创建Socket失败
        CREATE_SOCKET_ERROR = 7,
        //从连接池中取连接失败
        POOL_GET_CONNECTION = 8,
        //无效的连接
        POOL_INVALID_CONNECTION = 9,
    };

    /**
     * @brief Construct a new Http Result object
     * 
     * @param _result 错误码
     * @param _response HTTP响应结构体
     * @param _error 错误描述
     */
    HttpResult(int _result
            ,HttpResponse::ptr _response
            ,const std::string& _error) 
        :result(_result)
        ,response(_response)
        ,error(_error) {
    }
    //错误码
    int result;
    //Http响应结构体
    HttpResponse::ptr response;
    //错误描述
    std::string error;

    std::string toString() const;
};

class HttpConnectionPool;
/**
 * @brief HTTP客户端类
 * 
 */
class HttpConnection : public SocketStream {
friend class HttpConnectionPool;
public:
    typedef std::shared_ptr<HttpConnection> ptr;

    /**
     * @brief 发送HTTP的GET请求
     * 
     * @param url 请求的url
     * @param timeout_ms 超时时间(ms)
     * @param headers 请求头部参数
     * @param body 请求消息体
     * @return HttpResult::ptr HTTP结果结构体
     */
    static HttpResult::ptr DoGet(const std::string& url
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");
    /**
     * @brief 发送HTTP的GET请求
     * 
     * @param uri URI结构体
     * @param timeout_ms 超时时间(ms)
     * @param headers 请求头部参数
     * @param body 请求消息体
     * @return HttpResult::ptr HTTP结果结构体
     */
    static HttpResult::ptr DoGet(Uri::ptr uri
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");

    /**
     * @brief 发送HTTP的POST请求
     * 
     * @param url 请求的url
     * @param timeout_ms 超时时间(ms)
     * @param headers 请求头部参数
     * @param body 请求消息体
     * @return HttpResult::ptr HTTP结果结构体
     */
    static HttpResult::ptr DoPost(const std::string& url
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");
    
    /**
     * @brief 发送HTTP的POST请求
     * 
     * @param uri URI结构体
     * @param timeout_ms 超时时间
     * @param headers 请求头部参数
     * @param body 请求消息体
     * @return HttpResult::ptr HTTP结果结构体
     */
    static HttpResult::ptr DoPost(Uri::ptr uri
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = ""); 

    /**
     * @brief 发送HTTP请求
     * 
     * @param method 请求类型
     * @param url 请求的url
     * @param timeout_ms 超时时间
     * @param headers 请求头部参数
     * @param body 请求消息体
     * @return HttpResult::ptr HTTP结果结构体
     */
    static HttpResult::ptr DoRequest(HttpMethod method
                            , const std::string& url
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");

    /**
     * @brief 发送HTTP请求
     * 
     * @param method 请求类型
     * @param uri URI结构体
     * @param timeout_ms 超时时间
     * @param headers 请求头部参数
     * @param body 请求消息体
     * @return HttpResult::ptr HTTP结果结构体
     */
    static HttpResult::ptr DoRequest(HttpMethod method
                            , Uri::ptr uri
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");

    /**
     * @brief 发送HTTP请求
     * 
     * @param req req请求结构体
     * @param uri URI结构体
     * @param timeout_ms 超时时间
     * @return HttpResult::ptr 返回HTTP结果结构体
     */
    static HttpResult::ptr DoRequest(HttpRequest::ptr req
                            , Uri::ptr uri
                            , uint64_t timeout_ms);

    /**
     * @brief Construct a new Http Connection object
     * 
     * @param sock Scoket类
     * @param owner 是否掌握socket所有权
     */
    HttpConnection(Socket::ptr sock, bool owner = true);

    /**
     * @brief Destroy the Http Connection object
     * 
     */
    ~HttpConnection();

    /**
     * @brief 接收HTTP响应
     * 
     * @return HttpResponse::ptr 
     */
    HttpResponse::ptr recvResponse();
    /**
     * @brief 发送HTTP请求
     * 
     * @param req HTTP请求结构体
     * @return int 发送字节数
     */
    int sendRequest(HttpRequest::ptr req);

private:
    uint64_t m_createTime = 0;
    uint64_t m_request = 0;
};    

class HttpConnectionPool {
public:
    typedef std::shared_ptr<HttpConnectionPool> ptr;
    typedef Mutex MutexType;

    HttpConnectionPool(const std::string& host
                        , const std::string& vhost
                        , uint32_t port
                        , uint32_t max_size
                        , uint32_t max_alive_time
                        , uint32_t max_request);
    HttpConnection::ptr getConnection();

    /**
     * @brief 发送HTTP的GET请求
     * 
     * @param url 请求的url
     * @param timeout_ms 超时时间(ms)
     * @param headers 请求头部参数
     * @param body 请求消息体
     * @return HttpResult::ptr HTTP结果结构体
     */
    HttpResult::ptr doGet(const std::string& url
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");
    /**
     * @brief 发送HTTP的GET请求
     * 
     * @param uri URI结构体
     * @param timeout_ms 超时时间(ms)
     * @param headers 请求头部参数
     * @param body 请求消息体
     * @return HttpResult::ptr HTTP结果结构体
     */
    HttpResult::ptr doGet(Uri::ptr uri
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");

    /**
     * @brief 发送HTTP的POST请求
     * 
     * @param url 请求的url
     * @param timeout_ms 超时时间(ms)
     * @param headers 请求头部参数
     * @param body 请求消息体
     * @return HttpResult::ptr HTTP结果结构体
     */
    HttpResult::ptr doPost(const std::string& url
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");
    
    /**
     * @brief 发送HTTP的POST请求
     * 
     * @param uri URI结构体
     * @param timeout_ms 超时时间
     * @param headers 请求头部参数
     * @param body 请求消息体
     * @return HttpResult::ptr HTTP结果结构体
     */
    HttpResult::ptr doPost(Uri::ptr uri
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = ""); 

    /**
     * @brief 发送HTTP请求
     * 
     * @param method 请求类型
     * @param url 请求的url
     * @param timeout_ms 超时时间
     * @param headers 请求头部参数
     * @param body 请求消息体
     * @return HttpResult::ptr HTTP结果结构体
     */
    HttpResult::ptr doRequest(HttpMethod method
                            , const std::string& url
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");

    /**
     * @brief 发送HTTP请求
     * 
     * @param method 请求类型
     * @param uri URI结构体
     * @param timeout_ms 超时时间
     * @param headers 请求头部参数
     * @param body 请求消息体
     * @return HttpResult::ptr HTTP结果结构体
     */
    HttpResult::ptr doRequest(HttpMethod method
                            , Uri::ptr uri
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");

    /**
     * @brief 发送HTTP请求
     * 
     * @param req req请求结构体
     * @param timeout_ms 超时时间
     * @return HttpResult::ptr 返回HTTP结果结构体
     */
    HttpResult::ptr doRequest(HttpRequest::ptr req
                            , uint64_t timeout_ms);

private:
    static void ReleasePtr(HttpConnection* ptr, HttpConnectionPool* pool);

private:
    std::string m_host;
    std::string m_vhost;
    uint32_t m_port;
    uint32_t m_maxSize;
    uint32_t m_maxAliveTime;
    uint32_t m_maxRequest;

    MutexType m_mutex;
    std::list<HttpConnection*> m_conns;
    std::atomic<int32_t> m_total = {0};
};

}
} // namespace dory
