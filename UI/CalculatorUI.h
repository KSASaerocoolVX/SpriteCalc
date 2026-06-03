#include <SFML/Graphics.hpp>
#include "Button.h"
#include "ITransformable.h"


class CalculatorUI: public ITransformable
{
private:
    sf::Texture m_texture;
    sf::Texture m_buttonIdleTexture;
    sf::Texture m_buttonHoverTexture;
    sf::Texture m_infoTexture;
    sf::Texture m_bigIntTexture;

    sf::Sprite m_sprite;
    sf::Font m_font;
    std::vector<std::unique_ptr<ITransformable>> children;
public:
    CalculatorUI(sf::Vector2f position);

    virtual void Draw(sf::RenderWindow& window);

    virtual void Move(sf::Vector2f targetPosition);

    virtual void Scale(sf::Vector2f targetScale);
};