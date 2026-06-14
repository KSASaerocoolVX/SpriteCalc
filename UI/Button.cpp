#include <SFML/Graphics.hpp>
#include "Button.h"
#include <iostream>
#include "InputScreen.h"

Button::Button(sf::Vector2f localPosition, const sf::Texture& idleTexture, const sf::Texture& hoverTexture, const sf::Font& font, const std::string& label): m_sprite(idleTexture), m_text(font), m_label(label)
{
	m_idleTexture = &idleTexture;
	m_hoverTexture = &hoverTexture;

	m_localPosition = localPosition;

	onClick = [this]() {
		this->PrintLabel();
	};

	sf::Vector2u textureSize = idleTexture.getSize();

	m_sprite.setPosition(localPosition);

	m_text->setString(m_label);
	m_text->setCharacterSize(56);
	m_text->setScale(sf::Vector2f(0.25f, 0.25f));
	m_text->setFillColor(textColor);

	m_text->setPosition(localPosition);
}

Button::Button(sf::Vector2f localPosition, const sf::Texture& idleTexture, const sf::Texture& hoverTexture) : m_sprite(idleTexture)
{
	m_idleTexture = &idleTexture;
	m_hoverTexture = &hoverTexture;

	m_localPosition = localPosition;

	m_sprite.setPosition(localPosition);
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

void Button::UpdateTransform(sf::Vector2f parentPosition, sf::Vector2f parentScale)
{
	float xPos = parentPosition.x + (m_localPosition.x * parentScale.x);
	float yPos = parentPosition.y + (m_localPosition.y * parentScale.y);

	m_sprite.setPosition(sf::Vector2f(xPos,yPos));
	m_sprite.setScale(sf::Vector2f(parentScale.x, parentScale.y));

	if (m_text.has_value())
	{
		m_text->setPosition(sf::Vector2f(xPos, yPos));
		float textScaleX = parentScale.x * 0.25f;
		float textScaleY = parentScale.y * 0.25f;

		m_text->setScale(sf::Vector2f(textScaleX, textScaleY));
	}
}




void Button::PrintLabel()
{
	std::cout << "clicked on" << m_label << std::endl;
}
