module;

#include <SFML/Graphics.hpp> 
#include <string>
#include <cmath>

module InputScreen;

InputScreen::InputScreen(sf::Vector2f localPosition, const sf::Texture& texture, const sf::Font& font): m_sprite(texture), m_inputText(font), m_outputText(font)
{
	m_localPosition = localPosition;

	m_inputText->setString("4+3");
	m_inputText->setCharacterSize(56);
	m_inputText->setScale(sf::Vector2f(0.25f, 0.25f));
	m_inputText->setFillColor(textColor);

	m_inputText->setPosition(localPosition);
	//
	m_outputText->setString("4+3");
	m_outputText->setCharacterSize(56);
	m_outputText->setScale(sf::Vector2f(0.15f, 0.15f));
	m_outputText->setFillColor(textColor);

	m_outputText->setPosition(localPosition);

	m_sprite.setPosition(localPosition);
}

void InputScreen::Draw(sf::RenderWindow& window)
{
	window.draw(m_sprite);

	if (m_inputText.has_value())
	{
		window.draw(*m_inputText);
	}
}

void InputScreen::UpdateTransform(sf::Vector2f parentPosition, sf::Vector2f parentScale)
{
	float xPos = parentPosition.x + (m_localPosition.x * parentScale.x);
	float yPos = parentPosition.y + (m_localPosition.y * parentScale.y);

	m_sprite.setPosition(sf::Vector2f(xPos, yPos));
	m_sprite.setScale(sf::Vector2f(parentScale.x, parentScale.y));

	if (m_inputText.has_value())
	{
		m_inputText->setPosition(sf::Vector2f(xPos, yPos));
		float textScaleX = parentScale.x * 0.15f;
		float textScaleY = parentScale.y * 0.15f;

		m_inputText->setScale(sf::Vector2f(textScaleX, textScaleY));
	}

	if (m_outputText.has_value())
	{
		m_inputText->setPosition(sf::Vector2f(xPos, yPos));
		float textScaleX = parentScale.x * 0.15f;
		float textScaleY = parentScale.y * 0.15f;

		m_inputText->setScale(sf::Vector2f(textScaleX, textScaleY));
	}
}