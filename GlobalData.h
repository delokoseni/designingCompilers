#pragma once
#include "TriadHelper.h"
#include <stack>

struct GlobalData {
    // текущая лексема
    int prevTerm;
    type_lex prevLex;

    // объявление
    bool flagDeclaration = false;
    bool isConstDeclaration = false; // если константа
    bool isInit = false;             // инициализация при объявлении
    DATA_TYPE dataType = TYPE_UNKNOWN;

    // указатель на идентификатор
    SemanticTree* identPtr = nullptr;

    // стек областей видимости
    std::stack<SemanticTree*> levels;

    // Стек операндов для генерации триад
    std::stack<Operand> operands;
    std::stack<DATA_TYPE> operandTypes;
    std::deque<Triad> resultTriads;
};
