#include "Scaner.h"
#include "Diagram.h"
#include <iostream>
#include <iomanip>
#include <windows.h>
#include <fstream>

int main()
{
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    Scaner* scaner = new Scaner("input.txt");
    Translate* translate = new Translate();
    Diagram* diagram = new Diagram(scaner, translate);

    diagram->Run();

    int type; type_lex lex;
    type = scaner->UseScaner(lex);
    if (type == typeEnd) {
        std::cout << "OK" << std::endl;
        translate->printTree();
    }
    else
        std::cout << "ERROR" << std::endl;

    return 0;
}
