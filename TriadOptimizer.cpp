#include "TriadOptimizer.h"

void TriadOptimizer::optimize()
{
    constantFolding();
    commonSubexpressionElimination();
}

void TriadOptimizer::constantFolding() {

}

void TriadOptimizer::commonSubexpressionElimination() {

}

void TriadOptimizer::setTriads(GlobalData* global){
    this->global = global;
}