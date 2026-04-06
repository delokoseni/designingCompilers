#pragma once
#include "GrammarSymbols.h"
#include <iostream>

#define EMPTY -1

enum DATA_TYPE {
    TYPE_UNKNOWN = 0,
    TYPE_INTEGER,
    TYPE_LONG,
    TYPE_SHORT,
    TYPE_FLOAT
};

enum TYPE_OBJECT {
    OBJ_VAR,
    OBJ_FUNC,
    OBJ_CONST,
    OBJ_UNKNOWN
};

struct Node
{
    // общее
    type_lex id;
    DATA_TYPE dataType;
    TYPE_OBJECT objType;

    // обязательные
    int flagConst;
    char* data;
    int n;
    int hg; //?
    int flagDef;
    int param;

    // необязательные
    int flagInit;
    char* addr;
};

class SemanticTree
{
protected:
    Node* node;
    SemanticTree* up, * left, * right;

public:
    static SemanticTree* curNode;
    SemanticTree(SemanticTree*, SemanticTree*, SemanticTree*, Node*);
    SemanticTree();
    ~SemanticTree();
    void setLeft(Node*);
    void setRight(Node*);
    SemanticTree* findUp(SemanticTree*, type_lex);
    SemanticTree* findUp(type_lex);
    SemanticTree* findUpOneLevel(SemanticTree*, type_lex);
    SemanticTree* findRightLeft(SemanticTree*, type_lex);
    SemanticTree* findRightLeft(type_lex);
    void print();
    void setCurNode(SemanticTree*);
    SemanticTree* getCurNode();
    SemanticTree* semInclude(type_lex, TYPE_OBJECT);
    void semSetType(SemanticTree*, DATA_TYPE);
    void semSetObj(SemanticTree*, TYPE_OBJECT);
    SemanticTree* semGetType(type_lex);
    SemanticTree* semGetFunc(type_lex);
    std::string getStringType(DATA_TYPE);
    std::string getStringTypeObj(TYPE_OBJECT);
    bool checkDuplicate(SemanticTree*, type_lex);
    SemanticTree* semAddNode();
    DATA_TYPE findDataType(int);
    void printError(std::string, type_lex);
    TYPE_OBJECT getTypeObject(SemanticTree*);
    std::string getId(SemanticTree*);
    DATA_TYPE getDataType(SemanticTree* addr);
    void setNodeId(const char* lex);
    SemanticTree* getLeft() const { return left; }
    SemanticTree* getRight() const { return right; }
    Node* getNode() const { return node; }
    void setLeftChild(SemanticTree* child) { left = child; }
    void setRightChild(SemanticTree* child) { right = child; }
};