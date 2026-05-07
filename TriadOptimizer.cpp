#include "TriadOptimizer.h"

#include <cstring>
#include <cstdlib>
#include <cstdio>

void TriadOptimizer::optimize()
{
    constantFolding();
    commonSubexpressionElimination();
}

void TriadOptimizer::constantFolding()
{
    for (auto& triad : global->resultTriads)
    {
        // Берём операцию
        const char* op = triad.operation;

        // Проверяем, что это нужная операция
        bool isSupportedOperation =
            strcmp(op, "+") == 0 ||
            strcmp(op, "-") == 0 ||
            strcmp(op, "*") == 0 ||
            strcmp(op, "/") == 0 ||
            strcmp(op, "%") == 0 ||
            strcmp(op, "<<") == 0 ||
            strcmp(op, ">>") == 0 ||
            strcmp(op, "<") == 0 ||
            strcmp(op, "<=") == 0 ||
            strcmp(op, ">") == 0 ||
            strcmp(op, ">=") == 0 ||
            strcmp(op, "==") == 0 ||
            strcmp(op, "!=") == 0;

        if (!isSupportedOperation)
            continue;

        // Оба операнда должны быть константами
        if (!triad.firstOperand.isConst ||
            !triad.secondOperand.isConst)
        {
            continue;
        }

        // Получаем значения
        int left = atoi(triad.firstOperand.lex);
        int right = atoi(triad.secondOperand.lex);

        int result = 0;

        // Вычисление
        if (strcmp(op, "+") == 0)
        {
            result = left + right;
        }
        else if (strcmp(op, "-") == 0)
        {
            result = left - right;
        }
        else if (strcmp(op, "*") == 0)
        {
            result = left * right;
        }
        else if (strcmp(op, "/") == 0)
        {
            if (right == 0)
                continue;

            result = left / right;
        }
        else if (strcmp(op, "%") == 0)
        {
            if (right == 0)
                continue;

            result = left % right;
        }
        else if (strcmp(op, "<<") == 0)
        {
            result = left << right;
        }
        else if (strcmp(op, ">>") == 0)
        {
            result = left >> right;
        }
        else if (strcmp(op, "<") == 0)
        {
            result = left < right;
        }
        else if (strcmp(op, "<=") == 0)
        {
            result = left <= right;
        }
        else if (strcmp(op, ">") == 0)
        {
            result = left > right;
        }
        else if (strcmp(op, ">=") == 0)
        {
            result = left >= right;
        }
        else if (strcmp(op, "==") == 0)
        {
            result = left == right;
        }
        else if (strcmp(op, "!=") == 0)
        {
            result = left != right;
        }

        strcpy_s(triad.operation, maxLex, "=");

        // Первый операнд очищаем
        triad.firstOperand.number = -1000;
        triad.firstOperand.lex[0] = '\0';
        triad.firstOperand.isConst = false;
        triad.firstOperand.isLink = false;
        triad.firstOperand.isFunc = false;

        // Второй операнд = вычисленная константа
        triad.secondOperand.isConst = true;
        triad.secondOperand.isLink = false;
        triad.secondOperand.isFunc = false;

        sprintf_s(triad.secondOperand.lex, "%d", result);
    }
}

void TriadOptimizer::commonSubexpressionElimination() {

}

void TriadOptimizer::setTriads(GlobalData* global){
    this->global = global;
}