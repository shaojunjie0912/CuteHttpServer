#include <exception>
//
#include <cutemuduo/logger.hpp>
//
#include <cutehttpserver/middleware/middleware_chain.hpp>

namespace cutehttpserver {

void MiddlewareChain::AddMiddleware(std::shared_ptr<Middleware> middleware_ptr) {
    middlewares_.push_back(middleware_ptr);
}

void MiddlewareChain::ProcessRequest(HttpRequest& request) {
    for (auto const& middleware : middlewares_) {
        middleware->HandleRequest(request);
    }
}

void MiddlewareChain::ProcessResponse(HttpResponse& response) {
    try {
        for (auto it{middlewares_.rbegin()}; it != middlewares_.rend(); ++it) {
            // TODO: 是否添加空指针检查?
            (*it)->HandleResponse(response);
        }
    } catch (std::exception const& e) {
        LOG_ERROR("Error in middleware process response: %s", e.what());
    }
}

}  // namespace cutehttpserver
