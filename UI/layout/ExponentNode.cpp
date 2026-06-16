module;
#include <SFML/Graphics.hpp> 
module ExponentNode;

ExponentNode::ExponentNode(std::unique_ptr<IMathNode> base, std::unique_ptr<IMathNode> power)
    : m_base(std::move(base)), m_power(std::move(power)) {
}

MathMetrics ExponentNode::Measure() {
    MathMetrics baseM = m_base ? m_base->Measure() : MathMetrics{ 0,0,0 };
    MathMetrics powM = m_power ? m_power->Measure() : MathMetrics{ 0,0,0 };

    float powerOffsetY = powM.height * 0.6f;

    m_metrics.width = baseM.width + powM.width;
    m_metrics.height = baseM.height + powerOffsetY;

    m_metrics.baselineY = baseM.baselineY + powerOffsetY;

    return m_metrics;
}
void ExponentNode::Arrange() {
    MathMetrics baseM = m_base ? m_base->m_metrics : MathMetrics{ 0,0,0 };
    MathMetrics powM = m_power ? m_power->m_metrics : MathMetrics{ 0,0,0 };

    float powerOffsetY = powM.height * 0.6f;

    if (m_base) {
        m_base->setPosition(sf::Vector2f(0.f, powerOffsetY));
        m_base->Arrange();
    }
    if (m_power) {
        m_power->setPosition(sf::Vector2f(baseM.width, 0.f));
        m_power->Arrange();
    }
}

void ExponentNode::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    if (m_base) target.draw(*m_base, states);
    if (m_power) target.draw(*m_power, states);
}