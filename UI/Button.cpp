#include <SFML/Graphics.hpp>
#include "Button.h"
#include <iostream>

Button::Button(sf::Vector2f position, const sf::Texture& texture): m_sprite(texture)
{
	m_sprite.setPosition(position);
}

void Button::Draw(sf::RenderWindow& window)
{
	window.draw(m_sprite);
}
