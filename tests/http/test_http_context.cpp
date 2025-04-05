#include <cutehttpserver/http/http_context.hpp>
#include <cutemuduo/buffer.hpp>
#include <cutemuduo/timestamp.hpp>
#include <iostream>
#include <string>

using namespace std;
using namespace cutehttpserver::http;
using namespace cutemuduo;

int main() {
    std::string request =
        "POST /api/data?id=123 HTTP/1.1\r\n"
        "Host: 127.0.0.1\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: 20\r\n"
        "\r\n"
        "{\"a\":1,\"b\":\"text\"}\r\n";

    HttpContext http_context;
    Buffer buffer;
    buffer.Append(request.c_str(), request.size());
    if (http_context.ParseRequest(&buffer, Timestamp::Now())) {
        auto request{http_context.GetRequest()};
        cout << "Method: " + request.GetStringMethod() << '\n';
        cout << "Path: " + request.GetPath() << '\n';
        cout << "Version: " + request.GetVersion() << '\n';
        cout << "Content-Length: " + std::to_string(request.GetContentLength()) << '\n';
        cout << "Content: " + request.GetRequestBody() << '\n';
        cout << "Query Params: " + request.GetQueryParams("id") << '\n';
        cout << "Header: " + request.GetHeader("Host") << '\n';
        cout << "Success!\n";
    } else {
        cout << "Failed!\n";
    }
    return 0;
}
