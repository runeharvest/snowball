#pragma once
#include <memory>
#include <string>
#include <vector>
#include <expected>

template <typename T>
using Result = std::expected<T, std::string>;

struct MessageDB
{
	virtual ~MessageDB() = default;

	[[nodiscard]] virtual Result<void> Connect(std::string host, uint8_t port) = 0;
	[[nodiscard]] virtual Result<void> Close() = 0;
	[[nodiscard]] virtual Result<void> SendRaw(std::string message) = 0;


	[[nodiscord]] virtual Result<void> Listen(std::string host, uint8_t port) = 0;
};
