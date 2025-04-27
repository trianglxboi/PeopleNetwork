#pragma once

#define ARG_NONE (-1)

#include <string_view>
#include <string>

namespace PeopleNetwork
{
	struct CommandLine
	{
		char** List = nullptr;
		int    Size = 0;

		CommandLine(const CommandLine&) = default;
		CommandLine& operator=(const CommandLine&) = default;
		CommandLine(char** list = nullptr, int size = 0);

		int Find(std::string_view arg, int& adjacent) const;
		int Find(std::string_view arg) const;

		bool Contains(std::string_view arg, int& adjacent) const;
		bool Contains(std::string_view arg) const;
		bool ContainsAdjacent(std::string_view arg, int& adjacent) const;

		char* operator[](int i) const { return List[i]; }
	};
	inline CommandLine g_CmdLine;
}
