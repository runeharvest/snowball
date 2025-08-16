#pragma once
#include "user.h"
#include "user_db.h"
#include <optional>
#include <unordered_map>

// ---------- In-memory UserDB implementation ----------
class UserMemory final : public UserDB
{
public:
	UserMemory() = default;
	~UserMemory() override = default;

	// UserDB interface
	std::vector<std::shared_ptr<User>> Users() override;
	std::shared_ptr<User> UserByLogin(const std::string &login) override;
	std::shared_ptr<User> UserByUID(int32_t uid) override;
	std::vector<std::shared_ptr<User>> UsersByState(UserState state) override;
	std::vector<std::shared_ptr<User>> UsersByShardID(int32_t shardId) override;
	std::shared_ptr<User> UserByCookie(const std::string &cookie) override;
	std::shared_ptr<User> UserCreate(const User &u) override;
	bool UserUpdate(const User &u) override;

private:
	int32_t nextId_ = 1;
	std::vector<std::shared_ptr<User>> all_;

	static std::shared_ptr<User> CloneToPtr(const User &u);
};
