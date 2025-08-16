#include "login_service.h"
#include <expected>

std::expected<std::vector<std::shared_ptr<User>>, std::string> LoginService::Users()
{
	return userDB_.Users();
}

std::shared_ptr<User> LoginService::UserByLogin(const std::string &login)
{
	return userDB_.UserByLogin(login);
}

std::shared_ptr<User> LoginService::UserByUID(int32_t uid)
{
	return userDB_.UserByUID(uid);
}

std::vector<std::shared_ptr<User>> LoginService::UsersByShardID(int32_t shardId)
{
	return userDB_.UsersByShardID(shardId);
}

std::vector<std::shared_ptr<User>> LoginService::UsersByState(UserState state)
{
	return userDB_.UsersByState(state);
}

std::shared_ptr<User> LoginService::UserByCookie(const std::string &cookie)
{
	return userDB_.UserByCookie(cookie);
}

std::shared_ptr<User> LoginService::UserCreate(const User &u)
{
	return userDB_.UserCreate(u);
}

bool LoginService::UserUpdate(const User &u)
{
	return userDB_.UserUpdate(u);
}
