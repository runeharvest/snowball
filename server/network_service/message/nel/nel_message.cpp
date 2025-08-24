#include "nel_message.h"
#include <algorithm>
#include <mutex>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <thread>
#include <cstring>
#include <fcntl.h>

Result<void> NelMessage::ConnectUDP(std::string host, uint8_t port)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (socket_fd_ != -1) {
        return std::unexpected("socket already open");
    }

    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0) {
		return std::unexpected("Failed to create socket");
	}

	sockaddr_in server_addr {};
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);

	if (inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr) <= 0)
	{
		close(sockfd);
		return std::unexpected("Invalid host address");
	}

	// For UDP, connect is optional, but can be used to set default peer
	if (connect(sockfd, reinterpret_cast<sockaddr *>(&server_addr), sizeof(server_addr)) < 0)
	{
		close(sockfd);
		return std::unexpected("Connection failed");
	}

	socket_fd_ = sockfd;

	is_tcp_ = false;

	// identification: UN_SIDENT unified_network:864
	return {};
}

Result<void> NelMessage::ConnectTCP(std::string host, uint8_t port)
{
	std::lock_guard<std::mutex> lock(mutex_);

	if (socket_fd_ != -1)
	{
		return std::unexpected("socket already open");
	}

	int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd < 0)
	{
		return std::unexpected("Failed to create socket");
	}

	sockaddr_in server_addr {};
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);

	if (inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr) <= 0)
	{
		close(sockfd);
		return std::unexpected("Invalid host address");
	}

	if (connect(sockfd, reinterpret_cast<sockaddr *>(&server_addr), sizeof(server_addr)) < 0)
	{
		close(sockfd);
		return std::unexpected("Connection failed");
	}

	socket_fd_ = sockfd;
	is_tcp_ = true;

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
	polling_ = false;
	return {};
}

Result<void> NelMessage::ListenUDP(std::string host, uint8_t port)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (socket_fd_ != -1) {
        return std::unexpected("socket already open");
    }

    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0) {
		return std::unexpected("create socket: " + std::string(strerror(errno)));
	}

	sockaddr_in server_addr {};
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	if (host.empty())
	{
		server_addr.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces
	}
	else
	{
		if (inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr) <= 0)
		{
			close(sockfd);
			return std::unexpected("bind address: " + std::string(strerror(errno)));
		}
	}

	if (bind(sockfd, reinterpret_cast<sockaddr *>(&server_addr), sizeof(server_addr)) < 0)
	{
		close(sockfd);
		return std::unexpected("bind: " + std::string(strerror(errno)));
	}

	socket_fd_ = sockfd;

	is_tcp_ = false;
	polling_ = true;
	poll_thread_ = std::thread([this]() {
		while (polling_)
		{
			pollMessages();
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	});
	return {};
}

Result<void> NelMessage::ListenTCP(std::string host, uint8_t port)
{
	std::lock_guard<std::mutex> lock(mutex_);

	if (socket_fd_ != -1)
	{
		return std::unexpected("socket already open");
	}

	int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd < 0)
	{
		return std::unexpected("create socket: " + std::to_string(errno));
	}

	sockaddr_in server_addr {};
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	if (host.empty())
	{
		server_addr.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces
	}
	else
	{
		if (inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr) <= 0)
		{
			close(sockfd);
			return std::unexpected("bind address: " + std::to_string(errno));
		}
	}

	int optval = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	if (bind(sockfd, reinterpret_cast<sockaddr *>(&server_addr), sizeof(server_addr)) < 0)
	{
		close(sockfd);
		return std::unexpected("bind: " + std::to_string(errno));
	}

	if (listen(sockfd, SOMAXCONN) < 0)
	{
		close(sockfd);
		return std::unexpected("listen: " + std::to_string(errno));
	}

	socket_fd_ = sockfd;
	is_tcp_ = true;
	polling_ = true;
	fcntl(socket_fd_, F_SETFL, O_NONBLOCK); // make the connection non-blocking
	poll_thread_ = std::thread([this]() {
		while (polling_)
		{
			pollMessages();
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		printf("poll thread exited");
	});
	return {};
}

void NelMessage::handleClient(int client_fd)
{
	char buffer[1024];
	while (true)
	{
		ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
		if (bytes_received <= 0)
		{
			// Client disconnected or error
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			if (!polling_)
			{
				break;
			}
			continue;
		}
		buffer[bytes_received] = '\0';
		printf("Received TCP message: %s\n", buffer);

		// Optionally, send a response here
		// send(client_fd, ...);
	}
	close(client_fd);
}

void NelMessage::handleUDPMessage(const char *buffer, ssize_t bytes_received, const sockaddr_in &client_addr)
{
	char msg[1024];
	std::memcpy(msg, buffer, bytes_received);
	msg[bytes_received] = '\0';
	printf("Received UDP message: %s\n", msg);

	// Optionally, send a response using sendto()
	// sendto(socket_fd_, ...);
}

void NelMessage::pollMessages()
{
    int fd;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        fd = socket_fd_;
    }
    if (fd == -1) return;

	if (is_tcp_)
	{
		sockaddr_in client_addr {};
		socklen_t addr_len = sizeof(client_addr);
		int client_fd = accept(fd, reinterpret_cast<sockaddr *>(&client_addr), &addr_len);
		if (client_fd < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				// No pending connection, just return and try again next poll
				return;
			}
			// No pending connection or error
			return;
		}

		// Spawn a thread to handle this client
		std::thread(&NelMessage::handleClient, this, client_fd).detach();
		return;
	}
	char buffer[1024];
	sockaddr_in client_addr {};
	socklen_t addr_len = sizeof(client_addr);
	ssize_t bytes_received = recvfrom(fd, buffer, sizeof(buffer) - 1, 0,
	    reinterpret_cast<sockaddr *>(&client_addr), &addr_len);
	if (bytes_received < 0)
	{
		// No data or error
		return;
	}

	// Immediately handle the UDP message
	handleUDPMessage(buffer, bytes_received, client_addr);
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