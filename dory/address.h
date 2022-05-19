#pragma once

#include <memory>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <vector>
#include <map>

namespace dory {

class IPAddress;

class Address {
public:
    typedef std::shared_ptr<Address> ptr;

    /**
     * @brief 通过sockaddr指针创建Address
     * @param[in] addr sockaddr指针
     * @param[in] addrlen sockaddr的长度
     * @return 返回和sockaddr相匹配的Address，失败返回nullptr
     */
    static Address::ptr Create(const sockaddr* addr, socklen_t addrlen);

    /**
     * @brief 通过host地址返回对应条件的所有Address
     * 
     * @param[out] result 保存满足条件的Address
     * @param[in] host 域名，服务器名等，举例:www.baidu.com[:80](方括号为可选内容)
     * @param[in] family 协议族(AF_INET,AF_INET6,AF_UNIX)
     * @param[in] type socket类型SOCK_STREAM,SOCK_DGRAM等 
     * @param[in] protocol 协议，IPPROTO_TCP、IPPROTO_DUP等
     * @return true 成功
     * @return false 失败
     */
    static bool Lookup(std::vector<Address::ptr>& result, const std::string& host,
            int family = AF_INET, int type = 0, int protocol = 0);

    /**
     * @brief 通过host地址返回对应条件的任意Address
     * 
     * @param host host 域名，服务器名等，举例:www.baidu.com[:80](方括号为可选内容)
     * @param family family 协议族(AF_INET,AF_INET6,AF_UNIX)
     * @param type type socket类型SOCK_STREAM,SOCK_DGRAM等 
     * @param protocol protocol 协议，IPPROTO_TCP、IPPROTO_DUP等
     * @return Address::ptr 满足条件的任意Address，失败返回nullptr
     */
    static Address::ptr LookupAny(const std::string& host,
            int family = AF_INET, int type = 0, int protocol = 0);

    /**
     * @brief 返回本机所有网卡的<网卡名，地址，子网掩码位数>
     * 
     * @param[in] host host 域名，服务器名等，举例:www.baidu.com[:80](方括号为可选内容)
     * @param[in] family 协议族(AF_INET,AF_INET6,AF_UNIX)
     * @param[in] type type socket类型SOCK_STREAM,SOCK_DGRAM等 
     * @param[in] protocol protocol 协议，IPPROTO_TCP、IPPROTO_DUP等
     * @return IPAddress::ptr 满足条件的IPAddress，失败返回nullptr
     */
    static std::shared_ptr<IPAddress> LookupAnyIPAddress(const std::string& host,
            int family = AF_INET, int type = 0, int protocol = 0);

    /**
     * @brief 返回本机所有网卡的<网卡名，地址，子网掩码位数>
     * 
     * @param[out] result 保存本机所有地址
     * @param[in] family 协议族(AF_INET,AF_INET6,AF_UNIX)
     * @return true 成功
     * @return false 失败
     */
    static bool GetInterfaceAddress(std::multimap<std::string
                    ,std::pair<Address::ptr, uint32_t> >&result,
                    int family = AF_INET);
    /**
     * @brief 获取指定网卡的地址和子网掩码位数
     * 
     * @param[out] result 保存指定网卡所有地址
     * @param[in] iface 网卡名
     * @param[in] family 协议族(AF_INET,AF_INET6,AF_UNIX)
     * @return true 成功
     * @return false 失败
     */
    static bool GetInterfaceAddress(std::vector<std::pair<Address::ptr, uint32_t> >&result
                    ,const std::string& iface, int family = AF_INET);

    virtual ~Address() {}

    int getFamily() const;

    virtual const sockaddr* getAddr() const = 0;
    virtual sockaddr* getAddr() = 0;
    virtual const socklen_t getAddrLen() const = 0;

    virtual std::ostream& insert(std::ostream& os) const = 0;
    std::string toString();

    bool operator<(const Address& rhs) const;
    bool operator==(const Address& rhs) const;
    bool operator!=(const Address& rhs) const;
};

class IPAddress : public Address {
public:
    typedef std::shared_ptr<IPAddress> ptr;

    static IPAddress::ptr Create(const char* address, uint16_t port = 0);

    //获取广播地址
    virtual IPAddress::ptr broadcastAddress(uint32_t prefix_len) = 0;
    //获取网关地址
    virtual IPAddress::ptr networdAddress(uint32_t prefix_len) = 0;
    //获取子网掩码
    virtual IPAddress::ptr subnetMask(uint32_t prefix_len) = 0;
    
    //获取端口号
    virtual uint32_t getPort() const = 0;
    virtual void setPort(uint16_t v) = 0;
};

class IPv4Address : public IPAddress {
public:
    typedef std::shared_ptr<IPv4Address> ptr;

    static IPv4Address::ptr Create(const char* address, uint16_t port = 0);

    IPv4Address(const sockaddr_in& address);
    IPv4Address(uint32_t address = INADDR_ANY, uint16_t port = 0);

    const sockaddr* getAddr() const override;
    sockaddr* getAddr()  override;
    const socklen_t getAddrLen() const override;
    std::ostream& insert(std::ostream& os) const override;

    //获取广播地址
    IPAddress::ptr broadcastAddress(uint32_t prefix_len) override;
    //获取网段
    IPAddress::ptr networdAddress(uint32_t prefix_len) override;
    //获取子网掩码
    IPAddress::ptr subnetMask(uint32_t prefix_len) override;
    
    //获取端口号
    uint32_t getPort() const override;
    void setPort(uint16_t v) override;
private:
    sockaddr_in m_addr;
};

class IPv6Address : public IPAddress {
public:
    typedef std::shared_ptr<IPv6Address> ptr;
    static IPv6Address::ptr Create(const char* address, uint16_t port = 0);

    IPv6Address();
    IPv6Address(const sockaddr_in6& address);
    IPv6Address(const uint8_t address[16], uint16_t port = 0);

    const sockaddr* getAddr() const override;
    sockaddr* getAddr() override;
    const socklen_t getAddrLen() const override;
    std::ostream& insert(std::ostream& os) const override;

    //获取广播地址
    IPAddress::ptr broadcastAddress(uint32_t prefix_len) override;
    //获取网关地址
    IPAddress::ptr networdAddress(uint32_t prefix_len) override;
    //获取子网掩码
    IPAddress::ptr subnetMask(uint32_t prefix_len) override;
    
    //获取端口号
    uint32_t getPort() const override;
    void setPort(uint16_t v) override;
private:
    sockaddr_in6 m_addr;
};

class UnixAddress : public Address {
public:
    typedef std::shared_ptr<UnixAddress> ptr;
    UnixAddress();
    UnixAddress(const std::string& path);

    const sockaddr* getAddr() const override;
    sockaddr* getAddr() override;
    const socklen_t getAddrLen() const override;
    void setAddrLen(uint32_t v);
    std::ostream& insert(std::ostream& os) const override;
private:
    struct sockaddr_un m_addr;
    //sockaddr_un的最大长度
    socklen_t m_length;
};

class UnknownAddress : public Address {
public:
    typedef std::shared_ptr<UnknownAddress> ptr;
    UnknownAddress(int family);
    UnknownAddress(const sockaddr& addr);
    const sockaddr* getAddr() const override;
    sockaddr* getAddr() override;
    const socklen_t getAddrLen() const override;
    std::ostream& insert(std::ostream& os) const override;
private:
    sockaddr m_addr;
};

}