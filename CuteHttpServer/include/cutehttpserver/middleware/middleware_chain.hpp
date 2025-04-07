#pragma once

#include <memory>
#include <vector>
//
#include <cutehttpserver/middleware/middleware.hpp>

namespace cutehttpserver {

class MiddlewareChain {
public:
    void AddMiddleware(std::shared_ptr<Middleware> middleware_ptr);

    void ProcessRequest(HttpRequest& request);

    void ProcessResponse(HttpResponse& response);

private:
    std::vector<std::shared_ptr<Middleware>> middlewares_;
};

}  // namespace cutehttpserver
