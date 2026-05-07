#pragma once
#include "TriadHelper.h"
#include <vector>

class TriadOptimizer
{
private:
    std::vector<Triad>* triads;
    void constantFolding();
    void commonSubexpressionElimination();

public:
    void setTriads(std::vector<Triad>* triads);

    void optimize();
};