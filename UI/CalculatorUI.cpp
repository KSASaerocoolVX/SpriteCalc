#include "CalculatorUI.h"
#include <iostream>
#include <string>

std::string intToLabel(int index)
{
	switch (index)
	{
	case 0: return "AC";
	case 1: return "+/-";
	case 2: return "%";
	case 3: return "/";
	case 4: return "7";
	case 5: return "8";
	case 6: return "9";
	case 7: return "*";
	case 8: return "4";
	case 9: return "5";
	case 10: return "6";
	case 11: return "-";
	case 12: return "1";
	case 13: return "2";
	case 14: return "3";
	case 15: return "+";
	case 16: return "0";
	case 17: return "?";
	case 18: return ".";
	case 19: return "=";

	default: return "P";
	}
}

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

	if (!m_font.openFromFile("UI/assets/RetroGaming.ttf")) {
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

	float offsetY = texHeight * 0.325f;
	float offsetX = texWidth * 0.11f;

	float paddingX = texWidth * 0.028f;
	float paddingY = texHeight * 0.02f;

	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			float stepX = (m_buttonTexture.getSize().x + paddingX) * j;
			float stepY = ((m_buttonTexture.getSize().y + paddingY) * i);
			auto pos = sf::Vector2f(offsetX+ stepX,offsetY + stepY);

			std::string label = intToLabel((i * 4) + j);
			buttonPositions.emplace_back(pos, m_buttonTexture, m_font, label);
		}
	}


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

