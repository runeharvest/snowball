#pragma once
#include <memory>
#include <string>
#include <vector>
#include <expected>

template <typename T>
using Result = std::expected<T, std::string>;

struct ConfigDB
{
	virtual ~ConfigDB() = default;

	[[nodiscard]] virtual Result<void> Load(const std::string dir, const std::string inProjectName) = 0;
	[[nodiscard]] virtual Result<std::string> ValueResult(const std::string inCategory, const std::string inKey) = 0;
	[[nodiscard]] virtual Result<bool> ValueBoolResult(const std::string inCategory, const std::string inKey) = 0;
	[[nodiscard]] virtual Result<std::vector<std::string>> ValuesResult(const std::string inCategory, const std::string inKey) = 0;
	[[nodiscard]] virtual Result<int32_t> ValueInt32Result(const std::string inCategory, const std::string inKey) = 0;
	[[nodiscard]] virtual Result<float> ValueFloat32Result(const std::string inCategory, const std::string inKey) = 0;

	[[nodiscard]] virtual std::string Value(const std::string inCategory, const std::string inKey) = 0;
	[[nodiscard]] virtual bool ValueBool(const std::string inCategory, const std::string inKey) = 0;
	[[nodiscard]] virtual std::vector<std::string> Values(const std::string inCategory, const std::string inKey) = 0;
	[[nodiscard]] virtual int32_t ValueInt32(const std::string inCategory, const std::string inKey) = 0;
	[[nodiscard]] virtual float ValueFloat32(const std::string inCategory, const std::string inKey) = 0;

	[[nodiscard]] virtual Result<void> SetValue(const std::string inCategory, const std::string inKey, const std::string value) = 0;
};
