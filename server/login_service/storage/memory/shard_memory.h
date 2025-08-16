#pragma once
#include "shard.h"
#include "shard_db.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <mutex>

// In-memory ShardDB
class ShardMemory final : public ShardDB
{
public:
	ShardMemory() = default;
	~ShardMemory() override = default;

	// ShardDB interface
	std::vector<std::shared_ptr<Shard>> Shards() override;
	std::shared_ptr<Shard> ShardByShardID(int32_t shardId) override;
	std::shared_ptr<Shard> ShardByWSAddr(const std::string &wsAddr) override;
	std::shared_ptr<Shard> ShardCreate(const Shard &s) override;
	bool ShardUpdate(const Shard &s) override;
	std::vector<std::shared_ptr<Shard>> ShardsByClientApplication(const std::string &clientApp) override;

private:
	int32_t nextId_ = 0;
	std::vector<std::shared_ptr<Shard>> all_;
	mutable std::mutex mutex_;

	static std::shared_ptr<Shard> CloneToPtr(const Shard &s);
};
