#pragma once
#include "SemanticTree.h"
#include "GlobalData.h"

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
    void deltaSetInitValue();
};