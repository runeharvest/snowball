#pragma once
#include <domain/shard.h>
#include "message_db.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <mutex>
#include <expected>
#include <thread>
#include <netinet/in.h>
#include <atomic>

template <typename T>
using Result = std::expected<T, std::string>;

// In-memory MessageDB
class NelMessage final : public MessageDB
{
public:
	NelMessage() = default;
	~NelMessage() override = default;

	[[nodiscard]] Result<void> ConnectUDP(std::string host, uint8_t port) override;
	[[nodiscard]] Result<void> ConnectTCP(std::string host, uint8_t port) override;
	[[nodiscard]] Result<void> Close() override;
	[[nodiscard]] Result<void> SendRaw(std::string message) override;

	[[nodiscard]] Result<void> ListenUDP(std::string host, uint8_t port) override;
	[[nodiscard]] Result<void> ListenTCP(std::string host, uint8_t port) override;

private:
	void pollMessages();
	void handleClient(int client_fd);
	mutable std::mutex mutex_;
	void handleUDPMessage(const char *buffer, ssize_t bytes_received, const sockaddr_in &client_addr);
	int socket_fd_{-1};
	std::atomic<bool> polling_ { false };
	bool is_tcp_ { false };
	std::thread poll_thread_;
};
