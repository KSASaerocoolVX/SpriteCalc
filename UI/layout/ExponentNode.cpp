module;
#include <SFML/Graphics.hpp> 
module ExponentNode;

ExponentNode::ExponentNode(std::unique_ptr<MathRow> power)
    : m_power(std::move(power))
{
}

MathMetrics ExponentNode::Measure() {
    MathMetrics powM = m_power ? m_power->Measure() : MathMetrics{ 10.f, 18.f, 13.5f };

    m_metrics.width = powM.width;

    float shiftUp = powM.height * 0.6f;

    m_metrics.height = powM.height + shiftUp;
    m_metrics.baselineY = powM.baselineY + shiftUp;

    return m_metrics;
}
void ExponentNode::Arrange() {
    if (m_power) {
        m_power->setPosition(sf::Vector2f(0.f, 0.f));
        m_power->Arrange();
    }
}

std::string ExponentNode::ToString() const
{
    std::string powStr = m_power ? m_power->ToString() : "";

    return "^(" + powStr + ")";
}

void ExponentNode::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    if (m_power) target.draw(*m_power, states);
}

