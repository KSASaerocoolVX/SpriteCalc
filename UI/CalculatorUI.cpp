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

			//float stepX = (m_buttonIdleTexture.getSize().x + paddingX) * j;
			//float stepY = ((m_buttonIdleTexture.getSize().y + paddingY) * i);
			//auto localPosition = sf::Vector2f(offsetX + stepX, offsetY + stepY);

			//std::string label = intToLabel((i * 4) + j);

			//auto btn = std::make_unique<Button>(localPosition, m_buttonIdleTexture, m_buttonHoverTexture, m_font, label);

			//btn->onClick = [this, label]() {
			//	this->HandleButtonPress(label);
			//	};

			//children.push_back(std::move(btn));
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
	if (label == "AC") {
		m_inputBuffer.clear(); 
	}
	else if (label == "=") {
		//todo на потом
		//parser.parse(m_inputBuffer).evaluate();
		auto mainRow = std::make_unique<MathRow>();

		mainRow->AddChild(std::make_unique<TextNode>("X"));
		mainRow->AddChild(std::make_unique<TextNode>(" + "));

		auto numRow = std::make_unique<MathRow>();
		numRow->AddChild(std::make_unique<TextNode>("3")); 

		auto denRow = std::make_unique<MathRow>();
		denRow->AddChild(std::make_unique<TextNode>("4"));

		auto fraction = std::make_unique<FractionNode>(std::move(numRow), std::move(denRow));

		mainRow->AddChild(std::move(fraction));

		m_screenRef->SetInput(std::move(mainRow));
	}
	else if (label != "P" && label != "?") {
		m_inputBuffer += label;
	}

	if (m_screenRef) {
	/*	auto topNode = std::make_unique<TextNode>("123", m_font);
		auto botRow = std::make_unique<TextNode>("456",m_font);

		auto fraction = std::make_unique<FractionNode>(std::move(topNode), std::move(botRow));

		m_screenRef->SetExpression(std::move(fraction));

		m_screenRef->UpdateTransform(m_sprite.getPosition(), m_sprite.getScale());*/
	}

	std::cout << "buffer: \"" << m_inputBuffer << "\"\n";
}

void CalculatorUI::Update(float deltaTime, sf::Vector2f mousePos)
{
	sf::Vector2f localMousePos = getInverseTransform().transformPoint(mousePos);
	for (auto& child : children)
	{
		child->Update(deltaTime, localMousePos);
		std::cout << localMousePos.x << " " << localMousePos.y << " Local mouse pos" << std::endl;
		std::cout << mousePos.x << " " << mousePos.y << " Local mouse pos" << std::endl;
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
