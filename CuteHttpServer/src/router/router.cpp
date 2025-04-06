#include <cutehttpserver/router/router.hpp>

namespace cutehttpserver {

void Router::RegisterHandler(HttpRequestMethod method, std::string path, HandlerPtr handler) {
    handlers_.emplace(RouteKey{method, std::move(path)}, std::move(handler));
}

void Router::RegisterCallback(HttpRequestMethod method, std::string path,
                              HandlerCallback callback) {
    callbacks_.emplace(RouteKey{method, std::move(path)}, std::move(callback));
}

void Router::RegisterRegexHandler(HttpRequestMethod method, std::string path, HandlerPtr handler) {
    regex_handlers_.emplace_back(method, ConvertToRegex(std::move(path)), std::move(handler));
}

void Router::RegisterRegexCallback(HttpRequestMethod method, std::string path,
                                   HandlerCallback callback) {
    regex_callbacks_.emplace_back(method, ConvertToRegex(std::move(path)), std::move(callback));
}

bool Router::Route(HttpRequest const& request, HttpResponse* response) {
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
    // 查找动态路由 (vector 线性查找)
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

std::regex Router::ConvertToRegex(std::string path_pattern) {
    std::string regex_pattern =
        "^" +
        std::regex_replace(std::move(path_pattern), std::regex(R"(/:([^/]+))"), R"(/([^/]+))") +
        "$";
    return std::regex(regex_pattern);
}

void Router::ExtractPathParams(std::smatch const& match, HttpRequest& request) {
    // 假设第一个匹配成功的是完整路径, 从第二个开始, 提取路径参数
    for (size_t i{1}; i < match.size(); ++i) {
        request.SetPathParams("param" + std::to_string(i), match[i].str());
    }
}

}  // namespace cutehttpserver