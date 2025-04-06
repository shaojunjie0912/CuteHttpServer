#pragma once

#include <concepts>
#include <cstdint>
#include <string>
#include <unordered_map>
//
#include <cutemuduo/timestamp.hpp>

namespace cutehttpserver {

template <typename T>
concept StringType = std::same_as<std::remove_cvref_t<T>, std::string>;

enum class HttpRequestMethod {
    kInvalid,
    kGet,
    kPost,
    kHead,
    kPut,
    kDelete,
    kOptions,
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
