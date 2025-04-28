#pragma once

#include <string>
#include <vector>

namespace PeopleNetwork
{
	typedef size_t PRSHANDLE;

	struct Person
	{
		PRSHANDLE Handle = 0;
		Person();

		std::string Name;
		std::string Origin; // Country of Origin
		std::string ShortDesc;

		enum class HistoricalPeriod
		{
			PreIslamic,
			Islamic,
			Republic
		} Period;
		static std::string_view HistoricalPeriodToString(HistoricalPeriod period);
		static HistoricalPeriod HistoricalPeriodFromString(std::string_view period);

		std::pair<int, int> Lifetime;

		enum ImportanceEnum
		{
			LowImportance,
			MidImportance,
			HighImportance
		} Importance;
		static std::string_view ImportanceToString(ImportanceEnum importance);
		static ImportanceEnum ImportanceFromString(std::string_view importance);

		struct StructDataGUI
		{
			struct PositionStruct
			{
				int X, Y;
			} Position;
			struct ColorStruct
			{
				int R, G, B;
			} Color;
		} DataGUI;

		struct ContentSection
		{
			std::string Header;  // Section Header (i.e. Name)
			std::string Content; // Actual text content
		};
		std::vector<ContentSection> ContentSections;

		bool PopulateFromJson(std::string_view json);
	};
	inline Person INVALID_PERSON_HANDLE = {};
}
