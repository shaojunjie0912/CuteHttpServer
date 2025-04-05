#include <sstream>
//
#include <cutehttpserver/http/http_context.hpp>
#include <cutemuduo/buffer.hpp>

namespace cutehttpserver {

bool HttpContext::ParseRequest(cutemuduo::Buffer* buff, cutemuduo::Timestamp const& receive_time) {
    while (true) {
        // GET /index.html HTTP/1.1\r\n ← 请求行
        // Host: www.example.com\r\n    ← 请求头
        // User-Agent: curl/7.68.0\r\n  ← 请求头
        // \r\n                         ← 空行，表示头部结束
        // 请求体（可选）               ← 此后是正文，不以 \r\n 分隔
        switch (state_) {
            // 解析请求行
            case HttpRequestParseState::REQUEST_LINE: {
                auto crlf{buff->FindCRLF()};
                if (crlf) {
                    std::string line(buff->Peek(), crlf);
                    buff->RetrieveUntil(crlf + 2);  // 跳过 \r\n
                    if (!ParseRequestLine(line)) {  // 解析请求行
                        return false;
                    }
                    request_.SetReceiveTime(receive_time);
                    state_ = HttpRequestParseState::HEADERS;  // 转 -> 解析请求头
                } else {
                    return false;  // 没有足够数据(都没有 \r\n)
                }
                break;
            }
            // 解析请求头
            case HttpRequestParseState::HEADERS: {
                auto crlf{buff->FindCRLF()};
                if (crlf) {
                    std::string line(buff->Peek(), crlf);
                    buff->RetrieveUntil(crlf + 2);     // 跳过 \r\n
                    if (!line.empty()) {               // 不是空行
                        if (!ParseHeaderLine(line)) {  // 解析请求头中的一行
                            return false;
                        }
                    } else {  // 遇到空行说明头部解析结束
                        auto content_length{request_.GetHeader("Content-Length")};
                        if (content_length.empty()) {  // 没有 Content-Length 说明没有请求体
                            state_ = HttpRequestParseState::FINISH;  // 可以结束了
                            return true;
                        } else {  // 有 Content-Length 继续解析请求体
                            request_.SetContentLength(std::stoul(content_length));
                            state_ = HttpRequestParseState::BODY;
                        }
                    }
                } else {
                    return false;  // 需要更多数据
                }
                break;
            }
            // 解析请求体
            case HttpRequestParseState::BODY: {
                auto content_length{request_.GetContentLength()};
                if (buff->ReadableBytes() >= content_length) {
                    std::string body_data(buff->Peek(), buff->Peek() + content_length);
                    buff->Retrieve(content_length);
                    if (!ParseBody(body_data)) {  // 解析请求体
                        return false;
                    }
                    state_ = HttpRequestParseState::FINISH;
                    return true;
                } else {
                    return false;  // 需要更多数据
                }
                break;
            }
            case HttpRequestParseState::FINISH: {
                return true;
                break;
            }
        }
    }
}

bool HttpContext::ParseRequestLine(std::string const& line) {
    // GET /path/resource?key1=value1&key2=value2 HTTP/1.1
    std::istringstream iss{line};
    std::string method, path_query, version;
    iss >> method >> path_query >> version;  // NOTE: '>>' 一次性获取
    if (method.empty() || path_query.empty() || version.empty()) {
        return false;
    }
    request_.SetMethod(method);
    request_.SetVersion(version);

    // NOTE: /path?key1=value1&key2=value2
    // '?' 分割了 path 和 query
    // '&' 分割键值对
    // '=' 分割键和值
    auto pos{path_query.find('?')};
    if (pos != std::string::npos) {  // HACK: npos
        request_.SetPath(path_query.substr(0, pos));
        auto query_str{path_query.substr(pos + 1)};
        std::istringstream qs{query_str};  //
        std::string kv;                    // 键值对
        while (std::getline(qs, kv, '&')) {
            auto eq_pos{kv.find('=')};  // '=' 位置
            if (eq_pos != std::string::npos) {
                request_.SetQueryParams(kv.substr(0, eq_pos), kv.substr(eq_pos + 1));
            }
        }
    } else {
        request_.SetPath(path_query);
    }
    return true;
}

bool HttpContext::ParseHeaderLine(std::string const& line) {
    auto pos{line.find(':')};
    if (pos == std::string::npos) {
        return false;
    }
    auto key{line.substr(0, pos)};
    auto value{line.substr(pos + 1)};
    key.erase(key.find_last_not_of(" \t") + 1);      // 删除 key 末尾空格
    value.erase(0, value.find_first_not_of(" \t"));  // 删除 value 首部空格
    request_.AddHeader(key, value);
    return true;
}

bool HttpContext::ParseBody(std::string body_data) {
    request_.SetRequestBody(std::move(body_data));
    return true;
}

}  // namespace cutehttpserver
