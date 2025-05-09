#pragma once

#include <string>
#include <vector>
#include <limits>

#define DO_NOT_WRAP_CONTENT (std::numeric_limits<size_t>::max())

namespace PeopleNetwork
{
	typedef size_t PRSHANDLE;
	inline constexpr PRSHANDLE INVALID_PERSON_HANDLE = 0;

	struct Person
	{
		PRSHANDLE Handle = 0;
		Person();

		std::wstring Name;
		std::wstring Origin; // Country of Origin, DEPRECATED, NO LONGER USED!
		std::wstring ShortDesc;

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
			std::wstring Header;  // Section Header (i.e. Name)
			std::wstring Content; // Actual text content
		};
		std::vector<ContentSection> ContentSections;

		bool PopulateFromJson(std::string_view json, size_t wrapContentTolerance = DO_NOT_WRAP_CONTENT);
	};
	inline Person INVALID_PERSON_STRUCT = {};
}
