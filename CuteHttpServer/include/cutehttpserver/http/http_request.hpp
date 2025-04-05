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

namespace http {

enum class Method {
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
    HttpRequest() : method_(Method::kInvalid), version_("Unknown") {}

    ~HttpRequest() = default;

public:
    // 设置请求接收时间
    void SetReceiveTime(const cutemuduo::Timestamp& time) { receive_time_ = time; }

    // 获取请求接收时间
    cutemuduo::Timestamp GetReceiveTime() const { return receive_time_; }

    // 设置请求方法
    bool SetMethod(std::string const& method) {
        if (method == "GET") {
            method_ = Method::kGet;
        } else if (method == "POST") {
            method_ = Method::kPost;
        } else if (method == "PUT") {
            method_ = Method::kPut;
        } else if (method == "DELETE") {
            method_ = Method::kDelete;
        } else if (method == "OPTIONS") {
            method_ = Method::kOptions;
        } else {
            method_ = Method::kInvalid;
        }
        return method_ != Method::kInvalid;
    }

    // 获取请求方法
    Method GetMethod() const { return method_; }

    std::string GetStringMethod() const {
        switch (method_) {
            case Method::kGet:
                return "GET";
            case Method::kPost:
                return "POST";
            case Method::kPut:
                return "PUT";
            case Method::kDelete:
                return "DELETE";
            case Method::kOptions:
                return "OPTIONS";
            case Method::kHead:
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
        headers_.emplace(std::move(key), std::move(value));  // TODO: 如果键已经存在, 则不会被修改
    }

    std::string GetHeader(std::string key) const {
        if (auto it{headers_.find(std::move(key))}; it != headers_.end()) {
            return it->second;
        }
        return "";
    }

    // 设置查询参数 Add 和 Set 区别
    void SetQueryParams(std::string key, std::string value) { query_params_[std::move(key)] = std::move(value); }

    std::string GetQueryParams(std::string key) const {
        if (auto it{query_params_.find(std::move(key))}; it != query_params_.end()) {
            return it->second;
        }
        return "";
    }

    void SetVersion(std::string version) { version_ = std::move(version); }

    std::string GetVersion() const { return version_; }

    void SetContentLength(uint64_t length) { content_length_ = length; }

    void SetContent(std::string content) { content_ = std::move(content); }

    std::string GetContent() const { return content_; }

    uint64_t GetContentLength() const { return content_length_; }

private:
    Method method_;                                              // 请求方法
    std::string path_;                                           // 请求路径
    std::unordered_map<std::string, std::string> query_params_;  // 查询参数
    std::string version_;                                        // HTTP 版本
    std::unordered_map<std::string, std::string> headers_;       // 请求头
    uint64_t content_length_;                                    // 请求体长度
    std::string content_;                                        // 请求体
    cutemuduo::Timestamp receive_time_;                          // 请求接收时间
};

}  // namespace http

}  // namespace cutehttpserver
