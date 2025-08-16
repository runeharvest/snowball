#include "login_service.h"

std::vector<std::shared_ptr<Shard>> LoginService::Shards()
{
	return shardDB_.Shards();
}

std::shared_ptr<Shard> LoginService::ShardByShardID(int32_t shardId)
{
	return shardDB_.ShardByShardID(shardId);
}

std::shared_ptr<Shard> LoginService::ShardByWSAddr(const std::string &wsAddr)
{
	return shardDB_.ShardByWSAddr(wsAddr);
}

std::shared_ptr<Shard> LoginService::ShardCreate(const Shard &s)
{
	return shardDB_.ShardCreate(s);
}

bool LoginService::ShardUpdate(const Shard &s)
{
	return shardDB_.ShardUpdate(s);
}

std::vector<std::shared_ptr<Shard>> LoginService::ShardsByClientApplication(const std::string &clientApp)
{
	return shardDB_.ShardsByClientApplication(clientApp);
}
