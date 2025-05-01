#include "PeopleStorage.h"
#include "CommandLine.h"
#include "StringUtil.h"
#include "Renderer.h"
#include "Log.h"

#include "SFML/Graphics.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

enum ProgramState
{
	PPPLSEL, // People selection screen
	PPRSINF // Person info screen
};
ProgramState PSTATE = PPPLSEL;
PeopleNetwork::PRSHANDLE PPPLSELSUBJ = PeopleNetwork::INVALID_PERSON_HANDLE; // People Selection Subject (mouse hovered person)
PeopleNetwork::Person&   PPRSINFSUBJ = PeopleNetwork::INVALID_PERSON_STRUCT; // Person Info Subject

int main(int argc, char** argv)
{
	using namespace PeopleNetwork;
	g_CmdLine = { argv, argc };
	int adj = ARG_NONE;

	std::string title = "Kisiler Agi";
	int width = 0, height = 0;
	bool windowed = false;
	bool allowDynamicMode = false;

	const char *fontRegularFile = "Resources/Fonts/Roboto-Regular.ttf",
			   *fontBoldFile    = "Resources/Fonts/Roboto-Bold.ttf",
			   *fontItalicFile  = "Resources/Fonts/Roboto-Italic.ttf";
	const char* fontTypePersonTooltip = "regular";
	const char* peopleLoadDataFile = "Resources/PeopleLoadData.json";

	OVERRIDE_FROM_CMDLINE_STR("/window_title", title);
	OVERRIDE_FROM_CMDLINE_INT("/window_width", width);
	OVERRIDE_FROM_CMDLINE_INT("/window_height", height);
	OVERRIDE_FROM_CMDLINE_FLG("/windowed", windowed);
	OVERRIDE_FROM_CMDLINE_STR("/font_regular", fontRegularFile);
	OVERRIDE_FROM_CMDLINE_STR("/font_bold", fontBoldFile);
	OVERRIDE_FROM_CMDLINE_STR("/font_italic", fontItalicFile);
	OVERRIDE_FROM_CMDLINE_STR("/tooltip_font", fontTypePersonTooltip);
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

	// FIXME: Doesn't set with fullscreen for some reason.
	if (title == "Kisiler Agi")
	{
		sf::WindowHandle handle = window.getNativeHandle();
		SetWindowTextW(handle, L"Kiþiler Aðý");
	}

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
	sf::Font& fontRefPersonTooltip = fontRegular;
	if (strcmp(fontTypePersonTooltip, "regular") == 1)
	{
		fontRefPersonTooltip = fontRegular;
	}
	else if (strcmp(fontTypePersonTooltip, "bold") == 1)
	{
		fontRefPersonTooltip = fontBold;
	}
	else if (strcmp(fontTypePersonTooltip, "italic") == 1)
	{
		fontRefPersonTooltip = fontItalic;
	}

	Renderer r({ fontRegular, fontBold, fontItalic }, window);
	r.BuildPeopleBuffer(peopleStorage.GetPeople());
	r.BuildSortByBuffer(fontRegular);
	
	std::optional<sf::Event> e;
	while (!(e = window.waitEvent())->is<sf::Event::Closed>())
	{
		// Getting all event types we need
		auto* eKeyPress      = e->getIf<sf::Event::KeyPressed>();
		auto* eMouseMove     = e->getIf<sf::Event::MouseMoved>();
		auto* eMouseReleased = e->getIf<sf::Event::MouseButtonReleased>();

		if (eKeyPress)
		{
			// Best to probably convert into a switch statement.
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
			else if (eKeyPress->code == sf::Keyboard::Key::Q || eKeyPress->code == sf::Keyboard::Key::Escape)
			{
				return EXIT_SUCCESS;
			}
		}
		
	StateHandling:
		window.clear(sf::Color(225, 225, 245));
		switch (PSTATE)
		{
		/* People selection screen */
		case PPPLSEL:
		{
			if (eMouseReleased)
			{
				if (PPPLSELSUBJ != INVALID_PERSON_HANDLE && r.GetPeopleBuffer().at(PPPLSELSUBJ).getGlobalBounds().contains((sf::Vector2f) sf::Mouse::getPosition()))
				{
					PSTATE = PPRSINF;
					r.BuildPersonInfoBuffer(peopleStorage.GetPeople().at(PPPLSELSUBJ));
					goto StateHandling; // Re-execute loop
				}
			}

			r.RenderPeopleBuffer();
			r.RenderSortByBuffer();

			if (eMouseMove)
			{
				if (PPPLSELSUBJ != INVALID_PERSON_HANDLE)
				{
					const sf::CircleShape& shape = r.GetPeopleBuffer().at(PPPLSELSUBJ);

					// If mouse is still within the subject circle, we know the hover person did not change, so
					// we shouldn't check for where it is contained.
					if (shape.getGlobalBounds().contains((sf::Vector2f) eMouseMove->position))
					{
						goto PostHoverHandle;
					}
					// Subject changed or invalidated.
					else
					{
						PPPLSELSUBJ = INVALID_PERSON_HANDLE;
					}
				}

				for (auto&& [handle, shape] : r.GetPeopleBuffer())
				{
					if (shape.getGlobalBounds().contains((sf::Vector2f) eMouseMove->position))
					{
						PPPLSELSUBJ = handle;
						break;
					}
				}
			}
		PostHoverHandle:
			if (PPPLSELSUBJ)
			{
				r.RenderPersonTooltip(PPPLSELSUBJ, peopleStorage.GetPeople().at(PPPLSELSUBJ).Name, fontRefPersonTooltip);
			}

			break;
		}
		case PPRSINF:
		{
			if (eMouseReleased)
			{
				if (((sf::Sprite*) r.GetPersonInfoBuffer().at(PERSON_INFO_BUTTON_RETURN).get())->getGlobalBounds().contains((sf::Vector2f) sf::Mouse::getPosition()))
				{
					PSTATE      = PPPLSEL;
					PPPLSELSUBJ = INVALID_PERSON_HANDLE;
					PPRSINFSUBJ = INVALID_PERSON_STRUCT;

					goto StateHandling;
				}
			}

			r.RenderPersonInfoBuffer();
			break;
		}
		}

		window.display();
	}
}
