#pragma once
#include "user.h"
#include <memory>
#include <string>
#include <vector>
#include <expected>

// ---------- DB "interfaces" (pure-virtual structs) ----------

struct UserDB
{
	virtual ~UserDB() = default;

	// Mirrors LoginService user-facing methods
	virtual std::expected<std::vector<std::shared_ptr<User>>, std::string>  Users() = 0;
	virtual std::shared_ptr<User> UserByLogin(const std::string &login) = 0;
	virtual std::shared_ptr<User> UserByUID(int32_t uid) = 0;
	virtual std::vector<std::shared_ptr<User>> UsersByState(UserState state) = 0;
	virtual std::shared_ptr<User> UserByCookie(const std::string &cookie) = 0;
	virtual std::vector<std::shared_ptr<User>> UsersByShardID(int32_t shardId) = 0;

	// Create/Update return the stored object or status
	virtual std::shared_ptr<User> UserCreate(const User &u) = 0;
	virtual bool UserUpdate(const User &u) = 0;
};
