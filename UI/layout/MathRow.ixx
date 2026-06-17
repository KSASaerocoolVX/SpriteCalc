module;
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
export module MathRow;
import IMathNode;

export class MathRow : public IMathNode
{
private:
    std::vector<std::unique_ptr<IMathNode>> m_children;
    float m_padding = 0.0f; //todo настройка

    int m_cursorIndex = -1;
    bool m_showCursor = false;

public:
    MathRow() = default;

    void AddChild(std::unique_ptr<IMathNode> node);
    std::unique_ptr<IMathNode> PopLastChild();

    void Update(float deltaTime, sf::Vector2f mousePos) override;
    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window) override;
    std::string ToString() const override;

    void InsertChild(int index, std::unique_ptr<IMathNode> child);
    std::unique_ptr<IMathNode> RemoveChild(int index);
    IMathNode* GetChild(int index);
    int GetChildCount() const { return m_children.size(); }

    void SetCursor(int index, bool show);
    void ClearCursor();

    MathMetrics Measure() override;
    void Arrange() override;

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};