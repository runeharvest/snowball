#include "shard_memory.h"
#include <algorithm>
#include <mutex>

std::shared_ptr<Shard> ShardMemory::CloneToPtr(const Shard &s)
{
	return std::make_shared<Shard>(s);
}

std::vector<std::shared_ptr<Shard>> ShardMemory::Shards()
{
	std::lock_guard<std::mutex> lock(mutex_);
	return all_; // copy of vector of shared_ptrs
}

std::shared_ptr<Shard> ShardMemory::ShardByShardID(int32_t shardId)
{
	std::lock_guard<std::mutex> lock(mutex_);
	for (const auto &shardPtr : all_)
	{
		if (!shardPtr)
		{
			continue;
		}
		if (shardPtr->ShardID != shardId)
		{
			continue;
		}

		return shardPtr;
	}
	return nullptr;
}

std::vector<std::shared_ptr<Shard>> ShardMemory::ShardsByWSAddr(const std::string &wsAddr)
{
	std::lock_guard<std::mutex> lock(mutex_);
	std::vector<std::shared_ptr<Shard>> out;
	for (const auto &shardPtr : all_)
	{
		if (shardPtr && shardPtr->WSAddr == wsAddr)
		{
			out.emplace_back(shardPtr);
		}
	}
	return out;
}

std::shared_ptr<Shard> ShardMemory::ShardCreate(const Shard &s)
{
	std::lock_guard<std::mutex> lock(mutex_);
	Shard sCopy = s;
	if (sCopy.ShardID == 0)
	{
		sCopy.ShardID = nextId_;
		nextId_++;
	}

	auto ptr = CloneToPtr(sCopy);
	all_.push_back(ptr);
	// No id bound by default. Caller may BindShardID or AssignShardID.
	return ptr;
}

bool ShardMemory::ShardUpdate(const Shard &s)
{
	std::lock_guard<std::mutex> lock(mutex_);
	for (auto &p : all_)
	{
		if (p.get() == &s)
		{
			*p = s;
			return true;
		}
	}

	return false;
}

std::vector<std::shared_ptr<Shard>> ShardMemory::ShardsByClientApplication(const std::string &clientApp)
{
	std::lock_guard<std::mutex> lock(mutex_);
	std::vector<std::shared_ptr<Shard>> out;
	for (const auto &shardPtr : all_)
	{
		if (shardPtr && shardPtr->ClientApplication == clientApp)
		{
			out.emplace_back(shardPtr);
		}
	}
	return out;
}