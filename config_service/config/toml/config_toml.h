#pragma once
#include "config_db.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <mutex>
#include <expected>

template <typename T>
using Result = std::expected<T, std::string>;

// In-memory ConfigDB
class ConfigToml final : public ConfigDB
{
public:
	ConfigToml() = default;
	~ConfigToml() override = default;

	// ConfigDB interface
	[[nodiscard]] Result<void> Load(const std::string dir, const std::string inProjectName) override;
	[[nodiscard]] Result<std::string> ValueResult(const std::string inCategory, const std::string inKey) override;
	[[nodiscard]] Result<bool> ValueBoolResult(const std::string inCategory, const std::string inKey) override;
	[[nodiscard]] Result<std::vector<std::string>> ValuesResult(const std::string inCategory, const std::string inKey) override;
	[[nodiscard]] Result<int32_t> ValueInt32Result(const std::string inCategory, const std::string inKey) override;
	[[nodiscard]] Result<float> ValueFloat32Result(const std::string inCategory, const std::string inKey) override;

	[[nodiscard]] std::string Value(const std::string inCategory, const std::string inKey) override;
	[[nodiscard]] bool ValueBool(const std::string inCategory, const std::string inKey) override;
	[[nodiscard]] std::vector<std::string> Values(const std::string inCategory, const std::string inKey) override;
	[[nodiscard]] int32_t ValueInt32(const std::string inCategory, const std::string inKey) override;
	[[nodiscard]] float ValueFloat32(const std::string inCategory, const std::string inKey) override;

	[[nodiscard]] Result<void> SetValue(const std::string inCategory, const std::string inKey, const std::string value) override;

private:
	mutable std::mutex mutex_;
	std::unordered_map<std::string, std::string> table_;
};
