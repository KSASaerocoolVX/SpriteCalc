module;

#include <SFML/Graphics.hpp>
#include <string>
#include <optional>
#include <functional>

export module Button;

import UIElement;

export class Button : public UIElement
{
private:
    sf::Sprite m_sprite;
    const sf::Texture* m_idleTexture;
    const sf::Texture* m_hoverTexture;

    bool isHovered = false;

    std::string m_label;
    // opt для кнопки help и bigint
    std::optional<sf::Text> m_text;

public:
    std::function<void()> onClick;

    Button(sf::Vector2f position, const std::string& idleTexPath, const std::string& hoverTexPath, const std::string& label);

    void Update(float deltaTime, sf::Vector2f mousePos) override;
    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window) override;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

};
