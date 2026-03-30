#include "Translate.h"
#include <iostream>

Translate::Translate() {
    global = new GlobalData();
}

void Translate::setTree(SemanticTree* t) {
    tree = t;
}

void Translate::setLex(int term, type_lex lex) {
    global->prevTerm = term;
    strcpy_s(global->prevLex, lex);
}

void Translate::deltaStartDeclareData() {
    global->flagDeclaration = true;
    global->dataType = tree->findDataType(global->prevTerm);
}

void Translate::deltaEndDeclareData() {
    global->flagDeclaration = false;
}

void Translate::deltaSetId() {
    if (!tree->checkDuplicate(SemanticTree::curNode, global->prevLex)) {
        // Вставляем идентификатор в дерево
        global->identPtr = tree->semInclude(global->prevLex, OBJ_VAR);
        tree->semSetType(global->identPtr, global->dataType);
    } else {
        tree->printError("[Translate] duplicate identifier", global->prevLex);
    }
}

void Translate::deltaFindId() {
    global->identPtr = tree->findUp(global->prevLex);

    if (global->identPtr == nullptr) {
        tree->printError("identifier not declared", global->prevLex);
    }
}

void Translate::deltaSetFunction() {
    SemanticTree* func = tree->semInclude(global->prevLex, OBJ_FUNC);

    global->levels.push(func);
    tree->setCurNode(func);

    tree->semSetType(func, global->dataType);
}

void Translate::deltaEndFunction() {
    if (!global->levels.empty()) {
        SemanticTree* prev = global->levels.top();
        global->levels.pop();
        tree->setCurNode(prev);
    }
}

void Translate::deltaEnterBlock() {
    SemanticTree* newBlock = tree->semAddNode();

    global->levels.push(newBlock);
    tree->setCurNode(newBlock);
}

void Translate::deltaExitBlock() {
    if (!global->levels.empty()) {
        SemanticTree* prev = global->levels.top();
        global->levels.pop();
        tree->setCurNode(prev);
    }
}

void Translate::deltaCheckType() {
    if (tree->findDataType(global->prevTerm) == TYPE_UNKNOWN) {
        tree->printError("unknown data type", global->prevLex);
    }
}

void Translate::deltaCheckReturn() {
    if (global->identPtr == nullptr) {
        tree->printError("return without expression", global->prevLex);
    }
}

void Translate::deltaCallFunction() {
    SemanticTree* func = tree->semGetFunc(global->prevLex);

    if (func == nullptr) {
        tree->printError("function not found", global->prevLex);
    }
}

void Translate::deltaSetIntConst() {
    SemanticTree* c = tree->semInclude(global->prevLex, OBJ_CONST);
    tree->semSetType(c, TYPE_INTEGER);
}

void Translate::deltaSetFloatConst() {
    SemanticTree* c = tree->semInclude(global->prevLex, OBJ_CONST);
    tree->semSetType(c, TYPE_FLOAT);
}

void Translate::deltaEmptyStmt() {
    // ничего не делаем
}

void Translate::printTree() {
    tree->print();
}

void Translate::deltaAssignStart() {
    // Начало операции присваивания
    // Просто сохраняем идентификатор, с которым идёт присваивание
    if (global->identPtr == nullptr) {
        tree->printError("assign to undeclared identifier", global->prevLex);
        return;
    }
}

void Translate::deltaAssignEnd() {
    if (global->identPtr == nullptr) {
        tree->printError("assign to undeclared identifier", global->prevLex);
        return;
    }

    // Тип переменной слева
    DATA_TYPE varType = tree->getDataType(global->identPtr);

    // Тип выражения справа (упрощённо, по текущей лексеме)
    DATA_TYPE exprType = TYPE_UNKNOWN;

    switch (global->prevTerm) {
    case typeInt:
        exprType = TYPE_INTEGER; break;
    case typeShort:
        exprType = TYPE_SHORT; break;
    case typeLong:
        exprType = TYPE_LONG; break;
    case typeFloat:
        exprType = TYPE_FLOAT; break;
    default:
        exprType = varType; // если неизвестно, считаем совместимым
        break;
    }

    // Проверка совместимости типов
    if (varType != exprType && exprType != TYPE_UNKNOWN) {
        tree->printError("type mismatch in assignment", global->prevLex);
    }
}

void Translate::deltaWhileStart() {
    // начало while — можно запомнить точку входа, если понадобится
}

void Translate::deltaWhileCondition() {
    // проверка условия while (в будущем можно проверить тип = логический)
    if (global->identPtr == nullptr) {
        tree->printError("while condition error", global->prevLex);
    }
}

void Translate::deltaWhileEnd() {
    // конец while — завершение обработки цикла
}

void Translate::deltaPushOperand() {
    if (!global->identPtr) {
        tree->printError("operand not found", global->prevLex);
        return;
    }
    SemanticTree* prev = SemanticTree::curNode;
    SemanticTree* operandNode = prev->semAddNode();
    operandNode->semSetObj(operandNode, OBJ_VAR);
    operandNode->semSetType(operandNode, tree->getDataType(global->identPtr));
}

void Translate::deltaPushOperator() {
    // Проверяем, что текущая лексема задана
    if (global->prevLex[0] == '\0') {
        tree->printError("operator not found", global->prevLex);
        return;
    }

    // Получаем текущий узел дерева
    SemanticTree* cur = SemanticTree::curNode;
    if (!cur) {
        tree->printError("current node is null", global->prevLex);
        return;
    }

    // Создаём новый узел для оператора
    SemanticTree* operatorNode = cur->semAddNode();

    // Задаём лексему оператора через публичный метод
    operatorNode->setNodeId(global->prevLex);

    // Оператор — это не переменная/функция/константа, тип неизвестен
    operatorNode->semSetObj(operatorNode, OBJ_UNKNOWN);
    operatorNode->semSetType(operatorNode, TYPE_UNKNOWN);
}

void Translate::deltaProcessOperator() {
    // Берём текущий оператор в дереве
    SemanticTree* cur = SemanticTree::curNode;
    if (!cur) {
        tree->printError("current operator node is null", global->prevLex);
        return;
    }

    // Проверяем левый операнд через геттер
    SemanticTree* leftOperand = cur->getLeft();
    if (!leftOperand) {
        tree->printError("left operand missing for operator", global->prevLex);
        return;
    }

    // Правый операнд (если он уже есть)
    SemanticTree* rightOperand = cur->getRight();

    DATA_TYPE leftType = tree->getDataType(leftOperand);
    DATA_TYPE rightType = rightOperand ? tree->getDataType(rightOperand) : TYPE_UNKNOWN;

    if (rightType != TYPE_UNKNOWN && leftType != rightType) {
        tree->printError("type mismatch in binary operator", global->prevLex);
        return;
    }

    // Устанавливаем тип результата
    cur->getNode()->dataType = leftType;
    cur->getNode()->flagInit = 1;
}

void Translate::deltaCheckBinaryOp() {
    // Берём текущий оператор
    SemanticTree* cur = tree->getCurNode();
    if (!cur) {
        tree->printError("current operator node is null", global->prevLex);
        return;
    }

    // Левый и правый операнды должны быть правой и левой от текущего узла
    SemanticTree* leftOperand = cur->getCurNode()->semGetType(global->prevLex); // можно заменить на текущий левый
    SemanticTree* rightOperand = nullptr;

    // Проверяем, есть ли уже правый узел, если нет — создаём
    rightOperand = cur->semAddNode(); // semAddNode возвращает предыдущий узел, но создаёт новый справа
    if (!rightOperand) {
        tree->printError("right operand not found for operator", global->prevLex);
        return;
    }

    // Получаем типы через публичный метод
    DATA_TYPE leftType = tree->getDataType(leftOperand);
    DATA_TYPE rightType = tree->getDataType(rightOperand);

    // Проверка совместимости типов
    if (leftType != rightType) {
        // Разрешаем только числовое приведение
        if ((leftType == TYPE_INTEGER || leftType == TYPE_SHORT || leftType == TYPE_LONG || leftType == TYPE_FLOAT) &&
            (rightType == TYPE_INTEGER || rightType == TYPE_SHORT || rightType == TYPE_LONG || rightType == TYPE_FLOAT)) {
            DATA_TYPE resultType = leftType > rightType ? leftType : rightType;
            cur->setNodeId(""); // Можно использовать setNodeId для пометки или имени
            } else {
                tree->printError("type mismatch in binary operator", global->prevLex);
                return;
            }
    }

    // Пометка, что оператор обработан
    // Для этого у нас пока есть setNodeId как единственный публичный метод к Node
    cur->setNodeId(global->prevLex); // Можно сохранять лексему как "инициализированный" оператор
}

GlobalData* Translate::getGlobal() {
    return global;
}