#include <SFML/Graphics.hpp>

class Background
{
private:
	unsigned int width;
	unsigned int height;

	sf::Shader shader;
	sf::RectangleShape shape;

public:
	Background(unsigned int width, unsigned int height, const std::string);

	void Draw(sf::RenderWindow& window);
};