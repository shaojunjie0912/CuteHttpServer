#pragma once

#include <string>
//
#include <cutehttpserver/http/http_request.hpp>
#include <cutemuduo/tcp_server.hpp>

namespace cutehttpserver {

namespace http {

class HttpContext {
public:
    enum class HttpRequestParseState {
        REQUEST_LINE,
        HEADERS,
        BODY,
        FINISH,
    };

    HttpContext() : state_(HttpRequestParseState::REQUEST_LINE) {}
    ~HttpContext() = default;

public:
    bool ParseRequest(cutemuduo::Buffer* buff);

    // 解析请求行(<方法> <请求路径?查询参数> <协议版本>)
    bool ParseRequestLine(std::string const& line);

    // 解析请求头的其中一行
    bool ParseHeaderLine(std::string const& line);

    // 解析请求体
    bool ParseBody(std::string body_data);

public:
    HttpRequest GetRequest() const { return request_; }

private:
    HttpRequestParseState state_;  // 当前解析状态
    HttpRequest request_;          // 请求对象
};

}  // namespace http

}  // namespace cutehttpserver
