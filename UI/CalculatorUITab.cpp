module;
#include <SFML/Graphics.hpp>
#include <cmath>
#include <algorithm>
#include <string>

module CalculatorUITab;
import AssetManager;

float easeInOutCubicTab(float x) {
    return x < 0.5f ? 4.0f * x * x * x : 1.0f - std::pow(-2.0f * x + 2.0f, 3.0f) / 2.0f;
}

CalculatorUITab::CalculatorUITab(sf::Vector2f basePosition, const std::string& texturePath, float handleWidth, const std::string& label)
    : m_basePosition(basePosition),
    m_handleWidth(handleWidth),

    m_sprite(AssetManager::Instance().GetTexture(texturePath)),
    m_handleText(AssetManager::Instance().GetFont("UI/assets/RetroGaming.ttf"))
{
    sf::Vector2u texSize = m_sprite.getTexture().getSize();

    m_panelWidth = static_cast<float>(texSize.x) - m_handleWidth;
    m_panelHeight = static_cast<float>(texSize.y);

    m_handleText.setString(label);
    m_handleText.setCharacterSize(14);
    m_handleText.setFillColor(sf::Color::White);
    m_handleText.setRotation(sf::degrees(90.f));

    // Центрируем текст на ручке
    m_handleText.setPosition(sf::Vector2f(m_panelWidth + m_handleWidth / 2.f + 5.f, 10.f));

    // Прячем панель (сдвигаем влево на размер m_panelWidth)
    setPosition(sf::Vector2f(m_basePosition.x - m_panelWidth, m_basePosition.y));
}

void CalculatorUITab::AddButton(std::unique_ptr<Button> button) {
    m_buttons.push_back(std::move(button));
}

void CalculatorUITab::Update(float deltaTime, sf::Vector2f mousePos) {
    float speed = 4.0f;
    if (m_isOpen) {
        m_slideProgress += deltaTime * speed;
    }
    else {
        m_slideProgress -= deltaTime * speed;
    }
    m_slideProgress = std::clamp(m_slideProgress, 0.0f, 1.0f);

    float smoothProgress = easeInOutCubicTab(m_slideProgress);

    float currentX = (m_basePosition.x - m_panelWidth) + (smoothProgress * m_panelWidth);
    setPosition(sf::Vector2f(currentX, m_basePosition.y));

    sf::Vector2f localMousePos = getInverseTransform().transformPoint(mousePos);
    for (auto& btn : m_buttons) {
        btn->Update(deltaTime, localMousePos);
    }
}

//клик именно по ручке
void CalculatorUITab::HandleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    if (const auto* mouseClick = event.getIf<sf::Event::MouseButtonReleased>()) {
        if (mouseClick->button == sf::Mouse::Button::Left) {
            sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
            sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
            sf::Vector2f localPos = getInverseTransform().transformPoint(worldPos);

            if (localPos.x >= m_panelWidth && localPos.x <= m_panelWidth + m_handleWidth &&
                localPos.y >= 0.f && localPos.y <= m_panelHeight)
            {
                m_isOpen = !m_isOpen;
                return;
            }
        }
    }

    if (m_slideProgress > 0.8f) {
        for (auto& btn : m_buttons) {
            btn->HandleEvent(event, window);
        }
    }
}

void CalculatorUITab::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();

    target.draw(m_sprite, states);

    target.draw(m_handleText, states);

    for (const auto& btn : m_buttons) {
        target.draw(*btn, states);
    }
}