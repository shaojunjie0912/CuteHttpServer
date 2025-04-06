#pragma once

#include <cutehttpserver/session/session_storage.hpp>
#include <string>
#include <unordered_map>

namespace cutehttpserver {

class Session;

class MemorySessionStorage : public SessionStorage {
public:
    // 存储 session
    void Save(std::shared_ptr<Session> session_ptr) override;

    // 加载 session
    std::shared_ptr<Session> Load(std::string session_id) override;

    // 删除 session
    void Remove(std::string session_id) override;

private:
    std::unordered_map<std::string, std::shared_ptr<Session>> sessions_;
};

}  // namespace cutehttpserver
