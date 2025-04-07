#include <cutehttpserver/session/session.hpp>
#include <cutehttpserver/session/session_manager.hpp>

namespace cutehttpserver {

// TODO: 建立一个 session 后, 是否需要通过 manager 将其加入 storage 中?
//

Session::Session(std::string session_id, SessionManager* session_manager, int max_age)
    : session_id_(session_id),
      expiration_time_(std::chrono::system_clock::now() + std::chrono::seconds{max_age_}),
      max_age_(max_age),
      session_manager_(session_manager) {}

Session::~Session() {}

void Session::Refresh() {
    expiration_time_ = std::chrono::system_clock::now() + std::chrono::seconds{max_age_};
}

bool Session::IsExpired() const { return std::chrono::system_clock::now() > expiration_time_; }

void Session::SetValue(std::string key, std::string value) {
    session_data_[std::move(key)] = std::move(value);
    // 如果当前 session 有对应的 manager 则更新 session 指针(TODO: 其实没用?)
    // 只是 MemorySessionStorage 的 Update 看上去没啥用
    if (session_manager_) {
        session_manager_->UpdateSession(shared_from_this());
    }
}

std::string Session::GetValue(std::string key) const {
    if (auto it{session_data_.find(std::move(key))}; it != session_data_.end()) {
        return it->second;
    }
    return "";
}

void Session::SetManager(SessionManager* session_manager) { session_manager_ = session_manager; }

SessionManager* Session::GetManager() const { return session_manager_; }

std::string const& Session::GetId() const { return session_id_; }

void Session::Remove(std::string key) { session_data_.erase(std::move(key)); }

void Session::Clear() { session_data_.clear(); }

// void Set
}  // namespace cutehttpserver
