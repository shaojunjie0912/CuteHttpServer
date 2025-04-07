#pragma once

#include <memory>
//
#include <cutehttpserver/http/http_request.hpp>
#include <cutehttpserver/http/http_response.hpp>

namespace cutehttpserver {

// 中间件基类
class Middleware {
public:
    virtual ~Middleware() = default;

    // 请求前处理
    virtual void HandleRequest(HttpRequest& request) = 0;

    // 响应后处理
    virtual void HandleResponse(HttpResponse& response) = 0;

    // 设置下一个中间件
    void SetNextMiddleware(std::shared_ptr<Middleware> next_middleware_ptr) {
        next_middleware_ptr_ = next_middleware_ptr;
    }

protected:
    std::shared_ptr<Middleware> next_middleware_ptr_;
};

}  // namespace cutehttpserver
