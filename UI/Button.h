#include <SFML/Graphics.hpp>

class Button
{
private:
	sf::Sprite m_sprite;
	//sf::Text m_text;

public:

	Button(sf::Vector2f position, const sf::Texture& texture);

	void Draw(sf::RenderWindow &window);
};