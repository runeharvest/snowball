#include "login_service.h"

// ---------- Users ----------
std::vector<std::shared_ptr<User>> LoginService::Users()
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

std::vector<std::shared_ptr<User>> LoginService::UsersByState(UserState state)
{
	return userDB_.UsersByState(state);
}

std::vector<std::shared_ptr<User>> LoginService::UsersByCookie(const std::string &cookie)
{
	return userDB_.UsersByCookie(cookie);
}

std::shared_ptr<User> LoginService::UserCreate(const User &u)
{
	return userDB_.UserCreate(u);
}

bool LoginService::UserUpdate(const User &u)
{
	return userDB_.UserUpdate(u);
}

// ---------- Shards ----------
std::vector<std::shared_ptr<Shard>> LoginService::Shards()
{
	return shardDB_.Shards();
}

std::shared_ptr<Shard> LoginService::ShardByShardID(int32_t shardId)
{
	return shardDB_.ShardByShardID(shardId);
}

std::vector<std::shared_ptr<Shard>> LoginService::ShardsByWSAddr(const std::string &wsAddr)
{
	return shardDB_.ShardsByWSAddr(wsAddr);
}

std::shared_ptr<Shard> LoginService::ShardCreate(const Shard &s)
{
	return shardDB_.ShardCreate(s);
}

bool LoginService::ShardUpdate(const Shard &s)
{
	return shardDB_.ShardUpdate(s);
}

std::vector<std::shared_ptr<Shard>> LoginService::ShardsByClientApplication(const std::string &clientApp)
{
	return shardDB_.ShardsByClientApplication(clientApp);
}
