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
    HttpRequest() : method_(HttpRequestMethod::kInvalid), version_("Unknown") {}

    ~HttpRequest() = default;

public:
    // 设置请求接收时间
    void SetReceiveTime(const cutemuduo::Timestamp& time) { receive_time_ = time; }

    // 获取请求接收时间
    cutemuduo::Timestamp GetReceiveTime() const { return receive_time_; }

    // 设置请求方法
    bool SetMethod(std::string const& method) {
        if (method == "GET") {
            method_ = HttpRequestMethod::kGet;
        } else if (method == "POST") {
            method_ = HttpRequestMethod::kPost;
        } else if (method == "PUT") {
            method_ = HttpRequestMethod::kPut;
        } else if (method == "DELETE") {
            method_ = HttpRequestMethod::kDelete;
        } else if (method == "OPTIONS") {
            method_ = HttpRequestMethod::kOptions;
        } else {
            method_ = HttpRequestMethod::kInvalid;
        }
        return method_ != HttpRequestMethod::kInvalid;
    }

    // 获取请求方法
    HttpRequestMethod GetMethod() const { return method_; }

    std::string GetStringMethod() const {
        switch (method_) {
            case HttpRequestMethod::kGet:
                return "GET";
            case HttpRequestMethod::kPost:
                return "POST";
            case HttpRequestMethod::kPut:
                return "PUT";
            case HttpRequestMethod::kDelete:
                return "DELETE";
            case HttpRequestMethod::kOptions:
                return "OPTIONS";
            case HttpRequestMethod::kHead:
                return "HEAD";
            default:
                return "INVALID";
        }
    }

    // 设置请求路径
    void SetPath(std::string path) { path_ = std::move(path); }

    // 获取请求路径
    std::string GetPath() const { return path_; }

    // 添加请求头的一行
    void AddHeader(std::string key, std::string value) {
        request_headers_.emplace(std::move(key),
                                 std::move(value));  // TODO: 如果键已经存在, 则不会被修改
    }

    std::string GetHeader(std::string key) const {
        if (auto it{request_headers_.find(std::move(key))}; it != request_headers_.end()) {
            return it->second;
        }
        return "";
    }

    void SetPathParams(std::string key, std::string value) {
        path_params_[std::move(key)] = std::move(value);
    }

    std::string GetPathParams(std::string key) const {
        if (auto it{path_params_.find(std::move(key))}; it != path_params_.end()) {
            return it->second;
        }
        return "";
    }

    // 设置查询参数 Add 和 Set 区别
    void SetQueryParams(std::string key, std::string value) {
        query_params_[std::move(key)] = std::move(value);
    }

    std::string GetQueryParams(std::string key) const {
        if (auto it{query_params_.find(std::move(key))}; it != query_params_.end()) {
            return it->second;
        }
        return "";
    }

    void SetVersion(std::string version) { version_ = std::move(version); }

    std::string GetVersion() const { return version_; }

    void SetContentLength(uint64_t length) { body_length_ = length; }

    uint64_t GetContentLength() const { return body_length_; }

    void SetRequestBody(std::string body) { request_body_ = std::move(body); }

    std::string GetRequestBody() const { return request_body_; }

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
