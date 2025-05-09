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
			IMPORTANCE_BASE_RADII.at(person.Importance) * g_CommandLineOptions.RadiiScalar,
			g_CommandLineOptions.DetailFactor
		);
		shape.setOrigin({ shape.getRadius(), shape.getRadius() });
		const Person::StructDataGUI& gui = person.DataGUI;

		shape.setFillColor({ (uint8_t) gui.Color.R, (uint8_t) gui.Color.G, (uint8_t) gui.Color.B, 0xff });
		shape.setPosition
		({
			(m_Window.getSize().x / 2.0f + (float) gui.Position.X) + g_CommandLineOptions.MasterShiftX,
			(m_Window.getSize().y / 2.0f + (float) gui.Position.Y) + g_CommandLineOptions.MasterShiftY
		});
		shape.setOutlineColor({ g_CommandLineOptions.OutlineR, g_CommandLineOptions.OutlineG, g_CommandLineOptions.OutlineB, 255 });
		shape.setOutlineThickness(g_CommandLineOptions.OutlineThickness);

		if (g_CommandLineOptions.NewOutlineMethod)
		{
			switch (person.Period)
			{
			case Person::HistoricalPeriod::Islamic:
				shape.setOutlineColor({ 38, 127, 0, 255 });
				break;
			case Person::HistoricalPeriod::Republic:
				shape.setOutlineColor({ 227, 10, 23, 255 });
				break;
			}
		}
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

		std::shared_ptr<sf::Text> text = std::make_shared<sf::Text>(font, L"UNINITIALIZED_TEXT", 36);
		m_SortByBuffer[SORT_BY_HEADER_TEXT] = text; // NOTE: MUST BE DONE BEFORE THE CODE ON THE NEXT LINE!
		SetSortByOption(m_SortByOption); // Refreshes the contents of TEXT for HEADER.

		text->setFillColor(sf::Color::Black);
		text->setPosition({ 25.0f, 25.0f });

		auto load = [](std::string_view file, sf::Texture& ldTarget, bool& flagTarget, std::string_view logErrorDescriptor)
		{
			if (!flagTarget && !ldTarget.loadFromFile(file))
			{
				LOG_ERROR("Failed to load " << logErrorDescriptor << " period sort/by button image from file '" << file << "'!");
				std::exit(EXIT_FAILURE);
			}
			flagTarget = true;
		};

		load(g_CommandLineOptions.PreIslamicSB, m_TexturePreIslamic, m_bLoadedTexturePreIslamic, "PreIslamic");
		load(g_CommandLineOptions.IslamicSB,    m_TextureIslamic,    m_bLoadedTextureIslamic,    "Islamic");
		load(g_CommandLineOptions.RepublicSB,   m_TextureRepublic,   m_bLoadedTextureRepublic,   "Republic");

		float initialOffsetX = g_CommandLineOptions.SBOffsetX;
		float currentOffsetX = initialOffsetX;
		float offsetIncline  = initialOffsetX + g_CommandLineOptions.SBSeperationFactor;

		if (!g_CommandLineOptions.PassiveHistory)
		{
			std::shared_ptr<sf::Sprite> preIslamic = std::make_shared<sf::Sprite>(m_TexturePreIslamic);
			m_SortByBuffer[SORT_BY_PRE_ISLAMIC] = preIslamic;

			preIslamic->setPosition({ g_CommandLineOptions.SBOffsetX, g_CommandLineOptions.SBOffsetY });
			preIslamic->setScale(preIslamic->getScale() / 3.0f);

			currentOffsetX += offsetIncline;
		}
		else
		{
			m_SortByBuffer[SORT_BY_PRE_ISLAMIC] = nullptr;
		}

		std::shared_ptr<sf::Sprite> islamic = std::make_shared<sf::Sprite>(m_TextureIslamic);
		m_SortByBuffer[SORT_BY_ISLAMIC] = islamic;

		islamic->setPosition({ currentOffsetX, g_CommandLineOptions.SBOffsetY });
		islamic->setScale(islamic->getScale() / 3.0f);
		currentOffsetX += offsetIncline;

		std::shared_ptr<sf::Sprite> republic = std::make_shared<sf::Sprite>(m_TextureRepublic);
		m_SortByBuffer[SORT_BY_REPUBLIC] = republic;

		republic->setPosition({ currentOffsetX, g_CommandLineOptions.SBOffsetY });
		republic->setScale(republic->getScale() / 3.0f);

		std::shared_ptr<sf::Text> descriptor = std::make_shared<sf::Text>(font, L"Filtreyi kaldýrmak için aktif\ndönem filtreleme tuþuna\ntekrar basýn.", 24);
		m_SortByBuffer[SORT_BY_DESCRIPTOR] = descriptor; // NOTE: MUST BE DONE BEFORE THE CODE ON THE NEXT LINE!

		descriptor->setPosition({ 25.0f, 210.0f });
		descriptor->setFillColor(sf::Color::Black);
	}

	void Renderer::BuildPersonInfoBuffer(const Person& person)
	{
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
		m_PersonInfoBuffer.clear();

		std::wstring contentNameHeader =
			person.Name +
			std::wstring(L" (") + std::to_wstring(person.Lifetime.first) + L'-' + std::to_wstring(person.Lifetime.second) + L") [" + periodToTurkishText(person.Period) + L" Dönemi Düþünürü]\n";

		std::shared_ptr<sf::Text> textHeader = std::make_shared<sf::Text>(m_FontData.Bold, contentNameHeader, 56);
		m_PersonInfoBuffer[PERSON_INFO_NAME_HEADER] = textHeader;

		textHeader->setFillColor(sf::Color::Black);
		textHeader->setPosition({ 35.0f, 35.0f });

		std::wstring contentShortDesc = person.ShortDesc + L"\n----------------------------------------------------------------------------------------------------------";
		if (person.Period == Person::HistoricalPeriod::PreIslamic)
		{
			contentShortDesc.erase(person.ShortDesc.size() + 24, 1);
		}

		std::shared_ptr<sf::Text> textShortDesc = std::make_shared<sf::Text>(m_FontData.Regular, contentShortDesc, 32);
		m_PersonInfoBuffer[PERSON_INFO_SHORT_DESC] = textShortDesc;
		
		textShortDesc->setFillColor(sf::Color::Black);
		textShortDesc->setPosition({ 35.0f, 100.0f });

		std::wstring contentMain;
		for (const Person::ContentSection& section : person.ContentSections)
		{
			contentMain += std::wstring(L"Bölüm: ") + section.Header +
				L"\n----------------------------------------------------------------------------------------------------------------------------------------------------------------\n" +
				section.Content +
				L"\n----------------------------------------------------------------------------------------------------------------------------------------------------------------\n\n";
		}

		std::shared_ptr<sf::Text> textContent = std::make_shared<sf::Text>(m_FontData.Regular);
		m_PersonInfoBuffer[PERSON_INFO_CONTENT_TEXT] = textContent;
		
		auto wrapText = [](const std::wstring& string, const sf::Font& font, sf::Text& text, unsigned int characterSize, float maxWidth) -> std::wstring
		{
			text = sf::Text(font, L"", characterSize);
			std::unordered_map<std::wstring, float> widthCache;
			float spaceWidth;

			text.setString(L" ");
			spaceWidth = text.getLocalBounds().size.x;

			std::wistringstream lines(string);
			std::wstring line, finalResult;

			while (std::getline(lines, line))
			{
			    std::wistringstream words(line);
			    std::wstring word, currentLine;
			    float lineWidth = 0.f;

			    while (words >> word)
				{
			        float wordWidth;
			        if (widthCache.count(word))
					{
			            wordWidth = widthCache[word];
			        }
					else
					{
			            text.setString(word);
			            wordWidth = text.getLocalBounds().size.x;
			            widthCache[word] = wordWidth;
			        }

			        float totalWidth = lineWidth + (currentLine.empty() ? 0.f : spaceWidth) + wordWidth;

			        if (totalWidth > maxWidth)
					{
			            finalResult += currentLine + L'\n';
			            currentLine = word;
			            lineWidth = wordWidth;
			        }
					else
					{
						if (!currentLine.empty())
						{
							currentLine += L" ";
						}
			            currentLine += word;
			            lineWidth = totalWidth;
			        }
			    }

			    if (!currentLine.empty())
				{
			        finalResult += currentLine + L'\n';
			    }
				else
				{
			        finalResult += '\n';
			    }
			}

			if (!finalResult.empty() && finalResult.back() == L'\n')
			{
			    finalResult.pop_back();
			}
			
			text.setString(finalResult);
			return finalResult;
		};
		wrapText(contentMain, m_FontData.Regular, *textContent, 24, 1800.0f);

		textContent->setFillColor(sf::Color::Black);
		m_PersonInfoTextHeight = textContent->getLocalBounds().size.y + textContent->getLocalBounds().position.x;
		m_ContentTexture = sf::RenderTexture(sf::Vector2u((uint32_t) m_Window.getSize().x, (uint32_t) m_PersonInfoTextHeight));
		
		if (&m_bLoadedTextureRetButton && !m_TextureRetButton.loadFromFile(g_CommandLineOptions.ReturnButton))
		{
			LOG_ERROR("Failed to load return button image from file '" << g_CommandLineOptions.ReturnButton << "'!");
			std::exit(EXIT_FAILURE);
		}
		m_bLoadedTextureRetButton = true;

		std::shared_ptr<sf::Sprite> sprite = std::make_shared<sf::Sprite>(m_TextureRetButton);
		m_PersonInfoBuffer[PERSON_INFO_BUTTON_RETURN] = sprite;

		sprite->setOrigin({ m_TextureRetButton.getSize().x / 2.0f, m_TextureRetButton.getSize().y / 2.0f });
		sprite->setPosition({ m_Window.getSize().x - 95.0f, 95.0f });
		sprite->setScale(sprite->getScale() / 3.0f);
	}

	void Renderer::InitMasterQuit()
	{
		if (!m_TextureMasterQuit.loadFromFile(g_CommandLineOptions.MasterQuit))
		{
			LOG_ERROR("Failed to load MQ button image from file '" << g_CommandLineOptions.MasterQuit << "'!");
			std::exit(EXIT_FAILURE);
		}

		m_SpriteMasterQuit = sf::Sprite(m_TextureMasterQuit);
		m_SpriteMasterQuit.setOrigin({ m_TextureMasterQuit.getSize().x / 2.0f, m_TextureMasterQuit.getSize().y / 2.0f });
		m_SpriteMasterQuit.setPosition({ m_Window.getSize().x - 100.0f, m_Window.getSize().y - 100.0f });
		m_SpriteMasterQuit.setScale(m_SpriteMasterQuit.getScale() / 3.0f);
	}

	void Renderer::RenderPeopleBuffer(const PeopleStorage& ps)
	{
		for (auto&& [person, shape] : m_PeopleBuffer)
		{
			Person::HistoricalPeriod period = ps.GetPeople().at(person).Period;
			switch (m_SortByOption)
			{
			case SB_ALL:
			{
				if (period == Person::HistoricalPeriod::PreIslamic && g_CommandLineOptions.PassiveHistory)
				{
					continue;
				}
				m_Window.draw(shape);
				break;
			}
			case SB_PRE_ISLAMIC:
			{
				if (g_CommandLineOptions.PassiveHistory)
				{
					continue;
				}
				if (period == Person::HistoricalPeriod::PreIslamic)
				{
					m_Window.draw(shape);
				}
				break;
			}
			case SB_ISLAMIC:
			{
				if (period == Person::HistoricalPeriod::Islamic)
				{
					m_Window.draw(shape);
				}
				break;
			}
			case SB_REPUBLIC:
			{
				if (period == Person::HistoricalPeriod::Republic)
				{
					m_Window.draw(shape);
				}
				break;
			}
			}
		}
	}

	void Renderer::RenderSpriteMasterQuit()
	{
		m_Window.draw(m_SpriteMasterQuit);
	}

	void Renderer::RenderPersonTooltip(PRSHANDLE ih, std::wstring_view name, const sf::Font& font)
	{
		const float ySize = 32;
		float xSize{};

		xSize = (float)(name.size() * g_CommandLineOptions.XScaleFactor);
		const sf::CircleShape& personCircle = m_PeopleBuffer.at(ih);
		float personCircleRadius = personCircle.getRadius();
		sf::Vector2f targetPos = { personCircle.getPosition().x, personCircle.getPosition().y - (personCircleRadius + g_CommandLineOptions.XScaleFactor) };

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
		m_Window.draw(*m_SortByBuffer.at(SORT_BY_DESCRIPTOR));
	}

	void Renderer::RenderPersonInfoBuffer()
	{
		sf::Text* text = (sf::Text*) m_PersonInfoBuffer.at(PERSON_INFO_CONTENT_TEXT).get();
		sf::FloatRect textBounds = text->getLocalBounds();

		m_ScrollOffset = std::clamp(m_ScrollOffset, 0.0f, m_PersonInfoTextHeight >= 900.0f ? m_PersonInfoTextHeight - 900.0f : m_PersonInfoTextHeight);

		m_ContentTexture.clear(sf::Color::Transparent);
		m_ContentTexture.draw(*text);
		m_ContentTexture.display();

		sf::Sprite sprite(m_ContentTexture.getTexture());
		sprite.setTextureRect(sf::IntRect({ 0, (int) m_ScrollOffset }, { (int) m_Window.getSize().x, (int) m_Window.getSize().y }));
		sprite.setPosition({ 35.0f, 180.0f });

		m_Window.draw(*m_PersonInfoBuffer.at(PERSON_INFO_NAME_HEADER));
		m_Window.draw(*m_PersonInfoBuffer.at(PERSON_INFO_SHORT_DESC));
		m_Window.draw(sprite);
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
				return {};
			};

			text->setString(std::wstring(L"Dönem Bazlý Filtrele: ") + sboToStr(m_SortByOption).data());
		}
	}

	void Renderer::SetScrollOffset(float offset)
	{
		m_ScrollOffset = offset;
	}
	
	void Renderer::AddScrollOffset(float amount)
	{
		m_ScrollOffset += amount;
	}
}
