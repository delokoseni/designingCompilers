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