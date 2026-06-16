module;

#include <SFML/Graphics.hpp> 
#include <iostream>

module InputScreen;

import AssetManager;
import TextNode;

InputScreen::InputScreen(sf::Vector2f position, const std::string& texturePath) : m_sprite(AssetManager::Instance().GetTexture(texturePath))
{
	setPosition(position);
}

void InputScreen::SetExpression(std::unique_ptr<IMathNode> newExpression)
{
	m_expression = std::move(newExpression);

	if (m_expression)
	{
		m_expression->Measure();
		m_expression->Arrange();
		//m_expression->setPosition(sf::Vector2f(20.0f, 20.0f)); 
	}
}

void InputScreen::Update(float deltaTime, sf::Vector2f mousePos)
{
	if (m_expression) {
		sf::Vector2f localMousePos = getInverseTransform().transformPoint(mousePos);
		m_expression->Update(deltaTime, localMousePos);
	}
}

void InputScreen::HandleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
	if (m_expression) {
		m_expression->HandleEvent(event, window);
	}
}


void InputScreen::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	target.draw(m_sprite, states);

	if (m_expression) {
		target.draw(*m_expression, states);
	}
}
