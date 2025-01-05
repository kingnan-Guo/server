# 更新进度

2024/12/28   21: 47
添加 epoll类 ： Reactor_05_add_epoll_class

2024/12/28   23: 43
channel 登场 ： Reactor_06_add_channel_1


2024/12/29   22: 00
channel 登场 3： Reactor_07_add_channel_3

2024/12/29   22: 00
第一次回调： Reactor_08_callback


2024/12/29   23: 12
增加EventLoop类： Reactor_09_add_EventLoop_class_1


2024/12/30   00: 59
优化 EventLoop类： Reactor_10_add_EventLoop_class_2  （待完成）



2024/12/30   01: 02
TcpServer 类： Reactor_11_add_TcpServer_class



2024/12/31   09: 54
Acceptor 类： Reactor_12_add_Acceptor_class
    把 TcpServer 中 创建的 链接 封装为 Acceptor


2024/12/31   15: 13
Connection 类： Reactor_13_add_Connection_class
     封装 Connection 放到  Channel 中 替换 ： 为新客户端连接准备读事件


2024/12/31   21: 57
优化 Acceptor ： Reactor_14_Optimize_Accept_1
     优化 Acceptor , 把 Channel 的 newConnection 回调函数 放到 Acceptor 中， 老师大概的意思是 因为， 在 Acceptor 中  创建服务端用于监听的listenfd。
                     在传统的网络服务 链接中 由 监听的 fd 产生 客户端 连接的 fd ； Acceptor 封装了监听的fd 监听的 Channel， Connection类的对象由 Acceptor 类 创建， 也就是说 创建。Connection的代码应该 写在 Acceptor 类中， 而不是在 TcpServer 中。



2024/12/31   22: 00
优化 Acceptor ： Reactor_15_Optimize_Accept_2
     8-7 在 Acceptor 类中回调TcpServer类的成员函数，
        在 TcpServer 中 创建 Connection 对象，使用 回调函数的方式去实现
        // Acceptor 增加一个 成员函数 用于 回调函数 ，  设置处理新客户端 连接请求的。回调函数， 回调函数的参数是 Socket* ， 也就是 新连接的客户端的 fd， 
        // 在 TcpServer 中 创建 Connection 对象，使用 回调函数的方式去实现， 在 TcpServer 的 构造函数中 设置 回调函数， 设置 回调函数  acceptor_->setNewConnectionCallback


2025/01/01   22: 41
用map容器管理Connection对象 ： Reactor_16_add_Map
     1、 首先在 socket.h 中增加一个  ip port 的结构体， 用来保存客户端的 ip 和 port
     2、 在 Connection 类 中 成员函数 ， 可以 获取 客户端的 ip 和 port
     3、 在 TcpServer 类中 打印 ip 和端口
     4、 在 TcpServer 类中 增加 一个 map 容器， 用来保存 Connection 对象，   使用 fd 作为 map 的 key 连接的 connection 作为 value
     5、在 析构函数中 遍历 map 容器， 释放 Connection 对象的内存



2025/01/01   23: 00
在Channel类 中 回调 Connection类 的 成员函数 ： Reactor_17_add_Channel_closeCallBack_errorCallBack
     1、由于 在 tcpServer 中 添加了map 容器，把 全部的 connection 对象都保存起来， 所以 在 析构函数中 会释放全部 connection 对象， 但是这样做存在一个问题，不能只在 析构函数中释放全部的 connection 对象
     2、如果某一个 客户端断开了 或者连接 出错了 ， TcpServer 不知道的 ， 但是 Channel 类 知道，那么 需要 立即释放这个 connection 对象， 所以需要在 Channel 类中 回调 Connection类 的 成员函数 ， 释放 Connection 对象
     3、 在 Channel 的 事件处理函数中， 如果 连接断开 或者 出错 都会执行相应的 代码， 但是 不能在 Channel 类中直接 ，因为 Connection 类中 没有在 Channel 中， 属于 TcpServe类
     4、所以需要在 Channel 类中 回调 Connection类 的 成员函数 ， 释放 Connection 对象，  在 Connection 类中 增加 一个 成员函数， 释放 Connection 对象，  在 Channel 类中 回调 Connection类 的 成员函数 ， 释放 Connection 对象，所以使用 回调函数， 回调函数的代码只能写在 TcpServer 中
     5、 Channel 类是 Connection 类的底层 类， Connection 是 TcpServer 的底层类， Tcp 连接 断开的时候， 可以在 Channel 类中 回调 Connection 类的 成员 函数， 通过 Connection 类的成员函数  再回调 TcpServer 类的成员函数



2025/01/02   12: 43
在  Connection 类中 回调 TcpServer 类 的 成员函数 ： Reactor_18_add_Connection_closeCallBack_errorCallBack
     1、Connection 中 添加  closeCallBack 和 errorCallBack 的 成员函数，  
     2、在 TcpServer 中添加  closeConnection errorConnection (本来是要在这两个函数中 close(fd()), 但是 在 connection 中 会关闭 socket ，在 socket的析构函数中会 close(fd), 所以不需要在 TcpServer 中 close(fd) )， 并且在 newconnection 把他们 绑定到 Connection 的 回调函数 中
     3、最终的目的是要在 TcpServer 中创建连接， 也要在 TcpServer 中关闭连接， 所以需要把 closeConnection errorConnection 绑定到 Connection 的 回调函数 中， 在 Connection 中 回调 TcpServer 的 closeConnection errorConnection 函数， 在 TcpServer 中 close(fd)


2025/01/02   12：44
网络编程缓冲区 ：分包 粘包
     解决方法
          添加 一个  header 包含 数据包的长度， 服务器端先读取 header， 然后根据 header 中的长度去读取数据包， 这样就可以解决分包和粘包的问题



2025/01/02   14: 00
封装缓冲区 buffer 类： Reactor_19_add_Buffer_class
     1、添加 Buffer 类， 用来保存数据, 
     2、在 Connection 类中   Buffer 类型的成员变量， 作为 接收数据的缓冲区 inputBuffer_ 和 发送数据的缓冲区  outputBuffer_
     3、把 Channel 中的 onmessage  切换到 Connection 中
     4、在  Connection::Connection 中修改 接收的 回调函数绑定
     5、在 void Connection::onMessage() 把接收的数据放到 缓存区 里
     6、发送时：  把 发送缓存区的 数据发送出去

     修改bug *******************************
     socket 中  储存 ip 和端口，但是 存储 客户端连上来的 ip和端口 卸载 accept 中 ，客户端连接 不会调用 accept 所以无法 拿到  客户端 的 ip和端口， 所以 在  void Acceptor::newConnection() 中 创建完  clientSock 之后 使用     clientSock->setIpPort(clientaddr.ip(), clientaddr.port()); 设置 ip 和端口



2025/01/02   20: 00
使用接收缓存区 inputBuffer_ 保存接收的数据 ： Reactor_20_use_inputBuffer_Receive_data
     1、把 onmessage 切换到   void TcpServer::onMessage 中，在这里处理 业务曾逻辑
     2、当前定制 添加 4 个字节的 头er， 用来保存数据包的长度， 服务器端先读取 header， 然后根据 header 中的长度去读取数据包， 这样就可以解决分包和粘包的问题
     3、 所以 在 void Connection::onMessage() 收到的数据中 处理 头信息， 通过回调函数 调用 TcpServer::onMessage


2025/01/02   21: 00
使用发送缓存区 outPutBuffer_ 保存发送的数据 ： Reactor_21_use_outPutBuffer_Send_data
     有bug ****************************
     就是在 水平触发的时候 不会多次执行 打印  EPOLLOUT 不知道为什么？？？？？？


2025/01/01   23: 00
优化回调函数： Reactor_22_optimize_callback_function
     1、实现 setSendCompletionCallback 数据发送完成的 回调， Connection 调用 void TcpServer::sendCompletionCallback 
     2、实现 超时 回调， epoll 中的 超时 ，在 Eventloop 中判断 channels 然后如果超时 ，调取 TcpServer 中的超时 回调函数


2025/01/03   23:45
实现回显服务器EchoServer ： Reactor_23_EchoServer
     1、在 tcpServer 中 创建。回调函数
     2、EchoServer 调取这些 回调函数， EchoServer 中实现业务逻辑
     
     有 bug *****************************
     启动后， 第一次 客户端有值传上来 会收不到， 第二次再连上来 就可以 



2025/01/03   12: 57
优化Buffer 类： Reactor_24_optimize_Buffer_class
     1、 std::function<void(Connection*, std::string)> onMessageCallBack_;  改成 std::function<void(Connection*, std::string&)> onMessageCallBack_ ， 值引用的方式传递
     2、Buffer 类中 添加  处理数据 添加 报头文件

     



     修改bug *******************************
     Reactor_23_EchoServer 之前这里的 std::function<void(Connection*, std::string)> onMessageCallBack_;  部分 值传递 ，部分是值引用 ，所以在  Reactor_24_optimize_Buffer_class 中 解决 此 bug



     # 这个版本可以用来开发 业务了 #


2025/01/03   23: 00
封装线程池 ThreadPool ： Reactor_25_add_ThreadPool_class
     

2025/01/04   00:18
多线程的主从 Reactor 模型 ： Reactor_26_MultiThread_Reactor_Model
     1、 运行多个事件循环， 主从时间循环运行在主线程中，从事件循环运行 在线程池中， 
     2、 主线程负责接收客户端的连接， 把 Connection 连接交给从线程池中的线程处理， 从线程池中的线程负责处理业务逻辑
     3、在线程池中运行事件循环， 把 EventLoop 的 run（） 作为任务 添加给 线程池， 线程池中的线程运行这个任务， 就可以运行事件循环了
     4、在 EchoServer 中 传入 线程数
     5、处理 客户端的连接请求 ， 把 Connection 交给 线程池中的线程处理， 在线程池中的线程中处理业务逻辑；  Connection* connection = new Connection(subLoop_[clinetSocket->fd() % threadNum_], clinetSocket);




2025/01/04   11:20
增加工作线程： Reactor_27_add_work_thread
     1、Acceptor 运行在 Reactor 主进程中
     2、Connection 运行在 Reactor 从线程 （线程池）中
     3、一个从 Reactor 负责多个 Connection，每个Connection 的 工作内容包括 IO 计算 （处理业务）， IO不会阻塞事件循环，但是，计算可能会阻塞事件循环； 如果 计算阻塞了事件循，那么 在同一个 Reactor 中的全部 Connection 将会 被阻塞; 所以，把计算放到工作线程中运行;工作线程不会阻塞事件循环，因为工作线程是异步的，不会阻塞事件循环 ， 从 Reactor  Connection  只负责 IO

     4、 业务处理在 EchoServer 中， 所以 工作线程池也要在 EchoServer 中创建， 在处理任务的时候要将 处理任务 传给 threadPool_，逻辑 在工作线程中运行


     我的问题是  Connection 和 工作线程都是 从线程，为什么 Connection 会导致阻塞，但是  工作线程 不会阻塞？？？？？？？


     一个进程可以有多个线程，它们共享进程的资源（如内存），但彼此独立执行。



2025/01/04   13: 50
在多线程中如何管理资源： Reactor_28_manage_resource_in_multiThread_Demo

     
     当前问题： 如果 资源在 多个线程中 使用，如何保证资源是有效的，使用完成后谁来释放 资源 （谁来 调用析构函数）

     1、Connection 在 TcpServer 中创建， 主事件循环中创建 ，创建好之后 给到 从事件循环，可以说在主进程中创建，在 io 线程中 使用
     2、如果收到了 客户端的请求报文，也就是有业务需要处理，那么 Connection 对象 将会作为参数 传到 工作线程 中，
     3、但是如果 工作线程中 需要长时间的处理，而这时， 主事件循环中的 Connection 对象 可能  已经被  运行在 IO 线程的 closeContent  释放了，那么 工作线程中 就会使用一个已经被释放的对象，导致程序崩溃
     ```C
          Connection* connection = new Connection(subLoop_[clinetSocket->fd() % threadNum_], clinetSocket); // 这里new 出的对象没有释放
     ```


     添加一个 demo 使用 智能指针 在 ThreadPool.cpp
          Demo1 触发 野指针的现象
          Demo2 使用智能指针 解决 野指针的问题




2025/01/04   14: 30
使用智能shader_ptr指针管理资源：   Reactor_29_use_smart_ptr_to_manage_resource
     1、定义 定义别名 using spConnection = std::shared_ptr<Connection>; 把 普通的 指针 Connection* 改为 智能指针
     2、会报错，是因为  std::function<void(spConnection)> closeCallBack_; 在使用的时候 Connection 还未定义， 所以在 using 前先定义  class Connection; 
     3、 Connection.cpp 中的callBack 函数 传入的  this 会报错，因为 this是 普通指针，所以 要改成智能指针， C++ 提供一个方法 把 class Connection 继承 std::enable_shared_from_this<Connection>，然后 this 指针 改为  shared_from_this() 就可以了 ,    例如： sendCompletionCallback_(this); 改为   sendCompletionCallback_(shared_from_this());
     4、TcpServer.h  的 Connection*  改为 spConnection ;
     5、TcpServer.cpp 中的  Connection*  改为 spConnection;
           a、Connection* connection = new Connection(subLoop_[clinetSocket->fd() % threadNum_], clinetSocket);  改为  spConnection connection(new Connection(subLoop_[clinetSocket->fd() % threadNum_], clinetSocket));
          b、void TcpServer::closeConnection(Connection* connection) 改为 void TcpServer::closeConnection(spConnection connection)
          c、去掉 for (auto &fd:connections_)  {      delete fd.second;  }

     6、 客户端断开连接的时候 connection 不会被 析构， 只有在处理完成后才会被析构 
     7、 void TcpServer::closeConnection(spConnection connection) 中的 close(connection->fd());   关闭 客户端  fd ;  也要注掉 
     8、 void TcpServer::errorConnection(spConnection connection) 中的 close(connection->fd());   关闭 客户端  fd ;  也要注掉

     9、 如果tcpSever 断开，那么不再需要 在工作线程中调用 send ，取消关注 channel 全部的事件，从epoll 中删除；并且从事件循环中删除
     
     10、当前是 水平触发 // clientChannel_->useEt(); // 暂时 注掉 变成水平触发， 这样 EPOLLOUT 会触发很多次 之后会改回来



2025/01/04   17: 30
继续使用只能指针之 用 uniqueue_ptr 管理自己的资源：   Reactor_30_use_unique_ptr_to_manage_resource
     1、要把 裸指针 改为 std::unique_ptr
     2、
     
   
2025/01/05   21: 30
用eventfd实现事件通知：  Reactor_31_use_eventfd_to_notify_event_demo
     1、通知线程的方法： 条件变量、信号、socket 、管道、 eventFd
     2、事件循环阻塞在 epoll_wait() 函数， 条件变量、信号有自己的等待函数， 不适合用于通知事件循环
     3、socket 、管道、 eventFd 都是 fd， 都可以加入 epoll，都可以用于通知事件循环，但是 socket 、管道、 eventFd 都需要创建一个线程，而 eventFd 只需要创建一个文件描述符，所以选择 eventFd
     4、在同一个进程中 不可能同时调用两个等待函数
     5、

2025/01/05   22: 30
异步唤醒事件循环（上）: Reactor_31_async_wake_up_event_loop_one
改造 第一步将 智能指针 的 const std::unique_ptr<EventLoop>& loop_; 改回为 普通指针 EventLoop *loop_ = nullptr; 

     1、 关于 connection.cpp
          a、在 connection 中 的 Connection::send  在 工作线程中，
          b、在 connection 中  Connection::writeCallBack 在 IO 线程中
          c、但是 他们同时操作 发送缓冲区 outputBuffer_ 
          d、工作线程处理完数据后 ，如果要发送数据 ，可以把 发送数据的操作 交给 IO 线程，这样就不需要给 发送缓冲区 outputBuffer_ 加锁了
          e、如果计算任务 非常 小 ，那么也可以放到 IO 线程中，如果计算任务 非常大，那么就放到 工作线程中
          f、实现方法:
             1、给每个 从 Reactor 线程 添加 一个任务队列，有多少个 IO 线程就有多少个任务队列
             2、工作线程处理完数据后，把 发送数据的操作 放到 IO 线程的任务队列中，如果 connection 属于 其中一个IO线程 （也是 EventLoop）， 那么就把任务放到 当前 IO 线程 的 任务队列； 
             3、工作线程 把 数据发送给 任务队列后 ，唤醒 IO 线程，让 IO 线程执行任务，
             4、 IO 线程 阻塞在 事件循环 的Evenloop  函数中， 使用 eventFd 唤醒事件循环，让事件循环执行任务队列中的任务
     2、 如果计算任务 非常 小 ，那么也可以放到 IO 线程中，如果计算任务 非常大，那么就放到 工作线程中
          a、修改代码支持 没有工作线程的情况
          


          






# 笔记
1、智能指针的使用建议
     a、 如果生命周期难以 确定，则 使用 shard_ptr 来管理
     b、 类 自己所拥有的资源用 unique_ptr 来管理， 在类被销毁的时候， 将会自动释放资源
     c、不属于 自己、 但会使用的 资源，采用 unique_ptr& 或 shared_ptr 来管理会很麻烦，不易阅读， 可能有一系列问题，依旧采用 裸指针来管理















# ========================================


# server
Reactor server 

高并发服务器 test





==================================

# 创建 密钥
ssh-keygen -t rsa -b 4096 -C "373734199@qq.com"

SHA256:i12YdJPa7XY1OTf8Nx59f0UPSPcG85j0wwnrZvrMW6c


# 添加 SSH 密钥到 SSH 代理
eval "$(ssh-agent -s)"

ssh-add ~/.ssh/id_rsa


# 获取公钥内容
cat ~/.ssh/id_rsa.pub  得到

ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAACAQDMqNvlbGXKLuDGt6T6lnC1nmPHR5D9lJ4vwObwrSyR0sGx3BylH8M8bOrU52IG7T4m7AbjRoFj3QWuBLyHRJ9fxJmlTc+0phGRYsiHIYcBeCUqiSD9ZKb/1sZGCxJtHGnfz529gTIEAANs859a1yYs73U/zU0Vw11gpsImkgspwlM8zqUpiKieiJGz5A2ONThkkQwWtD7gqHoeyrGRXYl67dj+cyWlRYF9AmXdKp7VCKb5gkoGdjHwCtRBMh+xBWTGxYgbVvLw4l6Byfp46jXLwKCQ3N90UKBRbW9eey3OR83n7zU+Mp13i0qS3DDKxhz9q5KsVtikj9JEALMpFq89ToSTKTwp2v/cGWIBIbRS6KTWdzFdw9iqCDYvPDzuUbogo27a90wKdJlM2JUxX6O5ztOFS7wDTNABIoKAOuG7S37P4PA8Dh97CPpijHZ5ouAA+fyvhmtR6uhIEGsX0hnryTR26KoSxknmybfqeVZrFdPXnnl3ha0cxlIthH7ckJW6T2YZ2kcue7rcBDPijxzOWycvRinNmupbQPG8MKaebTYO1W4AYW8fXRh3NQ+fvEkTaojnlVsXqOnYHdysxCxdzmpbAlBVrSFxrSeKxIe3d/XlvtKlCadWdpLIJcjXw+DJvVck6OkzrOsO9cpMkWOYfla/pouxGbD9n78AlPQZzw== 373734199@qq.com


# 测试

ssh -T git@github.com




# 拉代码
git clone git@github.com:kingnan-Guo/server.git

git clone git@github.com:kingnan-Guo/server.git ReactorClient

# 检查分支 
git branch -r

# 切换分支 到 sylar
git checkout sylar

git checkout main

git checkout origin/Reactor







# 提交代码
git add .
git commit -m "first commit"
git push -u origin master
git push origin sylar






 # 上传前要登录
 
git config --global user.name "kingnan-Guo"
git config --global user.email "373734199@qq.com"

<!-- git remote add origin git@github.com:zhangyue/leetcode.git

git push -u origin master -->