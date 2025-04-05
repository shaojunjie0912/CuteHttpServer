#pragma once

#include <cstdint>
#include <sstream>
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
    HttpResponse(bool close = true)
        : status_code_(HttpStatusCode::kUnknown), close_connection_(close) {}

public:
    // 设置 HTTP 版本
    void SetVersion(std::string version) { http_version_ = std::move(version); }

    // 设置状态码
    void SetStatusCode(HttpStatusCode code) { status_code_ = code; }

    // 获取状态码
    HttpStatusCode GetStatusCode() const { return status_code_; }

    // 设置状态信息
    void SetStatusMessage(std::string message) { status_message_ = std::move(message); }

    void SetCloseConnection(bool close) { close_connection_ = close; }

    bool IsCloseConnection() const { return close_connection_; }

    // 添加响应头的一行
    void AddHeader(std::string key, std::string value) {
        response_headers_.emplace(std::move(key), std::move(value));
    }

    // void SetHeader(std::)

    // 设置响应头 Content-Type
    void SetContentType(std::string type) { response_headers_["Content-Type"] = std::move(type); }

    // 设置响应头 Content-Length
    void SetContentLength(uint64_t length) {
        response_headers_["Content-Length"] = std::to_string(length);
    }

    // 设置响应体
    void SetResponseBody(std::string body) {
        SetContentLength(body.size());
        response_body_ = std::move(body);
    }

public:
    std::string GetResponse() const {
        std::ostringstream oss_response;
        oss_response << http_version_ << " " << std::to_string(static_cast<int>(status_code_))
                     << " " << status_message_ << "\r\n";
        for (auto const& [key, value] : response_headers_) {
            oss_response << key << ": " << value << "\r\n";
        }
        oss_response << (close_connection_ ? "Connection: close\r\n" : "Connection: keep-alive\r\n")
                     << "\r\n";
        oss_response << response_body_;
        return oss_response.str();
    }

    void AppendToBuffer(cutemuduo::Buffer* buff) const { buff->Append(GetResponse().c_str()); }

private:
    std::string http_version_;                                       // HTTP 版本
    HttpStatusCode status_code_;                                     // 状态码
    std::string status_message_;                                     // 状态信息
    bool close_connection_;                                          // 是否关闭连接
    std::unordered_map<std::string, std::string> response_headers_;  // 响应头
    std::string response_body_;                                      // 响应体
};

}  // namespace cutehttpserver
