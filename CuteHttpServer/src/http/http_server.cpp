#include <any>
//
#include <cutemuduo/logger.hpp>
//
#include <cutehttpserver/http/http_server.hpp>

namespace cutehttpserver {

HttpServer::HttpServer(int port, std::string name, bool use_ssl, TcpServer::Option option)
    : listen_addr_(port),
      tcp_server_(&main_loop_, listen_addr_, name, option),
      http_callback_(
          [this](HttpRequest const& request, HttpResponse* response) { HandleRequest(request, response); }),
      use_ssl_(use_ssl) {
    // 设置回调函数
    tcp_server_.SetConnectionCallback([this](TcpConnectionPtr const& conn) { OnConnection(conn); });
    tcp_server_.SetMessageCallback([this](TcpConnectionPtr const& conn, Buffer* buffer,
                                          Timestamp receive_time) { OnMessage(conn, buffer, receive_time); });
}

void HttpServer::Start() {
    LOG_INFO("HttpServer[%s] starts listening on %s", tcp_server_.name().c_str(),
             tcp_server_.ip_port().c_str());
    tcp_server_.Start();
    main_loop_.Loop();
}

EventLoop* HttpServer::GetLoop() const { return tcp_server_.loop(); }

void HttpServer::SetThreadNum(int num_threads) { tcp_server_.SetThreadNum(num_threads); }

void HttpServer::SetHttpCallback(HttpCallback cb) { http_callback_ = std::move(cb); }

void HttpServer::Get(std::string path, HttpCallback cb) {
    router_.RegisterCallback(HttpRequestMethod::kGet, std::move(path), std::move(cb));
}

void HttpServer::Get(std::string path, Router::HandlerPtr handler) {
    router_.RegisterHandler(HttpRequestMethod::kGet, std::move(path), handler);
}

void HttpServer::Post(std::string path, HttpCallback cb) {
    router_.RegisterCallback(HttpRequestMethod::kPost, std::move(path), std::move(cb));
}

void HttpServer::Post(std::string path, Router::HandlerPtr handler) {
    router_.RegisterHandler(HttpRequestMethod::kPost, std::move(path), handler);
}

void HttpServer::AddRoute(HttpRequestMethod method, std::string path, Router::HandlerPtr handler) {
    router_.RegisterRegexHandler(method, std::move(path), handler);
}

void HttpServer::AddRoute(HttpRequestMethod method, std::string path, Router::HandlerCallback cb) {
    router_.RegisterRegexCallback(method, std::move(path), std::move(cb));
}

void HttpServer::SetSessionManager(std::unique_ptr<SessionManager> manager) {
    session_manager_ = std::move(manager);
}

SessionManager* HttpServer::GetSessionManager() const { return session_manager_.get(); }

void HttpServer::AddMiddleware(std::shared_ptr<Middleware> middleware) {
    middleware_chain_.AddMiddleware(middleware);
}

void HttpServer::EnableSsl(bool enable) { use_ssl_ = enable; }

void HttpServer::SetSslConfig(SslConfig const& config) {
    if (use_ssl_) {
        ssl_context_ = std::make_unique<SslContext>(config);
        if (!ssl_context_->Init()) {
            LOG_ERROR("Failed to initialize SSL context");
            std::terminate();
        }
    }
}

// TODO: 回顾
void HttpServer::OnConnection(TcpConnectionPtr const& conn) {
    if (conn->IsConnected()) {
        if (use_ssl_) {
            auto ssl_conn = std::make_unique<SslConnection>(conn, ssl_context_.get());
            ssl_conn->SetMessageCallback(
                [this](TcpConnectionPtr const& conn, Buffer* buffer, Timestamp receive_time) {
                    OnMessage(conn, buffer, receive_time);
                });
            ssl_connections_[conn] = std::move(ssl_conn);
            ssl_connections_[conn]->StartHandshake();
        }
        conn->SetContext(HttpContext{});
    } else {
        if (use_ssl_) {
            ssl_connections_.erase(conn);
        }
    }
}

// TODO: 回顾
void HttpServer::OnMessage(TcpConnectionPtr const& conn, Buffer* buf, Timestamp receive_time) {
    try {
        if (use_ssl_) {
            LOG_INFO("OnMessage use ssl is true");
            // 1.查找对应的SSL连接
            auto it = ssl_connections_.find(conn);
            if (it != ssl_connections_.end()) {
                LOG_INFO("onMessage sslConns_ is not empty");
                // 2. SSL连接处理数据
                it->second->OnRead(conn, buf, receive_time);

                // 3. 如果 SSL 握手还未完成，直接返回
                if (!it->second->IsHandshakeCompleted()) {
                    LOG_INFO("onMessage sslConns_ is not empty");
                    return;
                }

                // 4. 从SSL连接的解密缓冲区获取数据
                Buffer* decrypted_buf = it->second->GetDecryptedBuffer();
                if (decrypted_buf->ReadableBytes() == 0) {
                    return;  // 没有解密后的数据
                }

                // 5. 使用解密后的数据进行HTTP 处理
                buf = decrypted_buf;  // 将 buf 指向解密后的数据
                LOG_INFO("onMessage decryptedBuf is not empty");
            }
        }
        // HttpContext对象用于解析出buf中的请求报文，并把报文的关键信息封装到HttpRequest对象中
        HttpContext* context = std::any_cast<HttpContext>(conn->GetMutableContext());
        if (!context->ParseRequest(buf, receive_time))  // 解析一个http请求
        {
            // 如果解析http报文过程中出错
            conn->Send("HTTP/1.1 400 Bad Request\r\n\r\n");
            conn->Shutdown();
        }
        // 如果buf缓冲区中解析出一个完整的数据包才封装响应报文
        if (context->GotAll()) {
            OnRequest(conn, context->GetRequest());
            context->Reset();
        }
    } catch (std::exception const& e) {
        LOG_ERROR("Exception: OnMessage: %s", e.what());
        conn->Send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->Shutdown();
    }
}

void HttpServer::OnRequest(TcpConnectionPtr const& conn, HttpRequest const& request) {
    auto connection_field{request.GetHeader("Connection")};
    bool close{(connection_field == "close") ||
               (request.GetVersion() == "HTTP/1.0" && connection_field != "Keep-Alive")};
    HttpResponse response{close};
    // 根据请求报文信息封装响应报文
    http_callback_(request, &response);

    Buffer buffer;
    response.AppendToBuffer(&buffer);
    LOG_INFO("Sending response: %s", buffer.ToString().c_str());
    conn->Send(&buffer);
    // 短连接返回响应报文后直接断开连接
    if (response.IsCloseConnection()) {
        conn->Shutdown();
    }
}

void HttpServer::HandleRequest(HttpRequest const& request, HttpResponse* response) {
    try {
        auto mutable_request{request};
        // 中间件(前)
        middleware_chain_.ProcessRequest(mutable_request);
        // 路由处理
        if (!router_.Route(mutable_request, response)) {
            response->SetStatusCode(HttpStatusCode::k404NotFound);
            response->SetResponseBody("404 Not Found");
            response->SetCloseConnection(true);
        }
        // 中间件(后)
        middleware_chain_.ProcessResponse(*response);
    }
    // CORS 预检
    catch (HttpResponse const& resp) {
        *response = resp;
    }
    // 其他异常
    catch (std::exception const& e) {
        response->SetStatusCode(HttpStatusCode::k500InternalServerError);
        response->SetResponseBody(e.what());
    }
}

}  // namespace cutehttpserver
