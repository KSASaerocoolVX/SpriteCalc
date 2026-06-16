module;

#include <SFML/Graphics.hpp>

export module ITransformable;

export class ITransformable
{
public:
    virtual ~ITransformable() {}
    virtual void UpdateTransform(sf::Vector2f parentPosition, sf::Vector2f parentScale) = 0; //todo rotation can be added
    virtual void Draw(sf::RenderWindow& window) = 0;
    virtual void HandleEvent(const sf::Event& event, const sf::RenderWindow& window) = 0;

};
