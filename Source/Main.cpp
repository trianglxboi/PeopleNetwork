#include "PeopleStorage.h"
#include "CommandLine.h"
#include "StringUtil.h"
#include "FileUtil.h"
#include "Renderer.h"
#include "Log.h"

#include "SFML/Graphics.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

using namespace PeopleNetwork;

enum class ProgramState
{
	PeopleSelection,
	PersonInformation
};

ProgramState g_ProgramState             = ProgramState::PeopleSelection;
PRSHANDLE    g_PeopleSelectionSubject   = INVALID_PERSON_HANDLE;
Person&      g_PersonInformationSubject = INVALID_PERSON_STRUCT;

int main(int argc, char** argv)
{
	g_CmdLine = { argv, argc };
	g_CommandLineOptions.Populate(g_CmdLine);

	if ((g_CommandLineOptions.PreferRawOverride || g_CmdLine.Size <= 1) && std::filesystem::exists("RAW_OVERRIDE"))
	{
		std::string rawOverrideContent;
		if (FileUtil::Read("RAW_OVERRIDE", rawOverrideContent))
		{
			std::stringstream ss(rawOverrideContent);
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

			// Repopulate since g_CmdLine was rebuilt.
			g_CommandLineOptions.Populate(g_CmdLine);
		}
	}

	// Load people into memory
	PeopleStorage peopleStorage;
	peopleStorage.SetWrapContentTolerance(g_CommandLineOptions.WrapContentTolerance);
	{
		PeopleLoadData loadData;
		if (!loadData.PopulateFromJson(g_CommandLineOptions.LoadData))
		{
			LOG_ERROR("Failed to populate the main PeopleLoadData configuration from file '" << g_CommandLineOptions.LoadData << "'.");
			return EXIT_FAILURE;
		}

		if (!peopleStorage.LoadFromConfig(loadData))
		{
			LOG_ERROR("Failed to load people to the main people storage.");
			return EXIT_FAILURE;
		}
	}

	sf::VideoMode desktopVideoMode = sf::VideoMode::getDesktopMode();
	sf::VideoMode windowVideoMode;

	if (g_CommandLineOptions.WindowControl)
	{
		windowVideoMode =
			(g_CommandLineOptions.WindowWidth == 0 && g_CommandLineOptions.WindowHeight == 0) ?
			 g_CommandLineOptions.Windowed ? sf::VideoMode({ 1280, 720 }) : desktopVideoMode : sf::VideoMode(sf::Vector2u(g_CommandLineOptions.WindowWidth, g_CommandLineOptions.WindowHeight));
	}
	else
	{
		windowVideoMode = sf::VideoMode({ 1920, 1080 });
	}

	sf::RenderWindow window
	(
		windowVideoMode,
		g_CommandLineOptions.WindowTitle,
		sf::Style::Default,
		g_CommandLineOptions.WindowControl && g_CommandLineOptions.Windowed ? sf::State::Windowed : sf::State::Fullscreen
	);

	// FIXME(?): Doesn't set with fullscreen for some reason, not that it matters.
	if (g_CommandLineOptions.WindowTitle == "Kisiler Agi")
	{
		sf::WindowHandle handle = window.getNativeHandle();
		SetWindowTextW(handle, L"Kiþiler Aðý");
	}

	// Try init sf fonts
	sf::Font fontRegular, fontBold, fontItalic;
	{
		// FIXME(?): If regular succeeds, perhaps we should allow execution and make bold and italic fonts set to the regular one?
		// Or if any font load succeeds for that matter, perhaps.
		if (!fontRegular.openFromFile(g_CommandLineOptions.FontRegular))
		{
			LOG_ERROR("Failed to load regular font '" << g_CommandLineOptions.FontRegular << "'.");
			return EXIT_FAILURE;
		}
		if (!fontBold.openFromFile(g_CommandLineOptions.FontBold))
		{
			LOG_ERROR("Failed to load bold font '" << g_CommandLineOptions.FontBold << "'.");
			return EXIT_FAILURE;
		}
		if (!fontItalic.openFromFile(g_CommandLineOptions.FontItalic))
		{
			LOG_ERROR("Failed to load italic font '" << g_CommandLineOptions.FontItalic << "'.");
			return EXIT_FAILURE;
		}
	}
	sf::Font& fontRefPersonTooltip = fontRegular;
	if (g_CommandLineOptions.TooltipFont == "Regular")
	{
		fontRefPersonTooltip = fontRegular;
	}
	else if (g_CommandLineOptions.TooltipFont == "Bold")
	{
		fontRefPersonTooltip = fontBold;
	}
	else if (g_CommandLineOptions.TooltipFont == "Italic")
	{
		fontRefPersonTooltip = fontItalic;
	}

	Renderer r({ fontRegular, fontBold, fontItalic }, window);
	r.BuildPeopleBuffer(peopleStorage.GetPeople());
	r.BuildSortByBuffer(fontBold);
	r.InitMasterQuit();

	bool isDrag = false;
	float lastmousey = 0.0f;

	std::optional<sf::Event> e;
	while (!(e = window.waitEvent())->is<sf::Event::Closed>())
	{
		// Getting all event types we need
		auto* eKeyPress      = e->getIf<sf::Event::KeyPressed>();
		auto* eMouseMove     = e->getIf<sf::Event::MouseMoved>();
		auto* eMouseScrolled = e->getIf<sf::Event::MouseWheelScrolled>();
		auto* eMousePressed  = e->getIf<sf::Event::MouseButtonPressed>();
		auto* eMouseReleased = e->getIf<sf::Event::MouseButtonReleased>();

		if (eKeyPress)
		{
			// Best to probably convert into a switch statement.
			if (eKeyPress->code == sf::Keyboard::Key::F11 && g_CommandLineOptions.AllowDynamic)
			{
				g_CommandLineOptions.Windowed = !g_CommandLineOptions.Windowed;
				window.create
				(
					windowVideoMode,
					g_CommandLineOptions.WindowTitle,
					sf::Style::Default,
					g_CommandLineOptions.Windowed ? sf::State::Windowed : sf::State::Fullscreen
				);
			}
			else if (eKeyPress->code == sf::Keyboard::Key::Q || eKeyPress->code == sf::Keyboard::Key::Escape)
			{
				return EXIT_SUCCESS;
			}
		}
		
	StateHandling:
		window.clear(sf::Color(g_CommandLineOptions.DropoffR, g_CommandLineOptions.DropoffG, g_CommandLineOptions.DropoffB));
		switch (g_ProgramState)
		{
		/* People selection screen */
		case ProgramState::PeopleSelection:
		{
		#ifndef NDEBUG
			{
				sf::Text textReload(fontItalic, L"[Geliþtirici Derlemesi] Çalýþma zamanýnda kiþi bilgilerini yeniden yüklemek için R tuþu kullanýma açýlmýþtýr.", 24);
				textReload.setPosition({ 10.0f, window.getSize().y - 35.0f });
				textReload.setFillColor(sf::Color::Red);

				if (eKeyPress && eKeyPress->code == sf::Keyboard::Key::R)
				{
					PeopleLoadData loadData;
					if (!loadData.PopulateFromJson(g_CommandLineOptions.LoadData))
					{
						LOG_ERROR("Failed to populate the main PeopleLoadData configuration from file '" << g_CommandLineOptions.LoadData << "'.");
						return EXIT_FAILURE;
					}

					if (!peopleStorage.LoadFromConfig(loadData, true))
					{
						LOG_ERROR("Failed to load people to the main people storage.");
						return EXIT_FAILURE;
					}

					g_PeopleSelectionSubject = INVALID_PERSON_HANDLE;
					r.BuildPeopleBuffer(peopleStorage.GetPeople());
				}

				window.draw(textReload);
			}
		#endif

			if (!g_CommandLineOptions.DisableAuthority)
			{
				sf::Text authorityText(fontRegular, "    Kaan Kadim Levent\nOruç Reis Anadolu Lisesi", 24);
				authorityText.setPosition({ window.getSize().x - 300.0f, 25.0f });
				authorityText.setFillColor(sf::Color::Black);
				window.draw(authorityText);
			}

			if (eMouseReleased)
			{
				if (g_PeopleSelectionSubject != INVALID_PERSON_HANDLE && r.GetPeopleBuffer().at(g_PeopleSelectionSubject).getGlobalBounds().contains((sf::Vector2f) sf::Mouse::getPosition()))
				{
					g_ProgramState = ProgramState::PersonInformation;
					r.BuildPersonInfoBuffer(peopleStorage.GetPeople().at(g_PeopleSelectionSubject));
					goto StateHandling; // Re-execute loop
				}

				if (!g_CommandLineOptions.PassiveHistory && ((sf::Sprite*) r.GetSortByBuffer().at(SORT_BY_PRE_ISLAMIC).get())->getGlobalBounds().contains((sf::Vector2f) sf::Mouse::getPosition()))
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
				if (g_PeopleSelectionSubject != INVALID_PERSON_HANDLE)
				{
					const sf::CircleShape& shape = r.GetPeopleBuffer().at(g_PeopleSelectionSubject);

					// If mouse is still within the subject circle, we know the hover person did not change, so
					// we shouldn't check for where it is contained.
					if (shape.getGlobalBounds().contains((sf::Vector2f) eMouseMove->position))
					{
						goto PostHoverHandle;
					}
					// Subject changed or invalidated.
					else
					{
						g_PeopleSelectionSubject = INVALID_PERSON_HANDLE;
					}
				}

				for (auto&& [handle, shape] : r.GetPeopleBuffer())
				{
					if (shape.getGlobalBounds().contains((sf::Vector2f) eMouseMove->position))
					{
						g_PeopleSelectionSubject = handle;
						break;
					}
				}
			}
		PostHoverHandle:
			if (g_PeopleSelectionSubject)
			{
				r.RenderPersonTooltip(g_PeopleSelectionSubject, peopleStorage.GetPeople().at(g_PeopleSelectionSubject).Name, fontRefPersonTooltip);
			}

			break;
		}
		case ProgramState::PersonInformation:
		{
			if (eMouseReleased)
			{
				if (((sf::Sprite*) r.GetPersonInfoBuffer().at(PERSON_INFO_BUTTON_RETURN).get())->getGlobalBounds().contains((sf::Vector2f) sf::Mouse::getPosition()))
				{
					g_ProgramState             = ProgramState::PeopleSelection;
					g_PeopleSelectionSubject   = INVALID_PERSON_HANDLE;
					g_PersonInformationSubject = INVALID_PERSON_STRUCT;
					r.SetScrollOffset(0.0f);

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

			if (eMouseScrolled)
			{
				r.AddScrollOffset(-eMouseScrolled->delta * 50.0f);
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
