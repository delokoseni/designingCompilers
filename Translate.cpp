#include "Translate.h"
#include <iostream>

Translate::Translate() {
    global = new GlobalData();
}

void Translate::setTree(SemanticTree* t) {
    tree = t;
}

void Translate::deltaStartDeclareData() {
    global->flagDeclaration = true;
    global->dataType = tree->findDataType(global->prevTerm);
}

void Translate::deltaEndDeclareData() {
    global->flagDeclaration = false;
}

void Translate::deltaSetId() {
    if (!this->tree->checkDuplicate(SemanticTree::curNode, this->global->prevLex)) {

        this->global->identPtr = this->tree->semInclude(this->global->prevLex, OBJ_VAR);
        this->tree->semSetType(global->identPtr, this->global->dataType);
    }
    else {
        this->tree->printError("detected duplicate of identifier", this->global->prevLex);
    }
}

void Translate::deltaFindId() {
    global->identPtr = tree->findUp(global->prevLex);

    if (global->identPtr == nullptr) {
        tree->printError("identifier not declared", global->prevLex);
    }
}

void Translate::deltaSetFunction() {
    SemanticTree* func = tree->semInclude(global->prevLex, OBJ_FUNC);

    global->levels.push(func);
    tree->setCurNode(func);

    tree->semSetType(func, global->dataType);
}

void Translate::deltaEndFunction() {
    if (!global->levels.empty()) {
        SemanticTree* prev = global->levels.top();
        global->levels.pop();
        tree->setCurNode(prev);
    }
}

void Translate::deltaEnterBlock() {
    SemanticTree* newBlock = tree->semAddNode();

    global->levels.push(newBlock);
    tree->setCurNode(newBlock);
}

void Translate::deltaExitBlock() {
    if (!global->levels.empty()) {
        SemanticTree* prev = global->levels.top();
        global->levels.pop();
        tree->setCurNode(prev);
    }
}

void Translate::deltaCheckType() {
    if (tree->findDataType(global->prevTerm) == TYPE_UNKNOWN) {
        tree->printError("unknown data type", global->prevLex);
    }
}

void Translate::deltaCheckReturn() {
    if (global->identPtr == nullptr) {
        tree->printError("return without expression", global->prevLex);
    }
}

void Translate::deltaCallFunction() {
    SemanticTree* func = tree->semGetFunc(global->prevLex);

    if (func == nullptr) {
        tree->printError("function not found", global->prevLex);
    }
}

void Translate::deltaSetIntConst() {
    SemanticTree* c = tree->semInclude(global->prevLex, OBJ_CONST);
    tree->semSetType(c, TYPE_INTEGER);
}

void Translate::deltaSetFloatConst() {
    SemanticTree* c = tree->semInclude(global->prevLex, OBJ_CONST);
    tree->semSetType(c, TYPE_FLOAT);
}

void Translate::deltaEmptyStmt() {
    // ничего не делаем
}

void Translate::printTree() {
    tree->print();
}

void Translate::deltaAssignStart() {
    if (global->identPtr == nullptr) {
        tree->printError("assign to undeclared identifier", global->prevLex);
        return;
    }
}

void Translate::deltaAssignEnd() {
    if (global->identPtr == nullptr) {
        tree->printError("assign to undeclared identifier", global->prevLex);
        return;
    }

    DATA_TYPE varType = tree->getDataType(global->identPtr);

    DATA_TYPE exprType = TYPE_UNKNOWN;

    switch (global->prevTerm) {
    case typeInt:
        exprType = TYPE_INTEGER; break;
    case typeShort:
        exprType = TYPE_SHORT; break;
    case typeLong:
        exprType = TYPE_LONG; break;
    case typeFloat:
        exprType = TYPE_FLOAT; break;
    default:
        exprType = varType;
        break;
    }

    if (varType != exprType && exprType != TYPE_UNKNOWN) {
        tree->printError("type mismatch in assignment", global->prevLex);
    }
}

void Translate::deltaWhileStart() {

}

void Translate::deltaWhileCondition() {

    if (global->identPtr == nullptr) {
        tree->printError("while condition error", global->prevLex);
    }
}

void Translate::deltaWhileEnd() {

}

void Translate::deltaPushOperand() {
    if (!global->identPtr) {
        tree->printError("operand not found", global->prevLex);
        return;
    }
    SemanticTree* prev = SemanticTree::curNode;
    SemanticTree* operandNode = prev->semAddNode();
    operandNode->semSetObj(operandNode, OBJ_VAR);
    operandNode->semSetType(operandNode, tree->getDataType(global->identPtr));
}

void Translate::deltaPushOperator() {
    if (global->prevLex[0] == '\0') {
        tree->printError("operator not found", global->prevLex);
        return;
    }

    SemanticTree* cur = SemanticTree::curNode;
    if (!cur) {
        tree->printError("current node is null", global->prevLex);
        return;
    }

    SemanticTree* operatorNode = cur->semAddNode();

    operatorNode->setNodeId(global->prevLex);

    operatorNode->semSetObj(operatorNode, OBJ_UNKNOWN);
    operatorNode->semSetType(operatorNode, TYPE_UNKNOWN);
}

void Translate::deltaProcessOperator() {
    SemanticTree* cur = SemanticTree::curNode;
    if (!cur) {
        tree->printError("current operator node is null", global->prevLex);
        return;
    }

    SemanticTree* leftOperand = cur->getLeft();
    if (!leftOperand) {
        tree->printError("left operand missing for operator", global->prevLex);
        return;
    }

    SemanticTree* rightOperand = cur->getRight();

    DATA_TYPE leftType = tree->getDataType(leftOperand);
    DATA_TYPE rightType = rightOperand ? tree->getDataType(rightOperand) : TYPE_UNKNOWN;

    if (rightType != TYPE_UNKNOWN && leftType != rightType) {
        tree->printError("type mismatch in binary operator", global->prevLex);
        return;
    }

    cur->getNode()->dataType = leftType;
    cur->getNode()->flagInit = 1;
}

void Translate::deltaCheckBinaryOp() {
    SemanticTree* cur = tree->getCurNode();
    if (!cur) {
        tree->printError("current operator node is null", global->prevLex);
        return;
    }

    SemanticTree* leftOperand = cur->getCurNode()->semGetType(global->prevLex);
    SemanticTree* rightOperand = nullptr;

    rightOperand = cur->semAddNode();
    if (!rightOperand) {
        tree->printError("right operand not found for operator", global->prevLex);
        return;
    }

    DATA_TYPE leftType = tree->getDataType(leftOperand);
    DATA_TYPE rightType = tree->getDataType(rightOperand);

    if (leftType != rightType) {
        if ((leftType == TYPE_INTEGER || leftType == TYPE_SHORT || leftType == TYPE_LONG || leftType == TYPE_FLOAT) &&
            (rightType == TYPE_INTEGER || rightType == TYPE_SHORT || rightType == TYPE_LONG || rightType == TYPE_FLOAT)) {
            DATA_TYPE resultType = leftType > rightType ? leftType : rightType;
            cur->setNodeId("");
            } else {
                tree->printError("type mismatch in binary operator", global->prevLex);
                return;
            }
    }

    cur->setNodeId(global->prevLex);
}

GlobalData* Translate::getGlobal() {
    return global;
}

void Translate::setLex(int term, type_lex lex) {
    global->prevTerm = term;

    if (term == typeId || term == typeMain) {
        strcpy_s(global->prevLex, maxLex, lex);
    }
}