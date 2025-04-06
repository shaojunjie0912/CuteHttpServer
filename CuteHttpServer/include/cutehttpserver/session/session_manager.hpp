#pragma once

#include <memory>
#include <random>

namespace cutehttpserver {

class Session;
class SessionStorage;
class HttpRequest;
class HttpResponse;

class SessionManager {
public:
    explicit SessionManager(std::unique_ptr<SessionStorage> session_storage);

    std::shared_ptr<Session> GetSession(HttpRequest const& req, HttpResponse* resp);

    void DestroySession(const std::string& sessionId);

    void CleanExpiredSessions();

    void UpdateSession(std::shared_ptr<Session> session);

private:
    std::string GenerateSessionId();

    std::string GetSessionIdFromCookie(const HttpRequest& req);

    void SetSessionCookie(const std::string& sessionId, HttpResponse* resp);

private:
    std::unique_ptr<SessionStorage> session_storage_;
    std::mt19937 random_generator_;
};

}  // namespace cutehttpserver
