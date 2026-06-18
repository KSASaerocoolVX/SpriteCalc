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

std::string MathRow::ToString() const {
    std::string result = "";
    for (const auto& child : m_children) {
        result += child->ToString();
    }
    return result;
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

    //курсор
    if (m_cursorIndex >= 0 && m_showCursor) {
        float cursorX = 0.f;
        if (m_cursorIndex > 0 && m_cursorIndex <= m_children.size()) {
            auto& prevChild = m_children[m_cursorIndex - 1];
            cursorX = prevChild->getPosition().x + prevChild->m_metrics.width + (m_padding / 2.0f);
        }

        sf::RectangleShape cursorLine(sf::Vector2f(2.f, std::max(14.f, m_metrics.height)));
        cursorLine.setFillColor(sf::Color(0, 50, 0)); 
        cursorLine.setPosition(sf::Vector2f(cursorX, 0.f));
        target.draw(cursorLine, states);
    }
}

void MathRow::InsertChild(int index, std::unique_ptr<IMathNode> child) {
    if (child) {
        m_children.insert(m_children.begin() + index, std::move(child));
    }
}

std::unique_ptr<IMathNode> MathRow::RemoveChild(int index) {
    if (index >= 0 && index < m_children.size()) {
        auto node = std::move(m_children[index]);
        m_children.erase(m_children.begin() + index);
        return node;
    }
    return nullptr;
}

IMathNode* MathRow::GetChild(int index) {
    if (index >= 0 && index < m_children.size()) return m_children[index].get();
    return nullptr;
}

void MathRow::SetCursor(int index, bool show) {
    m_cursorIndex = index;
    m_showCursor = show;
}

void MathRow::ClearCursor() {
    m_cursorIndex = -1;
    m_showCursor = false;
    for (auto& child : m_children) {
        for (auto row : child->GetInteractableRows()) {
            static_cast<MathRow*>(row)->ClearCursor();
        }
    }
}