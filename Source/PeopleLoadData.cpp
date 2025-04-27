#include "PeopleLoadData.h"

#include "FileUtil.h"

namespace PeopleNetwork
{
	std::string_view PeopleLoadData::LoadRuleToString(LoadRule rule)
	{
		switch (rule)
		{
		case LoadRule::LoadAll:      return "LoadAll";
		case LoadRule::LoadSpecific: return "LoadSpecific";
		}

		return {};
	}

	PeopleLoadData::LoadRule PeopleLoadData::LoadRuleFromString(std::string_view rule)
	{
		if (rule == "LoadAll")      return LoadRule::LoadAll;
		if (rule == "LoadSpecific") return LoadRule::LoadSpecific;

		return (LoadRule) -1;
	}

	bool PeopleLoadData::PopulateFromJson(std::string_view json)
	{
		Json::Value root;
		if (!FileUtil::ReadJson(json, root))
		{
			return false;
		}

		LdRule = LoadRuleFromString(root["LoadRule"].asString());
		switch (LdRule)
		{
		case LoadRule::LoadAll:
		{
			for (Json::Value& loadExclude : root["LoadAllExcludes"])
			{
				LoadAllExcludes.push_back(loadExclude.asString());
			}
			LoadAllRoot = root["LoadAllRoot"].asString();
			break;
		}
		case LoadRule::LoadSpecific:
		{
			for (Json::Value& loadFile : root["LoadSpecificFiles"])
			{
				LoadSpecificFiles.push_back(loadFile.asString());
			}
			break;
		}
		}

		return true;
	}
}
