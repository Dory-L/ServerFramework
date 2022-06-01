#pragma once

#include <memory>
#include <functional>
#include "iomanager.h"
#include "address.h"
#include "socket.h"
#include "noncopyable.h"

namespace dory {

class TcpServer : public std::enable_shared_from_this<TcpServer> 
                    ,Noncopyable {
public:
    typedef std::shared_ptr<TcpServer> ptr;
    TcpServer(dory::IOManager* worker = dory::IOManager::GetThis()
              ,dory::IOManager* accept_worker = dory::IOManager::GetThis());
    virtual ~TcpServer();

    /**
     * @brief 绑定地址
     * 
     * @param addr 
     * @return true 绑定成功
     * @return false 
     */
    virtual bool bind(dory::Address::ptr addr);

    /**
     * @brief 绑定地址数组
     * 
     * @param addrs 需要绑定的地址数组
     * @param fails 绑定失败的地址
     * @return true 全部绑定成功
     * @return false 
     */
    virtual bool bind(const std::vector<Address::ptr>& addrs
                        ,std::vector<Address::ptr>& fails);

    /**
     * @brief 启动服务
     * 
     * @return true 成功
     * @return false 
     */
    virtual bool start();

    /**
     * @brief 停止服务
     * 
     */
    virtual void stop();

    uint64_t getRecvTimeout() const { return m_recvTimeout; }
    std::string getName() const { return m_name; }
    void setRecvTimeout(uint64_t v) { m_recvTimeout = v; }
    void setName(const std::string& v) { m_name = v; }

    bool isStop() const { return m_isStop; }
protected:
    /**
     * @brief 处理新连接的Socket类
     * 
     * @param client 客户端Socket对象指针
     */
    virtual void handleClient(Socket::ptr client);

    /**
     * @brief 开始接受连接
     * 
     * @param sock 需要接受连接的Socket对象指针
     */
    virtual void startAccept(Socket::ptr sock);
private:
    //监听Socket数组
    std::vector<Socket::ptr> m_socks;
    //新连接Socket工作的调度器
    IOManager* m_worker;
    //服务器Socket接受连接的调度器
    IOManager* m_acceptWorker;
    ///接受超时时间
    uint64_t m_recvTimeout;
    ///服务器名称
    std::string m_name;
    ///服务是否停止
    bool m_isStop;
};

}