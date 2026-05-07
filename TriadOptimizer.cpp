#include "TriadOptimizer.h"

#include <cstring>
#include <cstdio>
#include <vector>

void TriadOptimizer::optimize()
{
    constantFolding();
    deadCodeElimination();
    commonSubexpressionElimination();
}

void TriadOptimizer::constantFolding()
{
    bool changed = true;

    while (changed)
    {
        changed = false;

        for (int i = 0; i < global->resultTriads.size(); i++)
        {
            Triad& triad = global->resultTriads[i];

            Operand* operands[2] =
            {
                &triad.firstOperand,
                &triad.secondOperand
            };

            for (int j = 0; j < 2; j++)
            {
                Operand* op = operands[j];

                if (!op->isLink)
                    continue;

                int index = op->number;

                if (index < 0 || index >= global->resultTriads.size())
                    continue;

                Triad& linkedTriad = global->resultTriads[index];

                if (strcmp(linkedTriad.operation, "=") == 0 &&
                    linkedTriad.secondOperand.isConst)
                {
                    op->isConst = true;
                    op->isLink = false;

                    strcpy_s(op->lex,
                             linkedTriad.secondOperand.lex);

                    changed = true;
                }
            }

            const char* op = triad.operation;

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

            if (!triad.firstOperand.isConst ||
                !triad.secondOperand.isConst)
            {
                continue;
            }

            int left = atoi(triad.firstOperand.lex);
            int right = atoi(triad.secondOperand.lex);

            int result = 0;

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

            strcpy_s(triad.operation, "=");

            triad.firstOperand.isConst = false;
            triad.firstOperand.isLink = false;
            triad.firstOperand.isFunc = false;
            triad.firstOperand.lex[0] = '\0';

            triad.secondOperand.isConst = true;
            triad.secondOperand.isLink = false;
            triad.secondOperand.isFunc = false;

            sprintf_s(triad.secondOperand.lex, "%d", result);

            changed = true;
        }
    }

    for (auto& triad : global->resultTriads)
    {
        if (strcmp(triad.operation, "=") != 0)
            continue;

        if (!triad.secondOperand.isLink)
            continue;

        int index = triad.secondOperand.number;

        if (index < 0 || index >= global->resultTriads.size())
            continue;

        Triad& linked = global->resultTriads[index];

        if (strcmp(linked.operation, "=") != 0)
            continue;

        if (!linked.secondOperand.isConst)
            continue;

        triad.secondOperand.isLink = false;
        triad.secondOperand.isConst = true;

        strcpy_s(triad.secondOperand.lex,
                 linked.secondOperand.lex);
    }
}

void TriadOptimizer::commonSubexpressionElimination() {

}

void TriadOptimizer::setTriads(GlobalData* global){
    this->global = global;
}

void TriadOptimizer::deadCodeElimination()
{
    int size = global->resultTriads.size();

    std::vector<bool> used(size, false);

    for (int i = 0; i < size; i++)
    {
        Triad& triad = global->resultTriads[i];

        Operand* operands[2] =
        {
            &triad.firstOperand,
            &triad.secondOperand
        };

        for (int j = 0; j < 2; j++)
        {
            Operand* op = operands[j];

            if (op->isLink)
            {
                int index = op->number;

                if (index >= 0 && index < size)
                {
                    used[index] = true;
                }
            }
        }
    }

    std::deque<Triad> newTriads;

    std::vector<int> newIndices(size, -1);

    for (int i = 0; i < size; i++)
    {
        Triad& triad = global->resultTriads[i];

        bool removable = false;

        if (strcmp(triad.operation, "=") == 0)
        {
            bool isTempConst =
                triad.firstOperand.lex[0] == '\0' &&
                triad.secondOperand.isConst;

            if (isTempConst && !used[i])
            {
                removable = true;
            }
        }

        if (!removable)
        {
            newIndices[i] = newTriads.size();
            newTriads.push_back(triad);
        }
    }

    for (auto& triad : newTriads)
    {
        Operand* operands[2] =
        {
            &triad.firstOperand,
            &triad.secondOperand
        };

        for (int j = 0; j < 2; j++)
        {
            Operand* op = operands[j];

            if (!op->isLink)
                continue;

            int oldIndex = op->number;

            if (oldIndex < 0 ||
                oldIndex >= size)
            {
                continue;
            }

            op->number = newIndices[oldIndex];
        }
    }

    global->resultTriads = newTriads;
}