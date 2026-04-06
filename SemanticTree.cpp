#include "SemanticTree.h"
#include <cstring>

SemanticTree* SemanticTree::curNode = nullptr;

SemanticTree::SemanticTree(SemanticTree* left, SemanticTree* right, SemanticTree* up, Node* node)
{
    this->left = left;
    this->right = right;
    this->up = up;

    this->node = new Node();
    memcpy(this->node, node, sizeof(Node));
}

SemanticTree::SemanticTree()
{
    node = new Node();
    up = nullptr;
    left = nullptr;
    right = nullptr;

    memset(node->id, 0, maxLex);
    node->dataType = TYPE_UNKNOWN;
    node->objType = OBJ_UNKNOWN;
    node->flagConst = 0;
}

SemanticTree::~SemanticTree()
{
    if (left) delete left;
    if (right) delete right;
    delete node;
}

void SemanticTree::setLeft(Node* node)
{
    left = new SemanticTree(nullptr, nullptr, this, node);
}

void SemanticTree::setRight(Node* node)
{
    right = new SemanticTree(nullptr, nullptr, this, node);
}


SemanticTree* SemanticTree::findUp(SemanticTree* from, type_lex id)
{
    SemanticTree* cur = from;

    while (cur != nullptr && strcmp(cur->node->id, id) != 0)
        cur = cur->up;

    return cur;
}

SemanticTree* SemanticTree::findUp(type_lex id)
{
    return findUp(curNode, id);
}

SemanticTree* SemanticTree::findRightLeft(SemanticTree* from, type_lex id)
{
    SemanticTree* cur = from->right;

    while (cur != nullptr && strcmp(cur->node->id, id) != 0)
        cur = cur->left;

    return cur;
}

SemanticTree* SemanticTree::findRightLeft(type_lex id)
{
    return findRightLeft(curNode, id);
}

/*SemanticTree* SemanticTree::findUpOneLevel(SemanticTree* from, type_lex id)
{
    SemanticTree* cur = from;

    while (cur != nullptr && cur->up && cur->up->right != cur)
    {
        if (strcmp(cur->node->id, id) == 0)
            return cur;

        cur = cur->up;
    }

    return nullptr;
}*/

SemanticTree* SemanticTree::findUpOneLevel(SemanticTree* from, type_lex id)
{
    if (!from) return nullptr;

    SemanticTree* parent = from->up;
    if (!parent) return nullptr;

    SemanticTree* cur = parent->left;

    while (cur)
    {
        if (cur->node && strcmp(cur->node->id, id) == 0)
            return cur;

        cur = cur->right;
    }

    return nullptr;
}

bool SemanticTree::checkDuplicate(SemanticTree* addr, type_lex lex)
{
    return findUpOneLevel(addr, lex) != nullptr;
}

SemanticTree* SemanticTree::semInclude(type_lex lex, TYPE_OBJECT t)
{
    if (checkDuplicate(curNode, lex))
    {
        printError("duplicate identifier", lex);
        exit(1);
    }

    Node tmp{};
    strncpy_s(tmp.id, maxLex, lex, maxLex - 1);
    tmp.objType = t;
    tmp.dataType = TYPE_UNKNOWN;
    tmp.flagConst = 0;

    curNode->setLeft(&tmp);
    curNode = curNode->left;

    if (t == OBJ_FUNC)
    {
        SemanticTree* funcNode = curNode;

        Node empty{};
        memset(empty.id, 0, maxLex);
        empty.objType = OBJ_UNKNOWN;

        curNode->setRight(&empty);
        curNode = curNode->right;

        return funcNode;
    }

    return curNode;
}

void SemanticTree::semSetType(SemanticTree* addr, DATA_TYPE t)
{
    addr->node->dataType = t;
}

void SemanticTree::semSetObj(SemanticTree* addr, TYPE_OBJECT obj)
{
    addr->node->objType = obj;

    if (obj == OBJ_CONST)
        addr->node->flagConst = 1;
}

SemanticTree* SemanticTree::semGetType(type_lex lex)
{
    SemanticTree* tmp = findUp(lex);

    if (!tmp)
    {
        printError("identifier not found", lex);
        exit(1);
    }

    if (tmp->node->objType == OBJ_FUNC)
    {
        printError("used function as variable", lex);
        exit(1);
    }

    return tmp;
}

SemanticTree* SemanticTree::semGetFunc(type_lex lex)
{
    SemanticTree* tmp = findUp(lex);

    if (!tmp)
    {
        printError("function not found", lex);
        exit(1);
    }

    if (tmp->node->objType != OBJ_FUNC)
    {
        printError("identifier is not function", lex);
        exit(1);
    }

    return tmp;
}

SemanticTree* SemanticTree::semAddNode()
{
    SemanticTree* prev = curNode;

    Node tmp{};
    memset(tmp.id, 0, maxLex);
    tmp.objType = OBJ_UNKNOWN;

    curNode->setRight(&tmp);
    curNode = curNode->right;

    return prev;
}

void SemanticTree::setCurNode(SemanticTree* cur)
{
    curNode = cur;
}

SemanticTree* SemanticTree::getCurNode()
{
    return curNode;
}

DATA_TYPE SemanticTree::findDataType(int t)
{
    switch (t)
    {
    case typeInt: return TYPE_INTEGER;
    case typeLong: return TYPE_LONG;
    case typeShort: return TYPE_SHORT;
    case typeFloat: return TYPE_FLOAT;
    default: return TYPE_UNKNOWN;
    }
}

std::string SemanticTree::getStringType(DATA_TYPE t)
{
    switch (t)
    {
    case TYPE_INTEGER: return "int";
    case TYPE_LONG: return "long";
    case TYPE_SHORT: return "short";
    case TYPE_FLOAT: return "float";
    default: return "unknown";
    }
}

std::string SemanticTree::getStringTypeObj(TYPE_OBJECT t)
{
    switch (t)
    {
    case OBJ_VAR: return "variable";
    case OBJ_FUNC: return "function";
    case OBJ_CONST: return "constant";
    default: return "unknown";
    }
}

void SemanticTree::print()
{
    std::string idName = (node->id[0] == '\0') ? "____" : node->id;

    std::cout << "Node " << idName << " ("
        //<< getStringType(node->dataType) << ", "
        << getStringTypeObj(node->objType) << ")";

    if (left) std::cout << " -> L:" << left->node->id;
    if (right) std::cout << " -> R:" << right->node->id;

    std::cout << "\n";

    if (left) left->print();
    if (right) right->print();
}

void SemanticTree::printError(std::string message, type_lex lex)
{
    std::cout << "Semantic error: " << message << " [" << lex << "]\n";
}

TYPE_OBJECT SemanticTree::getTypeObject(SemanticTree* from)
{
    return from->node->objType;
}

std::string SemanticTree::getId(SemanticTree* node)
{
    return std::string(node->node->id);
}

DATA_TYPE SemanticTree::getDataType(SemanticTree* addr) {
    return addr->node->dataType;
}

void SemanticTree::setNodeId(const char* lex) {
    strncpy_s(node->id, maxLex, lex, maxLex - 1);
}