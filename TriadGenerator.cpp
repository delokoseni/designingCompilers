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

    // Генерация операнда в виде триады
    Operand triadOperand;
    triadOperand.isConst = false;
    triadOperand.isLink = true;
    triadOperand.number = this->number++;
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

    Operand triadOperand;
    triadOperand.isConst = false;
    triadOperand.isLink = true;
    triadOperand.isFunc = false;
    triadOperand.number = this->number++;
    triadOperand.lex[0] = '\0';
    this->global->operands.push(triadOperand);

    DATA_TYPE returnType = this->tree->getDataType(this->global->funPtr);
    this->global->operandTypes.push(returnType);
}

void TriadGenerator::deltaInitFunction() {
    if (this->global->funPtr == nullptr) {
        this->tree->printError("function not selected for entry generation", this->global->prevLex);
        return;
    }

    this->tree->setFunStartNumber(this->global->funPtr, static_cast<int>(this->global->resultTriads.size()));
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
