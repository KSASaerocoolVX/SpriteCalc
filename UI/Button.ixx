module;

#include <SFML/Graphics.hpp>
#include <string>
#include <optional>
#include <functional>

export module Button;

import ITransformable;

export class Button : public ITransformable
{
private:
    sf::Sprite m_sprite;
    const sf::Texture* m_idleTexture;
    const sf::Texture* m_hoverTexture;

    bool isHovered = false;

    std::string m_label;
    // opt для кнопки help и bigint
    std::optional<sf::Text> m_text;

    sf::Color textColor = sf::Color(99, 155, 255);


    sf::Vector2f m_localPosition;

public:
    std::function<void()> onClick;
    Button(sf::Vector2f position, const sf::Texture& idleTexture, const sf::Texture& hoverTexture, const sf::Font& font, const std::string& label);

    Button(sf::Vector2f position, const sf::Texture& idleTexture, const sf::Texture& hoverTexture);

    virtual void Draw(sf::RenderWindow& window);

    virtual void UpdateTransform(sf::Vector2f parentPosition, sf::Vector2f parentScale);

    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window) override;

    void PrintLabel();
};
