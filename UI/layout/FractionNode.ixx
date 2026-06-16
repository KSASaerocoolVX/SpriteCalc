module;

#include <SFML/Graphics.hpp>
#include <memory>

export module FractionNode;

import IMathNode;

export class FractionNode : public IMathNode
{
private:
	std::unique_ptr<IMathNode> m_numerator;
	std::unique_ptr<IMathNode> m_denominator;

	sf::RectangleShape m_bar;
	sf::Vector2f m_barLocalPos;

	MathMetrics m_metrics;
	sf::Vector2f m_localPosition;

	float m_padding = 4.0f;
	float m_barThickness = 2.0f;

public:
	FractionNode(std::unique_ptr<IMathNode> top, std::unique_ptr<IMathNode> bottom);

	MathMetrics Measure() override;
	void Arrange(sf::Vector2f position) override;

	void UpdateTransform(sf::Vector2f parentPosition, sf::Vector2f parentScale) override;
	void Draw(sf::RenderWindow& window) override;
	void HandleEvent(const sf::Event& event, const sf::RenderWindow& window) override;

};