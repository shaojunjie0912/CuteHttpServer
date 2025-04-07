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
    // 刷新会话过期时间(延长)
    void Refresh();

    // 判断会话是否过期
    bool IsExpired() const;

    // TODO: 存储? 会修改吗? 在会话数据中存储键值对??
    void SetValue(std::string key, std::string value);

    // 从会话数据中按键检索值
    std::string GetValue(std::string key) const;

    // 获取会话 ID
    std::string const& GetId() const;

    // 设置会话管理器
    void SetManager(SessionManager* session_manager);

    // 获取会话管理器
    SessionManager* GetManager() const;

    // 从会话数据中移除键值对
    void Remove(std::string key);

    // 清空会话数据
    void Clear();

private:
    std::string session_id_;                                     // 会话 ID
    std::unordered_map<std::string, std::string> session_data_;  // 会话数据
    std::chrono::system_clock::time_point expiration_time_;      // 会话过期时间
    int max_age_;                                                // 会话最大存活时间(秒)
    SessionManager* session_manager_;
};

}  // namespace cutehttpserver
