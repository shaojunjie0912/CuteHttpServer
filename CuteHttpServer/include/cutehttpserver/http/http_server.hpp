#pragma once

#include <functional>
//
#include <cutemuduo/event_loop.hpp>
#include <cutemuduo/inet_address.hpp>
#include <cutemuduo/tcp_server.hpp>
//
#include <cutehttpserver/http/http_context.hpp>
#include <cutehttpserver/http/http_request.hpp>
#include <cutehttpserver/http/http_response.hpp>

namespace cutehttpserver {

class HttpServer : cutemuduo::NonCopyable {
public:
    using HttpCallback = std::function<void(HttpRequest const&, HttpRequest const&)>;

    HttpServer(int port, std::string name, bool use_ssl = false,
               cutemuduo::TcpServer::Option option = cutemuduo::TcpServer::Option::kNoReusePort);

    void SetThreadNum(int num_threads) { tcp_server_.SetThreadNum(num_threads); }

    void Start();

private:
    void Initialize();

    void OnConnection(cutemuduo::TcpConnectionPtr const& conn);

    void OnMessage(cutemuduo::TcpConnectionPtr const& conn, cutemuduo::Buffer* buffer,
                   cutemuduo::Timestamp receive_time);

    void OnRequest(cutemuduo::TcpConnectionPtr const& conn, HttpRequest const& request);

    void HandleRequest(HttpRequest const& request, HttpResponse* response);

private:
    cutemuduo::InetAddress listen_addr_;
    cutemuduo::TcpServer tcp_server_;
    cutemuduo::EventLoop main_loop_;
    HttpCallback http_callback_;
    bool use_ssl_;
};

}  // namespace cutehttpserver
