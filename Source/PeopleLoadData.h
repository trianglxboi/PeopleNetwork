#pragma once

#include <string_view>
#include <string>
#include <vector>

namespace PeopleNetwork
{
	struct PeopleLoadData
	{
		enum class LoadRule
		{
			LoadAll,
			LoadSpecific
		} LdRule;
		static std::string_view LoadRuleToString(LoadRule rule);
		static LoadRule LoadRuleFromString(std::string_view rule);

		std::vector<std::string> LoadSpecificFiles;
		std::vector<std::string> LoadAllExcludes;
		std::string LoadAllRoot;

		bool PopulateFromJson(std::string_view json);
	};
}
