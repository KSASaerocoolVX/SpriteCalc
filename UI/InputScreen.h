#include <SFML/Graphics.hpp>
#include <string>
#include <optional>
#include "ITransformable.h"
#include <functional>

class InputScreen : public ITransformable
{
private:
	sf::Sprite m_sprite;

	bool isHovered = false;

	std::string m_label;
	std::optional<sf::Text> m_inputText;
	std::optional<sf::Text> m_outputText;

	sf::Color textColor = sf::Color(75,105,47);

	std::function<void()> onClick;

	sf::Vector2f m_localPosition;


public:

	InputScreen(sf::Vector2f position, const sf::Texture& texture, const sf::Font& font);

	virtual void Draw(sf::RenderWindow& window);

	virtual void UpdateTransform(sf::Vector2f parentPosition, sf::Vector2f parentScale);

	void PrintLabel();
};