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

	void CommandLineOptions::Populate(const CommandLine& cmdline)
	{
		int adj = ARG_NONE;
		#define OVERRIDE_FROM_CMDLINE(arg, ovr, valset) if (cmdline.ContainsAdjacent(arg, adj)) { ovr = valset; }
		#define OVERRIDE_FROM_CMDLINE_STR(arg, ovr) OVERRIDE_FROM_CMDLINE(arg, ovr, cmdline[adj])
		#define OVERRIDE_FROM_CMDLINE_INT(arg, ovr) OVERRIDE_FROM_CMDLINE(arg, ovr, atoi(cmdline[adj]))
		#define OVERRIDE_FROM_CMDLINE_UI8(arg, ovr) OVERRIDE_FROM_CMDLINE(arg, ovr, (uint8_t) atoi(cmdline[adj]))
		#define OVERRIDE_FROM_CMDLINE_FLT(arg, ovr) OVERRIDE_FROM_CMDLINE(arg, ovr, (float) atof(cmdline[adj]))
		#define OVERRIDE_FROM_CMDLINE_FLG(arg, bit) bit = cmdline.Contains(arg);

		/* RENDERER */
		OVERRIDE_FROM_CMDLINE_STR("/Window-Title",           WindowTitle);
		OVERRIDE_FROM_CMDLINE_INT("/Window-Width",           WindowWidth);
		OVERRIDE_FROM_CMDLINE_INT("/Window-Height",          WindowHeight);
		OVERRIDE_FROM_CMDLINE_FLG("/Windowed",               Windowed);
		OVERRIDE_FROM_CMDLINE_FLG("/Window-Control",         WindowControl);
		OVERRIDE_FROM_CMDLINE_FLG("/Allow-Dynamic",          AllowDynamic);

		/* MAIN */
		OVERRIDE_FROM_CMDLINE_FLG("/Prefer-Raw-Override",    PreferRawOverride);
		OVERRIDE_FROM_CMDLINE_STR("/Load-Data",              LoadData);
		OVERRIDE_FROM_CMDLINE_FLG("/Passive-History",        PassiveHistory);
		OVERRIDE_FROM_CMDLINE_INT("/Wrap-Content-Tolerance", WrapContentTolerance);

		/* RENDERER */
		OVERRIDE_FROM_CMDLINE_FLG("/Disable-Authority",      DisableAuthority);
		OVERRIDE_FROM_CMDLINE_UI8("/Dropoff-R",              DropoffR);
		OVERRIDE_FROM_CMDLINE_UI8("/Dropoff-G",              DropoffG);
		OVERRIDE_FROM_CMDLINE_UI8("/Dropoff-B",              DropoffB);
		OVERRIDE_FROM_CMDLINE_STR("/Font-Regular",           FontRegular);
		OVERRIDE_FROM_CMDLINE_STR("/Font-Bold",              FontBold);
		OVERRIDE_FROM_CMDLINE_STR("/Font-Italic",            FontItalic);
		OVERRIDE_FROM_CMDLINE_FLT("/X-Scale-Factor",         XScaleFactor);
		OVERRIDE_FROM_CMDLINE_FLT("/Radii-Scalar",           RadiiScalar);
		OVERRIDE_FROM_CMDLINE_INT("/Detail-Factor",          DetailFactor);
		OVERRIDE_FROM_CMDLINE_FLT("/Outline-Thickness",      OutlineThickness);
		OVERRIDE_FROM_CMDLINE_FLG("/New-Outline-Method",     NewOutlineMethod);
		OVERRIDE_FROM_CMDLINE_UI8("/Outline-R",              OutlineR);
		OVERRIDE_FROM_CMDLINE_UI8("/Outline-G",              OutlineG);
		OVERRIDE_FROM_CMDLINE_UI8("/Outline-B",              OutlineB);
		OVERRIDE_FROM_CMDLINE_STR("/Return-Button",          ReturnButton);
		OVERRIDE_FROM_CMDLINE_STR("/Pre-Islamic-SB",         PreIslamicSB);
		OVERRIDE_FROM_CMDLINE_STR("/Islamic-SB",             IslamicSB);
		OVERRIDE_FROM_CMDLINE_STR("/Republic-SB",            RepublicSB);
		OVERRIDE_FROM_CMDLINE_FLT("/SB-Offset-X",            SBOffsetX);
		OVERRIDE_FROM_CMDLINE_FLT("/SB-Offset-Y",            SBOffsetY);
		OVERRIDE_FROM_CMDLINE_FLT("/SB-Seperation-Factor",   SBSeperationFactor);
		OVERRIDE_FROM_CMDLINE_FLT("/MasterShiftX",           MasterShiftX);
		OVERRIDE_FROM_CMDLINE_FLT("/MasterShiftY",           MasterShiftY);

		#undef OVERRIDE_FROM_CMDLINE
		#undef OVERRIDE_FROM_CMDLINE_STR
		#undef OVERRIDE_FROM_CMDLINE_INT
		#undef OVERRIDE_FROM_CMDLINE_UI8
		#undef OVERRIDE_FROM_CMDLINE_FLT
		#undef OVERRIDE_FROM_CMDLINE_FLG
	}
}
