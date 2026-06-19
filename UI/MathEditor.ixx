module;
#include <memory>
#include <vector>
#include <utility>
#include <string>

export module MathEditor;


import IMathNode;

export class MathEditor {
public:
    struct Impl;

private:
    std::unique_ptr<Impl> m_impl;

public:
    MathEditor();
    ~MathEditor();

    void* GetRoot();
    void Update(float deltaTime);

    void MoveLeft();
    void MoveRight();
    void Delete();

    void InsertDigit(const std::string& digit);
    void InsertOperator(const std::string& op);
    void InsertFraction();
    void InsertExponent();
    void Clear();
};
