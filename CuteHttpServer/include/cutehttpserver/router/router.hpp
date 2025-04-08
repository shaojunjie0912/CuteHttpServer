#pragma once

#include <functional>
#include <memory>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>
//
#include <cutehttpserver/http/http_request.hpp>
#include <cutehttpserver/router/router_handler.hpp>

namespace cutehttpserver {

class Router {
public:
    using HandlerPtr = std::shared_ptr<RouterHandler>;
    using HandlerCallback = std::function<void(HttpRequest const&, HttpResponse*)>;  // TODO: 指针 * ?

    // 路由映射表的键 (由请求方法 + URI 组成)
    struct RouteKey {
        HttpRequestMethod method_;
        std::string path_;

        // NOTE: 作为哈希表的键需要有 == 方法
        bool operator==(RouteKey const& other) const {
            return method_ == other.method_ && path_ == other.path_;
        }
    };

    // RouteKey 哈希函数
    struct RouteKeyHash {
        size_t operator()(RouteKey const& key) const {
            size_t method_hash = std::hash<int>{}(static_cast<int>(key.method_));
            size_t path_hash = std::hash<std::string>{}(key.path_);
            return method_hash ^ (path_hash << 1);  // XOR hash values
        }
    };

    // 注册静态路由(精准匹配), 存储在哈希表中
    void RegisterHandler(HttpRequestMethod method, std::string path, HandlerPtr handler);

    // 注册回调函数
    void RegisterCallback(HttpRequestMethod method, std::string path, HandlerCallback callback);

    // 注册动态路由(模式匹配), 存储在动态数组中
    void RegisterRegexHandler(HttpRequestMethod method, std::string path, HandlerPtr handler);

    void RegisterRegexCallback(HttpRequestMethod method, std::string path, HandlerCallback callback);

    bool Route(HttpRequest const& request, HttpResponse* response);

private:
    // 将路径模式转换为正则表达式
    std::regex ConvertToRegex(std::string path_pattern);

    // 提取路径参数
    void ExtractPathParams(std::smatch const& match, HttpRequest& request);

private:
    // 基于对象的动态路由处理器
    struct RouteHandlerObj {
        HttpRequestMethod method_;
        std::regex path_regex_;
        HandlerPtr handler_;

        RouteHandlerObj(HttpRequestMethod method, std::regex path_regex, HandlerPtr handler)
            : method_(method), path_regex_(path_regex), handler_(handler) {}
    };

    struct RouteCallbackObj {
        HttpRequestMethod method_;
        std::regex path_regex_;
        HandlerCallback callback_;

        RouteCallbackObj(HttpRequestMethod method, std::regex path_regex, HandlerCallback const& callback)
            : method_(method), path_regex_(path_regex), callback_(callback) {}
    };

private:
    std::unordered_map<RouteKey, HandlerPtr, RouteKeyHash> handlers_;        // 静态路由 O(1)
    std::unordered_map<RouteKey, HandlerCallback, RouteKeyHash> callbacks_;  // 静态路由回调函数
    std::vector<RouteHandlerObj> regex_handlers_;                            // 动态路由 O(n)
    std::vector<RouteCallbackObj> regex_callbacks_;                          // 动态路由回调函数
};

}  // namespace cutehttpserver
