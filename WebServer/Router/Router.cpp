#include "Router.h"

Router::Router()
{
}

Router::~Router()
{
}


// // 
HttpMethod Router::methodFromString(const std::string& method) {
    if (method == "GET") {
        return HttpMethod::GET;
    }
    if (method == "POST") {
        return HttpMethod::POST;
    }
    if (method == "DELETE") {
        return HttpMethod::DELETE;
    }
    if (method == "PUT") {
        return HttpMethod::PUT;
    }
    throw std::invalid_argument("Unsupported HTTP method");
}


std::string Router::GetRoute(const HttpRequestPtr& req, const HttpResponsePtr& resp)  {

    //
    printf("GetRoute method: %s\n", req->method.c_str());
    printf("GetRoute path: %s\n", req->path.c_str());

    auto iter = pathHandlers_.find(req->path);
    if (iter != pathHandlers_.end()) {
        auto methodHandlers = iter->second;

        auto methodIter = methodHandlers.find(methodFromString(req->method));
        if (methodIter != methodHandlers.end()) {
                methodIter->second(req, resp);
            // 执行对应的处理器
            // methodIter->second(std::make_shared<HttpRequest>(request), std::make_shared<HttpResponse>());
            return "200 OK"; // 假设处理器执行成功
        } else {
            // 方法未找到，返回 405 Method Not Allowed
            return "405 Method Not Allowed";
        }
    }
    return "404 Not Found"; // 路径未匹配时返回 404
}




void  Router::AddRoute(std::string httpMethod, const char* relativePath, Handler handlerFunc){

    httpMethodHandlers method_handlers;
    auto iter = pathHandlers_.find(relativePath);
    // 
    if (iter == pathHandlers_.end()){
        pathHandlers_[relativePath] = method_handlers;
    }
    else {
        method_handlers = iter->second;//  iter->second 是什么 ? std::map<HttpMethod, Handler>; iter->first 是什么 ?  std::string
    }
    // std::map<HttpMethod, Handler> method_handlers = pathHandlers[relativePath];
    //给 method_handlers 添加新的 handler
    method_handlers[methodFromString(httpMethod)] = handlerFunc;
    // 给 iter 添加新的 handler
    pathHandlers_[relativePath] = method_handlers;
    
};




// void Router::AddRoute(std::string httpMethod, const char* relativePath, Handler handlerFunc){

//     httpMethodHandlers method_handlers;
//     auto iter = pathHandlers_.find(relativePath);
//     // 
//     if (iter == pathHandlers_.end()){
//         pathHandlers_[relativePath] = method_handlers;
//     }
//     else {
//         method_handlers = iter->second;//  iter->second 是什么 ? std::map<HttpMethod, Handler>; iter->first 是什么 ?  std::string
//     }
//     // std::map<HttpMethod, Handler> method_handlers = pathHandlers[relativePath];
//     //给 method_handlers 添加新的 handler
//     method_handlers[methodFromString(httpMethod)] = handlerFunc;
//     // 给 iter 添加新的 handler
//     pathHandlers_[relativePath] = method_handlers;

// };



int ROUTER_MAIN()
{
    Router router;


    router.GET("/hello", [](const HttpRequestPtr& req, const HttpResponsePtr& resp) {

        printf("GET /hello ======= \n");
        // 设置响应头
        resp->SetHeader("Content-Type", "text/plain");
        resp->SetHeader("X-Custom-Header", "CustomValue");
        // 设置状态码
        resp->SetStatusCode(200);
        // 设置响应体
        resp->String("Hello, this is the response body!");
        // 发送响应
        resp->Send();
    });


    router.POST("/echo", [](const HttpRequestPtr& req, const HttpResponsePtr& resp) {
        
        // 设置响应头
        resp->SetHeader("Content-Type", "text/plain");
        resp->SetHeader("X-Custom-Header", "CustomValue");
        // 设置状态码
        resp->SetStatusCode(200);
        // 设置响应体
        resp->String("Hello, this is the response body!");
        // 发送响应
        resp->Send();
    });



    // 模拟请求
    HttpRequestPtr req = std::make_shared<HttpRequest>();
    HttpResponsePtr resp = std::make_shared<HttpResponse>();



// GET /hello?data=123 HTTP/1.1
// Host: 47.99.38.142:8080
// User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/131.0.0.0 Safari/537.36
// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7
// Accept-Encoding: gzip, deflate
// Accept-Language: zh-CN,zh;q=0.9
// Upgrade-Insecure-Requests: 1


    std::string request_data =
        "GET /hello?data=123 HTTP/1.1\r\n"
        "Host: 47.99.38.142:8080\r\n"
        "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/131.0.0.0 Safari/537.36\r\n"
        "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7\r\n"
        "Accept-Encoding: gzip, deflate\r\n"
        "Accept-Language: zh-CN,zh;q=0.9\r\n"
        "Upgrade-Insecure-Requests: 1\r\n"
        "\r\n"; 

    HttpRequest request;
    if (request.parse(request_data)) {
        request.printRequest();

        req->method = request.method;
        req->url = request.url;
        req->body = request.body;
        req->headers = request.headers;
        req->path = request.path;

        router.GetRoute(req, resp);

    } else {
        std::cerr << "Failed to parse request." << std::endl;
    }






    // POST 请求示例
    // std::string postRequestData =
    //     "POST /submit HTTP/1.1\r\n"
    //     "Host: 47.99.38.142:8080\r\n"
    //     "Content-Length: 23\r\n"
    //     "User-Agent: Mozilla/5.0\r\n"
    //     "Accept: application/x-www-form-urlencoded\r\n"
    //     "\r\n"
    //     "data=123&name=John&age=30";

    // HttpRequest postRequest;
    // if (postRequest.parse(postRequestData)) {
    //     postRequest.printRequest();
    // } else {
    //     std::cerr << "Failed to parse POST request." << std::endl;
    // }



    // std::string response_data = 
    //     "HTTP/1.1 200 OK\r\n"
    //     "Content-Type: text/html; charset=UTF-8\r\n"
    //     "Content-Length: 138\r\n"
    //     "Connection: keep-alive\r\n"
    //     "\r\n"
    //     "<html><body>Hello World!</body></html>";




    // 测试 GET 请求
    // req->method = "GET";
    // req->url = "/hello";
    // router.GetRoute(req, resp);


        // 测试 POST 请求
    // req->method = "POST";
    // req->url = "/echo";
    // req->body = "This is a test";
    // router.GetRoute(req, resp);



    // 注册 POST 路由
    // router.POST("/echo", [](const HttpRequestPtr& req, const HttpResponsePtr& resp) {
    //     resp->Write("Echo: " + req->body);
    //     resp->Send();
    // });




    // router.GET("/wildcard*", [](HttpRequest* req, HttpResponse* resp) {
    //     // std::string str = req->path + " match /wildcard*";
    //     // return resp->String(str);
    // });

    // router.POST("/kv", [](HttpRequest* req, HttpResponse* resp) {
    //     std::string str = req->body + " match /wildcard*";
    //     return resp->String(str);
    // });

    // router.GET("/sleep", [](const HttpRequestPtr& req, const HttpResponseWriterPtr& writer) {
    //     writer->WriteHeader("X-Response-tid", hv_gettid());
    //     writer->response->Set("start_ms", start_ms);
    //     response_status(writer, 0, "OK");
    //     return 200;
    // });

    // router.POST("/echo", [](const HttpContextPtr& ctx) {
    //     return ctx->send(ctx->body(), ctx->type());
    // });

    // router.GET("/async", [](const HttpRequestPtr& req, const HttpResponseWriterPtr& writer) {
    //     writer->Begin();
    //     writer->WriteHeader("X-Response-tid", hv_gettid());
    //     writer->WriteHeader("Content-Type", "text/plain");
    //     writer->WriteBody("This is an async response.\n");
    //     writer->End();
    // })


}