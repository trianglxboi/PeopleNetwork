#include "Renderer.h"

#include "CommandLine.h"
#include "StringUtil.h"
#include "Log.h"

namespace PeopleNetwork
{
	Renderer::Renderer(const RendererFontData& fontData, sf::RenderWindow& window)
		: m_FontData(fontData), m_Window(window)
	{
	}

	Renderer::~Renderer()
	{
	}

	void Renderer::PushToPeopleBuffer(const Person& person)
	{
		int adj;

		float radiiScalar = IMPORTANCE_BASE_RADII_SCALAR;
		int detailFactor  = CIRCLE_RENDER_DETAIL_FACTOR;

		OVERRIDE_FROM_CMDLINE_FLT("/radii_scalar", radiiScalar);
		OVERRIDE_FROM_CMDLINE_INT("/detail_factor", detailFactor);

		sf::CircleShape& shape = m_PeopleBuffer[person.Handle] = sf::CircleShape
		(
			IMPORTANCE_BASE_RADII.at(person.Importance) * radiiScalar,
			detailFactor
		);
		shape.setOrigin({ shape.getRadius(), shape.getRadius() });
		const Person::StructDataGUI& gui = person.DataGUI;

		shape.setFillColor({ (uint8_t) gui.Color.R, (uint8_t) gui.Color.G, (uint8_t) gui.Color.B, 0xff });
		shape.setPosition({ m_Window.getSize().x / 2.0f + (float) gui.Position.X, m_Window.getSize().y / 2.0f + (float) gui.Position.Y });
	}

	void Renderer::BuildPeopleBuffer(const std::vector<Person>& people)
	{
		m_PeopleBuffer.clear();
		for (const Person& person : people)
		{
			PushToPeopleBuffer(person);
		}
	}

	void Renderer::BuildPeopleBuffer(const std::unordered_map<PRSHANDLE, Person>& people)
	{
		m_PeopleBuffer.clear();
		for (auto&& [handle, person] : people)
		{
			PushToPeopleBuffer(person);
		}
	}

	void Renderer::BuildSortByBuffer(const sf::Font& font)
	{
		int adj;
		m_SortByBuffer.clear();

		std::shared_ptr<sf::Text> text = std::make_shared<sf::Text>(font, L"UNINITIALIZED_TEXT", 36);
		m_SortByBuffer[SORT_BY_HEADER_TEXT] = text; // NOTE: MUST BE DONE BEFORE THE CODE ON THE NEXT LINE!
		SetSortByOption(m_SortByOption); // Refreshes the contents of TEXT for HEADER.

		text->setFillColor(sf::Color::Black);
		text->setPosition({ 25.0f, 25.0f });

		const char* preIslamicFile = "Resources/Sprites/Button_PreIslamic.png";
		const char* islamicFile = "Resources/Sprites/Button_Islamic.png";
		const char* republicFile = "Resources/Sprites/Button_Republic.png";

		OVERRIDE_FROM_CMDLINE_STR("/pre_islamic_sb", preIslamicFile);
		OVERRIDE_FROM_CMDLINE_STR("/islamic_sb", preIslamicFile);
		OVERRIDE_FROM_CMDLINE_STR("/republic_sb", preIslamicFile);

		auto load = [](std::string_view file, sf::Texture& ldTarget, bool& flagTarget, std::string_view logErrorDescriptor)
		{
			if (!flagTarget && !ldTarget.loadFromFile(file))
			{
				LOG_ERROR("Failed to load " << logErrorDescriptor << " period sort/by button image from file '" << file << "'!");
				std::exit(EXIT_FAILURE);
			}
			flagTarget = true;
		};

		load(preIslamicFile, m_TexturePreIslamic, m_bLoadedTexturePreIslamic, "PreIslamic");
		load(islamicFile,    m_TextureIslamic,    m_bLoadedTextureIslamic,    "Islamic");
		load(republicFile,   m_TextureRepublic,   m_bLoadedTextureRepublic,   "Republic");

		bool passive_history = false;
		OVERRIDE_FROM_CMDLINE_FLG("/passive_history", passive_history);

		float initial_favor_x = 25.0f;
		float favor_x = initial_favor_x, favor_y = 80.0f;
		float favor_xsepfactor = 120.0f;

		OVERRIDE_FROM_CMDLINE_FLT("/favor_x", initial_favor_x);
		OVERRIDE_FROM_CMDLINE_FLT("/favor_y", favor_y);
		OVERRIDE_FROM_CMDLINE_FLT("/favor_xsepfactor", favor_xsepfactor);
		favor_x = initial_favor_x;

		if (!passive_history)
		{
			std::shared_ptr<sf::Sprite> preIslamic = std::make_shared<sf::Sprite>(m_TexturePreIslamic);
			m_SortByBuffer[SORT_BY_PRE_ISLAMIC] = preIslamic;

			preIslamic->setPosition({ favor_x, favor_y });
			preIslamic->setScale(preIslamic->getScale() / 3.0f);

			favor_x += initial_favor_x;
			favor_x += favor_xsepfactor;
		}
		else
		{
			m_SortByBuffer[SORT_BY_PRE_ISLAMIC] = nullptr;
		}

		std::shared_ptr<sf::Sprite> islamic = std::make_shared<sf::Sprite>(m_TextureIslamic);
		m_SortByBuffer[SORT_BY_ISLAMIC] = islamic;

		islamic->setPosition({ favor_x, favor_y });
		islamic->setScale(islamic->getScale() / 3.0f);

		favor_x += initial_favor_x;
		favor_x += favor_xsepfactor;

		std::shared_ptr<sf::Sprite> republic = std::make_shared<sf::Sprite>(m_TextureRepublic);
		m_SortByBuffer[SORT_BY_REPUBLIC] = republic;

		republic->setPosition({ favor_x, favor_y });
		republic->setScale(republic->getScale() / 3.0f);
	}

	void Renderer::BuildPersonInfoBuffer(const Person& person)
	{
		int adj;
		m_PersonInfoBuffer.clear();

		std::wstring contentNameHeader =
			person.Name +
			std::wstring(L" (") + std::to_wstring(person.Lifetime.first) + L'-' + std::to_wstring(person.Lifetime.second) + L')';

		std::shared_ptr<sf::Text> textHeader = std::make_shared<sf::Text>(m_FontData.Bold, contentNameHeader, 56);
		m_PersonInfoBuffer[PERSON_INFO_NAME_HEADER] = textHeader;

		textHeader->setFillColor(sf::Color::Black);
		textHeader->setPosition({ 35.0f, 35.0f });

		auto periodToTurkishText = [](Person::HistoricalPeriod period) -> std::wstring
		{
			switch (period)
			{
			case Person::HistoricalPeriod::PreIslamic: return L"Ýslamiyet Öncesi";
			case Person::HistoricalPeriod::Islamic:    return L"Ýslamiyet";
			case Person::HistoricalPeriod::Republic:   return L"Cumhuriyet";
			}
			return {};
		};

		std::wstring contentShortDesc = person.ShortDesc;
		contentShortDesc += std::wstring(L" (") + periodToTurkishText(person.Period) + L" Dönemi Düþünürü)\n"
			L"----------------------------------------------------------------------------------------------------------";

		bool hardFixDisable = false;
		OVERRIDE_FROM_CMDLINE_FLG("/hard_fix_disable", hardFixDisable);

		if (!hardFixDisable && person.Period == Person::HistoricalPeriod::PreIslamic)
		{
			contentShortDesc.erase(person.ShortDesc.size() + 24, 1);
		}

		std::shared_ptr<sf::Text> textShortDesc = std::make_shared<sf::Text>(m_FontData.Regular, contentShortDesc, 32);
		m_PersonInfoBuffer[PERSON_INFO_SHORT_DESC] = textShortDesc;

		textShortDesc->setFillColor(sf::Color::Black);
		textShortDesc->setPosition({ 40.0f, 100.0f });

		std::wstring contentMain;
		for (const Person::ContentSection& section : person.ContentSections)
		{
			contentMain += std::wstring(L"Bölüm: ") + section.Header +
				L"\n----------------------------------------------------------------------------------------------------------------------------------------------------------------\n" +
				section.Content +
				L"\n----------------------------------------------------------------------------------------------------------------------------------------------------------------\n\n";
		}

		std::shared_ptr<sf::Text> textContent = std::make_shared<sf::Text>(m_FontData.Regular, contentMain, 24);
		m_PersonInfoBuffer[PERSON_INFO_CONTENT] = textContent;

		textContent->setFillColor(sf::Color::Black);
		textContent->setPosition({ 40.0f, 186.0f });

		const char* retButton = "Resources/Sprites/Button_Return.png";
		OVERRIDE_FROM_CMDLINE_STR("/return_button", retButton);

		if (&m_bLoadedTextureRetButton && !m_TextureRetButton.loadFromFile(retButton))
		{
			LOG_ERROR("Failed to load return button image from file '" << retButton << "'!");
			std::exit(EXIT_FAILURE);
		}
		m_bLoadedTextureRetButton = true;

		std::shared_ptr<sf::Sprite> sprite = std::make_shared<sf::Sprite>(m_TextureRetButton);
		m_PersonInfoBuffer[PERSON_INFO_BUTTON_RETURN] = sprite;

		sprite->setOrigin({ m_TextureRetButton.getSize().x / 2.0f, m_TextureRetButton.getSize().y / 2.0f });
		sprite->setPosition({ m_Window.getSize().x - 95.0f, 95.0f });
		sprite->setScale(sprite->getScale() / 3.0f);
	}

	void Renderer::RenderPeopleBuffer()
	{
		for (auto&& [person, shape] : m_PeopleBuffer)
		{
			m_Window.draw(shape);
		}
	}

	void Renderer::RenderPersonTooltip(PRSHANDLE ih, std::wstring_view name, const sf::Font& font)
	{
		const float ySize = 32;
		float xSize{};

		int adj;
		double factor = 13.0;
		OVERRIDE_FROM_CMDLINE_INT("/xscalefactor", factor);

		xSize = (float)(name.size() * factor);
		const sf::CircleShape& personCircle = m_PeopleBuffer.at(ih);
		float personCircleRadius = personCircle.getRadius();
		sf::Vector2f targetPos = { personCircle.getPosition().x, personCircle.getPosition().y - (personCircleRadius + FACTOR_SHIFT_PERSON_TOOLTIP) };

		sf::RectangleShape rectShape({ xSize, ySize });
		rectShape.setOrigin(rectShape.getSize() / 2.0f);
		rectShape.setPosition(targetPos);
		
		sf::Text nameText(font, name.data(), 24);
		sf::FloatRect nameTextBounds = nameText.getLocalBounds();
		nameText.setOrigin(nameTextBounds.position + nameTextBounds.size / 2.f);
		nameText.setPosition(targetPos);
		nameText.setFillColor({ 0, 0, 0, 255 });

		m_Window.draw(rectShape);
		m_Window.draw(nameText);
	}

	void Renderer::RenderSortByBuffer()
	{
		m_Window.draw(*m_SortByBuffer.at(SORT_BY_HEADER_TEXT));
		if (m_SortByBuffer.at(SORT_BY_PRE_ISLAMIC))
			m_Window.draw(*m_SortByBuffer.at(SORT_BY_PRE_ISLAMIC));
		m_Window.draw(*m_SortByBuffer.at(SORT_BY_ISLAMIC));
		m_Window.draw(*m_SortByBuffer.at(SORT_BY_REPUBLIC));
	}

	void Renderer::RenderPersonInfoBuffer()
	{
		m_Window.draw(*m_PersonInfoBuffer.at(PERSON_INFO_NAME_HEADER));
		m_Window.draw(*m_PersonInfoBuffer.at(PERSON_INFO_SHORT_DESC));
		m_Window.draw(*m_PersonInfoBuffer.at(PERSON_INFO_CONTENT));
		m_Window.draw(*m_PersonInfoBuffer.at(PERSON_INFO_BUTTON_RETURN));
	}

	void Renderer::SetSortByOption(SortByOptions option)
	{
		m_SortByOption = option;
		if (m_SortByBuffer.contains(SORT_BY_HEADER_TEXT) && m_SortByBuffer[SORT_BY_HEADER_TEXT])
		{
			sf::Text* text = (sf::Text*) m_SortByBuffer[SORT_BY_HEADER_TEXT].get();

			auto sboToStr = [](SortByOptions sbo) -> std::wstring_view
			{
				switch (sbo)
				{
				case SB_ALL:         return L"Hepsi";
				case SB_PRE_ISLAMIC: return L"Ýslamiyet Öncesi";
				case SB_ISLAMIC:     return L"Ýslamiyet Dönemi";
				case SB_REPUBLIC:    return L"Cumhuriyet Dönemi";
				}
			};

			text->setString(std::wstring(L"Dönem Bazlý Filtrele: ") + sboToStr(m_SortByOption).data());
		}
	}
}
