#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace domain {
enum class UserState : uint8_t
{
	Offline = 0,
	Online = 1,
	Waiting = 2,
	Authorized = 3,
};

struct User
{
	int32_t UserID = 0;
	int32_t ShardID = -1;
	int32_t BetaKeyId = 0;
	std::string Login;
	std::string Password;
	std::string Privilege;
	std::string ExtendedPrivilege;
	std::string GroupName;
	std::string FirstName;
	std::string LastName;
	std::string Birthday;
	std::string Country;
	std::string Email;
	std::string Address;
	std::string City;
	std::string PostalCode;
	std::string USState;
	std::string Chat;
	std::string CachedCoupons;
	std::string ProfileAccess;
	std::string Cookie;
	UserState State = UserState::Offline;
	uint8_t Gender = 0;
};

using Users = std::vector<User>;

} // namespace domain
