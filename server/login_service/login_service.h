#pragma once
#include <expected>
#include "user_db.h"
#include "shard_db.h"
#include <domain/user.h>
#include <domain/shard.h>

namespace domain {
struct User;
enum class UserState : std::uint8_t;
struct Shard;
}

template <typename T>
using Result = std::expected<T, std::string>;

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
    [[nodiscard]] virtual Result<domain::Users> Users();
    [[nodiscard]] virtual Result<domain::User> UserByLogin(const std::string login);
    [[nodiscard]] virtual Result<domain::User> UserByUserID(int32_t userID);
    [[nodiscard]] virtual Result<domain::Users> UsersByState(domain::UserState state);
    [[nodiscard]] virtual Result<domain::Users> UsersByShardID(int32_t shardID);
    [[nodiscard]] virtual Result<domain::User> UserByCookie(const std::string cookie);
    [[nodiscard]] virtual Result<domain::User> UserCreate(const domain::User user);
    [[nodiscard]] virtual bool UserUpdate(const domain::User user);

    // Shards
    [[nodiscard]] virtual Result<domain::Shards> Shards();
    [[nodiscard]] virtual Result<domain::Shard> ShardByShardID(int32_t shardID);
    [[nodiscard]] virtual Result<domain::Shard> ShardByWSAddr(const std::string wsAddr);
    [[nodiscard]] virtual Result<domain::Shard> ShardCreate(const domain::Shard shard);
    [[nodiscard]] virtual bool ShardUpdate(const domain::Shard shard);
    [[nodiscard]] virtual Result<domain::Shards> ShardsByClientApplication(const std::string clientApp);

protected:
	ShardDB &shardDB_;
	UserDB &userDB_;
};
