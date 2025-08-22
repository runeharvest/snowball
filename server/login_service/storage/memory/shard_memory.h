#pragma once
#include <domain/shard.h>
#include "shard_db.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <mutex>
#include <expected>
namespace domain {
struct Shard;
}

template <typename T>
using Result = std::expected<T, std::string>;

// In-memory ShardDB
class ShardMemory final : public ShardDB
{
public:
	ShardMemory() = default;
	~ShardMemory() override = default;

	// ShardDB interface
    [[nodiscard]] Result<domain::Shards> Shards() override;
    [[nodiscard]] Result<domain::Shard> ShardByShardID(int32_t shardId) override;
    [[nodiscard]] Result<domain::Shard> ShardByWSAddr(const std::string wsAddr) override;
    [[nodiscard]] Result<domain::Shard> ShardCreate(const domain::Shard shard) override;
    bool ShardUpdate(const domain::Shard s) override;
    [[nodiscard]] Result<domain::Shards> ShardsByClientApplication(const std::string clientApp) override;

private:
	int32_t nextId_ = 0;
    domain::Shards all_;
    mutable std::mutex mutex_;
};
