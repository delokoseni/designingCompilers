#pragma once
#include "GrammarSymbols.h"

struct Operand {
    bool isConst;
    bool isLink;
    bool isFunc;
    int number;
    type_lex lex;
};

struct Triad {
    type_lex operation;
    Operand firstOperand;
    Operand secondOperand;
};
