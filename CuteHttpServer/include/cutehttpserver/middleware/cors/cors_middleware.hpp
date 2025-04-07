#pragma once

#include <cutehttpserver/http/http_request.hpp>
#include <cutehttpserver/http/http_response.hpp>
#include <cutehttpserver/middleware/cors/cors_config.hpp>
#include <cutehttpserver/middleware/middleware.hpp>

namespace cutehttpserver {

class CorsMiddleware : public Middleware {
public:
    CorsMiddleware(CorsConfig const& cors_config = CorsConfig::DefaultConfig());

    void HandleRequest(HttpRequest& request) override;

    // TODO: 处理响应
    void HandleResponse(HttpResponse& response) override;

private:
    // 检查请求发起源是否被允许
    bool IsOriginAllowed(const std::string& origin) const;

    void HandlePreflightRequest(HttpRequest const& request, HttpResponse& response);

    void AddCorsHeaders(HttpResponse& response, const std::string& origin);

private:
    // 将字符串数组拼接成一个字符串
    std::string Join(const std::vector<std::string>& strings, const std::string& delimiter);

private:
    CorsConfig cors_config_;
};

}  // namespace cutehttpserver
