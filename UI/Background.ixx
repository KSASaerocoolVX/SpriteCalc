module;

#include <SFML/Graphics.hpp>

export module Background;

export class Background
{
private:
    unsigned int m_width;
    unsigned int m_height;

    sf::Shader m_shader;
    sf::RectangleShape m_shape;

public:
    Background(unsigned int width, unsigned int height, const std::string& shaderPath);

    void Draw(sf::RenderWindow& window);

    void Draw(sf::RenderWindow& window, float totalTime);
};
