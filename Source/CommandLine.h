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

	struct CommandLineOptions
	{
		/* WINDOW */
		std::string WindowTitle = "Kisiler Agi";
		int         WindowWidth{};
		int         WindowHeight{};
		bool        Windowed{};
		bool        AllowDynamic{};
		bool        WindowControl{};

		/* MAIN */
		bool        PreferRawOverride{};
		std::string LoadData    = "Resources/PeopleLoadData.json";
		bool        PassiveHistory{};
		int         WrapContentTolerance = 170;
		
		/* RENDERER */
		bool        DisableAuthority{};
		uint8_t     DropoffR     = 235;
		uint8_t     DropoffG     = 235;
		uint8_t     DropoffB     = 255;
		std::string FontRegular  = "Resources/Fonts/Roboto-Regular.ttf";
		std::string FontBold     = "Resources/Fonts/Roboto-Bold.ttf";
		std::string FontItalic   = "Resources/Fonts/Roboto-Italic.ttf";
		std::string TooltipFont{}; // Regular/Bold/Italic
		float       XScaleFactor = 12.0f;
		float       RadiiScalar  = 1.0f;
		int         DetailFactor = 360;
		float       OutlineThickness = 5.0f;
		bool        NewOutlineMethod{};
		uint8_t     OutlineR     = 200;
		uint8_t     OutlineG     = 200;
		uint8_t     OutlineB     = 225;
		std::string ReturnButton = "Resources/Sprites/Button_Return.png";
		std::string PreIslamicSB = "Resources/Sprites/Button_PreIslamic.png";
		std::string IslamicSB    = "Resources/Sprites/Button_Islamic.png";
		std::string RepublicSB   = "Resources/Sprites/Button_Republic.png";
		std::string MasterQuit   = "Resources/Sprites/Button_MasterQuit.png";
		float       SBOffsetX = 25.0f;
		float       SBOffsetY = 75.0f;
		float       SBSeperationFactor = 125.0f;
		float       MasterShiftX{};
		float       MasterShiftY{};
		
		void Populate(const CommandLine& cmdline);
	};
	inline CommandLineOptions g_CommandLineOptions{};
}
