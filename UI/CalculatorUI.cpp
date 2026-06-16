module;

#include <iostream>
#include <string>
#include <SFML/Graphics.hpp> 
#include <string>
#include <cmath>
#include <memory>

module CalculatorUI;

import InputScreen;
import TextNode;
import FractionNode;



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
	if (!m_texture.loadFromFile("UI/assets/calculator_empty.png")) {
		std::cerr << "failed to load calculator texture" << std::endl;
	}
	m_sprite.setTexture(m_texture, true);

	if (!m_buttonIdleTexture.loadFromFile("UI/assets/button_0.png")) {
		std::cerr << "failed to load button texture" << std::endl;
	}

	if (!m_buttonHoverTexture.loadFromFile("UI/assets/button_1.png")) {
		std::cerr << "failed to load button texture" << std::endl;
	}

	if (!m_infoTexture.loadFromFile("UI/assets/infoButton.png")) {
		std::cerr << "failed to load infobutton texture" << std::endl;
	}

	if (!m_bigIntTexture.loadFromFile("UI/assets/bigIntButton.png")) {
		std::cerr << "failed to load bigIntButton texture" << std::endl;
	}

	if (!m_bigIntTexture.loadFromFile("UI/assets/bigIntButton.png")) {
		std::cerr << "failed to load bigIntButton texture" << std::endl;
	}

	if (!m_font.openFromFile("UI/assets/RetroGaming.ttf")) {
		std::cerr << "failed to load bigIntButton texture" << std::endl;
	}

	if (!m_inputScreenTexture.loadFromFile("UI/assets/input_screen.png")) {
		std::cerr << "failed to load input screen texture" << std::endl;
	}

	sf::Vector2u textureSize = m_texture.getSize();
	//m_sprite.setOrigin(sf::Vector2f(textureSize.x / 2.0f, textureSize.y / 2.0f));
	//m_sprite.setScale(sf::Vector2f(1.0f, 1.0f));

	//bigint info buttons
	children.push_back(std::make_unique<Button>(sf::Vector2f(0, 0), m_infoTexture,m_infoTexture));
	children.push_back(std::make_unique<Button>(sf::Vector2f(textureSize.x - m_bigIntTexture.getSize().x, 0), m_bigIntTexture,m_bigIntTexture));

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
			float stepX = (m_buttonIdleTexture.getSize().x + paddingX) * j;
			float stepY = ((m_buttonIdleTexture.getSize().y + paddingY) * i);
			auto localPosition = sf::Vector2f(offsetX + stepX, offsetY + stepY);

			std::string label = intToLabel((i * 4) + j);

			auto btn = std::make_unique<Button>(localPosition, m_buttonIdleTexture, m_buttonHoverTexture, m_font, label);

			btn->onClick = [this, label]() {
				this->HandleButtonPress(label);
				};

			children.push_back(std::move(btn));
		}
	}

	//экран
	float screenOffsetY = texHeight * 0.1f;
	float screenOffsetX = texWidth * 0.08f;
	auto screenPos = sf::Vector2f(screenOffsetX, screenOffsetY);

	auto screen = std::make_unique<InputScreen>(screenPos, m_inputScreenTexture);
	m_screenRef = screen.get();

	//m_screenRef->SetInputText("");
	//m_screenRef->SetOutputText("");

	children.push_back(std::move(screen));
}

void CalculatorUI::HandleButtonPress(const std::string& label)
{
	if (label == "AC") {
		m_inputBuffer.clear(); 
	}
	else if (label == "=") {
		//todo на потом
		//parser.parse(m_inputBuffer).evaluate();
	}
	else if (label != "P" && label != "?") {
		m_inputBuffer += label;
	}

	if (m_screenRef) {
		auto topNode = std::make_unique<TextNode>("123", m_font);
		auto botRow = std::make_unique<TextNode>("456",m_font);

		auto fraction = std::make_unique<FractionNode>(std::move(topNode), std::move(botRow));

		m_screenRef->SetExpression(std::move(fraction));

		m_screenRef->UpdateTransform(m_sprite.getPosition(), m_sprite.getScale());
	}

	std::cout << "buffer: \"" << m_inputBuffer << "\"\n";
}

void CalculatorUI::UpdateTransform(sf::Vector2f position, sf::Vector2f scale)
{
	SetPosition(position);
	SetScale(scale);
}

void CalculatorUI::SetPosition(sf::Vector2f targetPosition)
{
	m_sprite.setPosition(targetPosition);

	for (auto& child : children)
	{
		child->UpdateTransform(m_sprite.getPosition(),m_sprite.getScale());
	}
}

void CalculatorUI::SetScale(sf::Vector2f targetScale)
{
	m_sprite.setScale(targetScale);

	for (auto& child : children)
	{
		child->UpdateTransform(m_sprite.getPosition(),m_sprite.getScale());
	}
}



void CalculatorUI::Draw(sf::RenderWindow& window)
{
	window.draw(m_sprite);

	for (auto& button : children)
	{
		button->Draw(window);
	}
}

void CalculatorUI::HandleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
	for (auto& child : children)
	{
		child->HandleEvent(event, window);
	}
}
