#include "user_memory.h"
#include <algorithm>

std::shared_ptr<User> UserMemory::CloneToPtr(const User &u)
{
	return std::make_shared<User>(u);
}

std::vector<std::shared_ptr<User>> UserMemory::Users()
{
	std::vector<std::shared_ptr<User>> out;
	out.reserve(all_.size());
	for (auto &u : all_) out.emplace_back(u);
	return out;
}

std::shared_ptr<User> UserMemory::UserByLogin(const std::string &login)
{
	for (auto &u : all_)
	{
		if (u->Login == login)
		{
			return u;
		}
	}
	return nullptr;
}

std::shared_ptr<User> UserMemory::UserByUID(int32_t uid)
{
	for (auto &u : all_)
	{
		if (u->UId == uid)
		{
			return u;
		}
	}
	return nullptr;
}

std::vector<std::shared_ptr<User>> UserMemory::UsersByShardID(int32_t shardId)
{
	std::vector<std::shared_ptr<User>> out;
	for (auto &u : all_)
	{
		if (u->ShardID == shardId)
		{
			out.emplace_back(u);
		}
	}
	return out;
}

std::vector<std::shared_ptr<User>> UserMemory::UsersByState(UserState state)
{
	std::vector<std::shared_ptr<User>> out;
	for (auto &u : all_)
	{
		if (u->State == state) out.emplace_back(u);
	}
	return out;
}

std::shared_ptr<User> UserMemory::UserByCookie(const std::string &cookie)
{
	for (auto &u : all_)
	{
		if (u->Cookie.empty())
		{
			continue;
		}
		if (u->Cookie == cookie)
		{
			return u;
		}
	}
	return nullptr;
}

std::shared_ptr<User> UserMemory::UserCreate(const User &uIn)
{
	User u = uIn;
	// assign id if not set or duplicate
	if (u.UId <= 0)
	{
		while (std::any_of(all_.begin(), all_.end(), [&](const auto &userPtr) { return userPtr->UId == nextId_; })) ++nextId_;
		u.UId = nextId_++;
	}

	auto ptr = CloneToPtr(u);
	all_.push_back(ptr);
	return ptr;
}

bool UserMemory::UserUpdate(const User &u)
{
	for (auto &userPtr : all_)
	{
		if (userPtr->UId == u.UId)
		{
			*userPtr = u;
			return true;
		}
	}
	return false;
}