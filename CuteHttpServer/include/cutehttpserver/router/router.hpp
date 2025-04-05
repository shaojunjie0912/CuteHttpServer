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
    using HandlerCallback =
        std::function<void(HttpRequest const&, HttpResponse*)>;  // TODO: 指针 * ?

    // 路由映射表的键 (由请求方法 + URI 组成)
    struct RouteKey {
        HttpRequestMethod method_;
        std::string path_;
    };

    struct RouteKeyHash {
        size_t operator()(RouteKey const& key) const {
            size_t method_hash = std::hash<int>{}(static_cast<int>(key.method_));
            size_t path_hash = std::hash<std::string>{}(key.path_);
            return method_hash ^ (path_hash << 1);  // XOR hash values
        }
    };

    // 注册静态路由(精准匹配), 存储在哈希表中
    void RegisterHandler(HttpRequestMethod method, std::string path, HandlerPtr handler) {
        handlers_.emplace(RouteKey{method, std::move(path)}, std::move(handler));
    }

    // 注册回调函数
    void RegisterCallback(HttpRequestMethod method, std::string path, HandlerCallback callback) {
        callbacks_.emplace(RouteKey{method, std::move(path)}, std::move(callback));
    }

    // 注册动态路由(模式匹配), 存储在动态数组中
    void RegisterRegexHandler(HttpRequestMethod method, std::string path, HandlerPtr handler) {
        regex_handlers_.emplace_back(method, ConvertToRegex(std::move(path)), std::move(handler));
    }

    void RegisterRegexCallback(HttpRequestMethod method, std::string path,
                               HandlerCallback callback) {
        regex_callbacks_.emplace_back(method, ConvertToRegex(std::move(path)), std::move(callback));
    }

    bool Route(HttpRequest const& request, HttpResponse* response) {
        RouteKey route_key{request.GetMethod(), request.GetPath()};
        // 查找静态路由
        if (auto handler_iter{handlers_.find(route_key)}; handler_iter != handlers_.end()) {
            handler_iter->second->Handle(request, response);  // TODO: Handle 实现? 为什么指针 *
            return true;
        }
        // 查找静态路由回调函数
        if (auto callback_iter{callbacks_.find(route_key)}; callback_iter != callbacks_.end()) {
            callback_iter->second(request, response);
            return true;
        }
        // 查找动态路由( vector 线性查找)
        for (auto const& [method, path_regex, handler] : regex_handlers_) {
            std::smatch match;
            std::string path_str{request.GetPath()};
            if (method == request.GetMethod() && std::regex_match(path_str, match, path_regex)) {
                HttpRequest new_request{request};  // 创建一个新的请求对象
                // 提取路径参数
                ExtractPathParams(match, new_request);
                handler->Handle(new_request, response);
                return true;
            }
        }
        // 查找动态路由回调函数
        for (auto const& [method, path_regex, callback] : regex_callbacks_) {
            std::smatch match;
            std::string path_str{request.GetPath()};
            if (method == request.GetMethod() && std::regex_match(path_str, match, path_regex)) {
                HttpRequest new_request{request};  // 创建一个新的请求对象
                // 提取路径参数
                ExtractPathParams(match, new_request);
                callback(new_request, response);
                return true;
            }
        }
        return false;
    }

private:
    // 将路径模式转换为正则表达式
    std::regex ConvertToRegex(std::string path_pattern) {
        std::string regex_pattern =
            "^" +
            std::regex_replace(std::move(path_pattern), std::regex(R"(/:([^/]+))"), R"(/([^/]+))") +
            "$";
        return std::regex(regex_pattern);
    }

    // 提取路径参数
    void ExtractPathParams(std::smatch const& match, HttpRequest& request) {
        // 假设第一个匹配成功的是完整路径, 从第二个开始, 提取路径参数
        for (size_t i{1}; i < match.size(); ++i) {
            request.SetPathParams("param" + std::to_string(i), match[i].str());
        }
    }

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

        RouteCallbackObj(HttpRequestMethod method, std::regex path_regex,
                         HandlerCallback const& callback)
            : method_(method), path_regex_(path_regex), callback_(callback) {}
    };

private:
    std::unordered_map<RouteKey, HandlerPtr, RouteKeyHash> handlers_;        // 静态路由 O(1)
    std::unordered_map<RouteKey, HandlerCallback, RouteKeyHash> callbacks_;  // 静态路由回调函数
    std::vector<RouteHandlerObj> regex_handlers_;                            // 动态路由 O(n)
    std::vector<RouteCallbackObj> regex_callbacks_;                          // 动态路由回调函数
};

}  // namespace cutehttpserver
