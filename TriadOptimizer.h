#pragma once

#include "GlobalData.h"

class TriadOptimizer
{
private:
    GlobalData* global;
    void constantFolding();
    void commonSubexpressionElimination();
    void deadCodeElimination();
    bool sameOperand(const Operand& a, const Operand& b);
    bool isCommutative(const char* op);

public:
    void setTriads(GlobalData* global);
    void optimize();
};