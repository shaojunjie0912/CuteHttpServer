#pragma once

#include <string>
//
#include <cutehttpserver/http/http_request.hpp>
#include <cutemuduo/tcp_server.hpp>

namespace cutehttpserver {

enum class HttpRequestParseState {
    REQUEST_LINE,  // 解析请求行
    HEADERS,       // 解析请求头
    BODY,          // 解析请求体
    FINISH,        // 解析完成
};

class HttpContext {
public:
    HttpContext();
    ~HttpContext();

public:
    // 解析请求
    // ParseRequest = ParseRequestLine + ParseHeaderLine + ParseBody
    bool ParseRequest(cutemuduo::Buffer* buff, cutemuduo::Timestamp const& receive_time);

    // 解析请求行 <方法> <请求路径?查询参数> <协议版本>
    bool ParseRequestLine(std::string const& line);

    // 解析请求头的其中一行
    bool ParseHeaderLine(std::string const& line);

    // 解析请求体
    bool ParseBody(std::string body_data);

public:
    bool GotAll() const { return state_ == HttpRequestParseState::FINISH; }

    void Reset() {
        state_ = HttpRequestParseState::REQUEST_LINE;
        HttpRequest dummy_data;
        request_.Swap(dummy_data);
    }

public:
    HttpRequest GetRequest() const;

private:
    HttpRequestParseState state_;  // 当前解析状态
    HttpRequest request_;          // 请求对象
};

}  // namespace cutehttpserver
