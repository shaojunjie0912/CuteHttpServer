#include <cutehttpserver/ssl/ssl_config.hpp>

namespace cutehttpserver {

SslConfig::SslConfig()
    : ssl_version_(SslVersion::TLS_1_2),
      cipher_list_("HIGH:!aNULL:!MD5"),
      verify_client_(false),
      verify_depth_(1),
      session_timeout_(300),
      session_cache_size_(1024 * 1024) {}

void SslConfig::SetCertificateFile(std::string certificate_file) {
    certificate_file_ = std::move(certificate_file);
}

std::string SslConfig::GetCertificateFile() const { return certificate_file_; }

void SslConfig::SetPrivateKeyFile(std::string private_key_file) {
    private_key_file_ = std::move(private_key_file);
}

std::string SslConfig::GetPrivateKeyFile() const { return private_key_file_; }

void SslConfig::SetCertificateChainFile(std::string certificate_chain_file) {
    certificate_chain_file_ = std::move(certificate_chain_file);
}

std::string SslConfig::GetCertificateChainFile() const { return certificate_chain_file_; }

void SslConfig::SetProtocolVersion(SslVersion ssl_version) { ssl_version_ = ssl_version; }

SslVersion SslConfig::GetProtocolVersion() const { return ssl_version_; }

void SslConfig::SetCipherList(std::string cipher_list) { cipher_list_ = std::move(cipher_list); }

std::string SslConfig::GetCipherList() const { return cipher_list_; }

void SslConfig::SetVerifyClient(bool verify_client) { verify_client_ = verify_client; }

bool SslConfig::GetVerifyClient() const { return verify_client_; }

void SslConfig::SetVerifyDepth(int verify_depth) { verify_depth_ = verify_depth; }

int SslConfig::GetVerifyDepth() const { return verify_depth_; }

void SslConfig::SetSessionTimeout(int session_timeout) { session_timeout_ = session_timeout; }

int SslConfig::GetSessionTimeout() const { return session_timeout_; }

void SslConfig::SetSessionCacheSize(long long session_cache_size) {
    session_cache_size_ = session_cache_size;
}

long long SslConfig::GetSessionCacheSize() const { return session_cache_size_; }

}  // namespace cutehttpserver
