#pragma once

#include <openssl/ssl.h>

#include <memory>
//
#include <cutemuduo/buffer.hpp>
#include <cutemuduo/noncopyable.hpp>
#include <cutemuduo/tcp_connection.hpp>
//
#include <cutehttpserver/ssl/ssl_context.hpp>

namespace cutehttpserver {

class SslConnection : cutemuduo::NonCopyable {
public:
    using Timestamp = cutemuduo::Timestamp;
    using TcpConnectionPtr = std::shared_ptr<cutemuduo::TcpConnection>;
    using BufferPtr = cutemuduo::Buffer*;
    using MessageCallback = std::function<void(TcpConnectionPtr const&, BufferPtr, Timestamp)>;

public:
    SslConnection(TcpConnectionPtr const& tcp_connection, SslContext* ssl_context);

    ~SslConnection();

public:
    void StartHandshake();

    void Send(const void* data, size_t len);

    void OnRead(const TcpConnectionPtr& conn, BufferPtr buf, Timestamp timestamp);

    bool IsHandshakeCompleted() const { return ssl_state_ == SslState::ESTABLISHED; }

    BufferPtr GetDecryptedBuffer() { return &decrypted_buffer_; }

    // SSL BIO 操作回调
    static int BioWrite(BIO* bio, const char* data, int len);

    static int BioRead(BIO* bio, char* data, int len);

    static long BioCtrl(BIO* bio, int cmd, long num, void* ptr);

    // 设置消息回调函数
    void SetMessageCallback(MessageCallback cb) { message_callback_ = std::move(cb); }

private:
    void HandleHandshake();

    void OnEncrypted(const char* data, size_t len);

    void OnDecrypted(const char* data, size_t len);

    SslError GetLastError(int ret);

    void HandleError(SslError error);

private:
    SSL* ssl_;                                    // SSL 连接
    SslContext* ssl_context_;                     // SSL 上下文
    cutemuduo::TcpConnectionPtr tcp_connection_;  // TCP 连接
    SslState ssl_state_;                          // SSL 状态
    BIO* read_bio_;                               // 网络数据 -> SSL
    BIO* write_bio_;                              // SSL -> 网络数据
    cutemuduo::Buffer read_buffer_;               // 读缓冲区
    cutemuduo::Buffer write_buffer_;              // 写缓冲区
    cutemuduo::Buffer decrypted_buffer_;          // 解密后的数据
    MessageCallback message_callback_;            // 消息回调
};

}  // namespace cutehttpserver
