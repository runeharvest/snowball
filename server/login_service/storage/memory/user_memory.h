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
	std::vector<std::shared_ptr<User>> UsersByCookie(const std::string &cookie) override;
	std::shared_ptr<User> UserCreate(const User &u) override;
	bool UserUpdate(const User &u) override;

	// Helpers for tests or higher layers
	// Cookies are not part of User entity, so expose bind/unbind here.
	void BindCookie(int32_t uid, const std::string &cookie);
	void UnbindCookie(const std::string &cookie);
	void Clear();

private:
	int32_t nextId_ = 1;

	std::unordered_map<int32_t, std::shared_ptr<User>> byId_;
	std::unordered_map<std::string, int32_t> loginToId_;
	std::unordered_map<std::string, std::vector<int32_t>> cookieToIds_;

	static std::shared_ptr<User> CloneToPtr(const User &u);
};
