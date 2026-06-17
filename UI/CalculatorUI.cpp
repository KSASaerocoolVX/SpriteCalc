module;

#include <iostream>
#include <string>
#include <SFML/Graphics.hpp> 
#include <cmath>
#include <memory>

module CalculatorUI;

import InputScreen;
import TextNode;
import AssetManager;
import MathRow;
import FractionNode;
import ExponentNode;
import MathEditor;

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

CalculatorUI::CalculatorUI() : m_sprite(AssetManager::Instance().GetTexture("UI/assets/calculator_empty.png"))
{

	sf::Vector2u textureSize = m_sprite.getTexture().getSize();

	children.push_back(std::make_unique<Button>(
		sf::Vector2f(0.0f,0.0f),
		"UI/assets/infoButton.png",
		"UI/assets/infoButton.png",
		""
	));

	//main buttons

	float texWidth = static_cast<float>(textureSize.x);
	float texHeight = static_cast<float>(textureSize.y);

	float offsetY = texHeight * 0.325f;
	float offsetX = texWidth * 0.11f;

	float paddingX = texWidth * 0.028f;
	float paddingY = texHeight * 0.02f;

	sf::Vector2u buttonSize = AssetManager::Instance().GetTexture("UI/assets/button_0.png").getSize();

	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			float stepX = (buttonSize.x + paddingX) * j;
			float stepY = (buttonSize.y + paddingY) * i;
			auto localPosition = sf::Vector2f(offsetX + stepX, offsetY + stepY);

			std::string label = intToLabel((i * 4) + j);

			auto button = std::make_unique<Button>(
				localPosition,
				"UI/assets/button_0.png",
				"UI/assets/button_1.png",
				label
			);

			button->onClick = [this, label]() {
				this->HandleButtonPress(label);
				};

			children.push_back(std::move(button));
		}
	}

	//экран
	float screenOffsetY = texHeight * 0.1f;
	float screenOffsetX = texWidth * 0.08f;
	auto screenPos = sf::Vector2f(screenOffsetX, screenOffsetY);

	auto screen = std::make_unique<InputScreen>(screenPos, "UI/assets/input_screen.png");
	m_screenRef = screen.get();



	children.push_back(std::move(screen));
}

void CalculatorUI::HandleButtonPress(const std::string& label)
{
	if (label == "AC")
	{
		m_editor.Clear();
	}
	else if (label == "/")
	{
		m_editor.InsertFraction();
	}
	else if (label == "P")
	{
		m_editor.InsertExponent();
	}
	else if (label == "?")
	{
		m_editor.StepOut();
	}
	else if (label == "+" || label == "-" || label == "*" || label == "=")
	{
		if (label == "=" && m_screenRef) {
			//todo парсер
		}
		else {
			m_editor.InsertOperator(" " + label + " "); 
		}
	}
	else {
		m_editor.InsertDigit(label);
	}

	m_editor.GetRoot()->Measure();
	m_editor.GetRoot()->Arrange();

	if (m_screenRef) {
		m_screenRef->SetInput(m_editor.GetRoot());
	}

	std::cout << "buffer: \"" << m_inputBuffer << "\"\n";
}

void CalculatorUI::Update(float deltaTime, sf::Vector2f mousePos)
{
	sf::Vector2f localMousePos = getInverseTransform().transformPoint(mousePos);
	for (auto& child : children)
	{
		child->Update(deltaTime, localMousePos);
	}
}

void CalculatorUI::HandleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
	for (auto& child : children)
	{
		child->HandleEvent(event, window);
	}
}

void CalculatorUI::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	target.draw(m_sprite, states);
	for (const auto& child : children)
	{
		target.draw(*child, states);
	}
}
