module;

#include <SFML/Graphics.hpp>

export module UIElement;

export class UIElement : public sf::Drawable, public sf::Transformable
{
public:
	virtual ~UIElement() = default;

	virtual void Update(float deltaTime, sf::Vector2f mousePos) {}

	virtual void HandleEvent(const sf::Event& event, const sf::RenderWindow& window) {}
	
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override = 0;
};