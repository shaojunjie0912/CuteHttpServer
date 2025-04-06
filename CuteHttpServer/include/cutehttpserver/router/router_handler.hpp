#pragma once

namespace cutehttpserver {

class HttpRequest;
class HttpResponse;

class RouterHandler {
public:
    virtual ~RouterHandler() = default;

    // 处理请求的纯虚函数(业务层继承此抽象类并重写该函数)
    virtual void Handle(HttpRequest const& request, HttpResponse* response);
};

}  // namespace cutehttpserver
