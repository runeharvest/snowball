#pragma once
#include "shard.h"
#include "shard_db.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>

// In-memory ShardDB
class ShardSql final : public ShardDB
{
public:
	ShardSql() = default;
	~ShardSql() override = default;

	// ShardDB interface
	std::vector<std::shared_ptr<Shard>> Shards() override;
	std::shared_ptr<Shard> ShardByShardID(int32_t shardId) override;
	std::shared_ptr<Shard> ShardByWSAddr(const std::string &wsAddr) override;
	std::shared_ptr<Shard> ShardCreate(const Shard &s) override;
	bool ShardUpdate(const Shard &s) override;
	std::vector<std::shared_ptr<Shard>> ShardsByClientApplication(const std::string &clientApp) override;

	// Helpers
	int32_t AssignShardID(const std::shared_ptr<Shard> &shard); // returns assigned id
	void BindShardID(int32_t shardId, const std::shared_ptr<Shard> &shard);
	void BindWSAddr(int32_t shardId, const std::string &wsAddr);
	void BindClientApplication(int32_t shardId, const std::string &clientApp);
	void UnbindWSAddr(const std::string &wsAddr);
	void UnbindClientApplication(const std::string &clientApp);
	void Clear();

private:
	int32_t nextId_ = 1;

	// Primary storage
	std::vector<std::shared_ptr<Shard>> all_;
	std::unordered_map<int32_t, std::shared_ptr<Shard>> byId_;

	// Secondary indexes maintained via helpers
	std::unordered_map<std::string, std::vector<int32_t>> wsAddrToIds_;
	std::unordered_map<std::string, std::vector<int32_t>> clientAppToIds_;

	static std::shared_ptr<Shard> CloneToPtr(const Shard &s);
};
