#pragma once

#include "Person.h"

#include "SFML/Graphics.hpp"

#include <unordered_map>
#include <memory>

namespace PeopleNetwork
{
	// Radius = BaseRadius * ImportanceBaseRadiusScalar 
	inline const std::unordered_map<Person::ImportanceEnum, float> IMPORTANCE_BASE_RADII =
	{
		{ Person::LowImportance,  7.5f  },
		{ Person::MidImportance,  15.0f },
		{ Person::HighImportance, 22.5f }
	};
	inline constexpr float IMPORTANCE_BASE_RADII_SCALAR = 1.0f;
	inline constexpr int   CIRCLE_RENDER_DETAIL_FACTOR  = 60; // 60 vertices per circle
	inline constexpr float FACTOR_SHIFT_PERSON_TOOLTIP  = 10.0f;

	typedef uint8_t SORTBYHANDLE;
	inline constexpr SORTBYHANDLE INVALID_SORT_BY_HANDLE = 0;
	inline constexpr SORTBYHANDLE SORT_BY_HEADER_TEXT    = 1;
	inline constexpr SORTBYHANDLE SORT_BY_ALL            = 2;
	inline constexpr SORTBYHANDLE SORT_BY_PRE_ISLAMIC    = 3;
	inline constexpr SORTBYHANDLE SORT_BY_ISLAMIC        = 4;
	inline constexpr SORTBYHANDLE SORT_BY_REPUBLIC       = 5;

	typedef uint8_t  PRSINFHANDLE;
	inline constexpr PRSINFHANDLE INVALID_PERSON_INFO_HANDLE = 0;
	inline constexpr PRSINFHANDLE PERSON_INFO_NAME_HEADER    = 1;
	inline constexpr PRSINFHANDLE PERSON_INFO_SHORT_DESC     = 2;
	inline constexpr PRSINFHANDLE PERSON_INFO_CONTENT        = 3;
	inline constexpr PRSINFHANDLE PERSON_INFO_BUTTON_RETURN  = 4;

	struct RendererFontData
	{
		sf::Font& Regular, &Bold, &Italic;
	};

	class Renderer
	{
	public:
		Renderer(const RendererFontData& fontData, sf::RenderWindow& window);
		~Renderer();

		void PushToPeopleBuffer(const Person& person);
		void BuildPeopleBuffer(const std::vector<Person>& people);
		void BuildPeopleBuffer(const std::unordered_map<PRSHANDLE, Person>& people);
		void BuildSortByBuffer(const sf::Font& font);
		void BuildPersonInfoBuffer(const Person& person);

		void RenderPeopleBuffer();
		void RenderPersonTooltip(PRSHANDLE ih, std::string_view name, const sf::Font& font);
		void RenderSortByBuffer();

		// Must be done on PSTATE PPRSINF.
		void RenderPersonInfoBuffer();

		const RendererFontData& GetFontData() const { return m_FontData; }
		const sf::RenderWindow& GetWindow() const { return m_Window; }
		const std::unordered_map<PRSHANDLE, sf::CircleShape>& GetPeopleBuffer() const { return m_PeopleBuffer; }
		const std::unordered_map<SORTBYHANDLE, std::shared_ptr<sf::Drawable>>& GetSortByBuffer() const { return m_SortByBuffer; }
		const std::unordered_map<PRSINFHANDLE, std::shared_ptr<sf::Drawable>>& GetPersonInfoBuffer() const { return m_PersonInfoBuffer; }
	private:
		RendererFontData m_FontData;
		sf::RenderWindow& m_Window;

		std::unordered_map<PRSHANDLE, sf::CircleShape> m_PeopleBuffer;
		std::unordered_map<SORTBYHANDLE, std::shared_ptr<sf::Drawable>> m_SortByBuffer;
		std::unordered_map<PRSINFHANDLE, std::shared_ptr<sf::Drawable>> m_PersonInfoBuffer;
		sf::Texture m_TextureRetButton;
		std::string m_PersonInfoContentBuffer;
	};
}
