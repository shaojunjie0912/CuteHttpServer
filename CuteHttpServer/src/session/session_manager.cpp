#include <sstream>
//
#include <cutemuduo/logger.hpp>
//
#include <cutehttpserver/session/session.hpp>
#include <cutehttpserver/session/session_manager.hpp>

// TODO: session_id 过期后怎么处理?

namespace cutehttpserver {

SessionManager::SessionManager(std::unique_ptr<SessionStorage> session_storage)
    : session_storage_ptr_(std::move(session_storage)), random_generator_(std::random_device{}()) {}

std::shared_ptr<Session> SessionManager::GetSession(HttpRequest const& request, HttpResponse* response) {
    // 1. 请求中包含 session_id 且 session_id 有对应的 session 且 session 没有过期
    if (auto session_id{GetSessionIdFromRequestCookie(request)}; !session_id.empty()) {
        if (auto session_ptr{session_storage_ptr_->Load(session_id)};
            !session_ptr && !session_ptr->IsExpired()) {
            session_ptr->SetManager(this);  // 为现有 session 设置管理器 TODO: 新创建的 session 要设置吗
            session_ptr->Refresh();         // 有新的 session 则刷新过期时间
            return session_ptr;
        }
    }
    // 2. 请求中不包含 session_id 或者 session_id 没有对应的 session 或者 session_id 过期
    LOG_INFO("SessionId not found in request or session with sessionid not found/expired!");
    auto new_session_id{GenerateSessionId()};
    auto new_session_ptr{std::make_shared<Session>(new_session_id, this)};
    SetSessionCookie(new_session_id, response);  // HACK: 设置 response 中 cookie
    new_session_ptr->Refresh();                  // 刷新过期时间
    // new_session_ptr->SetManager(this);            // TODO:
    AddSession(new_session_ptr);  // 保存新 session
    return new_session_ptr;
}

void SessionManager::DestroySession(std::string session_id) {
    session_storage_ptr_->Remove(std::move(session_id));
}

void SessionManager::CleanExpiredSessions() {
    // 注意：这个实现依赖于具体的存储实现
    // 对于内存存储，可以在加载时检查是否过期
    // 对于其他存储的实现，可能需要定期清理过期会话
}

void SessionManager::AddSession(std::shared_ptr<Session> session_ptr) {
    session_storage_ptr_->Save(session_ptr);
}

void SessionManager::UpdateSession(std::shared_ptr<Session> session_ptr) {
    session_storage_ptr_->Update(session_ptr);
}

std::string SessionManager::GenerateSessionId() {
    std::ostringstream oss;
    std::uniform_int_distribution<> dist{0, 15};
    for (int i{0}; i < 32; ++i) {
        oss << std::hex << dist(random_generator_);
    }
    return oss.str();
}

std::string SessionManager::GetSessionIdFromRequestCookie(const HttpRequest& request) {
    // Cookie: sessionid=abc123; userid=789xyz
    std::string cookie{request.GetHeader("Cookie")};
    if (cookie.empty()) {
        LOG_INFO("No cookie found in request.");
        return "";
    }
    std::string sessionid_field{"sessionid="};
    size_t field_size{sessionid_field.size()};
    if (size_t begin_pos{cookie.find(sessionid_field)}; begin_pos != std::string::npos) {
        std::string session_id;
        auto past{cookie.substr(begin_pos)};
        if (auto end_pos{past.find(';')}; end_pos != std::string::npos) {
            session_id = past.substr(field_size, end_pos - field_size);
        } else {
            session_id = past.substr(field_size);
        }
        return session_id;
    } else {
        LOG_INFO("No session ID found in cookie.");
        return "";
    }
}

void SessionManager::SetSessionCookie(std::string session_id, HttpResponse* response) {
    // 创建 cookie (绑定 sessionid)
    std::string cookie{"sessionid=" + std::move(session_id) + "; Path=/; HttpOnly"};
    // 增加 cookie 字段
    response->AddHeader("Set-Cookie", cookie);
}

}  // namespace cutehttpserver
