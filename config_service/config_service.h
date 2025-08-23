#pragma once
#include <expected>
#include "config_db.h"

template <typename T>
using Result = std::expected<T, std::string>;

class ConfigService
{
public:
	ConfigService(ConfigDB &configDB)
	    : configDB_(configDB)
	{
	}
	virtual ~ConfigService() = default; // Inline definition

	[[nodiscard]] virtual Result<void> Load(const std::string dir, const std::string projectName)
	{
		return configDB_.Load(dir, projectName);
	}
	[[nodiscard]] virtual Result<std::string> ValueResult(const std::string category, const std::string key)
	{
		return configDB_.ValueResult(category, key);
	}
	[[nodiscard]] virtual Result<bool> ValueBoolResult(const std::string category, const std::string key)
	{
		return configDB_.ValueBoolResult(category, key);
	}
	[[nodiscard]] virtual Result<std::vector<std::string>> ValuesResult(const std::string category, const std::string key)
	{
		return configDB_.ValuesResult(category, key);
	}
	[[nodiscard]] virtual Result<int32_t> ValueInt32Result(const std::string category, const std::string key)
	{
		return configDB_.ValueInt32Result(category, key);
	}
	[[nodiscard]] virtual Result<float> ValueFloat32Result(const std::string category, const std::string key)
	{
		return configDB_.ValueFloat32Result(category, key);
	}
	[[nodiscard]] virtual Result<void> SetValue(const std::string category, const std::string key, const std::string value)
	{
		return configDB_.SetValue(category, key, value);
	}

	[[nodiscard]] virtual std::string Value(const std::string category, const std::string key)
	{
		return configDB_.Value(category, key);
	}
	[[nodiscard]] virtual bool ValueBool(const std::string category, const std::string key)
	{
		return configDB_.ValueBool(category, key);
	}
	[[nodiscard]] virtual std::vector<std::string> Values(const std::string category, const std::string key)
	{
		return configDB_.Values(category, key);
	}
	[[nodiscard]] virtual int32_t ValueInt32(const std::string category, const std::string key)
	{
		return configDB_.ValueInt32(category, key);
	}
	[[nodiscard]] virtual float ValueFloat32(const std::string category, const std::string key)
	{
		return configDB_.ValueFloat32(category, key);
	}

protected:
	ConfigDB &configDB_;
};