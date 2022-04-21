#include "log.h"
#include <map>
#include <functional>
#include <time.h>
#include <string>
#include <stdio.h>
#include <stdarg.h>
#include "config.h"

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

    LogLevel::Level LogLevel::FromString(const std::string& str) {
#define XX(level, v) \
        if (str == #v) { \
            return LogLevel::level; \
        }

        XX(DEBUG, debug);
        XX(INFO, info);
        XX(WARN, warn);
        XX(ERROR, error);
        XX(FATAL, fatal);

        XX(DEBUG, DEBUG);
        XX(INFO, INFO);
        XX(WARN, WARN);
        XX(ERROR, ERROR);
        XX(FATAL, FATAL);
        return LogLevel::UNKNOW;
#undef XX
    }

    LogEventWrap::LogEventWrap(LogEvent::ptr e) 
        : m_event(e) {

    }
    LogEventWrap::~LogEventWrap() {
        m_event->getLogger()->log(m_event->getLevel(), m_event);//这样就能把自己写进去
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
            //os << logger->getName();
            os << event->getLogger()->getName();
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

    void Logger::setFormatter(LogFormatter::ptr val) {
        m_formatter = val;
    }
    void Logger::setFormatter(const std::string& val) {
        dory::LogFormatter::ptr new_val(new dory::LogFormatter(val));
        if (new_val->isError()) { //防止formatter出错
            std::cout << "Logger setFormatter name=" << m_name
                      << " value=" << val << " invalid formatter"
                      << std::endl;
            return;
        }
        m_formatter = new_val;
    }
    LogFormatter::ptr Logger::getFormatter() {
        return m_formatter;
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

    void Logger::clearAppenders() {
        m_appenders.clear();
    }

    void Logger::log(LogLevel::Level level, LogEvent::ptr event) {
        if (level >= m_level) {
            auto self = shared_from_this();
            if (!m_appenders.empty()) {       //m_appenders不为空就用，appender的log，否则用m_root
                for (auto& i : m_appenders) { //这里的m_appenders是LogAppender类型的，因此FileLogAppender和StdoutLogAppender切记不能重写父类的成员，不然将会调用LogAppender作用域的成员，如果没有初始化后果很严重
                    i->log(self, level, event);
                }
            } else if (m_root){
                m_root->log(level, event);
            }
        }
    }
    std::string Logger::toYamlString() {
        YAML::Node node;
        node["name"] = m_name;
        if (m_level != LogLevel::UNKNOW) {
            node["level"] = LogLevel::ToString(m_level);
        }
        if (m_formatter) {
            node["formatter"] = m_formatter->getPattern();
        }

        for (auto& i : m_appenders) {
            node["appenders"].push_back(YAML::Load(i->toYamlString()));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
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

    std::string StdoutLogAppender::toYamlString() {
        YAML::Node node;
        node["type"] = "StdoutLogAppender";
        if (m_level != LogLevel::UNKNOW) {
            node["level"] = LogLevel::ToString(m_level);
        }
        if (m_formatter) {
            node["formatter"] = m_formatter->getPattern();
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
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

    std::string FileLogAppender::toYamlString() {
        YAML::Node node;
        node["type"] = "FileLogAppender";
        node["file"] = m_filename;
        if (m_level != LogLevel::UNKNOW) {
            node["level"] = LogLevel::ToString(m_level);
        }
        if (m_formatter) {
            node["formatter"] = m_formatter->getPattern();
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
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
                m_error = true;
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
                    m_error = true;
                } else {
                    m_items.push_back(it->second(std::get<1>(i)));
                }
            } 
            // std::cout << "(" << std::get<0>(i) << ") - (" << std::get<1>(i) << ") - (" << std::get<2>(i) << ")" << std::endl;
        }
    }

    loggerManager::loggerManager() {
        m_root.reset(new Logger);
        m_root->addAppender(LogAppender::ptr(new StdoutLogAppender));

        m_loggers[m_root->m_name] = m_root; //指向了同一个成员
    }

    Logger::ptr loggerManager::getLogger(const std::string& name) {
        auto it = m_loggers.find(name);
        if (it !=  m_loggers.end()) {
            return it->second;
        }
        Logger::ptr logger(new Logger(name));
        logger->m_root = m_root;
        m_loggers[name] = logger;
        return logger;
    }

    struct LogAppenderDefine
    {
        int type = 0; //1 File, 2 Stdout
        LogLevel::Level level = LogLevel::UNKNOW;
        std::string formatter;
        std::string file;

        bool operator==(const LogAppenderDefine& oth) const {
            return type == oth.type
                && level == oth.level
                && formatter == oth.formatter
                && file == oth.file;
        }
    };

    struct LogDefine
    {
        std::string name;
        LogLevel::Level level = LogLevel::UNKNOW;
        std::string formatter;
        std::vector<LogAppenderDefine> appenders;
        Logger::ptr root;

        bool operator==(const LogDefine& oth) const {
            return name == oth.name
                && level == oth.level
                && formatter == oth.formatter
                && appenders == oth.appenders;
        }

        bool operator<(const LogDefine& oth) const {
            return name < oth.name;
        }
    };
    
    template<>
    class LexicalCast<std::string, std::set<LogDefine> > {
    public:
        std::set<LogDefine> operator() (const std::string& v) {
            YAML::Node node = YAML::Load(v);
            std::set<LogDefine> st;
            for (size_t i = 0; i < node.size(); ++i) {
                auto n = node[i];
                if (!n["name"].IsDefined()) {
                    std::cout << "log config error: name is null, " << n
                              << std::endl;
                    continue;
                }

                LogDefine ld;
                ld.name = n["name"].as<std::string>();
                ld.level = LogLevel::FromString(n["level"].IsDefined() ? n["level"].as<std::string>() : "");
                if (n["formatter"].IsDefined()) {
                    ld.formatter = n["formatter"].as<std::string>();
                }

                if (n["appenders"].IsDefined()) {
                    for (size_t x = 0; x < n["appenders"].size(); ++x) {
                        auto a = n["appenders"][x];
                        if (!a["type"].IsDefined()) {
                            std::cout << "log config error: appender type is null, " << a
                                      << std::endl;
                            continue;
                        }
                        std::string type = a["type"].as<std::string>();
                        LogAppenderDefine lad;
                        if (type == "FileLogAppender") {
                            lad.type = 1;
                            if (!a["file"].IsDefined()) {
                                std::cout << "log config error: fileappender file type is null, " << a
                                          << std::endl;
                                continue;
                            }
                            lad.file = a["file"].as<std::string>();
                            if (a["formatter"].IsDefined()) {
                                lad.formatter= a["formatter"].as<std::string>();
                            }
                        } else if(type == "StdoutLogAppender") {
                            lad.type = 2;
                        } else {
                            std::cout << "log config error: appender type is invalid, " << a
                                      << std::endl;
                            continue;
                        }
                        ld.appenders.push_back(lad);
                    }
                }
                st.insert(ld);
            }
            return st;
        }
    };

    template<>
    class LexicalCast<std::set<LogDefine>, std::string> {
    public:
        std::string operator() (const std::set<LogDefine>& v) {
            YAML::Node node;
            for (auto& i : v) {
                YAML::Node n;
                n["name"] = i.name;
                if (i.level == LogLevel::UNKNOW) {
                    n["level"] = LogLevel::ToString(i.level);
                }
                if (!i.formatter.empty()) {
                    n["formatter"] = i.formatter; //log的formtter
                }
                for (auto& a : i.appenders) {
                    YAML::Node na;
                    if (a.type == 1) {
                        na["type"] = "FileLogAppender";
                        na["file"] = a.file;
                    } else if (a.type == 2) {
                        na["type"] = "StdoutLogAppender";
                    }
                    if (a.level != LogLevel::UNKNOW) {
                        na["level"] = LogLevel::ToString(a.level);
                    }
                    if (!a.formatter.empty()) {
                        na["formatter"] = a.formatter;  //appender的formatter
                    }

                    n["appenders"].push_back(na);//把appenders变成sequence
                }
                node.push_back(n);
            }
            // for (auto& i : v) {
            //     node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
            // }
            std::stringstream ss;
            ss << node;
            return ss.str();
        }
    };
    
    
    dory::ConfigVar<std::set<LogDefine> >::ptr g_log_defines = //使用set去重，类型就需要定义<运算符
        dory::Config::Lookup("logs", std::set<LogDefine>(), "logs config");

    
    struct LogIniter {
        LogIniter() {
            g_log_defines->addListener(0xF1E231, [](const std::set<LogDefine>& old_value,
                    const std::set<LogDefine>& new_value){
                DORY_LOG_INFO(DORY_LOG_ROOT()) << "on_logger_conf_changed";
                for (auto& i : new_value) {
                    auto it = old_value.find(i);
                    dory::Logger::ptr logger;
                    if (it == old_value.end()) {
                        //新增logger
                        //logger.reset(new dory::Logger(i.name));
                        logger = DORY_LOG_NAME(i.name);

                    } else {
                        if (!(i == *it)) {
                            //修改的logger
                            logger = DORY_LOG_NAME(i.name);
                        }
                    }
                    logger->setLevel(i.level);
                    if (!i.formatter.empty()) {     //logger的formatter初始是空的
                        logger->setFormatter(i.formatter);
                    }
                    
                    logger->clearAppenders();
                    for (auto& a : i.appenders) {
                        dory::LogAppender::ptr ap;
                        if (a.type == 1) {
                            ap.reset(new FileLogAppender(a.file));
                        } else if (a.type == 2) {
                            ap.reset(new StdoutLogAppender);
                        }
                        ap->setLevel(a.level);
                        logger->addAppender(ap);
                    }
                }

                for (auto& i : old_value) {
                    auto it = new_value.find(i);
                    if (it == new_value.find(i)) {
                        //删除logger(不能真删除，当它没有，让它不能写即可)
                        auto logger = DORY_LOG_NAME(i.name);
                        logger->setLevel((LogLevel::Level)100);
                        logger->clearAppenders();
                    }
                }
            });
        }
    };
    
    //全局变量的构造在main函数之前执行，执行log的初始化
    static LogIniter __log_init;

    std::string loggerManager::toYamlString() {
        YAML::Node node;
        for (auto& i : m_loggers) {
            node.push_back(YAML::Load(i.second->toYamlString()));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }

    void loggerManager::init() {

    }    
}