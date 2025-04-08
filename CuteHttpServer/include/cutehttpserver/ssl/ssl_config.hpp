#pragma once

#include <string>
//
#include <cutehttpserver/ssl/ssl_types.hpp>

namespace cutehttpserver {

class SslConfig {
public:
    // TODO: files?
    SslConfig();

    ~SslConfig() = default;

public:
    void SetCertificateFile(std::string certificate_file);
    std::string GetCertificateFile() const;

    void SetPrivateKeyFile(std::string private_key_file);
    std::string GetPrivateKeyFile() const;

    void SetCertificateChainFile(std::string certificate_chain_file);
    std::string GetCertificateChainFile() const;

    void SetProtocolVersion(SslVersion ssl_version);
    SslVersion GetProtocolVersion() const;

    void SetCipherList(std::string cipher_list);
    std::string GetCipherList() const;

    void SetVerifyClient(bool verify_client);
    bool GetVerifyClient() const;

    void SetVerifyDepth(int verify_depth);
    int GetVerifyDepth() const;

    void SetSessionTimeout(int session_timeout);
    int GetSessionTimeout() const;

    void SetSessionCacheSize(long long session_cache_size);
    long long GetSessionCacheSize() const;

private:
    std::string certificate_file_;        // 证书文件
    std::string private_key_file_;        // 私钥文件
    std::string certificate_chain_file_;  // 证书链文件
    SslVersion ssl_version_;              // SSL版本
    std::string cipher_list_;             // 密码列表(加密套件?)
    bool verify_client_;                  // 是否启用客户端证书验证(即双向认证)
    int verify_depth_;                    // 客户端证书链的最大验证深度
    int session_timeout_;                 // ssl 会话超时时间 (默认 300s)
    long long session_cache_size_;        // 会话缓存大小(默认 1MB)
};

}  // namespace cutehttpserver
