#pragma once
#include <functional>
#include <string>
#include <map>
#include <sstream>

// class HttpService
// {
//     private:
//         std::string method;
//         std::string url;
//         std::string version;
//         std::map<std::string, std::string> headers;
//         std::string body;
//     public:
//         HttpService();
//         ~HttpService();


//         // void addRoute(const char* path, const std::string& method, const http_handler& handler);
//         // void getRoute();

//         bool parse(const std::string& data) {
//             std::istringstream stream(data);
//             std::string line;

//             // 解析请求行
//             if (!std::getline(stream, line)) return false;
//             std::istringstream requestLine(line);
//             if (!(requestLine >> method >> url >> version)) return false;

//             // 解析头部
//             while (std::getline(stream, line) && line != "\r") {
//                 auto pos = line.find(": ");
//                 if (pos != std::string::npos) {
//                     auto key = line.substr(0, pos);
//                     auto value = line.substr(pos + 2);
//                     headers[key] = value;
//                 }
//             }

//             // 解析正文
//             if (headers.count("Content-Length")) {
//                 int length = std::stoi(headers["Content-Length"]);
//                 body.resize(length);
//                 stream.read(&body[0], length);
//             }

//             return true;
//         }














// };

// HttpService::HttpService()
// {
// }

// HttpService::~HttpService()
// {
// }
