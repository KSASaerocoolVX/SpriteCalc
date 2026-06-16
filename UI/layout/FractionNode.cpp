module;
#include <cmath>
#include <SFML/Graphics.hpp> 
#include <string>
#include <cmath>

module FractionNode;

FractionNode::FractionNode(std::unique_ptr<IMathNode> top, std::unique_ptr<IMathNode> bottom)
{
	m_numerator = std::move(top);
	m_denominator = std::move(bottom);
	m_bar.setFillColor(sf::Color(75, 105, 47));
}

MathMetrics FractionNode::Measure()
{
	MathMetrics topM = m_numerator->Measure();
	MathMetrics botM = m_denominator->Measure();

	m_metrics.width = std::max(topM.width, botM.width) + m_padding * 2.f;

	m_metrics.height = topM.height + botM.height + m_barThickness + (m_padding * 2.f);

	m_metrics.baselineY = topM.height + m_padding + (m_barThickness / 2.f);

	return m_metrics;
}

void FractionNode::Arrange(sf::Vector2f position)
{
    m_localPosition = position;

    MathMetrics topM = m_numerator->Measure();
    MathMetrics botM = m_denominator->Measure();

    float topX = position.x + (m_metrics.width / 2.0f) - (topM.width / 2.0f);
    m_numerator->Arrange(sf::Vector2f(topX, position.y));

    m_barLocalPos = sf::Vector2f(position.x, position.y + topM.height + m_padding);

    float botY = m_barLocalPos.y + m_barThickness;
    float botX = position.x + (m_metrics.width / 2.0f) - (botM.width / 2.0f);
    m_denominator->Arrange(sf::Vector2f(botX, botY));
}

void FractionNode::UpdateTransform(sf::Vector2f parentPosition, sf::Vector2f parentScale)
{
    float xPos = std::round(parentPosition.x + (m_barLocalPos.x * parentScale.x));
    float yPos = std::round(parentPosition.y + (m_barLocalPos.y * parentScale.y));

    m_bar.setPosition(sf::Vector2f(xPos, yPos));
    m_bar.setSize(sf::Vector2f(m_metrics.width * parentScale.x, m_barThickness * parentScale.y));

    m_numerator->UpdateTransform(parentPosition, parentScale);
    m_denominator->UpdateTransform(parentPosition, parentScale);
}

void FractionNode::Draw(sf::RenderWindow& window)
{
    m_numerator->Draw(window);
    window.draw(m_bar);
    m_denominator->Draw(window);
}

void FractionNode::HandleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    m_numerator->HandleEvent(event, window);
    m_denominator->HandleEvent(event, window);
}
