#pragma once

#include "Person.h"

#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"

#include <unordered_map>

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

	class Renderer
	{
	public:
		Renderer(sf::RenderWindow& window);
		~Renderer();

		void BuildPeopleBuffer(const std::vector<Person>& people);
		void RenderPeopleBuffer();

		const sf::RenderWindow& GetWindow() const { return m_Window; }
		const std::unordered_map<PRSHANDLE, sf::CircleShape>& GetPeopleBuffer() const { return m_PeopleBuffer; }
	private:
		sf::RenderWindow& m_Window;
		std::unordered_map<PRSHANDLE, sf::CircleShape> m_PeopleBuffer;
	};
}
