#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t threadNum, const std::string& threadType):stop_(false), threadType_(threadType)
{
    for (size_t i = 0; i < threadNum; ++i)
    {
        threads_.emplace_back([this]{

            printf("create %s thread ( %d ) \r\n" , threadType_.c_str(), syscall(SYS_gettid));
            // printf("子线程 thread id:%lu\n",std::this_thread::get_id());

            while (stop_ == false)
            {
                // 用于存放 出队 的 元素
                std::function<void()> task;

                /// 锁 作用域 的 开始 
                {
                    //  作用是： 加锁，并且保证锁的作用域
                    std::unique_lock<std::mutex> lock(this->mutex_);

                    //  条件变量等待，直到有任务到来
                    // 在 wait 中，线程会释放锁并进入阻塞状态，直到条件变量被唤醒或其他线程发出通知
                    this->condition_.wait(lock, [this]{
                        // 如果 线程池停止 或者 有任务到来，则返回 true
                        return (this->stop_ == true) || (this->tasksQueue_.empty() == false);
                    });

                    // 如果 线程池停止 并且 任务队列 为空，则线程退出
                    if ((this->stop_ == true) && (this->tasksQueue_.empty() == true)){
                        return;
                    }

                    // 从任务队列中取出一个任务， 叫做 出队
                    task = std::move(this->tasksQueue_.front());// this->tasksQueue_.front() 返回队列的第一个元素，作为左值引用。; std::move 将其转换为右值引用 ; 通过右值引用，可以避免拷贝，直接转移对象的所有权或资源（比如动态内存）。


                    // 将队列的第一个元素移除
                    this->tasksQueue_.pop();

                } /// 锁作用域的结束。

                printf("%s execute task  %d \n", threadType_.c_str() ,syscall(SYS_gettid));
                // 执行出队的任务
                task();

            }
            
            
        });
    }
}



void ThreadPool::stop(){

    if(stop_){
        return;
    }
    stop_ = true;

    // 唤醒所有的线程
    // 的析构函数中调用 condition_.notify_all() 来唤醒所有线程是为了确保线程池中的所有工作线程在退出时能够正确处理并安全退出
    condition_.notify_all();

    // 等待所有的线程结束
    for(std::thread &th : threads_) {
        th.join();
    }
};




ThreadPool::~ThreadPool()
{
    stop();
}


// 把任务添加到队列中 
void ThreadPool::addTask(std::function<void()> task){


    {/// 锁 作用域 的 开始 
        // 作用是： 加锁，并且保证锁的作用域，如果在 加锁的过程中，发现，被  unique_lock 锁住还未释放，则当前线程会阻塞，直到锁被释放。
        std::lock_guard<std::mutex> lock(this->mutex_);

        // 把任务添加到队列中
        this->tasksQueue_.push(task);
    
    }/// 锁作用域的结束。


    /// 唤醒一个线程
    /// 唤醒的是某个正处于 std::condition_variable::wait 状态的线程，这个线程通常是一个工作线程（worker thread）。
    // 当一个新任务被添加到任务队列中（tasksQueue_），需要通知一个线程去处理这个任务。
    // 唤醒的线程会检查队列，取出任务并执行
    
    this->condition_.notify_one();
}; 


// 获取线程池中的线程数量
size_t ThreadPool::size(){
    return threads_.size();
}















////////////////////////////////////////////////////////////////////////////


// Demo 1 STRART ===========================
void show(int no, const std::string &name){
    printf("show 第 %d 个%s。\n", no, name.c_str());
}

void test()
{
    printf("test。\n");
}


//      ps -ef | grep Reactor
//      ps -T -p  21830
// int ThreadPool_MAIN1(){
//     ThreadPool threadPool(4);

//     std::string name = "kingnan";

    
//     threadPool.addTask(std::bind(show, 1, name));
//     sleep(1);
//     threadPool.addTask(std::bind(test));

//     threadPool.addTask(std::bind([]{
//         printf("lambda。\n");
//     }));
//     sleep(100);
//     return 0;
// }


void Demo1(){
    ThreadPool threadPool(4, "Demo1");

    std::string name = "kingnan";

    
    threadPool.addTask(std::bind(show, 1, name));
    sleep(1);
    threadPool.addTask(std::bind(test));

    threadPool.addTask(std::bind([]{
        printf("lambda。\n");
    }));
    sleep(100);
}

// Demo 1 END





// Demo 2 STRART ===========================
//  会出现野指针，因为 data2 在 5s 后被析构了

class Demo2Data{
    public:
        void show(){
            printf("Data show。\n");
        }
    ~Demo2Data(){
        printf("Data 析构。\n");
    }
};


void  fun2(Demo2Data* data2)
{
    sleep(5);
    data2->show();
}


void Demo2(){
    ThreadPool threadPool(2, "Demo2");


    {// 当前线程 中 
        Demo2Data* data2 = new Demo2Data();
        threadPool.addTask(
            std::bind(fun2,data2)
        );
        delete data2;// 会调用 析构函数， 5s 后 会调用 data2 的析构函数
    }
    sleep(10);
}

// Demo 2 END
#include <memory>

// Demo 3 STRART ===========================
// 使用智能指针

class Data3{
    public:
        void show(){
            printf("Data3 show。\n");
        }
    ~Data3(){
        printf("Data3 析构。\n");
    }
};


void  fun3(std::shared_ptr<Data3> data3)
{
    sleep(5);
    data3->show();
}

/**
 * 执行的结果是 
 * 
 * Data3 show。
 * Data3 析构。
 * 
 * 在 5s 后，data3 被析构了，但是 show 函数仍然可以调用，说明 show 函数中的 data3 是一个智能指针，而不是裸指针
*/
void Demo3(){
    ThreadPool threadPool(2, "Demo3");


    {// 当前线程 中 
        std::shared_ptr<Data3> data3(new Data3);
        threadPool.addTask(
            std::bind(fun3, data3)
        );
    }
    sleep(10);
}
// Demo 3 END

int ThreadPool_MAIN(){
    // demo1();
    // Demo2();
    Demo3();


    return 0;
}

