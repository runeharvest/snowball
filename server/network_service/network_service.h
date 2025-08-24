#pragma once
#include <expected>
#include "message_db.h"

template <typename T>
using Result = std::expected<T, std::string>;

// ---------- Business logic layer ----------
// Service suffix per requirement
class NetworkService
{
public:
	NetworkService(MessageDB &message)
	    : messageDB_(message)
	{
	}
	virtual ~NetworkService() = default;

	[[nodiscard]] virtual Result<void> ConnectUDP(std::string host, uint8_t port);
	[[nodiscard]] virtual Result<void> ConnectTCP(std::string host, uint8_t port);
	[[nodiscard]] virtual Result<void> Close();
	[[nodiscard]] virtual Result<void> SendRaw(std::string message);

	[[nodiscord]] virtual Result<void> ListenUDP(std::string host, uint8_t port);
	[[nodiscord]] virtual Result<void> ListenTCP(std::string host, uint8_t port);

protected:
	MessageDB &messageDB_;
};
