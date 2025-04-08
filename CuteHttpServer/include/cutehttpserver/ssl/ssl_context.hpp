#pragma once

#include <memory>
//
#include <cutemuduo/noncopyable.hpp>
//
#include <openssl/ssl.h>

#include <cutehttpserver/ssl/ssl_config.hpp>

namespace cutehttpserver {

class SslContext : cutemuduo::NonCopyable {
public:
    SslContext(SslConfig const& config);

    ~SslContext();

public:
    bool Init();

    SSL_CTX* GetNativeHandle();

private:
    bool LoadCertificates();

    bool SetupProtocol();

    void SetupSessionCache();

    static void HandleSslError(char const* msg);

private:
    SSL_CTX* ssl_ctx_;      // SSL 上下文
    SslConfig ssl_config_;  // SSL 配置
};

}  // namespace cutehttpserver
