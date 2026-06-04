#include "InputScreen.h"


InputScreen::InputScreen(sf::Vector2f localPosition, const sf::Texture& texture, const sf::Font& font): m_sprite(texture)
{
	m_localPosition = localPosition;

	m_sprite.setPosition(localPosition);
}

void InputScreen::Draw(sf::RenderWindow& window)
{
	window.draw(m_sprite);
}

void InputScreen::UpdateTransform(sf::Vector2f parentPosition, sf::Vector2f parentScale)
{
	float xPos = parentPosition.x + (m_localPosition.x * parentScale.x);
	float yPos = parentPosition.y + (m_localPosition.y * parentScale.y);

	m_sprite.setPosition(sf::Vector2f(xPos, yPos));
	m_sprite.setScale(sf::Vector2f(parentScale.x, parentScale.y));

}