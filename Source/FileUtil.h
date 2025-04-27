#pragma once

#include "json/json.h"

#include <string_view>
#include <filesystem>
#include <string>

namespace PeopleNetwork::FileUtil
{
	bool Read(std::string_view path, std::string& out);
	bool ReadJson(std::string_view path, Json::Value& out);
}
