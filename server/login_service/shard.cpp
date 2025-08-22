#include "login_service.h"

Result<domain::Shards> LoginService::Shards()
{
	return shardDB_.Shards();
}

Result<domain::Shard> LoginService::ShardByShardID(int32_t shardID)
{
    return shardDB_.ShardByShardID(shardID);
}

Result<domain::Shard> LoginService::ShardByWSAddr(const std::string wsAddr)
{
	return shardDB_.ShardByWSAddr(wsAddr);
}

Result<domain::Shard> LoginService::ShardCreate(const domain::Shard shard)
{
    return shardDB_.ShardCreate(shard);
}

bool LoginService::ShardUpdate(const domain::Shard shard)
{
    return shardDB_.ShardUpdate(shard);
}

Result<domain::Shards> LoginService::ShardsByClientApplication(const std::string clientApp)
{
	return shardDB_.ShardsByClientApplication(clientApp);
}
