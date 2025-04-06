#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <unordered_map>

namespace cutehttpserver {

class SessionManager;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(std::string session_id, SessionManager* session_manager, int max_age = 3600);

    ~Session();

public:
    bool IsExpired() const;

    void SetValue(std::string key, std::string value);

    // 获取会话数据
    std::string GetValue(std::string key) const;

    // 获取会话 ID
    std::string const& GetId() const;

    void SetManager(SessionManager* session_manager);

    SessionManager* GetManager() const;

    void Remove(std::string key);

    void Clear();

private:
    std::string session_id_;  // 会话 ID
    std::unordered_map<std::string, std::string> data_;
    std::chrono::system_clock::time_point expiration_time_;  // 会话过期时间
    int max_age_;                                            // 会话最大存活时间(秒)
    SessionManager* session_manager_;
};

}  // namespace cutehttpserver
