#pragma once

#include "PeopleStorage.h"

#include "SFML/Graphics.hpp"

#include <unordered_map>
#include <memory>

namespace PeopleNetwork
{
	// Radius = BaseRadius * ImportanceBaseRadiusScalar 
	inline const std::unordered_map<Person::ImportanceEnum, float> IMPORTANCE_BASE_RADII =
	{
		{ Person::LowImportance,  8.5f  },
		{ Person::MidImportance,  15.0f },
		{ Person::HighImportance, 22.5f }
	};
	inline constexpr float IMPORTANCE_BASE_RADII_SCALAR = 1.0f;
	inline constexpr int   CIRCLE_RENDER_DETAIL_FACTOR  = 360; // vertices per circle
	inline constexpr float FACTOR_SHIFT_PERSON_TOOLTIP  = 12.0f; // xpos shift

	typedef uint8_t  SORTBYHANDLE;
	inline constexpr SORTBYHANDLE INVALID_SORT_BY_HANDLE = 0x0;
	inline constexpr SORTBYHANDLE SORT_BY_HEADER_TEXT    = 0x1;
	inline constexpr SORTBYHANDLE SORT_BY_ALL            = 0x2;
	inline constexpr SORTBYHANDLE SORT_BY_PRE_ISLAMIC    = 0x3;
	inline constexpr SORTBYHANDLE SORT_BY_ISLAMIC        = 0x4;
	inline constexpr SORTBYHANDLE SORT_BY_REPUBLIC       = 0x5;
	inline constexpr SORTBYHANDLE SORT_BY_DESCRIPTOR     = 0x6;

	typedef uint8_t  PRSINFHANDLE;
	inline constexpr PRSINFHANDLE INVALID_PERSON_INFO_HANDLE  = 0x0;
	inline constexpr PRSINFHANDLE PERSON_INFO_NAME_HEADER     = 0x1;
	inline constexpr PRSINFHANDLE PERSON_INFO_SHORT_DESC      = 0x2;
	inline constexpr PRSINFHANDLE PERSON_INFO_CONTENT_TEXT    = 0x3;
	inline constexpr PRSINFHANDLE PERSON_INFO_BUTTON_RETURN   = 0x4;
	inline constexpr PRSINFHANDLE PERSON_INFO_CONTENT_SPRITE  = 0x5; // UNUSED, SPRITE IS NOW CREATED DURING RENDERCALL

	struct RendererFontData
	{
		sf::Font& Regular, &Bold, &Italic;
	};

	class Renderer
	{
	public:
		enum SortByOptions
		{
			SB_ALL,
			SB_PRE_ISLAMIC,
			SB_ISLAMIC,
			SB_REPUBLIC
		};
	public:
		Renderer(const RendererFontData& fontData, sf::RenderWindow& window);
		~Renderer();

		void PushToPeopleBuffer(const Person& person);
		void BuildPeopleBuffer(const std::vector<Person>& people);
		void BuildPeopleBuffer(const std::unordered_map<PRSHANDLE, Person>& people);
		void BuildSortByBuffer(const sf::Font& font);
		void BuildPersonInfoBuffer(const Person& person);
		void InitMasterQuit();

		void RenderPeopleBuffer(const PeopleStorage& ps);
		void RenderSpriteMasterQuit();
		void RenderPersonTooltip(PRSHANDLE ih, std::wstring_view name, const sf::Font& font);
		void RenderSortByBuffer();

		// Must be done on PSTATE PPRSINF.
		void RenderPersonInfoBuffer();

		void SetSortByOption(SortByOptions option);
		SortByOptions GetSortByOption() const { return m_SortByOption; }

		void SetScrollOffset(float offset);
		void AddScrollOffset(float amount);
		float GetScrollOffset() const { return m_ScrollOffset; }

		const RendererFontData& GetFontData() const { return m_FontData; }
		const sf::RenderWindow& GetWindow() const { return m_Window; }
		const std::unordered_map<PRSHANDLE, sf::CircleShape>& GetPeopleBuffer() const { return m_PeopleBuffer; }
		const sf::Sprite& GetSpriteMasterQuit() const { return m_SpriteMasterQuit; }
		const std::unordered_map<SORTBYHANDLE, std::shared_ptr<sf::Drawable>>& GetSortByBuffer() const { return m_SortByBuffer; }
		const std::unordered_map<PRSINFHANDLE, std::shared_ptr<sf::Drawable>>& GetPersonInfoBuffer() const { return m_PersonInfoBuffer; }

		bool IsPassiveHistory() const { return passive_history; }
	private:
		RendererFontData m_FontData;
		sf::RenderWindow& m_Window;

		std::unordered_map<PRSHANDLE, sf::CircleShape> m_PeopleBuffer;
		sf::Texture m_TextureMasterQuit;
		sf::Sprite m_SpriteMasterQuit = sf::Sprite(m_TextureMasterQuit);

		std::unordered_map<SORTBYHANDLE, std::shared_ptr<sf::Drawable>> m_SortByBuffer;
		sf::Texture m_TexturePreIslamic; bool m_bLoadedTexturePreIslamic = false;
		sf::Texture m_TextureIslamic;    bool m_bLoadedTextureIslamic = false;
		sf::Texture m_TextureRepublic;   bool m_bLoadedTextureRepublic = false;
		SortByOptions m_SortByOption = SB_ALL;

		std::unordered_map<PRSINFHANDLE, std::shared_ptr<sf::Drawable>> m_PersonInfoBuffer;
		sf::Texture m_TextureRetButton; bool m_bLoadedTextureRetButton = false;
		std::string m_PersonInfoContentBuffer;
		sf::View m_PersonInfoContentView;
		sf::Vector2f m_ContentViewSize = { 1920.0f, 1080.0f };
		sf::RenderTexture m_ContentTexture;
		float m_ScrollOffset = 0.0f;

		bool passive_history = false;
	};
}
