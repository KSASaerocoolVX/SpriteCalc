#include <SFML/Graphics.hpp>
#include "Button.h"
#include <iostream>

Button::Button(sf::Vector2f position, const sf::Texture& idleTexture, const sf::Texture& hoverTexture, const sf::Font& font, const std::string& label): m_sprite(idleTexture), m_text(font), m_label(label)
{
	m_idleTexture = &idleTexture;
	m_hoverTexture = &hoverTexture;


	onClick = [this]() {
		this->PrintLabel();
	};

	sf::Vector2u textureSize = idleTexture.getSize();

	//m_sprite.setOrigin(sf::Vector2f(textureSize.x / 2.0f, textureSize.y / 2.0f));

	m_sprite.setPosition(position);

	m_text->setString(m_label);
	m_text->setCharacterSize(14);
	m_text->setFillColor(textColor);

	sf::FloatRect bounds = m_text->getLocalBounds();

	//m_text->setOrigin(sf::Vector2f(
	//	bounds.position.x + (bounds.size.x / 2.0f),
	//	bounds.position.y + (bounds.size.y / 2.0f)
	//));

	m_text->setPosition(position);

}

Button::Button(sf::Vector2f position, const sf::Texture& idleTexture, const sf::Texture& hoverTexture) : m_sprite(idleTexture)
{
	m_idleTexture = &idleTexture;
	m_hoverTexture = &hoverTexture;

	m_sprite.setPosition(position);
}

void Button::Draw(sf::RenderWindow& window)
{
	auto mousePos = sf::Mouse::getPosition(window);

	sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));


	if (m_sprite.getGlobalBounds().contains(mousePosF))
		isHovered = true;
	else
		isHovered = false;

	if (isHovered)
	{
		m_sprite.setTexture(*m_hoverTexture);
	}
	else
	{
		m_sprite.setTexture(*m_idleTexture);
	}

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

void Button::Scale(sf::Vector2f targetScale)
{
	m_sprite.setScale(targetScale);

	if (m_text.has_value())
	{
		m_text->setCharacterSize(14 * targetScale.x);
	}
}


void Button::PrintLabel()
{
	std::cout << "clicked on" << m_label << std::endl;
}
