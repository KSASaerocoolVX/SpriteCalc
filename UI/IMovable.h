#pragma once
class IMovable
{
public:
    virtual ~IMovable() {}
    virtual void Move(sf::Vector2f targetPosition) = 0;
    virtual void Draw(sf::RenderWindow& window) = 0;
};