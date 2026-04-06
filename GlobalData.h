#pragma once
#include <stack>

struct GlobalData {
    // текущая лексема
    int prevTerm;
    type_lex prevLex;

    // объявление
    bool flagDeclaration = false;
    DATA_TYPE dataType = TYPE_UNKNOWN;

    // указатель на идентификатор
    SemanticTree* identPtr = nullptr;

    // стек областей видимости
    std::stack<SemanticTree*> levels;
};
