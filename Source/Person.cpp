#include "Person.h"
#include "FileUtil.h"

namespace PeopleNetwork
{
	std::string_view Person::HistoricalPeriodToString(HistoricalPeriod period)
	{
		switch (period)
		{
		case HistoricalPeriod::PreIslamic: return "PreIslamic";
		case HistoricalPeriod::Islamic:    return "Islamic";
		case HistoricalPeriod::Republic:   return "Republic";
		}

		return {};
	}

	Person::HistoricalPeriod Person::HistoricalPeriodFromString(std::string_view period)
	{
		if (period == "PreIslamic") return HistoricalPeriod::PreIslamic;
		if (period == "Islamic")    return HistoricalPeriod::Islamic;
		if (period == "Republic")   return HistoricalPeriod::Republic;

		return (HistoricalPeriod) -1;
	}

	std::string_view Person::ImportanceToString(ImportanceEnum importance)
	{
		switch (importance)
		{
		case LowImportance:  return "Low";
		case MidImportance:  return "Mid";
		case HighImportance: return "High";
		}

		return {};
	}

	Person::ImportanceEnum Person::ImportanceFromString(std::string_view importance)
	{
		if (importance == "Low")  return LowImportance;
		if (importance == "Mid")  return MidImportance;
		if (importance == "High") return HighImportance;

		return (ImportanceEnum) -1;
	}

	bool Person::PopulateFromJson(std::string_view json)
	{
		Json::Value root;
		if (!FileUtil::ReadJson(json, root))
		{
			return false;
		}

		Name = root["Name"].asString();
		Origin = root["Origin"].asString();
		ShortDesc = root["ShortDesc"].asString();
		Period = HistoricalPeriodFromString(root["Period"].asString());
		Lifetime = { root["Lifetime"][0].asInt(), root["Lifetime"][1].asInt() };
		Importance = ImportanceFromString(root["Importance"].asString());

		Json::Value& contentSection = root["ContentSection"];
		size_t i = 0;
		for (Json::Value& section : contentSection)
		{
			ContentSection& sec = ContentSections.emplace_back();
			sec.Header = contentSection.getMemberNames()[i++];
			sec.Content = section.asString();
		}

		return true;
	}
}
