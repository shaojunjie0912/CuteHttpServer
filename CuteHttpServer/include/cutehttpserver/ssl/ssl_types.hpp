#pragma once

namespace cutehttpserver {

// SSL/TLS 版本
enum class SslVersion {
    TLS_1_0,
    TLS_1_1,
    TLS_1_2,
    TLS_1_3,
};

// SSL 错误类型
enum class SslError {
    NONE,
    WANT_READ,
    WANT_WRITE,
    SYSCALL,
    SSL,
    UNKNOWN,
};

// SSL 连接状态
enum class SslState {
    HANDSHAKE,
    ESTABLISHED,
    SHUTDOWN,
    ERROR,
};

}  // namespace cutehttpserver
