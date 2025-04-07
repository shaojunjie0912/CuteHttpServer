#pragma once

#include <cutehttpserver/session/session_storage.hpp>
#include <string>
#include <unordered_map>

namespace cutehttpserver {

class Session;

class MemorySessionStorage : public SessionStorage {
public:
    // 存储新的 session
    void Save(std::shared_ptr<Session> session_ptr) override;

    // TODO: 更新旧的 session? 也还是用的 session_ptr 的 session_id 呀
    // 好像没啥用?
    void Update(std::shared_ptr<Session> session_ptr) override;

    // 加载 session
    std::shared_ptr<Session> Load(std::string session_id) override;

    // 删除 session
    void Remove(std::string session_id) override;

private:
    // 存储 session 的哈希表 {session_id : session}
    std::unordered_map<std::string, std::shared_ptr<Session>> sessions_;
};

}  // namespace cutehttpserver
