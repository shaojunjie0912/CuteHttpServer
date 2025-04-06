#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
//
#include <cutemuduo/buffer.hpp>

namespace cutehttpserver {

enum class HttpStatusCode {
    kUnknown,
    k200Ok = 200,
    k204NoContent = 204,
    k301MovedPermanently = 301,
    k400BadRequest = 400,
    k401Unauthorized = 401,
    k403Forbidden = 403,
    k404NotFound = 404,
    k409Conflict = 409,
    k500InternalServerError = 500,
};

class HttpResponse {
public:
    HttpResponse(bool close = true);

    ~HttpResponse();

public:
    // 设置 HTTP 版本
    void SetVersion(std::string version);

    // 设置状态码
    void SetStatusCode(HttpStatusCode code);

    // 获取状态码
    HttpStatusCode GetStatusCode() const;

    // 设置状态信息
    void SetStatusMessage(std::string message);

    void SetCloseConnection(bool close);

    bool IsCloseConnection() const;

    // 添加响应头的一行
    void AddHeader(std::string key, std::string value);

    // void SetHeader(std::)

    // 设置响应头 Content-Type
    void SetContentType(std::string type);

    // 设置响应头 Content-Length
    void SetContentLength(uint64_t length);

    // 设置响应体
    void SetResponseBody(std::string body);

public:
    std::string GetResponse() const;

    void AppendToBuffer(cutemuduo::Buffer* buff) const;

private:
    std::string http_version_;                                       // HTTP 版本
    HttpStatusCode status_code_;                                     // 状态码
    std::string status_message_;                                     // 状态信息
    bool close_connection_;                                          // 是否关闭连接
    std::unordered_map<std::string, std::string> response_headers_;  // 响应头
    std::string response_body_;                                      // 响应体
};

}  // namespace cutehttpserver
