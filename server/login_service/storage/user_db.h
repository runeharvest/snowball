#pragma once
#include <domain/user.h>
#include <memory>
#include <string>
#include <vector>
#include <expected>

namespace domain {
struct User;
enum class UserState : std::uint8_t;
}

template <typename T>
using Result = std::expected<T, std::string>;

struct UserDB
{
	virtual ~UserDB() = default;

	// Mirrors LoginService user-facing methods
    [[nodiscard]] virtual Result<domain::Users> Users() = 0;
    [[nodiscard]] virtual Result<domain::User> UserByLogin(const std::string login) = 0;
    [[nodiscard]] virtual Result<domain::User> UserByUserID(int32_t userID) = 0;
    [[nodiscard]] virtual Result<domain::Users> UsersByState(domain::UserState state) = 0;
    [[nodiscard]] virtual Result<domain::User> UserByCookie(const std::string cookie) = 0;
    [[nodiscard]] virtual Result<domain::Users> UsersByShardID(int32_t shardID) = 0;

    // Create/Update return the stored object or status
    [[nodiscard]] virtual Result<domain::User> UserCreate(const domain::User user) = 0;
    [[nodiscard]] virtual bool UserUpdate(const domain::User user) = 0;
};
