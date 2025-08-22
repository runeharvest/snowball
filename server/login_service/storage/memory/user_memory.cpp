#include "user_memory.h"
#include <algorithm>
#include <expected>

Result<domain::Users> UserMemory::Users()
{
    std::lock_guard<std::mutex> lock(mutex_);

    return all_;
}

Result<domain::User> UserMemory::UserByLogin(const std::string login)
{
    std::lock_guard<std::mutex> lock(mutex_);

    for (auto user : all_)
    {
        if (user.Login != login)
        {
            continue;
        }
        return user;
    }

    return std::unexpected("User not found");
}

Result<domain::User> UserMemory::UserByUserID(int32_t userID)
{
    std::lock_guard<std::mutex> lock(mutex_);

    for (auto user : all_)
    {
        if (user.UserID == userID)
        {
            return user;
        }
    }
    return std::unexpected("User not found");
}

Result<domain::Users> UserMemory::UsersByState(domain::UserState state)
{
    std::lock_guard<std::mutex> lock(mutex_);

    domain::Users out;
    for (auto user : all_)
    {
        if (user.State != state)
        {
            continue;
        }

        out.emplace_back(user);
    }
    return out;
}

Result<domain::Users> UserMemory::UsersByShardID(int32_t shardID)
{
    std::lock_guard<std::mutex> lock(mutex_);
    domain::Users out;
    for (const auto &user : all_)
    {
        if (user.ShardID == shardID)
        {
            out.emplace_back(user);
        }
    }
    return out;
}

Result<domain::User> UserMemory::UserByCookie(const std::string cookie)
{
    std::lock_guard<std::mutex> lock(mutex_);

    for (auto &user : all_)
    {
        if (user.Cookie.empty())
        {
            continue;
        }
        if (user.Cookie != cookie)
        {
            continue;
        }
        return user;
    }
    return std::unexpected("User not found");
}

Result<domain::User> UserMemory::UserCreate(domain::User user)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (user.UserID == 0)
    {
        user.UserID = nextId_;
        nextId_++;
    }

    all_.push_back(user);
    return user;
}

bool UserMemory::UserUpdate(const domain::User user)
{
    std::lock_guard<std::mutex> lock(mutex_);

    for (auto &existingUser : all_)
    {
        if (existingUser.UserID != user.UserID)
        {
            continue;
        }
        existingUser = user;
        return true;
    }
    return false;
}