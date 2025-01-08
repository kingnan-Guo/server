#pragma once
#include <functional>
#include <string>
#include <map>
#include <sstream>
#include <memory>// 使用只能指针
#include <list>
#include <iostream>
#include <map>


// Response是服务器对客户端请求的回应: 响应
class HttpResponse {
    // private:
    //     int status_code;
    //     std::string body;
    //     std::map<std::string, std::string> headers;

    //     // 根据状态码获取响应的状态信息
    //     std::string GetStatusMessage() const {
    //         switch (status_code) {
    //             case 200: return "OK";
    //             case 404: return "Not Found";
    //             case 500: return "Internal Server Error";
    //             default: return "Unknown";
    //         }
    //     }

    public:
        int status_code;
        std::string body;
        std::map<std::string, std::string> headers;

        // 根据状态码获取响应的状态信息
        std::string GetStatusMessage() const {
            switch (status_code) {
                case 200: return "OK";
                case 404: return "Not Found";
                case 500: return "Internal Server Error";
                default: return "Unknown";
            }
        };



        HttpResponse() : status_code(200) {}

        // 设置响应体
        void String(const std::string& str) {
            body = str;
        }

        // 设置状态码
        void SetStatusCode(int code) {
            status_code = code;
        }

        // 设置响应头
        void SetHeader(const std::string& key, const std::string& value) {
            headers[key] = value;
        }

        // 输出响应内容
        std::string Send() {
            // printf("HTTP/1.1 %d %s ============\r\n", status_code, GetStatusMessage().c_str() );
            // std::cout << "HTTP/1.1 " << status_code << " " << GetStatusMessage() << std::endl;
            // for (const auto& header : headers) {
            //     std::cout << header.first << ": " << header.second << std::endl;
            // }
            // std::cout << std::endl;
            // std::cout << "Response Body: " << body << std::endl;


            std::ostringstream response;
            response << "HTTP/1.1 " << status_code << " " << GetStatusMessage() << "\r\n";
            
            // 拼装响应头
            for (const auto& header : headers) {
                response << header.first << ": " << header.second << "\r\n";
            }

            // 添加空行
            response << "\r\n";
            
            // 拼装响应体
            response << body;
            
            // 发送响应 (这里只是打印在控制台，实际应用中可以通过套接字发送)
            std::cout << response.str();
            return response.str();




        }


};

// Request是浏览器向服务器发出的请求
// class HttpRequest {
// public:
//     std::string method;
//     std::string url;
//     std::string version;
//     std::map<std::string, std::string> headers;
//     std::string body;

//     bool parse(const std::string& data) {
//         std::istringstream stream(data);
//         std::string line;

//         // 解析请求行
//         if (!std::getline(stream, line)) return false;
//         std::istringstream requestLine(line);
//         if (!(requestLine >> method >> url >> version)) return false;

//         // 解析头部
//         while (std::getline(stream, line) && line != "\r") {
//             auto pos = line.find(": ");
//             if (pos != std::string::npos) {
//                 auto key = line.substr(0, pos);
//                 auto value = line.substr(pos + 2);
//                 headers[key] = value;
//             }
//         }

//         // 解析正文
//         if (headers.count("Content-Length")) {
//             int length = std::stoi(headers["Content-Length"]);
//             body.resize(length);
//             stream.read(&body[0], length);
//         }

//         return true;
//     }

//     // 打印解析结果
//     void printRequest() {
//         std::cout << "Method: " << method << std::endl;
//         std::cout << "URL: " << url << std::endl;
//         std::cout << "Version: " << version << std::endl;
//         std::cout << "Headers: " << std::endl;
//         for (const auto& header : headers) {
//             std::cout << header.first << ": " << header.second << std::endl;
//         }

//         std::cout << "Body: " << body << std::endl;
//     }

// };

class HttpRequest {
public:
    std::string method;                // HTTP 方法: GET, POST 等
    std::string url;                   // 请求 URL
    std::string path;                  // 请求路径
    std::string version;               // HTTP 版本
    std::map<std::string, std::string> headers;  // 请求头
    std::map<std::string, std::string> queryParams; // GET 请求的查询字符串参数
    std::map<std::string, std::string> postParams;  // POST 请求的表单参数
    std::string body;                  // 请求体



    // 解析查询字符串中的参数 (只针对 GET 请求)
    void parseQueryParams(const std::string& url) {
        size_t pos = url.find('?');
        if (pos == std::string::npos) return;

        std::string queryString = url.substr(pos + 1);  // 获取查询字符串部分
        std::istringstream queryStream(queryString);
        std::string param;

        while (std::getline(queryStream, param, '&')) {
            size_t equalPos = param.find('=');
            if (equalPos != std::string::npos) {
                std::string key = param.substr(0, equalPos);
                std::string value = param.substr(equalPos + 1);
                queryParams[key] = value;
            }
        }
    }

    // 解析 POST 请求的参数 (只针对 POST 请求)
    void parsePostParams(const std::string& body) {
        std::istringstream bodyStream(body);
        std::string param;
        while (std::getline(bodyStream, param, '&')) {
            size_t equalPos = param.find('=');
            if (equalPos != std::string::npos) {
                std::string key = param.substr(0, equalPos);
                std::string value = param.substr(equalPos + 1);
                postParams[key] = value;
            }
        }
    }

    void parePath() {
        size_t pos = url.find('?');
        if (pos != std::string::npos) {
            path = url.substr(0, pos);
        } else {
            path = url;
        }
    }

    bool parse(const std::string& data) {
        std::istringstream stream(data);
        std::string line;

        // 解析请求行
        if (!std::getline(stream, line)) return false;
        std::istringstream requestLine(line);
        if (!(requestLine >> method >> url >> version)) return false;

        // 解析查询字符串参数 (只针对 GET 请求)
        if (method == "GET") {
            parseQueryParams(url);
        }

        // 解析请求头部
        while (std::getline(stream, line) && line != "\r") {
            if (line.empty()) continue;
            auto pos = line.find(": ");
            if (pos != std::string::npos) {
                auto key = line.substr(0, pos);
                auto value = line.substr(pos + 2);
                headers[key] = value;
            }
        }

        // 解析 POST 请求的 body 参数 (只针对 POST 请求)
        if (method == "POST") {
            if (headers.count("Content-Length")) {
                int length = std::stoi(headers["Content-Length"]);
                body.resize(length);
                stream.read(&body[0], length);
                parsePostParams(body);
            }
        }

        parePath();

        return true;
    }



    void printRequest() {
        std::cout << "Method: " << method << std::endl;
        std::cout << "URL: " << url << std::endl;
        std::cout << "path: " << path << std::endl;
        std::cout << "Version: " << version << std::endl;

        if (method == "GET") {
            std::cout << "Query Parameters: " << std::endl;
            for (const auto& param : queryParams) {
                std::cout << "   " << param.first << ": " << param.second << std::endl;
            }
        } else if (method == "POST") {
            std::cout << "POST Body: " << body << std::endl;
            std::cout << "POST Parameters: " << std::endl;
            for (const auto& param : postParams) {
                std::cout << "   "  << param.first << ": " << param.second << std::endl;
            }
        }

        std::cout << "Headers: " << std::endl;
        for (const auto& header : headers) {
            std::cout << header.first << ": " << header.second << std::endl;
        }
    }
};




// HttpRequestPtr和HttpResponseWriterPtr是这些类的智能指针类型
using HttpRequestPtr = std::shared_ptr<HttpRequest>;
using HttpResponsePtr = std::shared_ptr<HttpResponse>;

// 处理函数类型
// using Handler = std::function<void(HttpRequestPtr, HttpResponsePtr)>;
using Handler = std::function<void(const HttpRequestPtr&, const HttpResponsePtr&)>;
// HTTP方法枚举
enum class HttpMethod { GET, POST, DELETE, PUT };
// 用于存储路由的结构
using httpMethodHandlers = std::map<HttpMethod, Handler>;

class Router
{
    private:
        // {
        //     "api":{
        //         "GET":function2(),
        //         "POST":function1()
        //     }
        // }
        std::map<std::string, httpMethodHandlers> pathHandlers_;

        // std::map<std::string, http_path_handlers> routes_; //  map(path, map(method, list<handler>))

    public:
        Router();
        ~Router();


        HttpMethod methodFromString(const std::string& method);



        std::string GetRoute(const HttpRequestPtr& req, const HttpResponsePtr& resp);

        

        void AddRoute(std::string httpMethod, const char* relativePath, Handler handlerFunc);


        void Handle(const char* httpMethod, const char* relativePath, Handler handlerFunc) {
            AddRoute(httpMethod, relativePath, handlerFunc);
        }


        template<typename Handler>
        void OPTION(const char* relativePath, Handler handlerFunc) {
            Handle("OPTION", relativePath, handlerFunc);
        }

        template<typename Handler>
        void GET(const char* relativePath, Handler handlerFunc) {
            Handle("GET", relativePath, handlerFunc);
        }

        // POST
        template<typename Handler>
        void POST(const char* relativePath, Handler handlerFunc) {
            Handle("POST", relativePath, handlerFunc);
        }


};


int ROUTER_MAIN();



