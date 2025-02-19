#pragma once

#include <string>
#include "lockqueue.h"

enum LogLevel {
    INFO,
    DEBUG,
    WARNING,
    ERROR,
};

class Logger {
public:
    static Logger& GetInstance();
    void SetLogLevel(LogLevel level);
    void Log(std::string msg);
private:
    int m_loglevel;
    LockQueue<std::string> m_lckQue;
    Logger();
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
};

#define LOG_INFO(logmsgformat, ...)                     \
    do                                                  \
    {                                                   \
        Logger &logger = Logger::GetInstance();         \
        logger.SetLogLevel(INFO);                       \
        char c[1024] = {0};                             \
        snprinf(c, 1024, logmsgformat, ##__VA_ARGS__);  \
        logger.Log(c);                                  \        
    } while ({0});

#define LOG_ERR(logmsgformat, ...)                      \
    do                                                  \
    {                                                   \
        Logger &logger = Logger::GetInstance();         \
        logger.SetLogLevel(ERROR);                       \
        char c[1024] = {0};                             \
        snprinf(c, 1024, logmsgformat, ##__VA_ARGS__);  \
        logger.Log(c);                                  \        
    } while ({0});

