#include "shard_memory.h"
#include <algorithm>

std::shared_ptr<Shard> ShardMemory::CloneToPtr(const Shard &s)
{
	return std::make_shared<Shard>(s);
}

std::vector<std::shared_ptr<Shard>> ShardMemory::Shards()
{
	return all_; // copy of vector of shared_ptrs
}

std::shared_ptr<Shard> ShardMemory::ShardByShardID(int32_t shardId)
{
	auto it = byId_.find(shardId);
	return (it == byId_.end()) ? nullptr : it->second;
}

std::vector<std::shared_ptr<Shard>> ShardMemory::ShardsByWSAddr(const std::string &wsAddr)
{
	std::vector<std::shared_ptr<Shard>> out;
	auto it = wsAddrToIds_.find(wsAddr);
	if (it == wsAddrToIds_.end()) return out;
	out.reserve(it->second.size());
	for (auto id : it->second)
	{
		auto fit = byId_.find(id);
		if (fit != byId_.end()) out.emplace_back(fit->second);
	}
	return out;
}

std::shared_ptr<Shard> ShardMemory::ShardCreate(const Shard &s)
{
	auto ptr = CloneToPtr(s);
	all_.push_back(ptr);
	// No id bound by default. Caller may BindShardID or AssignShardID.
	return ptr;
}

bool ShardMemory::ShardUpdate(const Shard &s)
{
	// Update by identity: caller should pass the same object previously returned by ShardCreate (*ptr).
	for (auto &p : all_)
	{
		if (p.get() == &s)
		{
			*p = s;
			return true;
		}
	}
	// If not found by identity, try any bound id that points to this address.
	for (auto &kv : byId_)
	{
		if (kv.second && kv.second.get() == &s)
		{
			*(kv.second) = s;
			return true;
		}
	}
	return false;
}

std::vector<std::shared_ptr<Shard>> ShardMemory::ShardsByClientApplication(const std::string &clientApp)
{
	std::vector<std::shared_ptr<Shard>> out;
	auto it = clientAppToIds_.find(clientApp);
	if (it == clientAppToIds_.end()) return out;
	out.reserve(it->second.size());
	for (auto id : it->second)
	{
		auto fit = byId_.find(id);
		if (fit != byId_.end()) out.emplace_back(fit->second);
	}
	return out;
}

int32_t ShardMemory::AssignShardID(const std::shared_ptr<Shard> &shard)
{
	if (!shard) return 0;
	while (byId_.count(nextId_)) ++nextId_;
	byId_[nextId_] = shard;
	return nextId_++;
}

void ShardMemory::BindShardID(int32_t shardId, const std::shared_ptr<Shard> &shard)
{
	if (!shard) return;
	byId_[shardId] = shard;
}

void ShardMemory::BindWSAddr(int32_t shardId, const std::string &wsAddr)
{
	auto &v = wsAddrToIds_[wsAddr];
	if (std::find(v.begin(), v.end(), shardId) == v.end()) v.push_back(shardId);
}

void ShardMemory::BindClientApplication(int32_t shardId, const std::string &clientApp)
{
	auto &v = clientAppToIds_[clientApp];
	if (std::find(v.begin(), v.end(), shardId) == v.end()) v.push_back(shardId);
}

void ShardMemory::UnbindWSAddr(const std::string &wsAddr)
{
	wsAddrToIds_.erase(wsAddr);
}

void ShardMemory::UnbindClientApplication(const std::string &clientApp)
{
	clientAppToIds_.erase(clientApp);
}

void ShardMemory::Clear()
{
	nextId_ = 1;
	all_.clear();
	byId_.clear();
	wsAddrToIds_.clear();
	clientAppToIds_.clear();
}
