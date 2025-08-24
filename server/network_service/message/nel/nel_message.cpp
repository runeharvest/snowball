#include "nel_message.h"
#include <algorithm>
#include <mutex>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <thread>


Result<void> NelMessage::Connect(std::string host, uint8_t port)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (socket_fd_ != -1) {
        return std::unexpected("socket already open");
    }

    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0) {
        return std::unexpected("Failed to create socket");
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr) <= 0) {
        close(sockfd);
        return std::unexpected("Invalid host address");
    }

    // For UDP, connect is optional, but can be used to set default peer
    if (connect(sockfd, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) < 0) {
        close(sockfd);
        return std::unexpected("Connection failed");
    }

    socket_fd_ = sockfd;

    // identification: UN_SIDENT unified_network:864
    return {};
}

Result<void> NelMessage::Close()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (socket_fd_ != -1) {
        close(socket_fd_);
        socket_fd_ = -1;
    }
    return {};
}

Result<void> NelMessage::Listen(std::string host, uint8_t port)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (socket_fd_ != -1) {
        return std::unexpected("socket already open");
    }

    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0) {
        return std::unexpected("create socket: "+std::to_string(errno));
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (host.empty()) {
        server_addr.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces
    } else {
        if (inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr) <= 0) {
            close(sockfd);
            return std::unexpected("bind address: "+std::to_string(errno));
        }
    }

    if (bind(sockfd, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) < 0) {
        close(sockfd);
        return std::unexpected("bind: "+std::to_string(errno));
    }

    socket_fd_ = sockfd;

    polling_ = true;
    poll_thread_ = std::thread([this]() {
        while (polling_) {
            pollMessages();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });
    return {};
}

void NelMessage::pollMessages()
{
    int fd;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        fd = socket_fd_;
    }
    if (fd == -1) return;

    char buffer[1024];
    sockaddr_in client_addr{};
    socklen_t addr_len = sizeof(client_addr);

    ssize_t bytes_received = recvfrom(fd, buffer, sizeof(buffer) - 1, 0,
                                      reinterpret_cast<sockaddr*>(&client_addr), &addr_len);
    if (bytes_received < 0) {
        // Handle error
        return;
    }

    buffer[bytes_received] = '\0';
    // Process buffer (may need to lock again if modifying shared state)

    printf("Received message: %s\n", buffer);
}


Result<void> NelMessage::SendRaw(std::string message)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (socket_fd_ == -1) {
        return std::unexpected("socket not open");
    }

    ssize_t bytes_sent = send(socket_fd_, message.data(), message.size(), 0);
    if (bytes_sent < 0) {
        return std::unexpected("send failed");
    }
    if (static_cast<size_t>(bytes_sent) != message.size()) {
        return std::unexpected("partial send");
    }
    return {};
}