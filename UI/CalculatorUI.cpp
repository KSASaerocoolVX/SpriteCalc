#include "CalculatorUI.h"
#include <iostream>

CalculatorUI::CalculatorUI(sf::Vector2f position) : m_sprite(m_texture)
{
	if (!m_texture.loadFromFile("UI/assets/calculator_ostov.png")) {
		std::cerr << "failed to load calculator texture" << std::endl;
	}
	m_sprite.setTexture(m_texture, true);

	if (!m_buttonTexture.loadFromFile("UI/assets/button_0.png")) {
		std::cerr << "failed to load button texture" << std::endl;
	}

	if (!m_infoTexture.loadFromFile("UI/assets/infoButton.png")) {
		std::cerr << "failed to load infobutton texture" << std::endl;
	}

	if (!m_bigIntTexture.loadFromFile("UI/assets/bigIntButton.png")) {
		std::cerr << "failed to load bigIntButton texture" << std::endl;
	}

	//m_sprite.setPosition(position);
	sf::Vector2u textureSize = m_texture.getSize();

	//bigint info buttons
	buttonPositions.emplace_back(sf::Vector2f(0,0),m_infoTexture);
	buttonPositions.emplace_back(sf::Vector2f(textureSize.x-m_bigIntTexture.getSize().x, 0), m_bigIntTexture);

	//main buttons

	float texWidth = static_cast<float>(textureSize.x);
	float texHeight = static_cast<float>(textureSize.y);

	float offsetY = texHeight * 0.4f;
	float offsetX = texWidth * 0.2f;

	float paddingX = texWidth * 0.1f;
	float paddingY = texHeight * 0.1f;

	//buttonPositions.emplace_back(sf::Vector2f(0, 0));
	//buttonPositions.emplace_back(sf::Vector2f(1, 0));
}

void CalculatorUI::Draw(sf::RenderWindow& window)
{
	window.draw(m_sprite);

	for (auto& button : buttonPositions)
	{
		button.Draw(window);
	}
}