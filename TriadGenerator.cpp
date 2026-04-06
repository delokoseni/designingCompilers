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