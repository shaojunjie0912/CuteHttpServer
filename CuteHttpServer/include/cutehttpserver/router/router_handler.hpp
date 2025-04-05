#pragma once

namespace cutehttpserver {

class HttpRequest;
class HttpResponse;

class RouterHandler {
public:
    virtual ~RouterHandler() = default;

    virtual void Handle(HttpRequest const& request, HttpResponse* response);
};

}  // namespace cutehttpserver
