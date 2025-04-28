#include "Renderer.h"

namespace PeopleNetwork
{
	Renderer::Renderer(sf::RenderWindow& window)
		: m_Window(window)
	{
	}

	Renderer::~Renderer()
	{
	}

	void Renderer::BuildPeopleBuffer(const std::vector<Person>& people)
	{
		m_PeopleBuffer.clear();
		for (const Person& person : people)
		{
			sf::CircleShape& shape = m_PeopleBuffer[person.Handle] = sf::CircleShape
			(
				IMPORTANCE_BASE_RADII.at(person.Importance) * IMPORTANCE_BASE_RADII_SCALAR,
				60
			);
			const Person::StructDataGUI& gui = person.DataGUI;

			shape.setFillColor({ (uint8_t) gui.Color.R, (uint8_t) gui.Color.G, (uint8_t) gui.Color.B, 0xff });
			shape.setPosition({ (float) gui.Position.X, (float) gui.Position.Y });
		}
	}

	void Renderer::RenderPeopleBuffer()
	{
		for (auto&& [person, shape] : m_PeopleBuffer)
		{
			m_Window.draw(shape);
		}
	}
}
