module;

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

export module CalculatorUI;

import ITransformable;
import Button;
import InputScreen;

export class CalculatorUI : public ITransformable
{
private:
    sf::Texture m_texture;
    sf::Texture m_buttonIdleTexture;
    sf::Texture m_buttonHoverTexture;
    sf::Texture m_infoTexture;
    sf::Texture m_bigIntTexture;
    sf::Texture m_inputScreenTexture;

    sf::Sprite m_sprite;
    sf::Font m_font;
    std::vector<std::unique_ptr<ITransformable>> children;

public:
    CalculatorUI(sf::Vector2f position);

    virtual void Draw(sf::RenderWindow& window);

    virtual void UpdateTransform(sf::Vector2f targetPosition, sf::Vector2f targetScale);

    void SetPosition(sf::Vector2f targetPosition);
    void SetScale(sf::Vector2f targetScale);
};
