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


MathRow* MathEditor::GetRoot() {
    return m_root.get();
}


void MathEditor::Clear() {
    m_root = std::make_unique<MathRow>();
    m_path.clear();
    m_currentRow = m_root.get();
    m_currentIndex = 0;
    RefreshCursor();
}

void MathEditor::Update(float deltaTime) {
    m_cursorTimer += deltaTime;
    if (m_cursorTimer > 0.5f) {
        m_cursorTimer = 0.0f;
        m_cursorVisible = !m_cursorVisible;
        RefreshCursor();
    }
}

void MathEditor::RefreshCursor() {
    m_root->ClearCursor();
    m_currentRow->SetCursor(m_currentIndex, m_cursorVisible);
}

void MathEditor::MoveLeft() {
    m_cursorVisible = true;
    m_cursorTimer = 0.0f;

    if (m_currentIndex > 0) {
        IMathNode* leftNode = m_currentRow->GetChild(m_currentIndex - 1);
        auto rows = leftNode->GetInteractableRows();
        if (!rows.empty()) {

            m_path.push_back({ m_currentRow, m_currentIndex - 1 });

            m_currentRow = rows.back();
            m_currentIndex = m_currentRow->GetChildCount();
        }
        else {
            m_currentIndex--;
        }
    }
    else {
        if (!m_path.empty()) {
            CursorState parent = m_path.back();
            IMathNode* parentNode = parent.row->GetChild(parent.index);
            auto siblingRows = parentNode->GetInteractableRows();

            int mySubRowIndex = -1;
            for (int i = 0; i < siblingRows.size(); i++) {
                if (siblingRows[i] == m_currentRow) {
                    mySubRowIndex = i;
                    break;
                }
            }

            if (mySubRowIndex > 0) {
                m_currentRow = siblingRows[mySubRowIndex - 1];
                m_currentIndex = m_currentRow->GetChildCount();
            }
            else {
                m_path.pop_back();
                m_currentRow = parent.row;
                m_currentIndex = parent.index; 
            }
        }
    }
    RefreshCursor();
}

void MathEditor::MoveRight() {
    m_cursorVisible = true;
    m_cursorTimer = 0.f;

    if (m_currentIndex < m_currentRow->GetChildCount()) {
        IMathNode* rightNode = m_currentRow->GetChild(m_currentIndex);
        auto rows = rightNode->GetInteractableRows();
        if (!rows.empty()) {
            m_path.push_back({ m_currentRow, m_currentIndex });
            m_currentRow = rows.front();
            m_currentIndex = 0;
        }
        else {
            m_currentIndex++;
        }
    }
    else {
        if (!m_path.empty()) {
            CursorState parent = m_path.back();
            IMathNode* parentNode = parent.row->GetChild(parent.index);
            auto siblingRows = parentNode->GetInteractableRows();

            int mySubRowIndex = -1;
            for (int i = 0; i < siblingRows.size(); i++) {
                if (siblingRows[i] == m_currentRow) {
                    mySubRowIndex = i;
                    break;
                }
            }

            if (mySubRowIndex != -1 && mySubRowIndex < siblingRows.size() - 1) {
                m_currentRow = siblingRows[mySubRowIndex + 1];
                m_currentIndex = 0;
            }
            else {
                m_path.pop_back();
                m_currentRow = parent.row;
                m_currentIndex = parent.index + 1; 
            }
        }
    }
    RefreshCursor();
}

void MathEditor::Delete() {
    if (m_currentIndex > 0) {
        IMathNode* leftNode = m_currentRow->GetChild(m_currentIndex - 1);

        if (leftNode->IsText() && !leftNode->IsOperator()) {
            TextNode* text = static_cast<TextNode*>(leftNode);
            if (text->PopChar()) {
                m_currentRow->RemoveChild(m_currentIndex - 1);
                m_currentIndex--;
            }
        }
        else {
            m_currentRow->RemoveChild(m_currentIndex - 1);
            m_currentIndex--;
        }
    }
    else if (!m_path.empty()) {
        MoveLeft();
    }
    RefreshCursor();
}

void MathEditor::InsertDigit(const std::string& digit) {
    if (m_currentIndex > 0) {
        IMathNode* prev = m_currentRow->GetChild(m_currentIndex - 1);
        if (prev->IsText() && !prev->IsOperator()) {
            static_cast<TextNode*>(prev)->AppendText(digit);
            RefreshCursor();
            return;
        }
    }
    auto newNode = std::make_unique<TextNode>(digit, 56, sf::Color(75, 105, 47));
    m_currentRow->InsertChild(m_currentIndex, std::move(newNode));
    m_currentIndex++;
    RefreshCursor();
}

void MathEditor::InsertOperator(const std::string& op) {
    auto newNode = std::make_unique<TextNode>(op, 56, sf::Color(75, 105, 47), true);
    m_currentRow->InsertChild(m_currentIndex, std::move(newNode));
    m_currentIndex++;
    RefreshCursor();
}

void MathEditor::InsertFraction() {
    std::unique_ptr<IMathNode> numContent;

    if (m_currentIndex > 0) {
        IMathNode* prev = m_currentRow->GetChild(m_currentIndex - 1);
        if (prev && !prev->IsOperator()) {
            numContent = m_currentRow->RemoveChild(m_currentIndex - 1);
            m_currentIndex--;
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
    m_currentRow->InsertChild(m_currentIndex, std::move(fraction));

    m_path.push_back({ m_currentRow, m_currentIndex });
    m_currentRow = ptrToDenominator;
    m_currentIndex = 0;
    RefreshCursor();
}

void MathEditor::InsertExponent() {
    std::unique_ptr<IMathNode> baseContent;

    if (m_currentIndex > 0) {
        IMathNode* prev = m_currentRow->GetChild(m_currentIndex - 1);
        if (prev && !prev->IsOperator()) {
            baseContent = m_currentRow->RemoveChild(m_currentIndex - 1);
            m_currentIndex--;
        }
    }

    auto baseRow = std::make_unique<MathRow>();
    if (baseContent) {
        baseRow->AddChild(std::move(baseContent));
    }
    else {
        baseRow->AddChild(std::make_unique<TextNode>(" ", 56, sf::Color(75, 105, 47)));
    }

    auto powRow = std::make_unique<MathRow>();
    MathRow* ptrToPower = powRow.get();

    auto exponent = std::make_unique<ExponentNode>(std::move(baseRow), std::move(powRow));
    m_currentRow->InsertChild(m_currentIndex, std::move(exponent));

    m_path.push_back({ m_currentRow, m_currentIndex });
    m_currentRow = ptrToPower;
    m_currentIndex = 0;
    RefreshCursor();
}

