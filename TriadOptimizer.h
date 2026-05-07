#pragma once

#include "GlobalData.h"

class TriadOptimizer
{
private:
    GlobalData* global;
    void constantFolding();
    void commonSubexpressionElimination();
    void deadCodeElimination();

public:
    void setTriads(GlobalData* global);
    void optimize();
};