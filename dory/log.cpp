#include "log.h"
#include <map>
#include <functional>
#include <time.h>
#include <string>
#include <stdio.h>
#include <stdarg.h>

namespace dory {

    const char* LogLevel::ToString(LogLevel::Level level) {
        switch (level)
        {
    #define XX(name) \
        case LogLevel::name: \
            return #name;
            break;
        XX(DEBUG);
        XX(INFO);
        XX(WARN);
        XX(ERROR);
        XX(FATAL);
    #undef XX
        default:
            return "UNKNOW";
        }
        return "UNKNOW";
    }

    LogEventWrap::LogEventWrap(LogEvent::ptr e) 
        : m_event(e) {

    }
    LogEventWrap::~LogEventWrap() {
        m_event->getLogger()->log(m_event->getLevel(), m_event);//这样就能把自己写进去
    }
    std::stringstream& LogEventWrap::getSS() {
        return m_event->getSS();
    }
    LogEvent::ptr LogEventWrap::getEvent() {
        return m_event;
    }

    void LogEvent::format(const char* fmt, ...) {
        va_list al;
        va_start(al, fmt);
        format(fmt, al);
        va_end(al);
    }

    void LogEvent::format(const char* fmt, va_list al) {
        char* buf = nullptr;
        int len = vasprintf(&buf, fmt, al);//自动分配内存
        if (len != -1) {
            m_ss << std::string(buf, len);//分配成功，就写入m_ss中
            free(buf);//释放内存
        }
    }

    class MessageFormatItem : public LogFormatter::FormatItem {
    public:
        MessageFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getContent();
        }
    };

    class LevelFormatItem : public LogFormatter::FormatItem {
    public:
        LevelFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << LogLevel::ToString(level);
        }
    };


    class ElapseFormatItem : public LogFormatter::FormatItem {
    public:
        ElapseFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getElapse();
        }
    };

    class NameFormatItem : public LogFormatter::FormatItem {
    public:
        NameFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << logger->getName();
        }
    };

    class ThreadIdFormatItem : public LogFormatter::FormatItem {
    public:
        ThreadIdFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getThreadId();
        }
    };

    class FiberIdFormatItem : public LogFormatter::FormatItem {
    public:
        FiberIdFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getFiberId();
        }
    };

    class DateTimeFormatItem : public LogFormatter::FormatItem {
    public:
        DateTimeFormatItem(const std::string& format = "%Y-%m-%d %H-%M-%S") 
            : m_format(format) {
            if (m_format.empty()) {
                m_format = "%Y-%m-%d %H:%M:%S";
            }
        }
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            struct tm tm;
            time_t time = event->getTime();
            localtime_r(&time, &tm);
            char buf[64];
            strftime(buf, sizeof(buf), m_format.c_str(), &tm);
            os << buf;
        }
    private:
        std::string m_format;
    };
    class FilenameFormatItem : public LogFormatter::FormatItem {
    public:
        FilenameFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getFile();
        }
    };
    class LineFormatItem : public LogFormatter::FormatItem {
    public:
        LineFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getLine();
        }
    };

    class NewLineFormatItem : public LogFormatter::FormatItem {
    public:
        NewLineFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << std::endl;
        }
    };

    class StringFormatItem : public LogFormatter::FormatItem {
    public:
        StringFormatItem(const std::string& str)
            : m_string(str) {
        }
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << m_string;
        }
    private:
        std::string m_string;
    };

    class TableFormatItem : public LogFormatter::FormatItem {
    public:
        TableFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << "\t";
        }
    private:
        std::string m_string;
    };
    
    LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char* file, int32_t line, uint32_t elapse 
            , uint32_t thread_id, uint32_t fiber_id, uint64_t time)
            : m_file(file)
            , m_line(line)
            , m_elapse(elapse)
            , m_threadId(thread_id)
            , m_fiberId(fiber_id)
            , m_time(time) 
            , m_logger(logger) 
            , m_level(level) {
    }

    Logger::Logger(const std::string& name)
        : m_name(name)
        , m_level(LogLevel::DEBUG) {
        m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
    }

    void Logger::addAppender(LogAppender::ptr appender) {
        if (!appender->getFormatter()) {         //appender如果没有formatter，就把自己的formatter给它
            appender->setFormatter(m_formatter);
        }
        m_appenders.push_back(appender);
    }

    void Logger::delAppender(LogAppender::ptr appender) {
        for (auto it = m_appenders.begin();
                it != m_appenders.end(); it++) {
            if (*it == appender) {
                m_appenders.erase(it);
                break;
            }
        }
    }

    void Logger::log(LogLevel::Level level, LogEvent::ptr event) {
        if (level >= m_level) {
            auto self = shared_from_this();
            for (auto& i : m_appenders) { //这里的m_appenders是LogAppender类型的，因此FileLogAppender和StdoutLogAppender切记不能重写父类的成员，不然将会调用LogAppender作用域的成员，如果没有初始化后果很严重
                i->log(self, level, event);
            }
        }
    }

    void Logger::debug(LogEvent::ptr event) {
        log(LogLevel::DEBUG, event);
    }
    void Logger::info(LogEvent::ptr event) {
        log(LogLevel::INFO, event);
    }
    void Logger::warn(LogEvent::ptr event) {
        log(LogLevel::WARN, event);
    }
    void Logger::error(LogEvent::ptr event) {
        log(LogLevel::ERROR, event);
    }
    void Logger::fatal(LogEvent::ptr event) {
        log(LogLevel::FATAL, event);
    }

    void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
        if (level >= m_level) {
            std::cout << m_formatter->format(logger, level, event);
        }
    }

    FileLogAppender::FileLogAppender(const std::string& filename) 
        : m_filename(filename){
        if (!reopen()) {
            std::cout << "open file failed!" << std::endl;
        }
    }

    void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
        if (level >= m_level) {
            // if (!m_formatter) {//bug，待修改
            //     return;
            // }
            m_filestream << m_formatter->format(logger, level, event);
        }
    }
    bool FileLogAppender::reopen() {
        if (m_filestream.is_open()) {//如果已经打开，则关闭重新打开
            m_filestream.close();
        }
        m_filestream.open(m_filename, std::ios::out);
        return m_filestream.is_open();
    }

    LogFormatter::LogFormatter(const std::string& pattern)
        : m_pattern(pattern) {
        init();
    }

    std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
        std::stringstream ss;
        for (auto& i : m_items) {
            i->format(ss, logger, level, event);
        }
        return ss.str();
    }

    //%xxx %xxx{xxx} %% 三种模式
    void LogFormatter::init() {
        //str, format, type
        std::vector<std::tuple<std::string, std::string, int> > vec;//tuple相当于是一个结构体
        std::string nstr;
        for (size_t i = 0; i < m_pattern.size(); ++i) {
            if (m_pattern[i] != '%') {
                nstr.append(1, m_pattern[i]);
                continue;
            }

            if ((i + 1) < m_pattern.size()) {
                if (m_pattern[i + 1] == '%') { //%%，他就是个%，因为第一个%是转义符
                    nstr.append(1, '%');
                    continue;
                }
            }

            size_t n = i + 1;
            int fmt_status = 0;//初始化状态
            size_t fmt_begin = 0;

            std::string str;
            std::string fmt;
            while (n < m_pattern.size()) {
                if (!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{' && m_pattern[n] != '}')) {//如果不是空格就是连续的，否则就退出
                    str = m_pattern.substr(i + 1, n - i - 1);
                    break;
                }
                if (fmt_status == 0) {
                    if (m_pattern[n] == '{') {
                        str = m_pattern.substr(i + 1, n - i - 1);
                        fmt_status = 1;//解析格式
                        fmt_begin = n;
                        ++n;
                        continue;
                    }
                }
                else if (fmt_status == 1) {
                    if (m_pattern[n] == '}') {
                        fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                        fmt_status = 0;
                        ++n;
                        break;
                    }
                }
                ++n;
                if (n == m_pattern.size()) {
                    if (str.empty()) {
                        str = m_pattern.substr(i + 1);
                    }
                }
            }

            if (fmt_status == 0) {
                if (!nstr.empty()) {
                    vec.push_back(std::make_tuple(nstr, std::string(), 0));
                    nstr.clear();
                }
                vec.push_back(std::make_tuple(str, fmt, 1));
                i = n - 1;
            } else if (fmt_status == 1) {
                std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
                vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
            } /* else if (fmt_status == 2) {
                if (!nstr.empty()) {
                    vec.push_back(std::make_tuple(nstr, "", 0));
                    nstr.clear();
                }
                vec.push_back(std::make_tuple(str, fmt, 1));
                i = n - 1;
            } */
        }

        if (!nstr.empty()) {
            vec.push_back(std::make_tuple(nstr, std::string(), 0));
        }

        //%m -- 消息体
        //%p -- Level
        //%r -- 启动后的时间
        //%c -- 日志名称
        //%t -- 线程id
        //%n -- 回车
        //%d -- 时间格式
        //%f -- 文件名
        //%l -- 行号
    
        static std::map<std::string, std::function<LogFormatter::FormatItem::ptr(const std::string& str)> > s_format_items = {
    #define XX(str, C) \
            {#str, [](const std::string& fmt) { return LogFormatter::FormatItem::ptr(new C(fmt)); } }

            XX(m, MessageFormatItem),
            XX(p, LevelFormatItem),
            XX(r, ElapseFormatItem),
            XX(c, NameFormatItem),
            XX(t, ThreadIdFormatItem),
            XX(n, NewLineFormatItem),
            XX(d, DateTimeFormatItem),
            XX(f, FilenameFormatItem),
            XX(l, LineFormatItem),
            XX(T, TableFormatItem),
            XX(F, FiberIdFormatItem),
    #undef XX
        };

        for (auto& i : vec) {
            if (std::get<2>(i) == 0) {//string类型
                m_items.push_back(LineFormatItem::ptr(new StringFormatItem(std::get<0>(i))));
            } else {
                auto it = s_format_items.find(std::get<0>(i));
                if (it == s_format_items.end()) {
                    m_items.push_back(LogFormatter::FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
                } else {
                    m_items.push_back(it->second(std::get<1>(i)));
                }
            } 

            // std::cout << "(" << std::get<0>(i) << ") - (" << std::get<1>(i) << ") - (" << std::get<2>(i) << ")" << std::endl;
        }
    }

    loggerManager::loggerManager() {
        m_root.reset(new Logger);
        m_root->delAppender(LogAppender::ptr(new StdoutLogAppender));
    }

    Logger::ptr loggerManager::getLogger(const std::string& name) {
        auto it = m_loggers.find(name);
        return it == m_loggers.end() ? m_root : it->second;
    }
    void loggerManager::init() {

    }

    
}