#pragma once
#include <user_db.h>
#include <optional>
#include <unordered_map>
#include <expected>
#include <domain/user.h>

template <typename T>
using Result = std::expected<T, std::string>;

class UserMemory final : public UserDB
{
public:
	UserMemory() = default;
	~UserMemory() override = default;

	// UserDB interface
    [[nodiscard]] Result<domain::Users> Users() override;
    [[nodiscard]] Result<domain::User> UserByLogin(const std::string login) override;
    [[nodiscard]] Result<domain::User> UserByUserID(int32_t userID) override;
    [[nodiscard]] Result<domain::Users> UsersByState(domain::UserState state) override;
    [[nodiscard]] Result<domain::Users> UsersByShardID(int32_t shardID) override;
    [[nodiscard]] Result<domain::User> UserByCookie(const std::string cookie) override;
    [[nodiscard]] Result<domain::User> UserCreate(const domain::User user) override;
    [[nodiscard]] bool UserUpdate(const domain::User user) override;

private:
	int32_t nextId_ = 1;
    domain::Users all_;

    mutable std::mutex mutex_;
};
