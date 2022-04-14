#include <iostream>
#include <thread>
#include "../dory/log.h"
#include "../dory/util.h"

int main(int argc, char const *argv[])
{
    dory::Logger::ptr logger(new dory::Logger);
    logger->addAppender(dory::LogAppender::ptr(new dory::StdoutLogAppender));

    dory::FileLogAppender::ptr file_appender(new dory::FileLogAppender("log.txt"));
    dory::LogFormatter::ptr fmt(new dory::LogFormatter("%d%T%p%T%m%n"));
    file_appender->setFormatter(fmt);
    file_appender->setLevel(dory::LogLevel::ERROR);

    logger->addAppender(file_appender);

    // dory::LogEvent::ptr event(new dory::LogEvent(__FILE__, __LINE__, 0, dory::GetThreadId(), dory::GetFiberId(), time(0)));

    // logger->log(dory::LogLevel::DEBUG, event);
    DORY_LOG_INFO(logger) << "test macro";
    DORY_LOG_ERROR(logger) << "test macro error";

    DORY_LOG_FMT_ERROR(logger, "test macro fmt error %s", "aa");

    auto l = dory::LoggerMgr::GetInstance()->getLogger("xx");
    DORY_LOG_INFO(l) << "xxx";
    return 0;
}
