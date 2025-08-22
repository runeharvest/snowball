#include "shard_memory.h"
#include <algorithm>
#include <mutex>

Result<domain::Shards> ShardMemory::Shards()
{
	std::lock_guard<std::mutex> lock(mutex_);
    return all_;
}

Result<domain::Shard> ShardMemory::ShardByShardID(int32_t shardId)
{
	std::lock_guard<std::mutex> lock(mutex_);
    for (const auto shard : all_)
    {
        if (shard.ShardID != shardId)
        {
            continue;
        }

        return shard;
    }
    return std::unexpected("Shard not found");
}

Result<domain::Shard> ShardMemory::ShardByWSAddr(const std::string wsAddr)
{
	std::lock_guard<std::mutex> lock(mutex_);
    domain::Shards out;
    for (const auto shard : all_)
    {
        if (shard.WSAddr != wsAddr)
        {
            continue;
        }
        return shard;
    }
    return std::unexpected("Shard not found");
}

Result<domain::Shard> ShardMemory::ShardCreate(domain::Shard shard)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (shard.ShardID == 0)
    {
        shard.ShardID = nextId_;
        nextId_++;
    }

    all_.push_back(shard);
    // No id bound by default. Caller may BindShardID or AssignShardID.
    return shard;
}

bool ShardMemory::ShardUpdate(const domain::Shard shard)
{
	std::lock_guard<std::mutex> lock(mutex_);
    for (auto &existingShard : all_)
    {
        if (existingShard.ShardID != shard.ShardID)
        {
            continue;
        }
        existingShard = shard;
        return true;
    }

    return false;
}

Result<domain::Shards> ShardMemory::ShardsByClientApplication(const std::string clientApp)
{
	std::lock_guard<std::mutex> lock(mutex_);
    domain::Shards out;
    for (const auto shard : all_)
    {
        if (shard.ClientApplication != clientApp)
        {
            continue;
        }
        out.emplace_back(shard);
    }
    return out;
}