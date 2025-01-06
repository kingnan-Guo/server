#include "TimeStamp.h"




TimeStamp::TimeStamp()
{
    secsinceepoch_ = time(0);
}

TimeStamp::TimeStamp(int64_t secsinceepoch_): secsinceepoch_(secsinceepoch_)
{
}

TimeStamp TimeStamp::Now(){
    return TimeStamp();// 返回当前时间
}


// 转换为 时间戳
time_t TimeStamp::toInt() const {
    return secsinceepoch_;
}


std::string TimeStamp::toString() const{
    char buf[32] = {0};
    // struct tm tm = *localtime(&secsinceepoch_);
    // strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    // return buf;

    tm *tm_time = localtime(&secsinceepoch_);
    snprintf(buf, 20, "%4d-%02d-%02d %02d:%02d:%02d",
             tm_time->tm_year + 1900,
             tm_time->tm_mon + 1,
             tm_time->tm_mday,
             tm_time->tm_hour,
             tm_time->tm_min,
             tm_time->tm_sec);
    return buf;

}




TimeStamp::~TimeStamp()
{
}


void TIMESTAMP_MAIN(){
    TimeStamp t;
    std::cout << "t.toInt() =" << t.toInt() << std::endl;
    std::cout << "t.toString() =" << t.toString() << std::endl;

    sleep(1);

    std::cout << "TimeStamp::Now().toInt() =" << TimeStamp::Now().toInt() << std::endl;
    std::cout << "TimeStamp::Now().toString() =" << TimeStamp::Now().toString() << std::endl;


}

