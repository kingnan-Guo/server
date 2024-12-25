#ifndef __SYLAR_LOG_H
#define __SYLAR_LOG_H

// 定义命名空间
namespace log {
    class Logger {
    public:
        Logger();
        void log_info(const std::string& message);
        void log_error(const std::string& message);
    };
}


void LOGGER_MAIN();

#endif