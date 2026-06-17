module;

#include <SFML/Graphics.hpp>

export module IMathNode;
import UIElement;

export struct MathMetrics {
    float width;
    float height;
    float baselineY;
};

export class MathRow; 

export class IMathNode : public UIElement
{
public:
    MathMetrics m_metrics;

    virtual ~IMathNode() = default;

    virtual MathMetrics Measure() = 0;
    virtual void Arrange() = 0;

    virtual std::vector<MathRow*> GetInteractableRows() { return {}; }

    virtual bool IsOperator() const { return false; }
    virtual bool IsText() const { return false; }

    virtual std::string ToString() const = 0;

};