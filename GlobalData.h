#pragma once
#include "TriadHelper.h"
#include "SemanticTree.h"
#include "GrammarSymbols.h"

#include <stack>
#include <deque>

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
    //Указатель на функцию в дереве
    SemanticTree* funPtr = nullptr;

    // стек областей видимости
    std::stack<SemanticTree*> levels;

    // Стек операндов для генерации триад
    std::stack<Operand> operands;
    std::stack<DATA_TYPE> operandTypes;
    std::stack<int> whileStartIndices;
    std::stack<int> whileFalseJumpIndices;
    std::deque<Triad> resultTriads;
};
