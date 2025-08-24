#include "network_service.h"

Result<void> NetworkService::Connect(std::string host, uint8_t port)
{
    return messageDB_.Connect(host, port);
}

Result<void> NetworkService::Close()
{
    return messageDB_.Close();
}

Result<void> NetworkService::Listen(std::string host, uint8_t port)
{
    return messageDB_.Listen(host, port);
}

Result<void> NetworkService::SendRaw(std::string message)
{
    return messageDB_.SendRaw(message);
}