#include <cutemuduo/logger.hpp>
//
#include <cutehttpserver/session/memory_session_storage.hpp>
#include <cutehttpserver/session/session.hpp>

namespace cutehttpserver {

void MemorySessionStorage::Save(std::shared_ptr<Session> session_ptr) {
    sessions_.emplace(session_ptr->GetId(), session_ptr);  // TODO: 新增
}

void MemorySessionStorage::Update(std::shared_ptr<Session> session_ptr) {
    sessions_[session_ptr->GetId()] = session_ptr;  // TODO: 更新
}

std::shared_ptr<Session> MemorySessionStorage::Load(std::string session_id) {
    // NOTE: 加一个判断吧~
    if (session_id.empty()) {
        LOG_ERROR("Session ID is empty, cannot load session.");
        return nullptr;
    }
    if (auto it{sessions_.find(std::move(session_id))}; it == sessions_.end()) {
        LOG_INFO("Session not found, ID: %s", session_id.c_str());
        return nullptr;
    } else {
        if (it->second->IsExpired()) {  // 如果会话过期则删除 TODO: 在 session 存储器中做了过期判断?
            LOG_WARNING("Session expired, removing session with ID: %s", it->first.c_str());
            sessions_.erase(it);
            return nullptr;
        }
        return it->second;
    }
}

void MemorySessionStorage::Remove(std::string session_id) {
    sessions_.erase(std::move(session_id));
}

}  // namespace cutehttpserver
