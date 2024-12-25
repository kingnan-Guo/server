#include <iostream>
#include <stdio.h>
#include "log.h"


log::Logger::Logger() {
    // 可以在这里进行初始化操作
    std::cout << "Logger initialized." << std::endl;
}

// log 方法的实现
void log::Logger::log_info(const std::string& message) {
    std::cout << "log_info: " << message << std::endl;
}

void log::Logger::log_error(const std::string& message) {
    std::cout << "log_error: " << message << std::endl;
}


// ======================s
using namespace log;
void LOGGER_MAIN() {
    Logger logger =  Logger();
    logger.log_info("this is a info log");
    logger.log_error("this is a log_error");
}