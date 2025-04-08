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

void HttpServer::SetThreadNum(int num_threads) { tcp_server_.SetThreadNum(num_threads); }

void HttpServer::OnMessage(TcpConnectionPtr const& conn, Buffer* buffer, Timestamp receive_time) {
    try {
        if (use_ssl_) {
            LOG_INFO("OnMessage use ssl");
            // TODO: 写不下去了...
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
