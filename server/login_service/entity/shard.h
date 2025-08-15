#pragma once
#include <cstdint>
#include <memory>
#include <string>

struct Shard
{
	int32_t Level = 0;
	int32_t CurrentFunds = 0;
	int32_t ValidIdBilling = 0;
	int32_t GMId = 0;
	std::string IdBilling;
	std::string Community;
	std::string Account;
	std::string CurrentSubLength;
	std::string ExtendedPrivilege;
	std::string ToolsGroup;
	std::string Unsubscribe;
	std::string SubDate;
	std::string SubIp;
	std::string SecurePassword;
	std::string LastInvoiceEmailCheck;
	std::string FromSource;
	std::string ValidMerchantCode;
	std::string ApiKeySeed;
	uint8_t Newsletter = 0;
	uint8_t ChoiceSubLength = 0;
	uint8_t PBC = 0;
};
