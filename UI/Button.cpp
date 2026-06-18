module;
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <cmath>

module Button;

import AssetManager;

Button::Button(sf::Vector2f position, const std::string& idleTexPath, const std::string& hoverTexPath, const std::string& label): m_sprite(AssetManager::Instance().GetTexture(idleTexPath))
{
	setPosition(position);

	m_idleTexture = &AssetManager::Instance().GetTexture(idleTexPath);
	m_hoverTexture = &AssetManager::Instance().GetTexture(hoverTexPath);

	if (!label.empty())
	{
		auto& font = AssetManager::Instance().GetFont("UI/assets/RetroGaming.ttf");
		m_text.emplace(font);
		m_text->setString(label);
		m_text->setCharacterSize(56);
		m_text->setScale(sf::Vector2f(0.25f, 0.25f));
		m_text->setFillColor(sf::Color(99, 155, 255));
		m_text->setPosition(sf::Vector2f(0.0f, 0.0f));
	}
}

void Button::Update(float deltaTime, sf::Vector2f mousePos)
{
	sf::FloatRect bounds = getTransform().transformRect(m_sprite.getLocalBounds());
	isHovered = bounds.contains(mousePos);

	m_sprite.setTexture(isHovered ? *m_hoverTexture : *m_idleTexture);
}

void Button::HandleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
	if (const auto* mouseClick = event.getIf<sf::Event::MouseButtonReleased>())
	{
		if (mouseClick->button == sf::Mouse::Button::Left && isHovered)
		{
			if (onClick) onClick();
		}
	}
}

void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();

	target.draw(m_sprite, states);
	if (m_text.has_value())
	{
		target.draw(*m_text, states);
	}
}


