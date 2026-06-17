module;
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <string>

module MathEditor;

MathEditor::MathEditor()
{
    Clear();
}

MathRow* MathEditor::GetCurrentRow() {
    return m_rowStack.back();
}

MathRow* MathEditor::GetRoot() {
    return m_root.get();
}

void MathEditor::Clear() {
    m_root = std::make_unique<MathRow>();
    m_rowStack.clear();
    m_rowStack.push_back(m_root.get());
    m_activeTextNode = nullptr;
}

void MathEditor::InsertDigit(const std::string& digit) {
    if (m_activeTextNode) {
        m_activeTextNode->AppendText(digit);
    }
    else {
        auto newNode = std::make_unique<TextNode>(digit, 56, sf::Color(75, 105, 47));
        m_activeTextNode = newNode.get();
        GetCurrentRow()->AddChild(std::move(newNode));
    }
}

void MathEditor::InsertOperator(const std::string& op)
{
    m_activeTextNode = nullptr;
    auto newNode = std::make_unique<TextNode>(op, 56, sf::Color(75, 105, 47));
    GetCurrentRow()->AddChild(std::move(newNode));
}

void MathEditor::InsertFraction() {
    m_activeTextNode = nullptr; 
    auto current = GetCurrentRow();

    //последний символ в дробь
    auto lastNode = current->PopLastChild();

    auto numRow = std::make_unique<MathRow>();
    if (lastNode) {
        numRow->AddChild(std::move(lastNode));
    }
    else {
        numRow->AddChild(std::make_unique<TextNode>(" ", 56, sf::Color(75, 105, 47)));
    }

    auto denRow = std::make_unique<MathRow>();
    MathRow* ptrToDenominator = denRow.get(); 

    auto fraction = std::make_unique<FractionNode>(std::move(numRow), std::move(denRow));
    current->AddChild(std::move(fraction));

    m_rowStack.push_back(ptrToDenominator);
}

void MathEditor::InsertExponent() {
    m_activeTextNode = nullptr;
    auto current = GetCurrentRow();

    auto lastNode = current->PopLastChild();

    auto baseRow = std::make_unique<MathRow>();
    if (lastNode) {
        baseRow->AddChild(std::move(lastNode));
    }
    else {
        baseRow->AddChild(std::make_unique<TextNode>(" ", 56, sf::Color(75, 105, 47)));
    }

    auto powRow = std::make_unique<MathRow>();
    MathRow* ptrToPower = powRow.get();

    auto exponent = std::make_unique<ExponentNode>(std::move(baseRow), std::move(powRow));
    current->AddChild(std::move(exponent));

    m_rowStack.push_back(ptrToPower);
}

void MathEditor::StepOut() {
    m_activeTextNode = nullptr;

    if (m_rowStack.size() > 1) {
        m_rowStack.pop_back();
    }
}
