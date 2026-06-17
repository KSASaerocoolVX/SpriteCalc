module;
#include <SFML/Graphics.hpp> 
#include <algorithm>
module MathRow;

void MathRow::AddChild(std::unique_ptr<IMathNode> node) {
    if (node) m_children.push_back(std::move(node));
}

std::unique_ptr<IMathNode> MathRow::PopLastChild()
{
    if (m_children.empty()) return nullptr;
    auto lastNode = std::move(m_children.back());
    m_children.pop_back();
    return lastNode;
}

MathMetrics MathRow::Measure() {
    m_metrics = { 0.f, 0.f, 0.f };
    if (m_children.empty()) return m_metrics;

    float maxBaselineY = 0.f;
    float maxDepth = 0.f;

    for (auto& child : m_children) {
        MathMetrics m = child->Measure();
        m_metrics.width += m.width + m_padding;
        maxBaselineY = std::max(maxBaselineY, m.baselineY);
        maxDepth = std::max(maxDepth, m.height - m.baselineY);
    }

    if (!m_children.empty()) m_metrics.width -= m_padding;

    m_metrics.baselineY = maxBaselineY;
    m_metrics.height = maxBaselineY + maxDepth;

    return m_metrics;
}

void MathRow::Arrange() {
    float currentX = 0.f;
    for (auto& child : m_children) {
        MathMetrics m = child->m_metrics;
        float yOffset = m_metrics.baselineY - m.baselineY;

        child->setPosition(sf::Vector2f(currentX, yOffset));
        child->Arrange();

        currentX += m.width + m_padding;
    }
}

void MathRow::Update(float deltaTime, sf::Vector2f mousePos) {
    for (auto& child : m_children) child->Update(deltaTime, mousePos);
}
void MathRow::HandleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    for (auto& child : m_children) child->HandleEvent(event, window);
}
void MathRow::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    for (auto& child : m_children) target.draw(*child, states);
}