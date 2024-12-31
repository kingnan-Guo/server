#include "Acceptor.h"

Acceptor::Acceptor(EventLoop* loop, const std::string &ip, uint16_t port): loop_(loop)
{
    


    // Socket serverScoket(createnonblocking()); 不能使用这个 ，因为 离开 TcpServer 后 会调用  ~Socket() ，会关闭fd，导致epoll监听失败。 所以使用下面的 
    serverScoket_ = new Socket(createnonblocking()); // 
    Socket * serverScoket = serverScoket_;

    InetAddress servaddr(ip, port);

    serverScoket->setreuseaddr(true);
    serverScoket->settcpnodelay(true);
    serverScoket->setreuseport(true);
    serverScoket->setkeepalive(true);

    serverScoket->bind(servaddr);
    serverScoket->listen();

    //---------------


    // 创建服务端用于监听的listenfd。
    int listenfd = serverScoket->fd();


    // // 创建epoll句柄（红黑树）。
    // Epoll ep;
    
    // // 让epoll监视listenfd的读事件，采用水平触发。
    // Channel* servChannel = new Channel(&ep, listenfd);

    // EventLoop eLoop;
    // Channel* servChannel = new Channel(eLoop.ep(), listenfd);// 这里new出来的对象没有释放，这个问题以后再解决。
    acceptChannel_ = new Channel(loop_, listenfd);


    // 指定回调函数 
    // acceptChannel_->setReadCallback(
    //     // 
    //     // 回调函数，当有新的客户端连接时，此函数会被调用。
    //     // @param &Channel::newConnection       Channel对象中的成员函数。
    //     // @param servChannel                   服务端用于监听的Channel对象。
    //     //  @param &serverScoket                newConnection 的传值参数。
    //     // 
    //     std::bind(&Channel::newConnection, acceptChannel_, serverScoket) // 绑定函数
    // );

    acceptChannel_->setReadCallback(
        // 
        // 回调函数，当有新的客户端连接时，此函数会被调用。
        // @param &Acceptor::newConnection       成员函数，也就是要调用的函数
        // @param this    或者使用 &Acceptor                       这是绑定到 newConnection 的隐式第一个参数，也就是 this 指针。 因为 newConnection 是成员函数，所以需要一个 Acceptor 类的对象或指针来调用该函数
        //  @param &serverScoket                newConnection 的传值参数。
        // 
        std::bind(&Acceptor::newConnection, this) // 绑定函数
    );



    // 启用 读事件
    acceptChannel_->enableReading();


}

Acceptor::~Acceptor()
{
    delete serverScoket_;
    delete acceptChannel_;
}






//==========================    =========================================
#include "Connection.h"
// 将 客户端连上来 和  连接的客户端的fd有事件 封装成 回调函数
// 处理新客户端连接请求
void Acceptor::newConnection(){
    InetAddress clientaddr;    
    Socket* clientSock = new Socket(serverScoket_->accept(clientaddr));

    printf ("accept client(fd=%d, ip=%s, port=%d ) ok.\n", clientSock->fd(), clientaddr.ip(), clientaddr.port());


    /*
    ///////////////////////////////////////////////////////////////

    // // 创建Channel对象，并添加到epoll中。
    // Channel* clientChannel = new Channel(ep_, clientSock->fd());
    Channel* clientChannel = new Channel(loop_, clientSock->fd());


    clientChannel->setReadCallback(
        std::bind(&Channel::onMessage, clientChannel)
    );

    // 客户端采用边缘触发
    clientChannel->useEt();
    clientChannel->enableReading();
    
    ///////////////////////////////////////////////////////////////
    */

   

   // 创建Connection对象，并添加到epoll中。
   // Connection* connection = new Connection(loop_, clientSock);

    // 回调TcpServer::newconnection() ,替换 上面 两行
    newConnectionCallback_(clientSock);


}; 



 // 设置处理新客户端 连接请求的。回调函数，
void Acceptor::setNewConnectionCallback(std::function<void(Socket*)> callback){
    newConnectionCallback_ = callback;
}



