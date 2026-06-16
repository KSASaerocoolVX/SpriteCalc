module;

#include <SFML/Graphics.hpp>
#include <string>
#include <optional>
#include <functional>

export module InputScreen;

import ITransformable;
import IMathNode;

export class InputScreen : public ITransformable
{
private:
    sf::Sprite m_sprite;
    sf::Vector2f m_localPosition;

    std::unique_ptr<IMathNode> m_expression;


public:
    InputScreen(sf::Vector2f position, const sf::Texture& texture);

    void SetExpression(std::unique_ptr<IMathNode> newExpression);

    virtual void Draw(sf::RenderWindow& window) override;
    virtual void UpdateTransform(sf::Vector2f parentPosition, sf::Vector2f parentScale) override;
    virtual void HandleEvent(const sf::Event& event, const sf::RenderWindow& window) override;

};
