#include <cutehttpserver/session/memory_session_storage.hpp>
#include <cutehttpserver/session/session.hpp>

namespace cutehttpserver {

void MemorySessionStorage::Save(std::shared_ptr<Session> session_ptr) {
    sessions_[session_ptr->GetId()] = session_ptr;
}

std::shared_ptr<Session> MemorySessionStorage::Load(std::string session_id) {
    if (auto it{sessions_.find(std::move(session_id))}; it != sessions_.end()) {
        if (it->second->IsExpired()) {  // 如果会话过期则删除
            sessions_.erase(it);
            return nullptr;
        } else {
            return it->second;
        }
    }
    return nullptr;
}

void MemorySessionStorage::Remove(std::string session_id) {
    sessions_.erase(std::move(session_id));
}

}  // namespace cutehttpserver
