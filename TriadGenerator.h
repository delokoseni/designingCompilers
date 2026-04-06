#pragma once
#include "SemanticTree.h"
#include "TriadHelper.h"
#include "GlobalData.h"
#include <stack>

class TriadGenerator
{
private:
    SemanticTree* tree;
    GlobalData* global;
    int number = 0;
    std::stack<Triad*> triads;

    void deltaMatch(); // !
    void generateTriad(const type_lex operation); // !

public:
    TriadGenerator(); // !
    void setTree(SemanticTree*); // !
    void setGlobal(GlobalData*); // !

    // Арифметические и сравнения
    void deltaAdd(); // !
    void deltaMinus(); // !
    void deltaDiv(); // !
    void deltaMult(); // !
    void deltaMod(); // !
    void deltaLess(); // !
    void deltaLessEq(); // !
    void deltaMore(); // !
    void deltaMoreEq(); // !
    void deltaShiftLeft();
    void deltaShiftRight();
    void deltaEval(); // !
    void deltaUnEval(); // !

    // Присваивание и операнды
    void deltaAssign(); // !
    void deltaPushOperand(bool); // !
    void deltaPushOperator(const type_lex op);

    // Управление while
    void deltaWhileStart();
    void deltaWhileCondition();
    void deltaWhileEnd();

    // Вызов функции без параметров
    void deltaCallFunction(); // !

    // Печать триад
    void printTriad(); // !
};