#include "TriadOptimizer.h"

#include <cstring>
#include <cstdio>
#include <vector>

void TriadOptimizer::optimize()
{
    constantFolding();
    commonSubexpressionElimination();
    deadCodeElimination();
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

void TriadOptimizer::commonSubexpressionElimination()
{
    int size = global->resultTriads.size();

    for (int i = 0; i < size; i++)
    {
        Triad& first = global->resultTriads[i];

        const char* op = first.operation;

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

        for (int j = i + 1; j < size; j++)
        {
            Triad& second = global->resultTriads[j];

            // triad уже удалена
            if (second.operation[0] == '\0')
                continue;

            if (strcmp(first.operation, second.operation) != 0)
                continue;

            bool sameFirst =
                sameOperand(first.firstOperand,
                            second.firstOperand);

            bool sameSecond =
                sameOperand(first.secondOperand,
                            second.secondOperand);

            if (!sameFirst || !sameSecond)
                continue;

            // проверка модификации переменных
            bool modified = false;

            for (int k = i + 1; k < j; k++)
            {
                Triad& middle = global->resultTriads[k];

                if (strcmp(middle.operation, "=") != 0)
                    continue;

                const char* changedVar =
                    middle.firstOperand.lex;

                if (!first.firstOperand.isConst &&
                    !first.firstOperand.isLink &&
                    strcmp(first.firstOperand.lex,
                           changedVar) == 0)
                {
                    modified = true;
                }

                if (!first.secondOperand.isConst &&
                    !first.secondOperand.isLink &&
                    strcmp(first.secondOperand.lex,
                           changedVar) == 0)
                {
                    modified = true;
                }
            }

            if (modified)
                continue;

            // заменить ссылки j -> i
            for (int k = 0; k < size; k++)
            {
                Triad& triad = global->resultTriads[k];

                Operand* operands[2] =
                {
                    &triad.firstOperand,
                    &triad.secondOperand
                };

                for (int m = 0; m < 2; m++)
                {
                    Operand* opnd = operands[m];

                    if (opnd->isLink &&
                        opnd->number == j)
                    {
                        opnd->number = i;
                    }
                }
            }

            // полная очистка triad
            second.operation[0] = '\0';

            second.firstOperand.isConst = false;
            second.firstOperand.isLink = false;
            second.firstOperand.isFunc = false;
            second.firstOperand.number = -1;
            second.firstOperand.lex[0] = '\0';

            second.secondOperand.isConst = false;
            second.secondOperand.isLink = false;
            second.secondOperand.isFunc = false;
            second.secondOperand.number = -1;
            second.secondOperand.lex[0] = '\0';
        }
    }

    // удаление пустых triad
    std::deque<Triad> newTriads;

    std::vector<int> newIndices(size, -1);

    for (int i = 0; i < size; i++)
    {
        if (global->resultTriads[i].operation[0] == '\0')
            continue;

        newIndices[i] = newTriads.size();

        newTriads.push_back(global->resultTriads[i]);
    }

    // обновление ссылок
    for (auto& triad : newTriads)
    {
        Operand* operands[2] =
        {
            &triad.firstOperand,
            &triad.secondOperand
        };

        for (int i = 0; i < 2; i++)
        {
            Operand* op = operands[i];

            if (!op->isLink)
                continue;

            int oldIndex = op->number;

            if (oldIndex < 0 || oldIndex >= size)
            {
                op->isLink = false;
                op->number = -1;
                continue;
            }

            if (newIndices[oldIndex] == -1)
            {
                op->isLink = false;
                op->number = -1;
                continue;
            }

            op->number = newIndices[oldIndex];
        }
    }

    global->resultTriads = newTriads;
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

            if (oldIndex < 0 || oldIndex >= size)
            {
                op->isLink = false;
                op->number = -1;
                continue;
            }

            if (newIndices[oldIndex] == -1)
            {
                op->isLink = false;
                op->number = -1;
                continue;
            }

            op->number = newIndices[oldIndex];
        }
    }

    global->resultTriads = newTriads;
}

bool TriadOptimizer::sameOperand(const Operand &a, const Operand &b) {
    if (a.isConst != b.isConst)
        return false;

    if (a.isLink != b.isLink)
        return false;

    if (a.isFunc != b.isFunc)
        return false;

    if (strcmp(a.lex, b.lex) != 0)
        return false;

    if (a.isLink && a.number != b.number)
        return false;

    return true;
}