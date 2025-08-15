#pragma once
#include <cstdint>
#include <memory>
#include <string>

// Simple enums that mirror DB enum domains
enum class UserState : uint8_t
{
	Offline = 0,
	Online = 1
};

// ---------- Entities (property-only structs) ----------

struct User
{
	// ints
	int32_t UId = 0;
	int32_t ShardId = 0;
	int32_t BetaKeyId = 0;

	// strings
	std::string Login;
	std::string Password;
	std::string Privilege;
	std::string GroupName;
	std::string FirstName;
	std::string LastName;
	std::string Birthday; // varchar(32)
	std::string Country; // char(2)
	std::string Email;
	std::string Address;
	std::string City;
	std::string PostalCode; // varchar(10)
	std::string USState; // char(2)
	std::string Chat; // char(2)
	std::string CachedCoupons;
	std::string ProfileAccess; // varchar(45)

	// enums / tinyints
	UserState State = UserState::Offline;
	uint8_t Gender = 0; // tinyint(1)
};
