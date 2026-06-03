#include <SFML/Graphics.hpp>
#include <string>
#include <optional>
#include "IMovable.h"
#include <functional>

class Button: public IMovable
{
private:
	sf::Sprite m_sprite;
	sf::Texture m_textureIdle;
	sf::Texture m_textureHighlighted;

	std::string m_label;
	//opt для кнопок help и bigint
	std::optional<sf::Text> m_text;

	sf::Color textColor = sf::Color(99,155,255);

	std::function<void()> onClick;

	bool isHovered;

public:

	Button(sf::Vector2f position, const sf::Texture& texture, const sf::Font& font, const std::string& label);

	Button(sf::Vector2f position, const sf::Texture& texture);

	void Draw(sf::RenderWindow &window);

	virtual void Move(sf::Vector2f targetPosition);

	void PrintLabel();
};