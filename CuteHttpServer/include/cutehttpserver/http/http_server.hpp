#pragma once

// std
#include <functional>
#include <unordered_map>
// cutemuduo
#include <cutemuduo/event_loop.hpp>
#include <cutemuduo/inet_address.hpp>
#include <cutemuduo/tcp_server.hpp>
// cutehttpserver
#include <cutehttpserver/http/http_context.hpp>
#include <cutehttpserver/http/http_request.hpp>
#include <cutehttpserver/http/http_response.hpp>
#include <cutehttpserver/middleware/middleware_chain.hpp>
#include <cutehttpserver/router/router.hpp>
#include <cutehttpserver/session/session_manager.hpp>
#include <cutehttpserver/ssl/ssl_connection.hpp>
#include <cutehttpserver/ssl/ssl_context.hpp>

namespace cutehttpserver {

using cutemuduo::Buffer;
using cutemuduo::EventLoop;
using cutemuduo::TcpConnectionPtr;
using cutemuduo::TcpServer;
using cutemuduo::Timestamp;

class HttpServer : cutemuduo::NonCopyable {
public:
    using HttpCallback = std::function<void(HttpRequest const&, HttpResponse*)>;

    HttpServer(int port, std::string name, bool use_ssl = false,
               TcpServer::Option option = TcpServer::Option::kNoReusePort);

public:
    void Start();

public:
    void SetThreadNum(int num_threads);

    EventLoop* GetLoop() const;

    void SetHttpCallback(HttpCallback cb);

    void Get(std::string path, HttpCallback cb);

    void Get(std::string path, Router::HandlerPtr handler);

    void Post(std::string path, HttpCallback cb);

    void Post(std::string path, Router::HandlerPtr handler);

    void AddRoute(HttpRequestMethod method, std::string path, Router::HandlerPtr handler);

    void AddRoute(HttpRequestMethod method, std::string path, Router::HandlerCallback cb);

    void SetSessionManager(std::unique_ptr<SessionManager> manager);

    SessionManager* GetSessionManager() const;

    void AddMiddleware(std::shared_ptr<Middleware> middleware);

    void EnableSsl(bool enable);

    void SetSslConfig(SslConfig const& config);

private:
    void OnConnection(TcpConnectionPtr const& conn);

    void OnMessage(TcpConnectionPtr const& conn, Buffer* buffer, Timestamp receive_time);

    void OnRequest(TcpConnectionPtr const& conn, HttpRequest const& request);

    void HandleRequest(HttpRequest const& request, HttpResponse* response);

private:
    cutemuduo::InetAddress listen_addr_;                                                    // 监听地址
    cutemuduo::TcpServer tcp_server_;                                                       // TCP 服务器
    cutemuduo::EventLoop main_loop_;                                                        // 主循环
    HttpCallback http_callback_;                                                            //
    Router router_;                                                                         // 路由
    std::unique_ptr<SessionManager> session_manager_;                                       // 会话管理器
    MiddlewareChain middleware_chain_;                                                      // 中间件链
    bool use_ssl_;                                                                          // 是否使用 SSL
    std::unique_ptr<SslContext> ssl_context_;                                               // SSL 上下文
    std::unordered_map<TcpConnectionPtr, std::unique_ptr<SslConnection>> ssl_connections_;  // SSL 连接
};

}  // namespace cutehttpserver
