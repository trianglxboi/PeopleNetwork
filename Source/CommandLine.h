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

#define OVERRIDE_FROM_CMDLINE(arg, ovr, valset) if (g_CmdLine.ContainsAdjacent(arg, adj)) { ovr = valset; }
#define OVERRIDE_FROM_CMDLINE_STR(arg, ovr) OVERRIDE_FROM_CMDLINE(arg, ovr, g_CmdLine[adj])
#define OVERRIDE_FROM_CMDLINE_INT(arg, ovr) OVERRIDE_FROM_CMDLINE(arg, ovr, atoi(g_CmdLine[adj]))
#define OVERRIDE_FROM_CMDLINE_FLT(arg, ovr) OVERRIDE_FROM_CMDLINE(arg, ovr, atof(g_CmdLine[adj]))
#define OVERRIDE_FROM_CMDLINE_FLG(arg, bit) bit = g_CmdLine.Contains(arg);
