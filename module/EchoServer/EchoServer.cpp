#include "EchoServer.h"
#include "Router.h"

/**
 * 
 * @EchoServer 构造函数
 * 
 * @ip 服务器ip地址
 * @port 服务器端口号
 * @subThreadNum 子线程数量
 * @workThreadNum 工作线程数量
 * 
 * threadPool_(workThreadNum)  创建工作线程池, 有了线程池 之后 把 业务处理 放到线程池中执行
*/
EchoServer::EchoServer(const std::string &ip, const uint16_t port, int subThreadNum, int workThreadNum):tcpServer_(ip, port, subThreadNum), threadPool_(workThreadNum, "WORKS")
{

    // 设置回调函数
    tcpServer_.setNewConnectionCallBack(
        std::bind(&EchoServer::HandleNewConnection, this, std::placeholders::_1)
    );
    tcpServer_.setCloseConnectionCallBack(
        std::bind(&EchoServer::HandleClose, this, std::placeholders::_1)
    );
    tcpServer_.setErrorConnectionCallBack(
        std::bind(&EchoServer::HandleError, this, std::placeholders::_1)
    );
    tcpServer_.setOnMessageCallBack(
        std::bind(&EchoServer::HandleMessage, this, std::placeholders::_1, std::placeholders::_2)
    );
    tcpServer_.setSendCompleteCallBack(
        std::bind(&EchoServer::HandleSendComplete, this, std::placeholders::_1)
    );
    tcpServer_.setTimeOutCallBack(
        std::bind(&EchoServer::HandleTimeOut, this, std::placeholders::_1)
    );

    // HandleRemove
    tcpServer_.setRemoveConnectionCallBack(
        std::bind(&EchoServer::HandleRemove, this, std::placeholders::_1)
    );


}

EchoServer::~EchoServer()
{
}




void EchoServer::Start(){
    tcpServer_.start();
};

 // 处理新客户端连接请求，在TcpServer类中回调此函数。
void EchoServer::HandleNewConnection(spConnection connection){
    // std::cout << "New Connection Come in." << std::endl;
    // std::cout << "New Connection Come in. HandleNewConnection   thread id : " << syscall(SYS_gettid) << std::endl;
};

                // 关闭客户端的连接，在TcpServer类中回调此函数。 
void EchoServer::HandleClose(spConnection connection){
    std::cout << "EchoServer conn closed." << std::endl;
};

 // 客户端的连接错误，在TcpServer类中回调此函数。
void EchoServer::HandleError(spConnection connection){
    std::cout << "EchoServer conn error." << std::endl;
};     

// 处理客户端的请求报文，在TcpServer类中回调此函数。 这里是接收数据，
void EchoServer::HandleMessage(spConnection connection, std::string& message){


    // std::cout << "HandleMessage   thread id : " << syscall(SYS_gettid) << " message :\n" << message << std::endl;
        // 在这里，将经过若干步骤的运算。
        // std::cout << "HandleMessage   data: " << message << std::endl;
        // 将运算结果返回给客户端


        // / 自定义协议，报文格式为：报文长度（4字节）+报文内容。
        // int len = message.size();                   // 计算回应报文的大小。
        // std::string tmpbuf((char*)&len,4);  // 把报文头部填充到回应报文中。
        // tmpbuf.append(message);             // 把报文内容填充到回应报文中。
        // //send(connection->fd(),tmpbuf.data(),tmpbuf.size(),0);   // 把临时缓冲区中的数据直接send()出去。
        // connection->send(tmpbuf.data(),tmpbuf.size());   // 把临时缓冲区中的数据直接send()出去。

        // connection->send(message.data(),message.size());







        Router router;

        router.GET("/hello", [this, &connection](const HttpRequestPtr& req, const HttpResponsePtr& resp) {
            // 先发送响应头，告诉客户端请求已收到，正在处理中
            // resp->SetHeader("Content-Type", "application/json");
            // // resp->SetHeader("X-Status", "Processing");
            // // 保证连接保持活跃
            // // resp->SetHeader("Connection", "keep-alive");
            // // 发送响应头（客户端接收到这个后可以开始处理）
            // std::ostringstream initial_response;
            // initial_response << "HTTP/1.1 " << resp->status_code << " " << resp->GetStatusMessage() << "\r\n";
            // for (const auto& header : resp->headers) {
            //     initial_response << header.first << ": " << header.second << "\r\n";
            // }
            // initial_response << "\r\n";  // 空行表示头部结束
            // // 发送响应头（客户端接收到这个后可以开始处理）
            // std::string responseStr = initial_response.str();
            // std::cout << responseStr;

            // PreparationSend(connection, responseStr);
            // // std::this_thread::sleep_for(std::chrono::seconds(4));
            // // sleep(4);
            // std::string jsonResponse = R"({data: {info: "kingan"}})";
            // PreparationSend(connection, jsonResponse);





            std::ostringstream response;
            response << "HTTP/1.1 " << resp->status_code << " " << resp->GetStatusMessage() << "\r\n";
            for (const auto& header : resp->headers) {
                response << header.first << ": " << header.second << "\r\n";
            }
            response << "\r\n"; // 结束响应头

            std::string responseStr = response.str();
            PreparationSend(connection, responseStr);

            // 然后发送数据
            // std::string jsonResponse = "{\"message\":\"Hello, world!\"}";
            // PreparationSend(connection, jsonResponse);


            // 然后发送数据
            std::string jsonResponse; // 初始化为空字符串
            jsonResponse += "{\n";    // JSON 的起始大括号
            for (const auto& param : req->queryParams) {
                std::cout << "   " << param.first << ": " << param.second << std::endl;
                // 拼接 JSON 格式字符串
                jsonResponse += "  \"" + param.first + "\": \"" + param.second + "\",\n";
            }
            // 删除最后一个逗号并添加结束大括号
            if (!req->queryParams.empty()) {
                jsonResponse.pop_back(); // 删除最后一个逗号
                jsonResponse.pop_back(); // 删除最后一个换行符
            }
            jsonResponse += "\n}";

            // printf("JSON Response: %s\n", jsonResponse.c_str());
            // 发送 JSON 数据
            PreparationSend(connection, jsonResponse);


        
        });



        router.POST("/api", [this, &connection](const HttpRequestPtr& req, const HttpResponsePtr& resp) {
            // 先发送响应头，告诉客户端请求已收到，正在处理中
            // resp->SetHeader("Content-Type", "application/json");
            // resp->SetHeader("X-Status", "Processing");
            // // 保证连接保持活跃
            // // resp->SetHeader("Connection", "keep-alive");
            // // 发送响应头（客户端接收到这个后可以开始处理）
            // std::ostringstream initial_response;
            // initial_response << "HTTP/1.1 " << resp->status_code << " " << resp->GetStatusMessage() << "\r\n";
            // for (const auto& header : resp->headers) {
            //     initial_response << header.first << ": " << header.second << "\r\n";
            // }
            // initial_response << "\r\n";  // 空行表示头部结束
            // // 发送响应头（客户端接收到这个后可以开始处理）
            // std::string responseStr = initial_response.str();
            // std::cout << responseStr;

            // PreparationSend(connection, responseStr);

            // std::string jsonResponse = "123";

            // PreparationSend(connection, jsonResponse);
        


            std::ostringstream response;
            response << "HTTP/1.1 " << resp->status_code << " " << resp->GetStatusMessage() << "\r\n";
            for (const auto& header : resp->headers) {
                response << header.first << ": " << header.second << "\r\n";
            }
            response << "\r\n"; // 结束响应头

            std::string responseStr = response.str();
            PreparationSend(connection, responseStr);

            std::string jsonResponse = "{\"message\":\"Hello, world!\"}";
            PreparationSend(connection, jsonResponse);


        });

    




        HttpRequest request;
        HttpRequestPtr req = std::make_shared<HttpRequest>();
        HttpResponsePtr resp = std::make_shared<HttpResponse>();
        if (req->parse(message)) {
            // req->printRequest();
            router.GetRoute(req, resp);

        } else {
            std::cerr << "Failed to parse request." << std::endl;

        }


        // // 这里是将 收到的数据 又 发送回去，正常业务中 不是这样的，这里只是为了测试
        // // 判断 工作线程池的大小
        // if(threadPool_.size() == 0){ // 没有工作线程，那么直接调用 IO 线程
        //     OnMessage(connection, message);
        // }
        // else{ // 有工作线程，那么把业务放到 线程池 的任务队列中
        //     // 把业务放到 线程池 的任务队列中
        //     threadPool_.addTask(
        //         std::bind(&EchoServer::OnMessage, this, connection, message)
        //     );
        // }

};


// 
void EchoServer::PreparationSend(spConnection connection, std::string& message){
    if(threadPool_.size() == 0){ // 没有工作线程，那么直接调用 IO 线程
        OnMessage(connection, message);
    }
    else{ // 有工作线程，那么把业务放到 线程池 的任务队列中
        // 把业务放到 线程池 的任务队列中
        threadPool_.addTask(
            std::bind(&EchoServer::OnMessage, this, connection, message)
        );
    }
}


void EchoServer::OnMessage(spConnection connection, std::string& message){
    // sleep(2);
    // std::cout << "OnMessage 处理业务完成  要调用 connection " << message << std::endl;
    // message = " server echo " + message;
    connection->send(message.data(), message.size());
    
}





// 数据发送完成后，在TcpServer类中回调此函数。
void EchoServer::HandleSendComplete(spConnection connection){
    std::cout << "Message send complete." << std::endl;


    tcpServer_.closeEventLoopConnection(connection);
};

// epoll_wait()超时，在TcpServer类中回调此函数。   
void EchoServer::HandleTimeOut(EventLoop *loop){
    std::cout << "EchoServer timeout." << std::endl;
};


// 
void EchoServer::Stop(){
    // 停止 工作线程； 工作线程 threadPool_ 是 EchoServer 的成员变量，
    threadPool_.stop();
    printf("工作线程  stop\n");
    // 停止 IO 线程 （事件循环）
    tcpServer_.stop();
}



void EchoServer::HandleRemove(int fd){
    // std::cout << "HandleRemove. fd ="<< fd << std::endl;
    // 从 userMap_ 中删除 fd
    std::lock_guard<std::mutex> gd(userMapMutex_);
    userMap_.erase(fd);
}

