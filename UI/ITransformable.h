#pragma once
class ITransformable
{
public:
    virtual ~ITransformable() {}
    virtual void Move(sf::Vector2f targetPosition) = 0;
    virtual void Draw(sf::RenderWindow& window) = 0;
};