#include <openssl/err.h>

#include <cutemuduo/logger.hpp>

//
#include <cutehttpserver/ssl/ssl_context.hpp>

namespace cutehttpserver {

SslContext::SslContext(SslConfig const& config) : ssl_ctx_(nullptr), ssl_config_(config) {}

SslContext::~SslContext() {
    if (ssl_ctx_) {
        SSL_CTX_free(ssl_ctx_);
        ssl_ctx_ = nullptr;
    }
}

bool SslContext::Init() {
    // 初始化 OpenSSL
    OPENSSL_init_ssl(OPENSSL_INIT_LOAD_SSL_STRINGS | OPENSSL_INIT_LOAD_CRYPTO_STRINGS, nullptr);

    // 创建 SSL 上下文
    const SSL_METHOD* method = TLS_server_method();
    ssl_ctx_ = SSL_CTX_new(method);
    if (!ssl_ctx_) {
        HandleSslError("Failed to create SSL context");
        return false;
    }

    // 设置 SSL 选项
    long options =
        SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_COMPRESSION | SSL_OP_CIPHER_SERVER_PREFERENCE;
    SSL_CTX_set_options(ssl_ctx_, options);

    // 加载证书和私钥
    if (!LoadCertificates()) {
        return false;
    }

    // 设置协议版本
    if (!SetupProtocol()) {
        return false;
    }

    // 设置会话缓存
    SetupSessionCache();

    LOG_INFO("SSL context initialized successfully");
    return true;
}

SSL_CTX* SslContext::GetNativeHandle() { return ssl_ctx_; }

bool SslContext::LoadCertificates() {
    // 加载证书
    if (SSL_CTX_use_certificate_file(ssl_ctx_, ssl_config_.GetCertificateFile().c_str(), SSL_FILETYPE_PEM) <=
        0) {
        HandleSslError("Failed to load server certificate");
        return false;
    }

    // 加载私钥
    if (SSL_CTX_use_PrivateKey_file(ssl_ctx_, ssl_config_.GetPrivateKeyFile().c_str(), SSL_FILETYPE_PEM) <=
        0) {
        HandleSslError("Failed to load private key");
        return false;
    }

    // 验证私钥
    if (!SSL_CTX_check_private_key(ssl_ctx_)) {
        HandleSslError("Private key does not match the certificate");
        return false;
    }

    // 加载证书链
    if (!ssl_config_.GetCertificateChainFile().empty()) {
        if (SSL_CTX_use_certificate_chain_file(ssl_ctx_, ssl_config_.GetCertificateChainFile().c_str()) <=
            0) {
            HandleSslError("Failed to load certificate chain");
            return false;
        }
    }
    return true;
}

bool SslContext::SetupProtocol() {
    // 设置 SSL/TLS 协议版本
    long options = SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3;
    switch (ssl_config_.GetProtocolVersion()) {
        case SslVersion::TLS_1_0:
            options |= SSL_OP_NO_TLSv1;
            break;
        case SslVersion::TLS_1_1:
            options |= SSL_OP_NO_TLSv1_1;
            break;
        case SslVersion::TLS_1_2:
            options |= SSL_OP_NO_TLSv1_2;
            break;
        case SslVersion::TLS_1_3:
            options |= SSL_OP_NO_TLSv1_3;
            break;
    }
    SSL_CTX_set_options(ssl_ctx_, options);

    // 设置加密套件
    if (!ssl_config_.GetCipherList().empty()) {
        if (SSL_CTX_set_cipher_list(ssl_ctx_, ssl_config_.GetCipherList().c_str()) <= 0) {
            HandleSslError("Failed to set cipher list");
            return false;
        }
    }

    return true;
}

void SslContext::SetupSessionCache() {
    SSL_CTX_set_session_cache_mode(ssl_ctx_, SSL_SESS_CACHE_SERVER);
    SSL_CTX_sess_set_cache_size(ssl_ctx_, ssl_config_.GetSessionCacheSize());
    SSL_CTX_set_timeout(ssl_ctx_, ssl_config_.GetSessionTimeout());
}

void SslContext::HandleSslError(char const* msg) {
    char buf[256];
    ERR_error_string_n(ERR_get_error(), buf, sizeof(buf));
    LOG_ERROR("%s: %s", msg, buf);
}

}  // namespace cutehttpserver