cmake_minimum_required(VERSION 4.2)
project(designingCompilers)

set(CMAKE_CXX_STANDARD 20)

add_executable(designingCompilers main.cpp
        Scaner.cpp
        Scaner.h
        Diagram.cpp
        Diagram.h
        Translate.cpp
        Translate.h
        SemanticTree.cpp
        SemanticTree.h
        GrammarSymbols.h)
