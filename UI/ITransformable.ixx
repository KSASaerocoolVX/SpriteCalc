#pragma once

#include <SFML/Graphics.hpp>

class ITransformable
{
public:
    virtual ~ITransformable() = default;
    virtual void UpdateTransform(sf::Vector2f parentPosition, sf::Vector2f parentScale) = 0;
    virtual void Draw(sf::RenderWindow& window) = 0;
    virtual void HandleEvent(const sf::Event& event, const sf::RenderWindow& window) = 0;
};
