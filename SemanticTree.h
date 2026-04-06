#pragma once
#include "GrammarSymbols.h"
#include <iostream>

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

union DataValue {
    int dataInt;
    float dataFloat;
};

struct Data {
    DataValue val;   // значение
    DATA_TYPE type;  // тип данных
};

struct Node
{
    // Общее для всех объектов
    type_lex id;          // идентификатор
    DATA_TYPE dataType;   // тип данных переменной
    TYPE_OBJECT objType;  // тип объекта

    // Обязательные данные
    int flagConst;        // константа?
    Data data;            // значение переменной (int/double)
    int n;                // номер или счетчик (если нужен)
    int hg;               // размер массива / вспомогательное
    int flagDef;          // определено ли?
    int param;            // параметр функции?

    // Необязательные
    int flagInit;         // инициализировано ли?
    char* addr;           // адрес переменной (если нужен)
    int numberStartFunction; // номер начала функции
    int typeData;         // доп. тип для SU-перевода
    int typeObj;          // доп. тип объекта для SU-перевода

    // Конструктор по умолчанию для безопасной инициализации
    Node() {
        memset(id, 0, maxLex);
        dataType = TYPE_UNKNOWN;
        objType = OBJ_UNKNOWN;
        flagConst = 0;
        data.type = TYPE_UNKNOWN;
        data.val.dataInt = 0;
        n = 0;
        hg = 0;
        flagDef = 0;
        param = 0;
        flagInit = 0;
        addr = nullptr;
        numberStartFunction = 0;
        typeData = 0;
        typeObj = 0;
    }
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
    DATA_TYPE checkTypeExpression(DATA_TYPE type1, DATA_TYPE type2);
    bool nodeIsConst(type_lex lex);
    bool nodeIsInit(type_lex lex);
    SemanticTree* semGetVar(type_lex lex);
    void setFunStartNumber(SemanticTree* tmp, int number);
    int getFunStartNumber(SemanticTree* tmp);
};