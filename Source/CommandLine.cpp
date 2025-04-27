#include "CommandLine.h"

namespace PeopleNetwork
{
	CommandLine::CommandLine(char** list, int size)
		: List(list), Size(size)
	{
	}

	int CommandLine::Find(std::string_view arg, int& adjacent) const
	{
		for (int i = 1; i < Size; i++)
		{
			if (arg == List[i])
			{
				int next = i + 1;
				adjacent = next != Size ? next : ARG_NONE;
				return i;
			}
		}

		adjacent = ARG_NONE;
		return adjacent;
	}
	
	int CommandLine::Find(std::string_view arg) const
	{
		int adjacent;
		return Find(arg, adjacent);
	}
	
	bool CommandLine::Contains(std::string_view arg, int& adjacent) const
	{
		return Find(arg, adjacent) != ARG_NONE;
	}

	bool CommandLine::Contains(std::string_view arg) const
	{
		return Find(arg) != ARG_NONE;
	}

	bool CommandLine::ContainsAdjacent(std::string_view arg, int& adjacent) const
	{
		return Find(arg, adjacent) && adjacent != ARG_NONE;
	}
}
