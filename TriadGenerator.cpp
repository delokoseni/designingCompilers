#include "TriadGenerator.h"
#include "Translate.h"
#include <vector>
#include <iostream>

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

    // Семантические проверки
    // Проверка на изменение констант
    if (!firstOperand.isLink && operation == "=") {
        // Попробуем найти в дереве, если это идентификатор
        SemanticTree* pos = this->tree->findUp(this->tree->curNode, firstOperand.lex);
        if (pos != nullptr)
            if (pos->curNode->nodeIsConst(firstOperand.lex)) {
                this->tree->printError("can not change constant value", firstOperand.lex);

            }
    }

    // Инициализация переменной
    if (!secondOperand.isLink) {
        // Попробуем найти в дереве, если это идентификатор
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