module;

#include <SFML/Graphics.hpp> 
#include <cmath>
#include <algorithm>

module FractionNode;

FractionNode::FractionNode(std::unique_ptr<MathRow> top, std::unique_ptr<MathRow> bottom)
    : m_numerator(std::move(top))
    , m_denominator(std::move(bottom))
    , m_bar(sf::Vector2f(0.f, 0.f))
{
    m_bar.setFillColor(sf::Color(75, 105, 47));
}

MathMetrics FractionNode::Measure()
{
    MathMetrics topM = m_numerator ? m_numerator->Measure() : MathMetrics{ 0.f, 0.f, 0.f };
    MathMetrics botM = m_denominator ? m_denominator->Measure() : MathMetrics{ 0.f, 0.f, 0.f };

    m_metrics.width = std::max(topM.width, botM.width) + (m_padding * 2.f);

    m_metrics.height = topM.height + botM.height + m_barThickness + (m_padding * 2.f);

    m_metrics.baselineY = topM.height + m_padding + (m_barThickness / 2.f);

    return m_metrics;
}

void FractionNode::Arrange()
{
    MathMetrics topM = m_numerator ? m_numerator->m_metrics : MathMetrics{ 0.f, 0.f, 0.f };
    MathMetrics botM = m_denominator ? m_denominator->m_metrics : MathMetrics{ 0.f, 0.f, 0.f };

    float centerX = m_metrics.width / 2.0f;

    if (m_numerator) {
        m_numerator->setPosition(sf::Vector2f(centerX - (topM.width / 2.0f), 0.0f));
        m_numerator->Arrange(); 
    }

    m_bar.setPosition(sf::Vector2f(0.0f, topM.height + m_padding));
    m_bar.setSize(sf::Vector2f(m_metrics.width, m_barThickness));

    if (m_denominator) {
        float botY = topM.height + (m_padding * 2.0f) + m_barThickness;
        m_denominator->setPosition(sf::Vector2f(centerX - (botM.width / 2.0f), botY));
        m_denominator->Arrange(); 
    }
}

void FractionNode::Update(float deltaTime, sf::Vector2f mousePos)
{
    if (m_numerator) m_numerator->Update(deltaTime, mousePos);
    if (m_denominator) m_denominator->Update(deltaTime, mousePos);
}

void FractionNode::HandleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    if (m_numerator) m_numerator->HandleEvent(event, window);
    if (m_denominator) m_denominator->HandleEvent(event, window);
}

void FractionNode::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();

    if (m_numerator) target.draw(*m_numerator, states);

    target.draw(m_bar, states);

    if (m_denominator) target.draw(*m_denominator, states);
}