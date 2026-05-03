#include "TriadGenerator.h"
#include "Translate.h"
#include <iostream>
#include <cstring>

void TriadGenerator::deltaMatch() {
    DATA_TYPE second = this->global->operandTypes.top();
    this->global->operandTypes.pop();
    DATA_TYPE first = this->global->operandTypes.top();
    this->global->operandTypes.pop();

    DATA_TYPE result = this->tree->checkTypeExpression(first, second);
    this->global->operandTypes.push(result);
}

void TriadGenerator::generateTriad(const type_lex operation) {

    Operand secondOperand = this->global->operands.top();
    this->global->operands.pop();
    Operand firstOperand = this->global->operands.top();
    this->global->operands.pop();

    if (!firstOperand.isLink && operation == "=") {
        SemanticTree* pos = this->tree->findUp(this->tree->curNode, firstOperand.lex);
        if (pos != nullptr)
            if (pos->curNode->nodeIsConst(firstOperand.lex)) {
                this->tree->printError("can not change constant value", firstOperand.lex);

            }
    }

    if (!secondOperand.isLink) {
        SemanticTree* pos = this->tree->findUp(this->tree->curNode, secondOperand.lex);
        if (pos != nullptr)
            this->tree->nodeIsInit(secondOperand.lex);
    }

    // Генерация триады по операции
    Triad newTriad;
    strncpy_s(newTriad.operation, maxLex, operation, maxLex - 1);
    newTriad.firstOperand = firstOperand;
    newTriad.secondOperand = secondOperand;
    this->global->resultTriads.push_back(newTriad);

    const int triadIndex = static_cast<int>(this->global->resultTriads.size()) - 1;

    // Генерация операнда в виде триады
    Operand triadOperand;
    triadOperand.isConst = false;
    triadOperand.isLink = true;
    triadOperand.isFunc = false;
    triadOperand.number = triadIndex;
    triadOperand.lex[0] = '\0';
    this->global->operands.push(triadOperand);
}

void TriadGenerator::setTree(SemanticTree* tree) {
    this->tree = tree;
}

void TriadGenerator::setGlobal(GlobalData* global) {
    this->global = global;
}

void TriadGenerator::deltaAdd() {
    this->deltaMatch();
    this->generateTriad("+");
}
void TriadGenerator::deltaMinus() {
    this->deltaMatch();
    this->generateTriad("-");
}
void TriadGenerator::deltaDiv() {
    this->deltaMatch();
    this->generateTriad("/");
}
void TriadGenerator::deltaMult() {
    this->deltaMatch();
    this->generateTriad("*");
}
void TriadGenerator::deltaMod() {
    this->deltaMatch();
    this->generateTriad("%");
}
void TriadGenerator::deltaLess() {
    this->deltaMatch();
    this->generateTriad("<");
}
void TriadGenerator::deltaLessEq() {
    this->deltaMatch();
    this->generateTriad("<=");
}
void TriadGenerator::deltaMoreEq() {
    this->deltaMatch();
    this->generateTriad(">=");
}
void TriadGenerator::deltaMore() {
    this->deltaMatch();
    this->generateTriad(">");
}

void TriadGenerator::deltaShiftLeft() {
    this->deltaMatch();
    this->generateTriad("<<");
}

void TriadGenerator::deltaShiftRight() {
    this->deltaMatch();
    this->generateTriad(">>");
}

void TriadGenerator::deltaEval() {
    this->deltaMatch();
    this->generateTriad("==");
}

void TriadGenerator::deltaUnEval() {
    this->deltaMatch();
    this->generateTriad("!=");
}

void TriadGenerator::deltaAssign() {
    this->deltaMatch();
    this->generateTriad("=");
}

void TriadGenerator::deltaPushOperand(bool isConst) {
    DATA_TYPE type = this->global->dataType;
    this->global->operandTypes.push(type);
    Operand newOperand;
    newOperand.isLink = false;
    newOperand.isConst = isConst;
    strncpy_s(newOperand.lex, maxLex, this->global->prevLex, maxLex - 1);
    this->global->operands.push(newOperand);
}

void TriadGenerator::deltaCallFunction() {
    if (this->global->funPtr == nullptr) {
        this->tree->printError("function not selected for call generation", this->global->prevLex);
        return;
    }

    Triad callFuncTriad;
    strcpy_s(callFuncTriad.operation, maxLex, "call ");
    strcat_s(callFuncTriad.operation, maxLex, this->global->prevLex);
    callFuncTriad.firstOperand.isLink = true;
    callFuncTriad.firstOperand.isConst = false;
    callFuncTriad.firstOperand.isFunc = true;
    callFuncTriad.firstOperand.number = this->tree->getFunStartNumber(this->global->funPtr);
    callFuncTriad.secondOperand.isLink = true;
    callFuncTriad.secondOperand.isConst = false;
    callFuncTriad.secondOperand.isFunc = false;
    callFuncTriad.secondOperand.number = -1000;
    this->global->resultTriads.push_back(callFuncTriad);

    const int triadIndex = static_cast<int>(this->global->resultTriads.size()) - 1;

    Operand triadOperand;
    triadOperand.isConst = false;
    triadOperand.isLink = true;
    triadOperand.isFunc = false;
    triadOperand.number = triadIndex;
    triadOperand.lex[0] = '\0';
    this->global->operands.push(triadOperand);

    DATA_TYPE returnType = this->tree->getDataType(this->global->funPtr);
    this->global->operandTypes.push(returnType);
}

/*void TriadGenerator::deltaInitFunction() {
    if (this->global->funPtr == nullptr) {
        this->tree->printError("function not selected for entry generation", this->global->prevLex);
        return;
    }

    this->tree->setFunStartNumber(this->global->funPtr, static_cast<int>(this->global->resultTriads.size()));
}*/

void TriadGenerator::deltaInitFunction() {
    if (this->global->funPtr == nullptr) {
        this->tree->printError("function not selected for entry generation", this->global->prevLex);
        return;
    }
    Triad procTriad;
    char* tmp = new char[strlen("proc ") + strlen(this->global->prevLex) + 1];
    strcpy_s(tmp, strlen("proc ") + 1, "proc ");
    strcat_s(
        tmp,
        strlen("proc ") + strlen(this->global->prevLex) + 1,
        this->global->prevLex
    );
    strncpy_s(procTriad.operation, maxLex, tmp, maxLex - 1);
    procTriad.firstOperand.number = -1000;
    procTriad.secondOperand.number = -1000;
    this->global->resultTriads.push_back(procTriad);
    delete[] tmp;
    this->tree->setFunStartNumber(
        this->global->funPtr,
        static_cast<int>(this->global->resultTriads.size() - 1)
    );
}

void TriadGenerator::deltaWhileStart() {
    this->global->whileStartIndices.push(static_cast<int>(this->global->resultTriads.size()));
}

void TriadGenerator::deltaWhileCondition() {
    if (this->global->operands.empty()) {
        this->tree->printError("while condition operand missing", this->global->prevLex);
        return;
    }

    Operand conditionOperand = this->global->operands.top();
    this->global->operands.pop();

    if (!this->global->operandTypes.empty()) {
        this->global->operandTypes.pop();
    }

    Triad ifFalseTriad{};
    strcpy_s(ifFalseTriad.operation, maxLex, "ifFalse");
    ifFalseTriad.firstOperand = conditionOperand;
    ifFalseTriad.secondOperand.isLink = true;
    ifFalseTriad.secondOperand.isConst = false;
    ifFalseTriad.secondOperand.isFunc = false;
    ifFalseTriad.secondOperand.number = -1;

    this->global->whileFalseJumpIndices.push(static_cast<int>(this->global->resultTriads.size()));
    this->global->resultTriads.push_back(ifFalseTriad);
}

void TriadGenerator::deltaWhileEnd() {
    if (this->global->whileStartIndices.empty() || this->global->whileFalseJumpIndices.empty()) {
        this->tree->printError("while stack is inconsistent", this->global->prevLex);
        return;
    }

    const int loopStartIndex = this->global->whileStartIndices.top();
    this->global->whileStartIndices.pop();

    const int falseJumpIndex = this->global->whileFalseJumpIndices.top();
    this->global->whileFalseJumpIndices.pop();

    Triad gotoTriad{};
    strcpy_s(gotoTriad.operation, maxLex, "goto");
    gotoTriad.firstOperand.isLink = true;
    gotoTriad.firstOperand.isConst = false;
    gotoTriad.firstOperand.isFunc = false;
    gotoTriad.firstOperand.number = loopStartIndex;
    gotoTriad.secondOperand.isLink = true;
    gotoTriad.secondOperand.isConst = false;
    gotoTriad.secondOperand.isFunc = false;
    gotoTriad.secondOperand.number = -1000;
    this->global->resultTriads.push_back(gotoTriad);

    this->global->resultTriads[falseJumpIndex].secondOperand.number =
        static_cast<int>(this->global->resultTriads.size());
}

void TriadGenerator::printTriad() {
    std::cout << "Triads:" << std::endl;
    for (int i = 0; i < this->global->resultTriads.size(); i++) {
        Triad triad = this->global->resultTriads[i];
        std::cout << "(" << i << ") " << triad.operation;

        if (std::strncmp(triad.operation, "call ", 5) == 0) {
            std::cout << "\n";
            continue;
        }

        if (triad.firstOperand.number != -1000) {
            if (triad.firstOperand.isLink)
                std::cout << " (" << triad.firstOperand.number << ") ";
            else {
                std::cout << " " << triad.firstOperand.lex << " ";
            }
        }

        if (triad.secondOperand.number != -1000) {
            if (triad.secondOperand.isLink)
                std::cout << "(" << triad.secondOperand.number << ") ";
            else {
                std::cout << triad.secondOperand.lex;
            }
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

void TriadGenerator::deltaProlog()
{
    Triad triad;

    strcpy_s(triad.operation, maxLex, "prolog");

    triad.firstOperand.number = -1000;
    triad.secondOperand.number = -1000;

    this->global->resultTriads.push_back(triad);
}

void TriadGenerator::deltaEpilog()
{
    Triad triad;

    triad.firstOperand.number = -1000;
    triad.secondOperand.number = -1000;

    strcpy_s(triad.operation, maxLex, "epilog");

    this->global->resultTriads.push_back(triad);
}

void TriadGenerator::deltaReturn()
{
    Triad triad;

    triad.firstOperand.number = -1000;
    triad.secondOperand.number = -1000;

    strcpy_s(triad.operation, maxLex, "ret");

    this->global->resultTriads.push_back(triad);
}

void TriadGenerator::deltaEndProc()
{
    Triad triad;

    triad.firstOperand.number = -1000;
    triad.secondOperand.number = -1000;

    strcpy_s(triad.operation, maxLex, "endp");

    this->global->resultTriads.push_back(triad);
}