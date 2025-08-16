#pragma once
#include <expected>
#include "user.h"
#include "user_db.h"
#include "shard.h"
#include "shard_db.h"

// ---------- Business logic layer ----------
// Service suffix per requirement
class LoginService
{
public:
	LoginService(ShardDB &shardDb, UserDB &userDb)
	    : shardDB_(shardDb)
	    , userDB_(userDb)
	{
	}
	virtual ~LoginService() = default;

	// Users
	virtual std::expected<std::vector<std::shared_ptr<User>>, std::string> Users();
	virtual std::shared_ptr<User> UserByLogin(const std::string &login);
	virtual std::shared_ptr<User> UserByUID(int32_t uid);
	virtual std::vector<std::shared_ptr<User>> UsersByState(UserState state);
	virtual std::vector<std::shared_ptr<User>> UsersByShardID(int32_t shardId);
	virtual std::shared_ptr<User> UserByCookie(const std::string &cookie);
	virtual std::shared_ptr<User> UserCreate(const User &u);
	virtual bool UserUpdate(const User &u);

	// Shards
	virtual std::vector<std::shared_ptr<Shard>> Shards();
	virtual std::shared_ptr<Shard> ShardByShardID(int32_t shardId);
	virtual std::shared_ptr<Shard> ShardByWSAddr(const std::string &wsAddr);
	virtual std::shared_ptr<Shard> ShardCreate(const Shard &s);
	virtual bool ShardUpdate(const Shard &s);
	virtual std::vector<std::shared_ptr<Shard>> ShardsByClientApplication(const std::string &clientApp);

protected:
	ShardDB &shardDB_;
	UserDB &userDB_;
};
