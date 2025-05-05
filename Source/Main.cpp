#include "PeopleStorage.h"
#include "CommandLine.h"
#include "StringUtil.h"
#include "FileUtil.h"
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

	// Note: May be technically memory unsafe, but those must be only deleted at the end of the program.
	// Since the OS does it manually, we won't bother as we technically need them at all times.
	if (std::filesystem::exists("RAW_OVERRIDE"))
	{
		std::string RAW_OVERRIDE;
		if (FileUtil::Read("RAW_OVERRIDE", RAW_OVERRIDE))
		{
			std::stringstream ss(RAW_OVERRIDE);
			std::string word;

			g_CmdLine.List = (char**) malloc(1);
			g_CmdLine.Size = 1; // Because argv[0] == EXEC_PATH

			while (ss >> word)
			{
				char* argument = (char*) malloc(word.size() + 1);
				strcpy(argument, word.data());
				argument[word.size()] = '\0'; // NULL_TERMINATOR

				g_CmdLine.Size++;
				g_CmdLine.List = (char**) realloc(g_CmdLine.List, g_CmdLine.Size * sizeof(char**));
				g_CmdLine.List[g_CmdLine.Size - 1] = argument;
			}
		}
	}

	std::string title = "Kisiler Agi";
	int width = 0, height = 0;
	bool windowed = false;
	bool allowDynamicMode = false, window_control = false;

	int dropoff_r = 225;
	int dropoff_g = 225;
	int dropoff_b = 245;

	const char *fontRegularFile = "Resources/Fonts/Roboto-Regular.ttf",
			   *fontBoldFile    = "Resources/Fonts/Roboto-Bold.ttf",
			   *fontItalicFile  = "Resources/Fonts/Roboto-Italic.ttf";
	const char* fontTypePersonTooltip = "regular";
	const char* peopleLoadDataFile = "Resources/PeopleLoadData.json";

	// Value default optimized only for 1920x1080.
	size_t wrapContentTolerance = 169;

	OVERRIDE_FROM_CMDLINE_STR("/window_title", title);
	OVERRIDE_FROM_CMDLINE_INT("/window_width", width);
	OVERRIDE_FROM_CMDLINE_INT("/window_height", height);
	OVERRIDE_FROM_CMDLINE_FLG("/windowed", windowed);
	OVERRIDE_FROM_CMDLINE_FLG("/window_control", window_control);
	OVERRIDE_FROM_CMDLINE_INT("/dropoff_r", dropoff_r);
	OVERRIDE_FROM_CMDLINE_INT("/dropoff_g", dropoff_g);
	OVERRIDE_FROM_CMDLINE_INT("/dropoff_b", dropoff_b);
	OVERRIDE_FROM_CMDLINE_STR("/font_regular", fontRegularFile);
	OVERRIDE_FROM_CMDLINE_STR("/font_bold", fontBoldFile);
	OVERRIDE_FROM_CMDLINE_STR("/font_italic", fontItalicFile);
	OVERRIDE_FROM_CMDLINE_STR("/tooltip_font", fontTypePersonTooltip);
	OVERRIDE_FROM_CMDLINE_STR("/load_data", peopleLoadDataFile);
	OVERRIDE_FROM_CMDLINE_FLG("/allowdyn", allowDynamicMode);
	OVERRIDE_FROM_CMDLINE_INT("/wrap_content_tolerance", wrapContentTolerance);

	// Load people into memory
	PeopleStorage peopleStorage; // 
	peopleStorage.SetWrapContentTolerance(wrapContentTolerance);
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
	sf::VideoMode wndmode;

	if (window_control)
	{
		wndmode = (width == 0 && height == 0) ? windowed ? sf::VideoMode({ 1280, 720 }) : fsMode : sf::VideoMode(sf::Vector2u(width, height));
	}
	else
	{
		wndmode = sf::VideoMode({ 1920, 1080 });
	}

	sf::RenderWindow window
	(
		wndmode,
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
	r.InitMasterQuit();

	bool isDrag = false;
	float lastmousey = 0.0f;

	std::optional<sf::Event> e;
	while (!(e = window.waitEvent())->is<sf::Event::Closed>())
	{
		// Getting all event types we need
		auto* eKeyPress      = e->getIf<sf::Event::KeyPressed>();
		auto* eMouseMove     = e->getIf<sf::Event::MouseMoved>();
		auto* eMousePressed  = e->getIf<sf::Event::MouseButtonPressed>();
		auto* eMouseReleased = e->getIf<sf::Event::MouseButtonReleased>();

		if (eKeyPress)
		{
			// Best to probably convert into a switch statement.
			if (eKeyPress->code == sf::Keyboard::Key::F11 && allowDynamicMode)
			{
				windowed = !windowed;
				window.create
				(
					wndmode,
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
		window.clear(sf::Color((uint8_t) dropoff_r, (uint8_t) dropoff_g, (uint8_t) dropoff_b));
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

				if (!r.IsPassiveHistory() && ((sf::Sprite*) r.GetSortByBuffer().at(SORT_BY_PRE_ISLAMIC).get())->getGlobalBounds().contains((sf::Vector2f) sf::Mouse::getPosition()))
				{
					r.SetSortByOption(r.GetSortByOption() == Renderer::SB_PRE_ISLAMIC ? Renderer::SB_ALL : Renderer::SB_PRE_ISLAMIC);
				}
				else if (((sf::Sprite*) r.GetSortByBuffer().at(SORT_BY_ISLAMIC).get())->getGlobalBounds().contains((sf::Vector2f) sf::Mouse::getPosition()))
				{
					r.SetSortByOption(r.GetSortByOption() == Renderer::SB_ISLAMIC ? Renderer::SB_ALL : Renderer::SB_ISLAMIC);
				}
				else if (((sf::Sprite*) r.GetSortByBuffer().at(SORT_BY_REPUBLIC).get())->getGlobalBounds().contains((sf::Vector2f) sf::Mouse::getPosition()))
				{
					r.SetSortByOption(r.GetSortByOption() == Renderer::SB_REPUBLIC ? Renderer::SB_ALL : Renderer::SB_REPUBLIC);
				}
				else if (r.GetSpriteMasterQuit().getGlobalBounds().contains((sf::Vector2f)sf::Mouse::getPosition()))
				{
					return 0;
				}
			}

			r.RenderPeopleBuffer(peopleStorage);
			r.RenderSortByBuffer();
			r.RenderSpriteMasterQuit();

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

			const float STEP_OFFSET = 10.0f;
			if (eKeyPress)
			{
				switch (eKeyPress->code)
				{
				case sf::Keyboard::Key::Down: r.AddScrollOffset( STEP_OFFSET); break;
				case sf::Keyboard::Key::Up:   r.AddScrollOffset(-STEP_OFFSET); break;
				default: break;
				}
			}

			if (eMousePressed && eMousePressed->button == sf::Mouse::Button::Left)
			{
				isDrag = true;
				lastmousey = (float) eMousePressed->position.y;
			}
			if (eMouseReleased && eMouseReleased->button == sf::Mouse::Button::Left)
			{
				isDrag = false;
			}

			if (eMouseMove && isDrag)
			{
				float cury = (float) eMouseMove->position.y;
				float dy   = cury - lastmousey;
				r.AddScrollOffset(-dy);
				lastmousey = cury;
			}

			r.RenderPersonInfoBuffer();
			break;
		}
		}

		window.display();
	}
}
