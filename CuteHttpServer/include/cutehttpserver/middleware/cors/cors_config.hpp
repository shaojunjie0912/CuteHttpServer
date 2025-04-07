#pragma once

#include <string>
#include <vector>

namespace cutehttpserver {

struct CorsConfig {
    std::vector<std::string> allowed_origins;  // 允许的源
    std::vector<std::string> allowed_methods;  // 允许的方法
    std::vector<std::string> allowed_headers;  // 允许的请求头
    bool allow_credentials{false};             // 是否允许凭据
    int max_age{3600};                         // 预检请求的最大有效期

    static CorsConfig DefaultConfig() {
        return {
            .allowed_origins = {"*"},
            .allowed_methods = {"GET", "POST", "PUT", "DELETE", "OPTIONS"},
            .allowed_headers = {"Content-Type", "Authorization"},
        };
    }
};

}  // namespace cutehttpserver
