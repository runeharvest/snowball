#include "user_memory.h"
#include <algorithm>

std::shared_ptr<User> UserMemory::CloneToPtr(const User &u)
{
	return std::make_shared<User>(u);
}

std::vector<std::shared_ptr<User>> UserMemory::Users()
{
	std::vector<std::shared_ptr<User>> out;
	out.reserve(byId_.size());
	for (auto &kv : byId_) out.emplace_back(kv.second);
	return out;
}

std::shared_ptr<User> UserMemory::UserByLogin(const std::string &login)
{
	auto it = loginToId_.find(login);
	if (it == loginToId_.end()) return nullptr;
	auto fit = byId_.find(it->second);
	return (fit == byId_.end()) ? nullptr : fit->second;
}

std::shared_ptr<User> UserMemory::UserByUID(int32_t uid)
{
	auto it = byId_.find(uid);
	return (it == byId_.end()) ? nullptr : it->second;
}

std::vector<std::shared_ptr<User>> UserMemory::UsersByState(UserState state)
{
	std::vector<std::shared_ptr<User>> out;
	for (auto &kv : byId_)
	{
		if (kv.second->State == state) out.emplace_back(kv.second);
	}
	return out;
}

std::vector<std::shared_ptr<User>> UserMemory::UsersByCookie(const std::string &cookie)
{
	std::vector<std::shared_ptr<User>> out;
	auto it = cookieToIds_.find(cookie);
	if (it == cookieToIds_.end()) return out;
	out.reserve(it->second.size());
	for (auto uid : it->second)
	{
		auto fit = byId_.find(uid);
		if (fit != byId_.end()) out.emplace_back(fit->second);
	}
	return out;
}

std::shared_ptr<User> UserMemory::UserCreate(const User &uIn)
{
	User u = uIn;

	// assign id if not set or duplicate
	if (u.UId <= 0 || byId_.count(u.UId))
	{
		// find next free id
		while (byId_.count(nextId_)) ++nextId_;
		u.UId = nextId_++;
	}

	// enforce unique login
	if (!u.Login.empty())
	{
		if (auto it = loginToId_.find(u.Login); it != loginToId_.end())
		{
			// If login exists, fail by returning nullptr
			return nullptr;
		}
	}

	auto ptr = CloneToPtr(u);
	byId_[u.UId] = ptr;
	if (!u.Login.empty()) loginToId_[u.Login] = u.UId;
	return ptr;
}

bool UserMemory::UserUpdate(const User &u)
{
	auto it = byId_.find(u.UId);
	if (it == byId_.end()) return false;

	// handle login reindex if changed
	const std::string oldLogin = it->second->Login;
	if (u.Login != oldLogin)
	{
		if (!u.Login.empty() && loginToId_.count(u.Login) && loginToId_[u.Login] != u.UId)
		{
			// conflicting login
			return false;
		}
		if (!oldLogin.empty()) loginToId_.erase(oldLogin);
		if (!u.Login.empty()) loginToId_[u.Login] = u.UId;
	}

	*(it->second) = u; // overwrite stored record
	return true;
}

void UserMemory::BindCookie(int32_t uid, const std::string &cookie)
{
	if (byId_.find(uid) == byId_.end()) return;
	auto &vec = cookieToIds_[cookie];
	if (std::find(vec.begin(), vec.end(), uid) == vec.end()) vec.push_back(uid);
}

void UserMemory::UnbindCookie(const std::string &cookie)
{
	cookieToIds_.erase(cookie);
}

void UserMemory::Clear()
{
	byId_.clear();
	loginToId_.clear();
	cookieToIds_.clear();
	nextId_ = 1;
}
