#include "PeopleStorage.h"
#include "CommandLine.h"
#include "Renderer.h"
#include "Log.h"

#include "SFML/Graphics.hpp"

#include <codecvt>

std::wstring u8tow(const std::string& string)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	return converter.from_bytes(string);
}

enum ProgramState
{
	PPPLSEL, // People selection screen
	PPRSINF // Person info screen
};
ProgramState PSTATE = PPPLSEL;
PeopleNetwork::Person& PPRSINFSUBJ = PeopleNetwork::INVALID_PERSON_HANDLE;

int main(int argc, char** argv)
{
	using namespace PeopleNetwork;
	g_CmdLine = CommandLine(argv, argc);
	int adj;

	std::string title = "Kisiler Agi";
	int width = 0, height = 0;
	bool windowed = false;
	bool allowDynamicMode = false;

	const char *fontRegularFile = "Resources/Fonts/Roboto-Regular.ttf",
			   *fontBoldFile    = "Resources/Fonts/Roboto-Bold.ttf",
			   *fontItalicFile  = "Resources/Fonts/Roboto-Italic.ttf";
	const char* peopleLoadDataFile = "Resources/PeopleLoadData.json";

#define OVERRIDE_FROM_CMDLINE(arg, ovr, valset) if (g_CmdLine.ContainsAdjacent(arg, adj)) { ovr = valset; }
#define OVERRIDE_FROM_CMDLINE_STR(arg, ovr) OVERRIDE_FROM_CMDLINE(arg, ovr, g_CmdLine[adj])
#define OVERRIDE_FROM_CMDLINE_INT(arg, ovr) OVERRIDE_FROM_CMDLINE(arg, ovr, atoi(g_CmdLine[adj]))
#define OVERRIDE_FROM_CMDLINE_FLG(arg, bit) bit = g_CmdLine.Contains(arg);

	OVERRIDE_FROM_CMDLINE_STR("/window_title", title);
	OVERRIDE_FROM_CMDLINE_INT("/window_width", width);
	OVERRIDE_FROM_CMDLINE_INT("/window_height", height);
	OVERRIDE_FROM_CMDLINE_FLG("/windowed", windowed);
	OVERRIDE_FROM_CMDLINE_STR("/font_regular", fontRegularFile);
	OVERRIDE_FROM_CMDLINE_STR("/font_bold", fontBoldFile);
	OVERRIDE_FROM_CMDLINE_STR("/font_italic", fontItalicFile);
	OVERRIDE_FROM_CMDLINE_STR("/load_data", peopleLoadDataFile);
	OVERRIDE_FROM_CMDLINE_FLG("/allowdyn", allowDynamicMode);

	// Load people into memory
	PeopleStorage peopleStorage; // 
	{
		PeopleLoadData ldData;
		if (!ldData.PopulateFromJson(peopleLoadDataFile))
		{
			LOG_ERROR("Failed to populate the main PeopleLoadData configuration from file '" << peopleLoadDataFile << "'.");
			return EXIT_FAILURE;
		}

		if (!peopleStorage.LoadFromConfig(ldData))
		{
			LOG_ERROR("Failed to load people to the main people storage.");
			return EXIT_FAILURE;
		}
	}

	sf::VideoMode fsMode = sf::VideoMode::getDesktopMode();
	sf::RenderWindow window
	(
		(width == 0 && height == 0) ? windowed ? sf::VideoMode({ 1280, 720 }) : fsMode : sf::VideoMode(sf::Vector2u(width, height)),
		title,
		sf::Style::Default,
		windowed ? sf::State::Windowed : sf::State::Fullscreen
	);

	// Try init sf fonts
	sf::Font fontRegular, fontBold, fontItalic;
	{
		// FIXME(?): If regular succeeds, perhaps we should allow execution and make bold and italic fonts set to the regular one?
		// Or if any font load succeeds for that matter, perhaps.
		if (!fontRegular.openFromFile(fontRegularFile))
		{
			LOG_ERROR("Failed to load regular font '" << fontRegularFile << "'.");
			return EXIT_FAILURE;
		}
		if (!fontBold.openFromFile(fontBoldFile))
		{
			LOG_ERROR("Failed to load bold font '" << fontBoldFile << "'.");
			return EXIT_FAILURE;
		}
		if (!fontItalic.openFromFile(fontItalicFile))
		{
			LOG_ERROR("Failed to load italic font '" << fontItalicFile << "'.");
			return EXIT_FAILURE;
		}
	}

	Renderer r(window);
	r.BuildPeopleBuffer(peopleStorage.GetPeople());

	std::optional<sf::Event> e;
	while (!(e = window.waitEvent())->is<sf::Event::Closed>())
	{
		auto* eKeyPress = e->getIf<sf::Event::KeyPressed>();
		if (eKeyPress)
		{
			if (eKeyPress->code == sf::Keyboard::Key::F11 && allowDynamicMode)
			{
				windowed = !windowed;
				window.create
				(
					(width == 0 && height == 0) ? windowed ? sf::VideoMode({ 1280, 720 }) : fsMode : sf::VideoMode(sf::Vector2u(width, height)),
					title,
					sf::Style::Default,
					windowed ? sf::State::Windowed : sf::State::Fullscreen
				);
			}
		}

		window.clear(sf::Color(225, 225, 255));

		switch (PSTATE)
		{
		/* People selection screen where */
		case PPPLSEL:
		{
			r.RenderPeopleBuffer();
		}
		}

		window.display();
	}
}
