module;

#include <SFML/Graphics.hpp> 
#include <iostream>

module InputScreen;

import TextNode;

InputScreen::InputScreen(sf::Vector2f localPosition, const sf::Texture& texture): m_sprite(texture)
{
	m_localPosition = localPosition;
	m_sprite.setPosition(localPosition);
}

void InputScreen::SetExpression(std::unique_ptr<IMathNode> newExpression)
{
	m_expression = std::move(newExpression);

	if (m_expression) {
		m_expression->Measure();
	}
}

void InputScreen::Draw(sf::RenderWindow& window)
{
	window.draw(m_sprite);
	if (m_expression) m_expression->Draw(window);
	else
	{
		std::cout << "No expression to draw" << std::endl;
	}
}

void InputScreen::UpdateTransform(sf::Vector2f parentPosition, sf::Vector2f parentScale)
{
	float xPos = parentPosition.x + (m_localPosition.x * parentScale.x);
	float yPos = parentPosition.y + (m_localPosition.y * parentScale.y);

	m_sprite.setPosition(sf::Vector2f(xPos, yPos));
	m_sprite.setScale(sf::Vector2f(parentScale.x, parentScale.y));

	if (m_expression)
	{
		//паддинг
		m_expression->Arrange(sf::Vector2f(2.0f,2.0f));
		m_expression->UpdateTransform(m_sprite.getPosition(), parentScale);
	}
}

void InputScreen::HandleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
	if (m_expression) m_expression->HandleEvent(event, window);
}
