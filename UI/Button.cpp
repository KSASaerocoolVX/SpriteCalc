#include <SFML/Graphics.hpp>
#include "Button.h"
#include <iostream>

Button::Button(sf::Vector2f position, const sf::Texture& texture, const sf::Font& font, const std::string& label): m_sprite(texture), m_text(font), m_label(label)
{
	m_sprite.setPosition(position);

	m_text->setString(m_label);
	m_text->setCharacterSize(16);
	m_text->setFillColor(textColor);

	sf::FloatRect bounds = m_text->getLocalBounds();

	m_text->setOrigin(sf::Vector2f(
		bounds.position.x + (bounds.size.x / 2.0f),
		bounds.position.y + (bounds.size.y / 2.0f)
	));

	sf::Vector2u textureSize = texture.getSize();
	float xCenter = position.x + (textureSize.x / 2.0f);
	float yCenter = position.y + (textureSize.y / 2.0f);

	sf::Vector2f textPos = sf::Vector2f(xCenter,yCenter);
	m_text->setPosition(textPos);

}

Button::Button(sf::Vector2f position, const sf::Texture& texture) : m_sprite(texture)
{
	m_sprite.setPosition(position);
}

void Button::Draw(sf::RenderWindow& window)
{
	window.draw(m_sprite);
	if (m_text.has_value())
	{
		window.draw(*m_text);
	}

}
