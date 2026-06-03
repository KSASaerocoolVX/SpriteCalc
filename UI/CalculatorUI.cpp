#include "CalculatorUI.h"
#include <iostream>

CalculatorUI::CalculatorUI(sf::Vector2f position) : m_sprite(m_texture)
{
	if (!m_texture.loadFromFile("UI/assets/calculator_ostov.png")) {
		std::cerr << "failed to load calculator texture" << std::endl;
	}

	m_sprite.setTexture(m_texture, true);
	m_sprite.setPosition(position);
}

void CalculatorUI::Draw(sf::RenderWindow& window)
{
	window.draw(m_sprite);
}