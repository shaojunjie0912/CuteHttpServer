#include <algorithm>
#include <exception>
#include <ranges>
#include <sstream>
//
#include <cutemuduo/logger.hpp>
//
#include <cutehttpserver/middleware/cors/cors_middleware.hpp>

namespace cutehttpserver {

CorsMiddleware::CorsMiddleware(CorsConfig const& cors_config) { cors_config_ = cors_config; }

void CorsMiddleware::HandleRequest(HttpRequest& request) {
    LOG_DEBUG("CORS middleware handle request");
    // NOTE: CORS 预检请求?
    if (request.GetMethod() == HttpRequestMethod::kOptions) {
        LOG_INFO("Handling CORS preflight request");
        HttpResponse response;
        HandlePreflightRequest(request, response);
        throw response;
    }
}

void CorsMiddleware::HandleResponse(HttpResponse& response) {
    LOG_DEBUG("CORS middleware handle response");
    // TODO: 简化了处理逻辑: 要么允许 "*" 要么允许第一个 origin (估计也只放了一个 origin)
    if (!cors_config_.allowed_origins.empty()) {
        // if (IsOriginAllowed(origin)) {} // TODO: 这个没法做, 参数没有 request, 不知道原origin
        if (std::ranges::find(cors_config_.allowed_origins, "*") != cors_config_.allowed_origins.end()) {
            AddCorsHeaders(response, "*");
        } else {
            // TODO:
            AddCorsHeaders(response,
                           cors_config_.allowed_origins[0]);  // 添加 config 中第一个允许的源
        }
    }
}

bool CorsMiddleware::IsOriginAllowed(const std::string& origin) const {
    // C++20 ranges::any_of lambda
    return std::ranges::any_of(cors_config_.allowed_origins, [&](std::string const& allowed_origin) {
        return allowed_origin == "*" || allowed_origin == origin;
    });
}

void CorsMiddleware::HandlePreflightRequest(HttpRequest const& request, HttpResponse& response) {
    auto origin{request.GetHeader("Origin")};

    if (!IsOriginAllowed(origin)) {
        LOG_WARNING("Origin not allowed: %s", origin.c_str());
        response.SetStatusCode(HttpStatusCode::k403Forbidden);
        return;
    }

    AddCorsHeaders(response, origin);
    response.SetStatusCode(HttpStatusCode::k204NoContent);  // TODO: 为啥是 204 NoContent
    LOG_INFO("Preflight request handled successfully");
}

void CorsMiddleware::AddCorsHeaders(HttpResponse& response, const std::string& origin) {
    // HTTP/1.1 200 OK
    // Content-Type: application/json
    // Access-Control-Allow-Origin: https://client.example.com
    // Access-Control-Allow-Methods: GET, POST, PUT, DELETE
    // Access-Control-Allow-Headers: Content-Type, Authorization
    // Access-Control-Allow-Credentials: true
    // Access-Control-Max-Age: 3600
    try {
        // 设置允许的源
        response.AddHeader("Access-Control-Allow-Origin", origin);

        // 设置允许的方法
        if (!cors_config_.allowed_methods.empty()) {
            response.AddHeader("Access-Control-Allow-Methods", Join(cors_config_.allowed_methods, ", "));
        }

        // 设置允许的请求头
        if (!cors_config_.allowed_headers.empty()) {
            response.AddHeader("Access-Control-Allow-Headers", Join(cors_config_.allowed_headers, ", "));
        }

        // 设置允许凭证
        if (cors_config_.allow_credentials) {
            response.AddHeader("Access-Control-Allow-Credentials", "true");
        }

        // 设置预检请求的有效期
        response.AddHeader("Access-Control-Max-Age", std::to_string(cors_config_.max_age));
        LOG_DEBUG("CORS headers added: %s", response.GetHeaders().c_str());
    } catch (std::exception const& e) {
        LOG_ERROR("Failed to add CORS headers: %s", e.what());
    }
}

std::string CorsMiddleware::Join(const std::vector<std::string>& strings, const std::string& delimiter) {
    std::ostringstream oss;
    for (size_t i = 0; i < strings.size(); ++i) {
        if (i != 0) {
            oss << delimiter;
        }
        oss << strings[i];
    }
    return oss.str();
}

// void CorsMiddleware

}  // namespace cutehttpserver
