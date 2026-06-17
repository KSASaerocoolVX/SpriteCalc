module;
#include <memory>
#include <vector>
#include <string>

export module MathEditor;

import MathRow;
import TextNode;
import FractionNode;
import ExponentNode;
import IMathNode;

struct CursorState {
    MathRow* row;
    int index;
};

export class MathEditor {
private:
    std::vector<CursorState> m_path; //todo вперед назад должны идти по истории спусков
    MathRow* m_currentRow;
    int m_currentIndex;

    float m_cursorTimer = 0.f;
    bool m_cursorVisible = true;

    void RefreshCursor();

    std::unique_ptr<MathRow> m_root;
    std::vector<MathRow*> m_rowStack;
    TextNode* m_activeTextNode = nullptr;

public:
    MathEditor();

    MathRow* GetRoot();
    void Update(float deltaTime);

    void MoveLeft();
    void MoveRight();
    void Delete();

    void InsertDigit(const std::string& digit);
    void InsertOperator(const std::string& op);
    void InsertFraction();
    void InsertExponent();
    //todo функции 
    void Clear();
};


