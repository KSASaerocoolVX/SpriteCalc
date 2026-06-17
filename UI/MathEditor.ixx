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

export class MathEditor {
private:
    std::unique_ptr<MathRow> m_root;
    std::vector<MathRow*> m_rowStack;
    TextNode* m_activeTextNode = nullptr;

public:
    MathEditor();

    MathRow* GetCurrentRow();
    MathRow* GetRoot();

    void InsertDigit(const std::string& digit);
    void InsertOperator(const std::string& op);
    void InsertFraction();
    void InsertExponent();
    void StepOut(); 
    void Clear();
};


