module;
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <string>

module MathEditor;

import MathRow;
import TextNode;
import FractionNode;
import ExponentNode;
import IMathNode;

struct MathEditor::Impl {
    std::vector<std::pair<void*, int>> m_path;
    IMathNode* m_currentRow;
    int m_currentIndex;

    float m_cursorTimer = 0.f;
    bool m_cursorVisible = true;

    std::unique_ptr<IMathNode> m_root;
    std::vector<IMathNode*> m_rowStack;
    TextNode* m_activeTextNode = nullptr;

    void RefreshCursor() {
        static_cast<MathRow*>(m_root.get())->ClearCursor();
        static_cast<MathRow*>(m_currentRow)->SetCursor(m_currentIndex, m_cursorVisible);
    }
};

MathEditor::MathEditor()
{
    m_impl = std::make_unique<Impl>();
    Clear();
}

MathEditor::~MathEditor() = default;

void* MathEditor::GetRoot() {
    return m_impl->m_root.get();
}

void MathEditor::Clear() {
    m_impl->m_root = std::make_unique<MathRow>();
    m_impl->m_path.clear();
    m_impl->m_currentRow = m_impl->m_root.get();
    m_impl->m_currentIndex = 0;
    m_impl->RefreshCursor();
}

void MathEditor::Update(float deltaTime) {
    m_impl->m_cursorTimer += deltaTime;
    if (m_impl->m_cursorTimer > 0.5f) {
        m_impl->m_cursorTimer = 0.0f;
        m_impl->m_cursorVisible = !m_impl->m_cursorVisible;
        m_impl->RefreshCursor();
    }
}

void MathEditor::MoveLeft() {
    m_impl->m_cursorVisible = true;
    m_impl->m_cursorTimer = 0.0f;

    if (m_impl->m_currentIndex > 0) {
        IMathNode* leftNode = static_cast<MathRow*>(m_impl->m_currentRow)->GetChild(m_impl->m_currentIndex - 1);
        auto rows = leftNode->GetInteractableRows();
        if (!rows.empty()) {
            m_impl->m_path.push_back({ m_impl->m_currentRow, m_impl->m_currentIndex - 1 });
            m_impl->m_currentRow = rows.back();
            m_impl->m_currentIndex = static_cast<MathRow*>(m_impl->m_currentRow)->GetChildCount();
        }
        else {
            m_impl->m_currentIndex--;
        }
    }
    else {
        if (!m_impl->m_path.empty()) {
            auto parent = m_impl->m_path.back();
            IMathNode* parentNode = static_cast<MathRow*>(parent.first)->GetChild(parent.second);
            auto siblingRows = parentNode->GetInteractableRows();

            int mySubRowIndex = -1;
            for (int i = 0; i < siblingRows.size(); i++) {
                if (siblingRows[i] == m_impl->m_currentRow) {
                    mySubRowIndex = i;
                    break;
                }
            }

            if (mySubRowIndex > 0) {
                m_impl->m_currentRow = siblingRows[mySubRowIndex - 1];
                m_impl->m_currentIndex = static_cast<MathRow*>(m_impl->m_currentRow)->GetChildCount();
            }
            else {
                m_impl->m_path.pop_back();
                m_impl->m_currentRow = static_cast<IMathNode*>(parent.first);
                m_impl->m_currentIndex = parent.second;
            }
        }
    }
    m_impl->RefreshCursor();
}

void MathEditor::MoveRight() {
    m_impl->m_cursorVisible = true;
    m_impl->m_cursorTimer = 0.f;

    if (m_impl->m_currentIndex < static_cast<MathRow*>(m_impl->m_currentRow)->GetChildCount()) {
        IMathNode* rightNode = static_cast<MathRow*>(m_impl->m_currentRow)->GetChild(m_impl->m_currentIndex);
        auto rows = rightNode->GetInteractableRows();
        if (!rows.empty()) {
            m_impl->m_path.push_back({ m_impl->m_currentRow, m_impl->m_currentIndex });
            m_impl->m_currentRow = rows.front();
            m_impl->m_currentIndex = 0;
        }
        else {
            m_impl->m_currentIndex++;
        }
    }
    else {
        if (!m_impl->m_path.empty()) {
            auto parent = m_impl->m_path.back();
            IMathNode* parentNode = static_cast<MathRow*>(parent.first)->GetChild(parent.second);
            auto siblingRows = parentNode->GetInteractableRows();

            int mySubRowIndex = -1;
            for (int i = 0; i < siblingRows.size(); i++) {
                if (siblingRows[i] == m_impl->m_currentRow) {
                    mySubRowIndex = i;
                    break;
                }
            }

            if (mySubRowIndex != -1 && mySubRowIndex < siblingRows.size() - 1) {
                m_impl->m_currentRow = siblingRows[mySubRowIndex + 1];
                m_impl->m_currentIndex = 0;
            }
            else {
                m_impl->m_path.pop_back();
                m_impl->m_currentRow = static_cast<IMathNode*>(parent.first);
                m_impl->m_currentIndex = parent.second + 1;
            }
        }
    }
    m_impl->RefreshCursor();
}

void MathEditor::Delete() {
    if (m_impl->m_currentIndex > 0) {
        static_cast<MathRow*>(m_impl->m_currentRow)->RemoveChild(m_impl->m_currentIndex - 1);
        m_impl->m_currentIndex--;
    }
    else if (!m_impl->m_path.empty()) {
        MoveLeft();
    }
    m_impl->RefreshCursor();
}

void MathEditor::InsertDigit(const std::string& digit) {
    auto newNode = std::make_unique<TextNode>(digit, 56, sf::Color(75, 105, 47));
    static_cast<MathRow*>(m_impl->m_currentRow)->InsertChild(m_impl->m_currentIndex, std::move(newNode));

    m_impl->m_currentIndex++;
    m_impl->RefreshCursor();
}

void MathEditor::InsertOperator(const std::string& op) {
    auto newNode = std::make_unique<TextNode>(op, 56, sf::Color(75, 105, 47), true);
    static_cast<MathRow*>(m_impl->m_currentRow)->InsertChild(m_impl->m_currentIndex, std::move(newNode));
    m_impl->m_currentIndex++;
    m_impl->RefreshCursor();
}

void MathEditor::InsertFraction() {
    std::unique_ptr<IMathNode> numContent;

    if (m_impl->m_currentIndex > 0) {
        IMathNode* prev = static_cast<MathRow*>(m_impl->m_currentRow)->GetChild(m_impl->m_currentIndex - 1);
        if (prev && !prev->IsOperator()) {
            numContent = static_cast<MathRow*>(m_impl->m_currentRow)->RemoveChild(m_impl->m_currentIndex - 1);
            m_impl->m_currentIndex--;
        }
    }

    auto numRow = std::make_unique<MathRow>();
    if (numContent) {
        numRow->AddChild(std::move(numContent));
    }
    else {
        numRow->AddChild(std::make_unique<TextNode>(" ", 56, sf::Color(75, 105, 47)));
    }

    auto denRow = std::make_unique<MathRow>();
    MathRow* ptrToDenominator = denRow.get();

    auto fraction = std::make_unique<FractionNode>(std::move(numRow), std::move(denRow));
    static_cast<MathRow*>(m_impl->m_currentRow)->InsertChild(m_impl->m_currentIndex, std::move(fraction));

    m_impl->m_path.push_back({ m_impl->m_currentRow, m_impl->m_currentIndex });
    m_impl->m_currentRow = ptrToDenominator;
    m_impl->m_currentIndex = 0;
    m_impl->RefreshCursor();
}

void MathEditor::InsertExponent() {
    auto powRow = std::make_unique<MathRow>();
    MathRow* ptrToPower = powRow.get();

    auto exponent = std::make_unique<ExponentNode>(std::move(powRow));

    static_cast<MathRow*>(m_impl->m_currentRow)->InsertChild(m_impl->m_currentIndex, std::move(exponent));

    m_impl->m_path.push_back({ m_impl->m_currentRow, m_impl->m_currentIndex });
    m_impl->m_currentRow = ptrToPower;
    m_impl->m_currentIndex = 0;
    m_impl->RefreshCursor();
}
