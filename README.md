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
     2、在 TcpServer 中添加  closeConnection errorConnection (本来是要在这两个函数中 close(fd()), 但是 在 connection 中 会关闭 socket ，在 socket的析构函数中会 close(fd), 所以不需要在 TcpServer 中 close(fd) )， 并且在 newconnection 把他们 绑定到 Connection 的 回调函数 中，

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
     














 # =========================================================================


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