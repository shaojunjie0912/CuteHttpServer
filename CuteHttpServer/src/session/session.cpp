#include <cutehttpserver/session/session.hpp>

namespace cutehttpserver {

Session::Session(std::string session_id, SessionManager* session_manager, int max_age)
    : session_id_(session_id),
      expiration_time_(std::chrono::system_clock::now() + std::chrono::seconds{max_age_}),
      max_age_(max_age),
      session_manager_(session_manager) {}

Session::~Session() {}

bool Session::IsExpired() const { return std::chrono::system_clock::now() > expiration_time_; }

void Session::SetValue(std::string key, std::string value) {
    data_[std::move(key)] = std::move(value);
    if (session_manager_) {
        session_manager_->UpdateSession(shared_from_this());
    }
}

std::string Session::GetValue(std::string key) const {
    if (auto it{data_.find(std::move(key))}; it != data_.end()) {
        return it->second;
    }
    return "";
}

void Session::SetManager(SessionManager* session_manager) { session_manager_ = session_manager; }

SessionManager* Session::GetManager() const { return session_manager_; }

std::string const& Session::GetId() const { return session_id_; }

void Session::Remove(std::string key) { data_.erase(std::move(key)); }

void Session::Clear() { data_.clear(); }

// void Set
}  // namespace cutehttpserver
