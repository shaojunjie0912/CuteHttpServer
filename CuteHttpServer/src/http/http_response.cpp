#include <cutehttpserver/http/http_response.hpp>
#include <sstream>

namespace cutehttpserver {

HttpResponse::HttpResponse(bool close)
    : status_code_(HttpStatusCode::kUnknown), close_connection_(close) {}

HttpResponse::~HttpResponse() = default;

void HttpResponse::SetVersion(std::string version) { http_version_ = std::move(version); }

void HttpResponse::SetStatusCode(HttpStatusCode code) { status_code_ = code; }

HttpStatusCode HttpResponse::GetStatusCode() const { return status_code_; }

void HttpResponse::SetStatusMessage(std::string message) { status_message_ = std::move(message); }

void HttpResponse::SetCloseConnection(bool close) { close_connection_ = close; }

bool HttpResponse::IsCloseConnection() const { return close_connection_; }

void HttpResponse::AddHeader(std::string key, std::string value) {
    response_headers_.emplace(std::move(key), std::move(value));
}

void HttpResponse::SetContentType(std::string type) {
    response_headers_["Content-Type"] = std::move(type);
}

void HttpResponse::SetContentLength(uint64_t length) {
    response_headers_["Content-Length"] = std::to_string(length);
}

void HttpResponse::SetResponseBody(std::string body) {
    SetContentLength(body.size());
    response_body_ = std::move(body);
}

std::string HttpResponse::GetResponse() const {
    std::ostringstream oss_response;
    oss_response << http_version_ << " " << std::to_string(static_cast<int>(status_code_)) << " "
                 << status_message_ << "\r\n";
    for (auto const& [key, value] : response_headers_) {
        oss_response << key << ": " << value << "\r\n";
    }
    oss_response << (close_connection_ ? "Connection: close\r\n" : "Connection: keep-alive\r\n")
                 << "\r\n";
    oss_response << response_body_;
    return oss_response.str();
}

void HttpResponse::AppendToBuffer(cutemuduo::Buffer* buff) const {
    buff->Append(GetResponse().c_str());
}

}  // namespace cutehttpserver