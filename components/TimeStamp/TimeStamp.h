#pragma once
#include <iostream>
#include <string>
#include <unistd.h>
#include <time.h>


class TimeStamp
{
    private:
        time_t secsinceepoch_;  // 整数表示 的时间，时间戳
    public:
        TimeStamp();                        // 用当前时间你初始化 对象
        TimeStamp(int64_t secsinceepoch_);  // 用 传入的 时间 初始化 对象

        static TimeStamp Now();  // 获取当前时间
        time_t toInt() const;  // 转换为 时间戳
        std::string toString() const;  // 转换为 字符串 : 2021-01-01 00:00:00
        ~TimeStamp();
};

void TIMESTAMP_MAIN();