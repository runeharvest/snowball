#include "config_toml.h"
#include <algorithm>
#include <numeric>
#include <mutex>
#include <toml++/toml.h>
#include <filesystem>
#include <optional>

Result<void> ConfigToml::Load(const std::string dir, const std::string inProjectName)
{
    std::lock_guard<std::mutex> lock(mutex_);

	std::string projectName = inProjectName;
	std::transform(projectName.begin(), projectName.end(), projectName.begin(), ::tolower);

	std::string commonPath = dir + "/common.toml";
	std::string projectPath = dir + "/" + projectName + ".toml";
	std::string overridePath = dir + "/" + projectName + ".override.toml";

	// Parse common.toml
	if (!std::filesystem::exists(commonPath))
	{
		return std::unexpected("common.toml not found");
	}
	auto commonTableResult = toml::parse_file(commonPath);
	if (!commonTableResult)
	{
		return std::unexpected("parse common.toml: " + std::string(commonTableResult.error().description()));
	}
	auto &commonTable = commonTableResult.table();

	// Parse project.toml
	if (!std::filesystem::exists(projectPath))
	{
		return std::unexpected(projectName + ".toml not found");
	}
	auto projectTableResult = toml::parse_file(projectPath);
	if (!projectTableResult)
	{
		return std::unexpected("parse " + projectName + ".toml: " + std::string(projectTableResult.error().description()));
	}
	auto &projectTable = projectTableResult.table();

	// Parse override.toml (optional)
	std::optional<toml::table> overrideTable;
	if (std::filesystem::exists(overridePath))
	{
		auto overrideTableResult = toml::parse_file(overridePath);
		if (overrideTableResult)
		{
			overrideTable = overrideTableResult.table();
		}
	}

	// Only merge string values from [projectName] section in each table
	// table_.clear(); // append to existing table new elements
	auto mergeTable = [this, &projectName](const toml::table &tbl) -> std::expected<void, std::string> {
		std::vector<std::string> projectNames = { projectName, "common" };
		for (const auto &projectName : projectNames)
		{
			if (auto section = tbl.get_as<toml::table>(projectName))
			{
				for (const auto &[baseKey, value] : *section)
				{
					std::string keyLower = std::string(baseKey.str());
					std::transform(keyLower.begin(), keyLower.end(), keyLower.begin(), ::tolower);

					auto key = projectName + "." + keyLower;
					if (value.is_string())
					{
						table_[std::string(key)] = value.value<std::string>().value();
						continue;
					}
					if (value.is_boolean())
					{
						table_[std::string(key)] = value.value<bool>().value() ? "true" : "false";
						continue;
					}
					if (value.is_integer())
					{
						table_[std::string(key)] = std::to_string(value.value<int64_t>().value());
						continue;
					}
					if (value.is_floating_point())
					{
						table_[std::string(key)] = std::to_string(value.value<double>().value());
						continue;
					}
					if (value.is_array())
					{
						const auto &arr = *value.as_array();
						std::vector<std::string> elems;
						for (const auto &elem : arr)
						{
							if (elem.is_string())
							{
								if (elem.value<std::string>()->find(',') != std::string::npos)
								{
									return std::unexpected(key + " array element contains a comma: " + elem.value<std::string>().value());
								}
								elems.push_back(elem.value<std::string>().value());
							}
							else if (elem.is_boolean())
							{
								elems.push_back(elem.value<bool>().value() ? "true" : "false");
							}
							else if (elem.is_integer())
							{
								elems.push_back(std::to_string(elem.value<int64_t>().value()));
							}
							else if (elem.is_floating_point())
							{
								elems.push_back(std::to_string(elem.value<double>().value()));
							}
						}
						if (elems.empty())
						{
							return std::unexpected(key + " has an empty value in array");
						}
						table_[std::string(key)] = std::accumulate(
						    std::next(elems.begin()), elems.end(), elems[0],
						    [](std::string a, std::string b) { return a + "," + b; });
						continue;
					}

					return std::unexpected("unknown TOML value type for key: " + std::string(key));
				}
			}
		}
		return std::expected<void, std::string> {};
	};

	auto commonResult = mergeTable(commonTable);
	if (!commonResult) {
		return std::unexpected("common.toml merge: "+commonResult.error());
	}
	auto projectResult = mergeTable(projectTable);
	if (!projectResult) {
		return std::unexpected(projectName + ".toml merge: " + projectResult.error());
	}


	if (overrideTable) {
		 auto overrideResult = mergeTable(*overrideTable);
		 if (!overrideResult) {
			 return std::unexpected(projectName + ".override.toml merge: " + overrideResult.error());
		 }
	}

	return {};
}

std::string ConfigToml::Value(const std::string inCategory, const std::string inKey)
{
	auto result = ValueResult(inCategory, inKey);
	if (!result)
	{
		return "";
	}
	return *result;
}

Result<std::string> ConfigToml::ValueResult(const std::string inCategory, const std::string inKey)
{
	std::lock_guard<std::mutex> lock(mutex_);

	std::string category = inCategory;
	std::string keyLower = inKey;
	std::transform(category.begin(), category.end(), category.begin(), ::tolower);
	std::transform(keyLower.begin(), keyLower.end(), keyLower.begin(), ::tolower);

	auto key = category + "." + keyLower;

	auto it = table_.find(key);
	if (it == table_.end())
	{
		return std::unexpected(key + " not found in config");
	}
	return it->second;
}

bool ConfigToml::ValueBool(const std::string inCategory, const std::string inKey)
{
	auto result = ValueBoolResult(inCategory, inKey);
	if (!result)
	{
		return false;
	}
	return *result;
}

Result<bool> ConfigToml::ValueBoolResult(const std::string inCategory, const std::string inKey)
{
	std::lock_guard<std::mutex> lock(mutex_);

	std::string category = inCategory;
	std::string keyLower = inKey;
	std::transform(category.begin(), category.end(), category.begin(), ::tolower);
	std::transform(keyLower.begin(), keyLower.end(), keyLower.begin(), ::tolower);

	auto key = category + "." + keyLower;

	auto it = table_.find(key);
	if (it == table_.end())
	{
		return std::unexpected(key + " not found in config");
	}
	std::string val = it->second;
	std::transform(val.begin(), val.end(), val.begin(), ::tolower);
	if (val == "true" || val == "1" || val == "yes" || val == "on")
	{
		return true;
	}
	else if (val == "false" || val == "0" || val == "no" || val == "off")
	{
		return false;
	}
	return std::unexpected("Invalid boolean value for key: " + key);
}

std::vector<std::string> ConfigToml::Values(const std::string inCategory, const std::string inKey)
{
	auto result = ValuesResult(inCategory, inKey);
	if (!result)
	{
		return {};
	}
	return *result;
}

Result<std::vector<std::string>> ConfigToml::ValuesResult(const std::string inCategory, const std::string inKey)
{
	std::lock_guard<std::mutex> lock(mutex_);

	std::string category = inCategory;
	std::string keyLower = inKey;
	std::transform(category.begin(), category.end(), category.begin(), ::tolower);
	std::transform(keyLower.begin(), keyLower.end(), keyLower.begin(), ::tolower);

	auto key = category + "." + keyLower;

	auto it = table_.find(key);
	if (it == table_.end())
	{
		return std::unexpected(key + " not found in config");
	}
	std::string val = it->second;
	std::vector<std::string> result;
	size_t start = 0;
	size_t end = val.find(',');
	while (end != std::string::npos)
	{
		result.push_back(val.substr(start, end - start));
		start = end + 1;
		end = val.find(',', start);
	}
	result.push_back(val.substr(start));
	return result;
}

std::int32_t ConfigToml::ValueInt32(const std::string inCategory, const std::string inKey)
{
	auto result = ValueInt32Result(inCategory, inKey);
	if (!result)
	{
		return 0;
	}
	return *result;
}

Result<std::int32_t> ConfigToml::ValueInt32Result(const std::string inCategory, const std::string inKey)
{
	std::lock_guard<std::mutex> lock(mutex_);

	std::string category = inCategory;
	std::string keyLower = inKey;
	std::transform(category.begin(), category.end(), category.begin(), ::tolower);
	std::transform(keyLower.begin(), keyLower.end(), keyLower.begin(), ::tolower);

	auto key = category + "." + keyLower;

	auto it = table_.find(key);
	if (it == table_.end())
	{
		return std::unexpected(key + " not found in config");
	}
	try
	{
		int32_t value = std::stoi(it->second);
		return value;
	}
	catch (const std::exception &e)
	{
		return std::unexpected("Invalid int32 value for key: " + key);
	}
}

float ConfigToml::ValueFloat32(const std::string inCategory, const std::string inKey)
{
	auto result = ValueFloat32Result(inCategory, inKey);
	if (!result)
	{
		return 0.0f;
	}
	return *result;
}

Result<float> ConfigToml::ValueFloat32Result(const std::string inCategory, const std::string inKey)
{
	std::lock_guard<std::mutex> lock(mutex_);

	std::string category = inCategory;
	std::string keyLower = inKey;
	std::transform(category.begin(), category.end(), category.begin(), ::tolower);
	std::transform(keyLower.begin(), keyLower.end(), keyLower.begin(), ::tolower);

	auto key = category + "." + keyLower;

	auto it = table_.find(key);
	if (it == table_.end())
	{
		return std::unexpected(key + " not found in config");
	}
	try
	{
		float value = std::stof(it->second);
		return value;
	}
	catch (const std::exception &e)
	{
		return std::unexpected("Invalid float value for key: " + key);
	}
}

Result<void> ConfigToml::SetValue(const std::string inCategory, const std::string inKey, const std::string value)
{
	std::lock_guard<std::mutex> lock(mutex_);

	std::string category = inCategory;
	std::string keyLower = inKey;
	std::transform(category.begin(), category.end(), category.begin(), ::tolower);
	std::transform(keyLower.begin(), keyLower.end(), keyLower.begin(), ::tolower);

	auto key = category + "." + keyLower;

	table_[key] = value;
	return {};
}