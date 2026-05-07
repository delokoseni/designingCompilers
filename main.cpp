#include "Scaner.h"
#include "Diagram.h"
#include <iostream>
#include <iomanip>
#include <windows.h>
#include <fstream>

#include "TriadOptimizer.h"

int main()
{
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    Scaner* scaner = new Scaner("input.txt");
    Translate* translate = new Translate();
    Diagram* diagram = new Diagram(scaner, translate);
    TriadOptimizer* optimizer = new TriadOptimizer();

    diagram->Run();

    type_lex lex;
    int type = scaner->UseScaner(lex);
    if (type == typeEnd) {
        std::cout << "OK" << std::endl;
        translate->printTree();
        diagram->printTriad();
        optimizer->setTriads(translate->getGlobal());
        optimizer->optimize();

        std::cout << "\nOptimized triads:\n";
        diagram->printTriad();
    }
    else
        std::cout << "ERROR" << std::endl;

    return 0;
}
