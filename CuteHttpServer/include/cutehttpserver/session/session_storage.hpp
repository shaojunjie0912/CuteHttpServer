#pragma once

#include <memory>

namespace cutehttpserver {

class Session;

// 会话存储基类
class SessionStorage {
public:
    virtual ~SessionStorage() = default;

    virtual void Save(std::shared_ptr<Session> session) = 0;

    virtual std::shared_ptr<Session> Load(std::string session_id) = 0;

    virtual void Remove(std::string session_id) = 0;
};

}  // namespace cutehttpserver
