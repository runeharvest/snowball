#pragma once
#include "shard.h"
#include <memory>
#include <string>
#include <vector>

struct ShardDB
{
	virtual ~ShardDB() = default;

	// Mirrors LoginService shard-facing methods
	virtual std::vector<std::shared_ptr<Shard>> Shards() = 0;
	virtual std::shared_ptr<Shard> ShardByShardID(int32_t shardId) = 0;
	virtual std::shared_ptr<Shard> ShardByWSAddr(const std::string &wsAddr) = 0;
	virtual std::shared_ptr<Shard> ShardCreate(const Shard &s) = 0;
	virtual bool ShardUpdate(const Shard &s) = 0;
	virtual std::vector<std::shared_ptr<Shard>> ShardsByClientApplication(const std::string &clientApp) = 0;
};
