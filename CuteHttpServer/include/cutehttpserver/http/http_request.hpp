#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
//
#include <cutemuduo/timestamp.hpp>

namespace cutehttpserver {

enum class HttpRequestMethod {
    kInvalid,
    kGet,      // 请求服务器发送指定的资源
    kPost,     // 向服务器发送数据
    kHead,     // 类似 GET, 但服务器只返回响应头, 不返回响应体
    kPut,      // 向服务器上传数据
    kDelete,   // 请求服务器删除指定的资源
    kOptions,  // 询问服务器支持哪些 HTTP 方法
};

class HttpRequest {
public:
    HttpRequest();

    ~HttpRequest();

public:
    // 设置请求接收时间
    void SetReceiveTime(const cutemuduo::Timestamp& time);

    // 获取请求接收时间
    cutemuduo::Timestamp GetReceiveTime() const;

    // 设置请求方法
    bool SetMethod(std::string const& method);

    // 获取请求方法
    HttpRequestMethod GetMethod() const;

    std::string GetStringMethod() const;

    // 设置请求路径
    void SetPath(std::string path);

    // 获取请求路径
    std::string GetPath() const;

    // 添加请求头的一行
    void AddHeader(std::string key, std::string value);

    std::string GetHeader(std::string key) const;

    void SetPathParams(std::string key, std::string value);

    std::string GetPathParams(std::string key) const;

    // 设置查询参数 Add 和 Set 区别
    void SetQueryParams(std::string key, std::string value);

    std::string GetQueryParams(std::string key) const;

    void SetVersion(std::string version);

    std::string GetVersion() const;

    void SetContentLength(uint64_t length);

    uint64_t GetContentLength() const;

    void SetRequestBody(std::string body);

    std::string GetRequestBody() const;

    void Swap(HttpRequest& other) {
        std::swap(method_, other.method_);
        std::swap(path_, other.path_);
        std::swap(path_params_, other.path_params_);
        std::swap(query_params_, other.query_params_);
        std::swap(version_, other.version_);
        std::swap(request_headers_, other.request_headers_);
        std::swap(receive_time_, other.receive_time_);
    }

private:
    HttpRequestMethod method_;                                      // 请求方法
    std::string path_;                                              // 请求路径
    std::unordered_map<std::string, std::string> path_params_;      // 路径参数
    std::unordered_map<std::string, std::string> query_params_;     // 查询参数
    std::string version_;                                           // HTTP 版本
    std::unordered_map<std::string, std::string> request_headers_;  // 请求头
    uint64_t body_length_;                                          // 请求体长度
    std::string request_body_;                                      // 请求体
    cutemuduo::Timestamp receive_time_;                             // 请求接收时间
};

}  // namespace cutehttpserver
