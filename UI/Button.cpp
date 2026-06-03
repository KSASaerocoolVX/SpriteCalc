#include <SFML/Graphics.hpp>
#include "Button.h"
#include <iostream>

Button::Button(sf::Vector2f position, const sf::Texture& texture, const sf::Font& font, const std::string& label): m_sprite(texture), m_text(font), m_label(label)
{
	onClick = [this]() {
		this->PrintLabel();
	};

	sf::Vector2u textureSize = texture.getSize();

	//m_sprite.setOrigin(sf::Vector2f(textureSize.x / 2.0f, textureSize.y / 2.0f));

	m_sprite.setPosition(position);

	m_text->setString(m_label);
	m_text->setCharacterSize(16);
	m_text->setFillColor(textColor);

	sf::FloatRect bounds = m_text->getLocalBounds();

	//m_text->setOrigin(sf::Vector2f(
	//	bounds.position.x + (bounds.size.x / 2.0f),
	//	bounds.position.y + (bounds.size.y / 2.0f)
	//));

	m_text->setPosition(position);

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

void Button::Move(sf::Vector2f offset)
{
	sf::Vector2f currentPosition = m_sprite.getPosition();

	m_sprite.move(offset);
	if (m_text.has_value())
	{
		m_text->move(offset);
	}
}

void Button::PrintLabel()
{
	std::cout << "clicked on" << m_label << std::endl;
}
