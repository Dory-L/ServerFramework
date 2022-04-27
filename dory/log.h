#pragma once

#include <string>
#include <stdint.h>
#include <memory>
#include <list>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include "singleton.h"
#include "util.h"
#include "Thread.h"

#define DORY_LOG_LEVEL(logger, level) \
    if (logger->getLevel() <= level) \
        dory::LogEventWrap(dory::LogEvent::ptr(new dory::LogEvent(logger, level, \
             __FILE__, __LINE__, 0, dory::GetThreadId(), \
            dory::GetFiberId(), time(0), dory::Thread::GetName()))).getSS()

#define DORY_LOG_DEBUG(logger) DORY_LOG_LEVEL(logger, dory::LogLevel::DEBUG)
#define DORY_LOG_INFO(logger) DORY_LOG_LEVEL(logger, dory::LogLevel::INFO)
#define DORY_LOG_WARN(logger) DORY_LOG_LEVEL(logger, dory::LogLevel::WARN)
#define DORY_LOG_ERROR(logger) DORY_LOG_LEVEL(logger, dory::LogLevel::ERROR)
#define DORY_LOG_FATAL(logger) DORY_LOG_LEVEL(logger, dory::LogLevel::FATAL)

#define DORY_LOG_FMT_LEVEL(logger, level, fmt, ...) \
    if (logger->getLevel() <= level) \
        dory::LogEventWrap(dory::LogEvent::ptr(new dory::LogEvent(logger, level, \
            __FILE__, __LINE__, 0, dory::GetThreadId(), \
            dory::GetFiberId(), time(0), dory::Thread::GetName()))).getEvent()->format(fmt, __VA_ARGS__)

#define DORY_LOG_FMT_DEBUG(logger, fmt, ...) DORY_LOG_FMT_LEVEL(logger, dory::LogLevel::DEBUG, fmt, __VA_ARGS__)
#define DORY_LOG_FMT_INFO(logger, fmt, ...) DORY_LOG_FMT_LEVEL(logger, dory::LogLevel::DEBUG, fmt, __VA_ARGS__)
#define DORY_LOG_FMT_WARN(logger, fmt, ...) DORY_LOG_FMT_LEVEL(logger, dory::LogLevel::DEBUG, fmt, __VA_ARGS__)
#define DORY_LOG_FMT_ERROR(logger, fmt, ...) DORY_LOG_FMT_LEVEL(logger, dory::LogLevel::DEBUG, fmt, __VA_ARGS__)
#define DORY_LOG_FMT_FATAL(logger, fmt, ...) DORY_LOG_FMT_LEVEL(logger, dory::LogLevel::DEBUG, fmt, __VA_ARGS__)

#define DORY_LOG_ROOT() dory::LoggerMgr::GetInstance()->getRoot()
#define DORY_LOG_NAME(name) dory::LoggerMgr::GetInstance()->getLogger(name)

namespace dory {

class Logger;
class LoggerManager;

//日志级别
class LogLevel
{
public:
    enum Level {
        UNKNOW = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };

    static const char* ToString(LogLevel::Level level);
    static LogLevel::Level FromString(const std::string& str);
};

//日志事件
class LogEvent
{
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char* file, int32_t m_line, uint32_t elapse 
            , uint32_t thread_id, uint32_t fiber_id, uint64_t time, const std::string& thread_name);
    const char* getFile() const { return m_file; }
    int32_t getLine() const { return m_line; }
    uint32_t getElapse() const { return m_elapse; }
    uint32_t getThreadId() const { return m_threadId; }
    uint32_t getFiberId() const { return m_fiberId; }
    uint64_t getTime() const { return m_time; }
    const std::string& getThreadName() const { return m_threadName; }
    std::string getContent() const { return m_ss.str();}
    std::shared_ptr<Logger> getLogger() const { return m_logger;}
    LogLevel::Level getLevel() const { return m_level; }

    std::stringstream& getSS() { return m_ss;}
    void format(const char* fmt, ...);
    void format(const char* fmt, va_list al);
private:
    const char* m_file = nullptr;   //文件名
    int32_t m_line = 0;             //行号
    uint32_t m_elapse = 0;          //程序启动开始到现在的毫秒数
    uint32_t m_threadId = 0;        //线程号
    uint32_t m_fiberId = 0;         //协程号
    uint64_t m_time = 0;            //时间戳
    std::string m_threadName;       //线程名
    std::stringstream m_ss;

    std::shared_ptr<Logger> m_logger;
    LogLevel::Level m_level;
};

//封装LogEvent，当LogEventWrap析构的时候将event写到log里面去
//wrap作为临时对象，在使用完智能指针之后直接析构，触发日志写入，然而日志本身的智能指针，如果在主函数里面，程序不结束就永远无法释放
class LogEventWrap {
public:
    LogEventWrap(LogEvent::ptr e);
    ~LogEventWrap();
    std::stringstream& getSS() { return m_event->getSS();};
    LogEvent::ptr getEvent() { return m_event; };
private:
    LogEvent::ptr m_event;
};



//日志格式器
class LogFormatter {
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    LogFormatter(const std::string& pattern);

    std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);//提供string给apppder输出

public:
    class FormatItem
    {
    public:
        typedef std::shared_ptr<FormatItem> ptr;
        virtual ~FormatItem() {}
        //格式化输出
        virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    };
    void init();

    //判断是否有错
    bool isError() const { return m_error; }
    const std::string getPattern() const { return m_pattern; }
private:
    std::string m_pattern;
    std::vector<FormatItem::ptr> m_items;//待输出的FormatItem列表
    bool m_error = false;
};

//日志输出地
class LogAppender {
friend class Logger;
public:
    typedef std::shared_ptr<LogAppender> ptr;
    typedef Spinlock MutexType;
    virtual ~LogAppender() {}

    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;

    virtual std::string toYamlString() = 0;

    //设置日志格式器
    void setFormatter(LogFormatter::ptr val);           //复杂类型的线程安全会导致内存错误
    //获取日志格式器
    LogFormatter::ptr getFormatter();
    //获取日志等级
    LogLevel::Level getLevel() const { return m_level; }    //基本数据类型的线程安全，只是导致值错误
    //设置日志等级
    void setLevel(LogLevel::Level val) { m_level = val; }
protected:
    LogLevel::Level m_level = LogLevel::DEBUG;//主要针对哪些日志定义的级别，子类用到
    bool m_hasFormatter = false;
    MutexType m_mutex;
    LogFormatter::ptr m_formatter;
};

//日志器
class Logger : public std::enable_shared_from_this<Logger> {
friend class loggerManager;
public:
    typedef std::shared_ptr<Logger> ptr;
    typedef Spinlock MutexType;

    Logger(const std::string& name = "root");
    void log(LogLevel::Level level, LogEvent::ptr event);

    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warn(LogEvent::ptr event);
    void error(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);

    void addAppender(LogAppender::ptr);
    void delAppender(LogAppender::ptr);
    void clearAppenders();
    LogLevel::Level getLevel() const { return m_level; }
    void setLevel(LogLevel::Level val) { m_level = val; }

    const std::string& getName() const { return m_name; }       //只在主线程中调用，不用加锁

    void setFormatter(LogFormatter::ptr val);
    void setFormatter(const std::string& val);
    LogFormatter::ptr getFormatter();

    std::string toYamlString();
private:
    std::string m_name;     //日志名称
    LogLevel::Level m_level;//日志器的日志级别，满足这个日志级别的日志才输出
    MutexType m_mutex;
    std::list<LogAppender::ptr> m_appenders ;//Appender集合
    LogFormatter::ptr m_formatter;//appender没有formatter时备用的
    Logger::ptr m_root;
};

//输出到控制台的Appender
class StdoutLogAppender : public LogAppender {
friend class Logger;
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
    std::string toYamlString() override;
};

//定义输出到文件的Appender
class FileLogAppender : public LogAppender {
friend class Logger;
public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string& filename);
    void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
    std::string toYamlString() override;

    //打开文件
    bool reopen();
private:
    //LogFormatter::ptr m_formatter;//LogAppender有m_formatter成员，不能重复定义，不然处bug
    std::string m_filename;
    std::ofstream m_filestream;

    uint64_t m_lastTime = 0;
};

//logger管理器，默认生成一个logger（StdoutLogAppender）
class loggerManager {
public:
    typedef Spinlock MutexType;
    loggerManager();
    //获取name对应的logger
    Logger::ptr getLogger(const std::string& name);

    void init();
    //获取初始logger
    Logger::ptr getRoot() const { return m_root; }

    std::string toYamlString();
private:
    MutexType m_mutex;
    std::map<std::string, Logger::ptr> m_loggers;
    Logger::ptr m_root;
};

//使用单例模式生成一个loggerManager对象
typedef dory::Singleton<loggerManager> LoggerMgr;

}