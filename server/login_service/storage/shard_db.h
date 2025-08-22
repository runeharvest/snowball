#pragma once
#include <memory>
#include <string>
#include <vector>
#include <domain/shard.h>
#include <expected>

template <typename T>
using Result = std::expected<T, std::string>;

struct ShardDB
{
	virtual ~ShardDB() = default;

	// Mirrors LoginService shard-facing methods
    [[nodiscard]] virtual Result<domain::Shards> Shards() = 0;
    [[nodiscard]] virtual Result<domain::Shard> ShardByShardID(int32_t shardID) = 0;
    [[nodiscard]] virtual Result<domain::Shard> ShardByWSAddr(const std::string wsAddr) = 0;
    [[nodiscard]] virtual Result<domain::Shard> ShardCreate(const domain::Shard shard) = 0;
    [[nodiscard]] virtual bool ShardUpdate(const domain::Shard shard) = 0;
    [[nodiscard]] virtual Result<domain::Shards> ShardsByClientApplication(const std::string clientApp) = 0;
};
