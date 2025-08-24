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

template <typename T>
using Result = std::expected<T, std::string>;

// In-memory MessageDB
class NelMessage final : public MessageDB
{
public:
	NelMessage() = default;
	~NelMessage() override = default;


	[[nodiscard]] Result<void> Connect(std::string host, uint8_t port) override;
	[[nodiscard]] Result<void> Close() override;
	[[nodiscard]] Result<void> SendRaw(std::string message) override;

	[[nodiscard]] Result<void> Listen(std::string host, uint8_t port) override;

private:
	void pollMessages();
    mutable std::mutex mutex_;
	int socket_fd_{-1};
	bool polling_{false};
	std::thread poll_thread_;
};
