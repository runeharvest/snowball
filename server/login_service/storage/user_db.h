#pragma once
#include "user.h"
#include <memory>
#include <string>
#include <vector>

// ---------- DB "interfaces" (pure-virtual structs) ----------

struct UserDB
{
	virtual ~UserDB() = default;

	// Mirrors LoginService user-facing methods
	virtual std::vector<std::shared_ptr<User>> Users() = 0;
	virtual std::shared_ptr<User> UserByLogin(const std::string &login) = 0;
	virtual std::shared_ptr<User> UserByUID(int32_t uid) = 0;
	virtual std::vector<std::shared_ptr<User>> UsersByState(UserState state) = 0;
	virtual std::vector<std::shared_ptr<User>> UsersByCookie(const std::string &cookie) = 0;

	// Create/Update return the stored object or status
	virtual std::shared_ptr<User> UserCreate(const User &u) = 0;
	virtual bool UserUpdate(const User &u) = 0;
};
