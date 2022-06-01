#pragma once

#include <memory>
#include "address.h"
#include "noncopyable.h"
namespace dory {
/**
 * @brief Socket封装类
 * 
 */
class Socket : public std::enable_shared_from_this<Socket>, Noncopyable {
public:
    typedef std::shared_ptr<Socket> ptr;
    typedef std::weak_ptr<Socket> weak_ptr;

    enum Type {
        TCP = SOCK_STREAM,
        UDP = SOCK_DGRAM
    };

    enum Family {
        IPv4 = AF_INET,
        IPv6 = AF_INET6,
        Unix = AF_UNIX
    };

    /**
     * @brief 创建TCP socket(满足地址类型)
     * 
     * @param address 地址
     * @return Socket::ptr 
     */
    static Socket::ptr CreateTCP(dory::Address::ptr address);

    /**
     * @brief 创建UDP socket(满足地址类型)
     * 
     * @param address 地址
     * @return Socket::ptr 
     */
    static Socket::ptr CreateUDP(dory::Address::ptr address);

    /**
     * @brief 创建IPv4的TCP socket
     * 
     * @return Socket::ptr 
     */
    static Socket::ptr CreateTCPSocket();

    /**
     * @brief 创建IPv4的UDP socket
     * 
     * @return Socket::ptr 
     */
    static Socket::ptr CreateUDPSocket();

    /**
     * @brief 创建IPv6的TCP socket
     * 
     * @return Socket::ptr 
     */
    static Socket::ptr CreateTCPSocket6();

    /**
     * @brief 创建IPv6的UDP socket
     * 
     * @return Socket::ptr 
     */
    static Socket::ptr CreateUDPSocket6();

    /**
     * @brief 创建Unix的TCP socket
     * 
     * @return Socket::ptr 
     */
    static Socket::ptr CreateUnixTCPSocket();

    /**
     * @brief 创建Unix的UDP socket
     * 
     * @return Socket::ptr 
     */
    static Socket::ptr CreateUnixUDPSocket();

    /**
     * @brief Construct a new Socket object
     * 
     * @param family 协议族
     * @param type 类型(tcp/udp)
     * @param protocol 协议
     */
    Socket(int family, int type, int protocol = 0);
    ~Socket();

    /**
     * @brief 获取发送超时时间(ms)
     * 
     * @return int64_t 
     */
    int64_t getSendTimeout();

    /**
     * @brief 设置发送超时时间(ms)
     * 
     * @param v
     */
    void setSendTimeout(int64_t v);

    /**
     * @brief 获取接受超时时间(ms)
     * 
     * @return int64_t 
     */
    int64_t getRecvTimeout();

    /**
     * @brief 设置接受超时时间(ms)
     * 
     * @param v 
     */
    void setRecvTimeout(int64_t v);

    /**
     * @brief 获取sockopt
     * 
     * @param level 
     * @param option 
     * @param result 
     * @param len 
     * @return true 
     * @return false 
     */
    bool getOption(int level, int option, void* result, socklen_t* len);

    template<class T>
    bool getOption(int level, int option, T& result) {
        size_t length = sizeof(T);
        return getOption(level, option, &result, &length);
    }

    /**
     * @brief 设置sockopt
     * 
     * @param level 
     * @param option 
     * @param value 
     * @param len 
     * @return true 
     * @return false 
     */
    bool setOption(int level, int option, const void* value, socklen_t len);
    template<class T>
    bool setOption(int level, int option, const T& value) {
        return setOption(level, option, &value, sizeof(T));
    }

    /**
     * @brief 接收connect连接
     * 
     * @return 成功返回新连接的socket，失败返回nullptr
     */
    Socket::ptr accept();

    bool bind(const Address::ptr addr);
    /**
     * @brief 连接地址
     * 
     * @param[in] addr 目标地址 
     * @param timeout_ms 超时时间
     * @return true 成功
     * @return false 
     */
    bool connect(const Address::ptr addr, uint64_t timeout_ms = -1);

    /**
     * @brief 监听socket
     * 
     * @param backlog 
     * @return true 
     * @return false 
     */
    bool listen(int backlog = SOMAXCONN);
    bool close();

    /**
     * @brief 发送数据
     * 
     * @param buffer 待发送数据的内存
     * @param length 待发送数据的长度
     * @param flags 标志字
     * @return int 
     *      @retval >0 发送成功对应大小的数据
     *      @retval =0 socket被关闭 
     *      @retval <0 socket出错 
     */
    int send(const void* buffer, size_t length, int flags = 0);

    /**
     * @brief 发送数据
     * 
     * @param buffers 待发送数据的内存(iovec数组)
     * @param length 待发送数据的长度(iovec长度)
     * @param flags 标志字
     * @return int 
     *      @retval >0 发送成功对应大小的数据
     *      @retval =0 socket被关闭 
     *      @retval <0 socket出错 
     */
    int send(const iovec* buffers, size_t length, int flags = 0);

    /**
     * @brief 发送数据
     * 
     * @param buffer 待发送数据的内存
     * @param length 待发送数据的长度
     * @param to 发送的目的地
     * @param flags 标志字
     * @return int 
     *      @retval >0 发送成功对应大小的数据
     *      @retval =0 socket被关闭 
     *      @retval <0 socket出错 
     */
    int sendTo(const void* buffer, size_t length, const Address::ptr to, int flags = 0);

    /**
     * @brief 发送数据
     * 
     * @param buffers 待发送数据的内存(iovec数组)
     * @param length 待发送数据的长度(iovec长度)
     * @param to 发送的目标地址
     * @param flags 标志字
     * @return int 
     *      @retval >0 发送成功对应大小的数据
     *      @retval =0 socket被关闭 
     *      @retval <0 socket出错 
     */
    int sendTo(const iovec* buffers, size_t length, const Address::ptr to, int flags = 0);

    /**
     * @brief 接受数据
     * 
     * @param buffer 接受数据的内存
     * @param length 接受数据的大小
     * @param flags 标志字
     * @return int 
     *      @retval >0 接受到对应大小的数据
     *      @retval =0 socket被关闭 
     *      @retval <0 socket出错 
     */
    int recv(void* buffer, size_t length, int flags = 0);

    /**
     * @brief 接受数据
     * 
     * @param buffers 接受数据的内存(iovec数组)
     * @param length 接受数据的大小(iovec长度)
     * @param flags 标志字
     * @return int 
     *      @retval >0 接受到对应大小的数据
     *      @retval =0 socket被关闭 
     *      @retval <0 socket出错 
     */
    int recv(iovec* buffers, size_t length, int flags = 0);

    /**
     * @brief 接受数据
     * 
     * @param buffer 接受数据的内存
     * @param length 接受数据的大小
     * @param from 发送端地址
     * @param flags 标志字
     * @return int 
     */
    int recvFrom(void* buffer, size_t length, Address::ptr from, int flags = 0);

    /**
     * @brief 接受数据
     * 
     * @param buffer 接受数据的内存(iovec数组)
     * @param length 接受数据的大小(iovec长度)
     * @param from 发送端地址
     * @param flags 标志字
     * @return int 
     */
    int recvFrom(iovec* buffers, size_t length, Address::ptr from, int flags = 0);

    /**
     * @brief Get the Remote Address
     * 
     * @return Address::ptr 
     */
    Address::ptr getRemoteAddress();

    /**
     * @brief Get the Local Address
     * 
     * @return Address::ptr 
     */
    Address::ptr getLocalAddress();

    int getFamily() const { return m_family; }
    int getType() const { return m_type; }
    int getProtocol() const { return m_protocol; }

    /**
     * @brief 返回是否连接
     * 
     * @return true 
     * @return false 
     */
    bool isConnected() const { return m_isConnected; }

    /**
     * @brief 是否有效(m_sock != -1)
     * 
     * @return true 
     * @return false 
     */
    bool isValid() const;

    /**
     * @brief 返回socket错误
     * 
     * @return int 
     */
    int getError();

    /**
     * @brief 输出信息到流中
     * 
     * @param os 
     * @return std::ostream& 
     */
    std::ostream& dump(std::ostream& os) const;

    /**
     * @brief 返回socket句柄
     * 
     * @return int 
     */
    int getSocket() const { return m_sock; }

    /**
     * @brief 取消读
     * 
     * @return true 
     * @return false 
     */
    bool cancelRead();

    /**
     * @brief 取消写
     * 
     * @return true 
     * @return false 
     */
    bool cancelWrite();

    /**
     * @brief 取消accept
     * 
     * @return true 
     * @return false 
     */
    bool cancelAccept();

    /**
     * @brief 取消所有事件
     * 
     * @return true 
     * @return false 
     */
    bool cancelAll();
private:
    /**
     * @brief 初始化socket
     * 
     */
    void initSock();
    /**
     * @brief 创建socket
     * 
     */
    void newSock();

    /**
     * @brief 初始化sock
     * 
     * @param sock 
     * @return true 
     * @return false 
     */
    bool init(int sock);
private:
    int m_sock;
    int m_family;
    int m_type;
    int m_protocol;
    int m_isConnected;

    Address::ptr m_localAddress;
    Address::ptr m_remoteAddress;
};

std::ostream& operator<<(std::ostream& os, const Socket& sock);

}