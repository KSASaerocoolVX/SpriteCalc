#include <SFML/Graphics.hpp>
#include <string>
#include <optional>
#include "IMovable.h"
#include <functional>

class Button: public IMovable
{
private:
	sf::Sprite m_sprite;
	const sf::Texture* m_idleTexture;
	const sf::Texture* m_hoverTexture;

	bool isHovered=false;

	std::string m_label;
	//opt для кнопок help и bigint
	std::optional<sf::Text> m_text;

	sf::Color textColor = sf::Color(99,155,255);

	std::function<void()> onClick;


public:

	Button(sf::Vector2f position, const sf::Texture& idleTexture, const sf::Texture& hoverTexture, const sf::Font& font, const std::string& label);

	Button(sf::Vector2f position, const sf::Texture& idleTexture, const sf::Texture& hoverTexture);

	void Draw(sf::RenderWindow &window);

	virtual void Move(sf::Vector2f targetPosition);

	void PrintLabel();
};