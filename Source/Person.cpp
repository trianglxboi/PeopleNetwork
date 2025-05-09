#include "Person.h"

#include "CommandLine.h"
#include "StringUtil.h"
#include "FileUtil.h"

#include <random>

namespace PeopleNetwork
{
	Person::Person()
	{
		static std::random_device rd;
		static std::mt19937_64 rng(rd());
		static std::uniform_int_distribution<size_t> dist(0, std::numeric_limits<size_t>::max());
		Handle = dist(rng);
	}

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

	bool Person::PopulateFromJson(std::string_view json, size_t wrapContentTolerance)
	{
		Json::Value root;
		if (!FileUtil::ReadJson(json, root))
		{
			return false;
		}

		Name = StringUtil::u8tow(root["Name"].asString());
		// DEPRECATED! Origin = StringUtil::u8tow(root["Origin"].asString());
		ShortDesc = StringUtil::u8tow(root["Desc"].asString());
		Period = HistoricalPeriodFromString(root["Period"].asString());
		Lifetime = { root["Lifetime"][0].asInt(), root["Lifetime"][1].asInt() };
		Importance = ImportanceFromString(root["Importance"].asString());

		Json::Value& dataGui = root["GUI"];
		Json::Value& position = dataGui["Position"];
		Json::Value& color = dataGui["Color"];

		DataGUI.Position.X = position[0].asInt();
		DataGUI.Position.Y = position[1].asInt();
		DataGUI.Color.R = color[0].asInt();
		DataGUI.Color.G = color[1].asInt();
		DataGUI.Color.B = color[2].asInt();

		Json::Value& contentSection = root["Content"];
		for (size_t i = 0; /*noexpr*/; i++)
		{
			std::string istring = std::to_string(i);
			if (!contentSection.isMember(istring))
			{
				break;
			}

			Json::Value& section = contentSection[istring];
			ContentSection& sec = ContentSections.emplace_back();
			sec.Header = StringUtil::u8tow(section["Header"].asString());
			sec.Content = StringUtil::u8tow(section["Content"].asString());

			//if (wrapContentTolerance != DO_NOT_WRAP_CONTENT)
			//{
			//	std::wstring& content = sec.Content;
			//	for (size_t i = 0, counter = 1; i < content.size(); i++, counter++)
			//	{
			//		if (content[i] == '\n')
			//		{
			//			counter = 1;
			//			continue;
			//		}
			//		
			//		size_t iNext = i + 1;
			//		if (counter == wrapContentTolerance)
			//		{
			//			if (iNext < content.size())
			//			{
			//				if (iswalnum(i))
			//				{
			//					if (iswalnum(iNext))
			//					{
			//						content.insert(i, L"-\n");
			//						i += 2;
			//					}
			//					else
			//					{
			//						content.insert(i, L"\n");
			//						i++;
			//					}
			//				}
			//				else if (iswalnum(iNext))
			//				{
			//					if (iswalnum(i))
			//					{
			//						content.insert(i, L"-\n");
			//						i += 2;
			//					}
			//					else
			//					{
			//						content.insert(i, L"\n");
			//						i++;
			//					}
			//				}
			//			}
			//			counter = 1;
			//		}
			//	}
			//}
		}

		return true;
	}
}
