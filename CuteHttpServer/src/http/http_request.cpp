#include <cutehttpserver/http/http_request.hpp>

namespace cutehttpserver {

HttpRequest::HttpRequest() : method_(HttpRequestMethod::kInvalid), version_("Unknown") {}

HttpRequest::~HttpRequest() = default;

void HttpRequest::SetReceiveTime(const cutemuduo::Timestamp& time) { receive_time_ = time; }

cutemuduo::Timestamp HttpRequest::GetReceiveTime() const { return receive_time_; }

bool HttpRequest::SetMethod(std::string const& method) {
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

HttpRequestMethod HttpRequest::GetMethod() const { return method_; }

std::string HttpRequest::GetStringMethod() const {
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

void HttpRequest::SetPath(std::string path) { path_ = std::move(path); }

std::string HttpRequest::GetPath() const { return path_; }

void HttpRequest::AddHeader(std::string key, std::string value) {
    request_headers_.emplace(std::move(key),
                             std::move(value));  // TODO: 如果键已经存在, 则不会被修改
}

std::string HttpRequest::GetHeader(std::string key) const {
    if (auto it{request_headers_.find(std::move(key))}; it != request_headers_.end()) {
        return it->second;
    }
    return "";
}

void HttpRequest::SetPathParams(std::string key, std::string value) {
    path_params_[std::move(key)] = std::move(value);
}

std::string HttpRequest::GetPathParams(std::string key) const {
    if (auto it{path_params_.find(std::move(key))}; it != path_params_.end()) {
        return it->second;
    }
    return "";
}

void HttpRequest::SetQueryParams(std::string key, std::string value) {
    query_params_[std::move(key)] = std::move(value);
}

std::string HttpRequest::GetQueryParams(std::string key) const {
    if (auto it{query_params_.find(std::move(key))}; it != query_params_.end()) {
        return it->second;
    }
    return "";
}

void HttpRequest::SetVersion(std::string version) { version_ = std::move(version); }

std::string HttpRequest::GetVersion() const { return version_; }

void HttpRequest::SetContentLength(uint64_t length) { body_length_ = length; }

uint64_t HttpRequest::GetContentLength() const { return body_length_; }

void HttpRequest::SetRequestBody(std::string body) { request_body_ = std::move(body); }

std::string HttpRequest::GetRequestBody() const { return request_body_; }

}  // namespace cutehttpserver
