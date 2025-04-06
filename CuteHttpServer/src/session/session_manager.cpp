#include <cutehttpserver/session/session_manager.hpp>

namespace cutehttpserver {

SessionManager::SessionManager(std::unique_ptr<SessionStorage> session_storage)
    : session_storage_(std::move(session_storage)), random_generator_(std::random_device{}()) {}

}  // namespace cutehttpserver
