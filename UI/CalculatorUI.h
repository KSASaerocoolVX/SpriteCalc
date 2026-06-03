#include <SFML/Graphics.hpp>
#include "Button.h"
#include "IMovable.h"


class CalculatorUI: public IMovable
{
private:
    sf::Texture m_texture;
    sf::Texture m_buttonTexture;
    sf::Texture m_infoTexture;
    sf::Texture m_bigIntTexture;

    sf::Sprite m_sprite;
    sf::Font m_font;
    std::vector<Button> buttonPositions;
public:
    CalculatorUI(sf::Vector2f position);

    void Draw(sf::RenderWindow& window);

    virtual void Move()
    {

    }
};