#include "login_service.h"
#include <expected>

Result<domain::Users> LoginService::Users()
{
	return userDB_.Users();
}

Result<domain::User> LoginService::UserByLogin(const std::string login)
{
	return userDB_.UserByLogin(login);
}

Result<domain::User> LoginService::UserByUserID(int32_t userID)
{
    return userDB_.UserByUserID(userID);
}

Result<domain::Users> LoginService::UsersByShardID(int32_t shardID)
{
    return userDB_.UsersByShardID(shardID);
}

Result<domain::Users> LoginService::UsersByState(domain::UserState state)
{
	return userDB_.UsersByState(state);
}

Result<domain::User> LoginService::UserByCookie(const std::string cookie)
{
	return userDB_.UserByCookie(cookie);
}

Result<domain::User> LoginService::UserCreate(const domain::User user)
{
    return userDB_.UserCreate(user);
}

bool LoginService::UserUpdate(const domain::User user)
{
    return userDB_.UserUpdate(user);
}
