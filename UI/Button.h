#include <SFML/Graphics.hpp>

class Button
{
private:
	sf::Sprite sprite;
	sf::Text text;

public:
	void Draw(sf::RenderWindow &window);
};