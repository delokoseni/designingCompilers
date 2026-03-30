#pragma once
#include "Scaner.h"
#include "GrammarSymbols.h"
#include "Translate.h"
#include <vector>

class Diagram {
private:
    bool wasMain;
    Scaner* scaner;
    Translate* translate;
    SemanticTree* tree;
    std::vector<int> mag;   // Магазин

    void Push(int symbol);
    int Pop();
    int Top();
    bool IsTerminal(int symbol);
    void ApplyRule(int nonterm, int lookahead);
    void eps() {}; // epsilon
    int LookForward(int pointer);
    int Scan(type_lex lex);
    void DeltaOperation(int delta);
    void PrintStack();

public:
    Diagram(Scaner* scaner, Translate *translate);
    int Run();
};
