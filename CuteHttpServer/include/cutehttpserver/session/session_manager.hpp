#pragma once

#include <memory>
#include <random>
//
#include <cutehttpserver/http/http_request.hpp>
#include <cutehttpserver/http/http_response.hpp>
#include <cutehttpserver/session/session_storage.hpp>

namespace cutehttpserver {

class Session;

class SessionManager {
public:
    explicit SessionManager(std::unique_ptr<SessionStorage> session_storage_ptr);

    // 获取会话 (如果)
    std::shared_ptr<Session> GetSession(HttpRequest const& request, HttpResponse* response);

    // 销毁会话
    void DestroySession(std::string session_id);

    // 清理过期会话
    void CleanExpiredSessions();

    // 添加会话
    void AddSession(std::shared_ptr<Session> session_ptr);

    // 更新会话
    void UpdateSession(std::shared_ptr<Session> session_ptr);

private:
    // 生成 session_id (32 个 16 进制数字组成的字符串)
    std::string GenerateSessionId();

    // 从请求报文的请求头中的 cookie 中获取 session_id
    std::string GetSessionIdFromRequestCookie(const HttpRequest& request);

    // 在响应报文的响应头中添加 cookie 字段并绑定 session
    void SetSessionCookie(std::string session_id, HttpResponse* response);

private:
    std::unique_ptr<SessionStorage> session_storage_ptr_;
    std::mt19937 random_generator_;  // 随机数生成器
};

}  // namespace cutehttpserver
