#pragma once
#include "SemanticTree.h"
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

class Translate
{
private:
    GlobalData* global;
    SemanticTree* tree;

public:
    Translate();
    void setTree(SemanticTree*);
    void deltaStartDeclareData();
    void deltaEndDeclareData();
    void deltaSetId();
    void deltaFindId();
    void deltaSetFunction();
    void deltaEndFunction();
    void deltaEnterBlock();
    void deltaExitBlock();
    void deltaCheckType();
    void deltaCheckReturn();
    void deltaCallFunction();
    void deltaSetIntConst();
    void deltaSetFloatConst();
    void deltaEmptyStmt();
    void printTree();
    void deltaAssignStart();
    void deltaAssignEnd();
    void deltaWhileStart();
    void deltaWhileCondition();
    void deltaWhileEnd();
    void deltaPushOperand();
    void deltaPushOperator();
    void deltaProcessOperator();
    void deltaCheckBinaryOp();
    GlobalData* getGlobal();
    void setLex(int term, type_lex lex);
};