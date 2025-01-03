#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <queue>
#include <sys/syscall.h>
#include <mutex>
#include <unistd.h>
#include <thread>
#include <condition_variable>
#include <functional>
#include <future>
#include <atomic>



class ThreadPool
{
private:
    std::vector<std::thread> threads_;              // 线程池 中的线程
    std::queue<std::function<void()>> tasksQueue_; // 任务队列 可以存放 事件
    std::mutex mutex_;                            // 任务队列同步的 互斥锁
    std::condition_variable condition_;            // 任务队列同步的 条件变量
    std::atomic_bool stop_;                      // 在析构函数中， 把 stop_的值设置为 true， 线程池内全部的线程 将退出
public:
    // 在构造函数中将 启动 threadNum 个 进程
    ThreadPool(size_t threadNum);
    // 在析构函数中 将停止线程
    ~ThreadPool();
    // 把任务添加到队列中 
    void addTask(std::function<void()> task); 
};

int ThreadPool_MAIN();


