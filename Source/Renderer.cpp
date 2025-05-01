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
		sf::CircleShape& shape = m_PeopleBuffer[person.Handle] = sf::CircleShape
		(
			IMPORTANCE_BASE_RADII.at(person.Importance) * IMPORTANCE_BASE_RADII_SCALAR,
			CIRCLE_RENDER_DETAIL_FACTOR
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
		m_SortByBuffer.clear();

		std::shared_ptr<sf::Text> text = std::make_shared<sf::Text>(font, L"Dönem Bazlý Filtrele:", 36);
		m_SortByBuffer[SORT_BY_HEADER_TEXT] = text;

		text->setFillColor(sf::Color::Black);
		text->setPosition({ 25.0f, 25.0f });
	}

	void Renderer::BuildPersonInfoBuffer(const Person& person)
	{
		m_PersonInfoBuffer.clear();

		std::wstring contentNameHeader =
			StringUtil::u8tow(person.Name) +
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

		std::wstring contentShortDesc = StringUtil::u8tow(person.ShortDesc);
		contentShortDesc += std::wstring(L" (") + periodToTurkishText(person.Period) + L" Dönemi Düþünürü)\n"
			L"----------------------------------------------------------------------------------------------------------";

		std::shared_ptr<sf::Text> textShortDesc = std::make_shared<sf::Text>(m_FontData.Regular, contentShortDesc, 32);
		m_PersonInfoBuffer[PERSON_INFO_SHORT_DESC] = textShortDesc;

		textShortDesc->setFillColor(sf::Color::Black);
		textShortDesc->setPosition({ 40.0f, 100.0f });

		std::wstring contentMain;
		for (const Person::ContentSection& section : person.ContentSections)
		{
			contentMain += std::wstring(L"Bölüm: ") + StringUtil::u8tow(section.Header) +
				L"\n----------------------------------------------------------------------------------------------------------------------------------------------------------------\n" +
				StringUtil::u8tow(section.Content) +
				L"\n----------------------------------------------------------------------------------------------------------------------------------------------------------------\n\n";
		}

		std::shared_ptr<sf::Text> textContent = std::make_shared<sf::Text>(m_FontData.Regular, contentMain, 24);
		m_PersonInfoBuffer[PERSON_INFO_CONTENT] = textContent;

		textContent->setFillColor(sf::Color::Black);
		textContent->setPosition({ 40.0f, 186.0f });

		if (!m_TextureRetButton.loadFromFile("Resources/Sprites/Button_Return.png"))
		{
			LOG_ERROR("Failed to load return button image!");
			std::exit(EXIT_FAILURE);
		}

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

	void Renderer::RenderPersonTooltip(PRSHANDLE ih, std::string_view name, const sf::Font& font)
	{
		const float ySize = 32;
		float xSize{};

		int adj;
		double factor = 11.0;
		OVERRIDE_FROM_CMDLINE_INT("/xscalefactor", factor);

		xSize = (float)(name.size() * factor);
		const sf::CircleShape& personCircle = m_PeopleBuffer.at(ih);
		float personCircleRadius = personCircle.getRadius();
		sf::Vector2f targetPos = { personCircle.getPosition().x, personCircle.getPosition().y - (personCircleRadius + FACTOR_SHIFT_PERSON_TOOLTIP) };

		sf::RectangleShape rectShape({ xSize, ySize });
		rectShape.setOrigin(rectShape.getSize() / 2.0f);
		rectShape.setPosition(targetPos);
		
		sf::Text nameText(font, StringUtil::u8tow(name.data()), 24);
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
	}

	void Renderer::RenderPersonInfoBuffer()
	{
		m_Window.draw(*m_PersonInfoBuffer.at(PERSON_INFO_NAME_HEADER));
		m_Window.draw(*m_PersonInfoBuffer.at(PERSON_INFO_SHORT_DESC));
		m_Window.draw(*m_PersonInfoBuffer.at(PERSON_INFO_CONTENT));
		m_Window.draw(*m_PersonInfoBuffer.at(PERSON_INFO_BUTTON_RETURN));
	}
}
