module;
#include <SFML/Graphics.hpp>
#include <memory>
export module ExponentNode;
import IMathNode;
import MathRow;

export class ExponentNode : public IMathNode
{
private:
    std::unique_ptr<MathRow> m_power;

public:
    ExponentNode(std::unique_ptr<MathRow> power);
    MathMetrics Measure() override;
    void Arrange() override;
    std::string ToString() const override;

    std::vector<IMathNode*> GetInteractableRows() override {
        std::vector<IMathNode*> rows;
        if (m_power) rows.push_back(m_power.get());
        return rows;
    }

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};