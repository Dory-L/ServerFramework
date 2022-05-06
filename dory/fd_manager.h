#pragma once

#include <memory>
#include <vector>
#include "Thread.h"
#include "iomanager.h"
#include "singleton.h"
#include "fd_manager.h"

namespace dory {

//文件句柄上下文类
class FdCtx : public std::enable_shared_from_this<FdCtx> {
public:
    typedef std::shared_ptr<FdCtx> ptr;
    FdCtx(int fd);
    ~FdCtx();

    bool init();
    bool isInit() const { return isInit; };
    bool isSocket() const { return m_isSocket; }
    bool isClosed() const { return m_isClosed; }
    bool close();

    //设置用户主动设置非阻塞
    void setUserNonblock(bool v) { m_userNonblock = v; }
    //获取用户是否主动设置非阻塞
    bool getUserNonblock() const { return m_userNonblock; } 
    //设置系统非阻塞
    void setSysNonblock(bool v) { m_sysNonblock = v; }
    //获取系统非阻塞
    bool getSysNonblock() const { return m_sysNonblock; }
    //设置超时时间
    void setTimeout(int type, uint64_t v);
    //获取超时时间
    uint64_t getTimeout(int type);
private:
    bool m_isInit : 1;
    bool m_isSocket : 1;
    bool m_sysNonblock : 1;
    bool m_userNonblock : 1;
    bool m_isClosed : 1;
    int m_fd;
    uint64_t m_recvTimeout;
    uint64_t m_sendTimeout;
    dory::IOManager* m_iomanager;
};

//文件句柄管理类
class FdManager {
public:
    typedef RWMutex RWMutexType;
    FdManager();

    //获取文件句柄类FdCtx
    //fd 文件句柄
    //auto_create 是否自动创建
    FdCtx::ptr get(int fd, bool auto_create = false);
    //删除文件句柄
    void def(int fd);

private:
    RWMutexType m_mutex;
    std::vector<FdCtx::ptr> m_datas;
}; 

typedef Singleton<FdManager> FdMgr;

}