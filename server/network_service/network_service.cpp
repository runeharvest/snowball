#include "network_service.h"

Result<void> NetworkService::ConnectUDP(std::string host, uint8_t port)
{
	return messageDB_.ConnectUDP(host, port);
}

Result<void> NetworkService::ConnectTCP(std::string host, uint8_t port)
{
	return messageDB_.ConnectTCP(host, port);
}

Result<void> NetworkService::Close()
{
    return messageDB_.Close();
}

Result<void> NetworkService::ListenUDP(std::string host, uint8_t port)
{
	return messageDB_.ListenUDP(host, port);
}

Result<void> NetworkService::ListenTCP(std::string host, uint8_t port)
{
	return messageDB_.ListenTCP(host, port);
}

Result<void> NetworkService::SendRaw(std::string message)
{
    return messageDB_.SendRaw(message);
}