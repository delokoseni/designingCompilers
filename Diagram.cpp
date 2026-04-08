#include "Diagram.h"

Diagram::Diagram(Scaner* scaner, Translate* translate)
{
	this->scaner = scaner;
	this->translate = translate;
	this->wasMain = false;

	Node rootNode;
	rootNode.dataType = TYPE_UNKNOWN;
	rootNode.objType = OBJ_UNKNOWN;
	this->tree = new SemanticTree(nullptr, nullptr, nullptr, &rootNode);

	this->tree->setCurNode(this->tree);
	this->translate->setTree(this->tree);
	this->triadGenerator = new TriadGenerator();
	this->triadGenerator->setGlobal(this->translate->getGlobal());
	this->triadGenerator->setTree(this->tree);
}

void Diagram::Push(int symbol)
{
	mag.push_back(symbol);
}

int Diagram::Pop()
{
	int s = mag.back();
	mag.pop_back();
	return s;
}

int Diagram::Top()
{
	return mag.back();
}

bool Diagram::IsTerminal(int symbol)
{
	return symbol <= MAX_TYPE_TERMINAL;
}

int Diagram::LookForward(int pos) 
{
	type_lex lex;
	int saved_pointer = scaner->GetUK();
	int next_type;
	for (int i = 0; i < pos; i++)
		next_type = scaner->UseScaner(lex);
	scaner->PutUK(saved_pointer);
	return next_type;
}

int Diagram::Run()
{
	type_lex lex;
	int term;

	mag.clear();

	// начальное состояние
	Push(typeEnd);
	Push(N_PROGRAM);

	term = scaner->UseScaner(lex);
	translate->setLex(term, lex);

	while (!mag.empty())
	{
		//PrintStack();
		int top = Top();

		if (IsTerminal(top))
		{
			if (top == term)
			{
				Pop();

				if (term == typeEnd)
					return 1;

				term = scaner->UseScaner(lex);
				translate->setLex(term, lex);
			}
			else
			{
				scaner->PrintError("Синтаксическая ошибка. Неожиданный символ ", scaner->GetCurrentLex());
				return 0;
			}
		}
		else if ((top >= DELTA_START_DECLARE_DATA && top <= DELTA_CHECK_BINARY_OP) ||
			(top >= TRIAD_ADD && top <= TRIAD_FIND_FUN))
		{
			DeltaOperation(top);
		}
		else
		{
			ApplyRule(top, term);
		}
	}

	return 1;
}

void Diagram::ApplyRule(int nonterm, int lookahead)
{
	Pop();

	switch (nonterm)
	{
		case N_PROGRAM: // <программа> → <описание> <программа> {△mergeProgram} | ε
			if (lookahead == typeInt || lookahead == typeShort ||
				lookahead == typeLong || lookahead == typeFloat)
			{
				Push(DELTA_END_DECLARE_DATA); //Нет
				Push(N_PROGRAM);
				Push(N_DESCRIPTION);
				Push(DELTA_START_DECLARE_DATA); //Нет
			}
			else if (lookahead == typeEnd || lookahead == typeRightBrace)
			{
				eps(); // ε
			}
			else
			{
				type_lex lex;
				scaner->PrintError("Ошибка в <программа>", scaner->GetCurrentLex());
			}
			break;

		case N_DESCRIPTION: // <описание> → <тип> <идентификатор> <описание1>
			if (lookahead == typeInt || lookahead == typeShort ||
				lookahead == typeLong || lookahead == typeFloat)
			{
				Push(N_DESCRIPTION1);
				//Push(DELTA_SET_ID); //Вызывает проблемы
				Push(N_IDENTIFIER);
				Push(N_TYPE);
			}
			else
			{
				type_lex lex;
				scaner->PrintError("Ожидался тип в <описание>", scaner->GetCurrentLex());
			}
			break;

		case N_DESCRIPTION1: // <описание1> → () △setFunction △enterBlock <составной оператор> △exitBlock △endFunction
							//                  | <переменная1> <список переменных> ; △endDeclareData
			if (lookahead == typeLeftBracket) // Функция
			{
				Push(DELTA_END_FUNC);
				Push(DELTA_EXIT_BLOCK);
				Push(N_COMPOSITE_OPERATOR);
				Push(DELTA_ENTER_BLOCK);
				Push(TRIAD_INIT_FUN);
				Push(DELTA_SET_FUNC);
				Push(typeRightBracket);
				Push(typeLeftBracket);
			}
			else if (lookahead == typeEval ||
				lookahead == typeComma ||
				lookahead == typeSemicolon)
			{
				Push(DELTA_END_DECLARE_DATA);
				Push(typeSemicolon);
				Push(N_LIST);
				Push(N_VARIABLE1);
				Push(DELTA_SET_ID);
			}
			else
			{
				type_lex lex;
				scaner->PrintError("Ошибка в <описание1>", scaner->GetCurrentLex());
			}
			break;

		case N_TYPE: // <тип> → int | short | long | float
		{
			type_lex lex; // для ошибок
			if (lookahead == typeInt || lookahead == typeShort || lookahead == typeLong || lookahead == typeFloat)
			{
				Push(DELTA_START_DECLARE_DATA);
				Push(lookahead);
			}
			else
			{
				scaner->PrintError("Ожидался тип (int, short, long, float)", scaner->GetCurrentLex());
			}
		}
		break;

		case N_IDENTIFIER: // <идентификатор> → typeId | typeMain
		{
			type_lex lex;
			if (lookahead == typeId || lookahead == typeMain)
			{
				Push(lookahead);
			}
			else
			{
				scaner->PrintError("Ожидался идентификатор", scaner->GetCurrentLex());
			}
		}
		break;

		case N_VARIABLE: // <переменная> → <идентификатор> <переменная1>
			Push(N_VARIABLE1);
			Push(DELTA_SET_ID);
			Push(N_IDENTIFIER);
			break;

		//TODO Разобраться с порядком
		case N_VARIABLE1: // <переменная1> → =<выражение> | ε
			if (lookahead == typeEval) // =
			{
				Push(TRIAD_ASSIGN);
				Push(DELTA_ASSIGN_END); // Нет
				Push(N_EXPRESSION);
				Push(DELTA_ASSIGN_START); // Нет
				Push(typeEval);
				Push(TRIAD_PUSH_SIMPLE);
				Push(TRIAD_INIT);
			}
			else if (lookahead == typeComma || lookahead == typeSemicolon)
			{
				eps();
			}
			else
			{
				type_lex lex;
				scaner->PrintError("Ошибка в <переменная1>", scaner->GetCurrentLex());
			}
			break;

		case N_LIST: // <список переменных> → , <переменная> <список переменных> | ε
			if (lookahead == typeComma)
			{
				Push(N_LIST);
				Push(N_VARIABLE);
				Push(typeComma);
			}
			else if (lookahead == typeSemicolon)
			{
				eps();
			}
			else
			{
				type_lex lex;
				scaner->PrintError("Ошибка в <список переменных>", scaner->GetCurrentLex());
			}
			break;
		//TODO Разобраться с порядком
		case N_ASSIGNMENT: // <присваивание> → <идентификатор> △findId △assignStart = <выражение> △assignEnd
			if (lookahead == typeId)
			{
				Push(TRIAD_ASSIGN);
				Push(DELTA_ASSIGN_END); //Нет
				Push(N_EXPRESSION);
				Push(typeEval);
				//Push(TRIAD_PUSH_CONST);
				Push(TRIAD_PUSH_SIMPLE);
				Push(TRIAD_INIT);
				Push(DELTA_ASSIGN_START); //Нет
				Push(DELTA_FIND_ID);
				Push(N_IDENTIFIER);
			}
			else
			{
				type_lex lex;
				scaner->PrintError("Ожидался идентификатор в <присваивание>", scaner->GetCurrentLex());
			}
			break;

		case N_EXPRESSION: // <выражение> → <сравнение> △pushOperand <выражение1>
			Push(N_EXPRESSION1);
			Push(N_COMPARISON);
			break;

		case N_EXPRESSION1: // <выражение1> → == △pushOperator <сравнение> △processOperator <выражение1> | != △pushOperator <сравнение> △processOperator <выражение1> | ε
		    if (lookahead == typeEq)   // ==
		    {
		        Push(N_EXPRESSION1);
	    		Push(TRIAD_EVAL);
		        Push(N_COMPARISON);
		        Push(typeEq);
		    }
		    else if (lookahead == typeUnEq) // !=
		    {
		        Push(N_EXPRESSION1);
	    		Push(TRIAD_UNEVAL);
		        Push(N_COMPARISON);
		        Push(typeUnEq);
		    }
		    else
		    {
		        eps();
		    }
		    break;
		case N_COMPOSITE_OPERATOR: // <составной оператор> → { △enterBlock <операторы и описания> } △exitBlock
			Push(DELTA_EXIT_BLOCK);
			Push(typeRightBrace);
			Push(N_OPERATORS_AND_DESCRIPTIONS);
			Push(DELTA_ENTER_BLOCK);
			Push(typeLeftBrace);
			break;

		case N_OPERATORS_AND_DESCRIPTIONS: // <операторы и описания>
			if (lookahead == typeInt || lookahead == typeShort ||
				lookahead == typeLong || lookahead == typeFloat)
			{
				Push(N_OPERATORS_AND_DESCRIPTIONS);
				Push(N_DESCRIPTION);
			}
			else if (lookahead == typeSemicolon ||
				lookahead == typeLeftBrace ||
				lookahead == typeWhile ||
				lookahead == typeReturn ||
				lookahead == typeId)
			{
				Push(N_OPERATORS_AND_DESCRIPTIONS);
				Push(N_OPERATOR);
			}
			else
			{
				eps();
			}
			break;

		case N_OPERATOR: // <оператор>
		{
			// ;
			if (lookahead == typeSemicolon)
			{
				Push(typeSemicolon);
			}

			// { ... }
			else if (lookahead == typeLeftBrace)
			{
				Push(N_COMPOSITE_OPERATOR);
			}

			// while (expr) {...}
			else if (lookahead == typeWhile) //Потенциально нужно заменить на N_CYCLE
			{
				Push(N_COMPOSITE_OPERATOR);
				Push(typeRightBracket);
				Push(N_EXPRESSION);
				Push(typeLeftBracket);
				Push(typeWhile);
			}

			// return expr ;
			else if (lookahead == typeReturn)
			{
				Push(DELTA_CHECK_RETURN); //Нет и многое иначе
				Push(typeSemicolon);
				Push(N_EXPRESSION);
				Push(typeReturn);
			}

			else if (lookahead == typeId || lookahead == typeMain)
			{
				int next = LookForward(1);

				if (next == typeEval)
				{
					Push(typeSemicolon);
					Push(N_ASSIGNMENT);
				}
				else if (next == typeLeftBracket)
				{
					Push(typeSemicolon);
					Push(N_FUNCTION_CALL);
				}
				else
				{
					type_lex lex;
					Scan(lex);
					scaner->PrintError("ожидался оператор (присваивание или вызов функции)", scaner->GetCurrentLex());
				}
			}
			else
			{
				type_lex lex;
				Scan(lex);
				scaner->PrintError("ожидался оператор", scaner->GetCurrentLex());
			}
		}
		break;

		case N_EMPTY_OPERATOR: // <пустой оператор> → ;
			if (lookahead == typeSemicolon)
			{
				Push(DELTA_EMPTY_STMT);
				Push(typeSemicolon);
			}
			else
			{
				type_lex lex;
				scaner->PrintError("Ожидался ;", scaner->GetCurrentLex());
			}
			break;

		case N_RETURN: // return <выражение> ;
			if (lookahead == typeReturn)
			{
				Push(typeSemicolon);
				Push(N_EXPRESSION);
				Push(typeReturn);
			}
			else
			{
				type_lex lex;
				scaner->PrintError("Ожидался return", scaner->GetCurrentLex());
			}
			break;

		case N_CYCLE: // <while> → while △whileStart ( <выражение> ) △whileCondition <составной оператор> △whileEnd
			if (lookahead == typeWhile)
			{
				Push(DELTA_WHILE_END);
				Push(N_COMPOSITE_OPERATOR);
				Push(DELTA_WHILE_CONDITION);
				Push(typeRightBracket);
				Push(N_EXPRESSION);
				Push(typeLeftBracket);
				Push(DELTA_WHILE_START);
				Push(typeWhile);
			}
			else
			{
				type_lex lex;
				scaner->PrintError("Ожидался while", scaner->GetCurrentLex());
			}
			break;

		case N_FUNCTION_CALL: // <вызов функции> → <идентификатор> △findId △callFunction ()
			{
				Push(TRIAD_CALL_FUN);
				Push(typeRightBracket);
				Push(typeLeftBracket);
				Push(TRIAD_FIND_FUN);
				Push(DELTA_CALL_FUNCTION); //Нет
				Push(N_IDENTIFIER);
			}
			break;

		case N_COMPARISON: // <сравнение> → <побитовый сдвиг> △pushOperand <сравнение1>
		    Push(N_COMPARISON1);
		    Push(N_BITWISE_SHIFT);
		    break;

		case N_COMPARISON1: // <сравнение1> → < | > | <= | >= <побитовый сдвиг> <сравнение1> | ε
		    if (lookahead == typeLess || lookahead == typeMore ||
		        lookahead == typeLessOrEq || lookahead == typeMoreOrEq)
		    {
		        Push(N_COMPARISON1);
	    		if (lookahead == typeLess) {
	    			Push(TRIAD_LESS);
	    		}
	    		else if (lookahead == typeLessOrEq) {
	    			Push(TRIAD_LESS_OR_EQUAL);
	    		}
	    		else if (lookahead == typeMore) {
	    			Push(TRIAD_MORE);
	    		}
	    		else {
	    			Push(TRIAD_MORE_OR_EQUAL);
	    		}
		        Push(N_BITWISE_SHIFT);
		        Push(lookahead);
		    }
		    else
		    {
		        eps();
		    }
		    break;
		case N_BITWISE_SHIFT: // <побитовый сдвиг> → <слагаемое> △pushOperand <побитовый сдвиг1>
		    Push(N_BITWISE_SHIFT1);
		    Push(N_SUMMAND);
		    break;

		case N_BITWISE_SHIFT1:
		    if (lookahead == typeBitwiseLeft || lookahead == typeBitwiseRight)
		    {
		        Push(N_BITWISE_SHIFT1);
	    		if (lookahead == typeBitwiseLeft) {
	    			Push(TRIAD_SHIFT_LEFT);
	    		}
	    		else {
	    			Push(TRIAD_SHIFT_RIGHT);
	    		}
		        Push(N_SUMMAND);
		        Push(lookahead);
		    }
		    else
		    {
		        eps();
		    }
		    break;

		case N_SUMMAND: // <слагаемое> → <множитель> △pushOperand <слагаемое1>
			Push(N_SUMMAND1);
			Push(N_MULTIPLIER);
			break;

		case N_SUMMAND1: // <слагаемое1> → + △pushOperator <множитель> △processOperator <слагаемое1> | ε
			if (lookahead == typePlus || lookahead == typeMinus)
			{
				Push(N_SUMMAND1);
				if (lookahead == typePlus) {
					Push(TRIAD_ADD);
				}
				else {
					Push(TRIAD_MINUS);
				}
				Push(N_MULTIPLIER);
				Push(lookahead);
			}
			else
			{
				eps();
			}
			break;

		case N_MULTIPLIER: // <множитель> → <унарная операция> △pushOperand <множитель1>
			Push(N_MULTIPLIER1);
			Push(N_UNARY_OPERATION);
			break;

		case N_MULTIPLIER1: // <множитель1> → * △pushOperator <унарная операция> △processOperator <множитель1> | ...
		    if (lookahead == typeMul || lookahead == typeDiv || lookahead == typeMod)
		    {
		        Push(N_MULTIPLIER1);
	    		if (lookahead == typeMul) {
	    			Push(TRIAD_MULT);
	    		}
	    		else if (lookahead == typeDiv) {
	    			Push(TRIAD_DIV);
	    		}
	    		else {
	    			Push(TRIAD_MOD);
	    		}
		        Push(N_UNARY_OPERATION);
		        Push(lookahead);
		    }
		    else
		    {
		        eps();
		    }
		    break;

		//TODO Разобраться с Push TRIAD_***
		case N_UNARY_OPERATION: // <унарная операция> → <знак> <элементарное выражение>
			Push(N_ELEMENTARY_EXPRESSION);
			Push(N_SIGN);
			break;

		//TODO Разобраться с Push TRIAD_***
		case N_SIGN: // <знак> → + | - | ε
			if (lookahead == typePlus || lookahead == typeMinus)
			{
				Push(lookahead);
			}
			else
			{
				eps();
			}
			break;
		//TODO Потенциально содержит ошибку
		case N_ELEMENTARY_EXPRESSION: // <элементарное выражение> → <идентификатор> △findId <элем1> | <константа> | ( <выражение> )
		    if (lookahead == typeId)
		    {
				int next = LookForward(1);
				if (next == typeLeftBracket)
				{
					Push(TRIAD_CALL_FUN);
					Push(typeRightBracket);
					Push(typeLeftBracket);
					Push(TRIAD_FIND_FUN);
					Push(DELTA_CALL_FUNCTION);
					Push(N_IDENTIFIER);
				}
				else
				{
		        	Push(N_ELEM1);
	    			Push(TRIAD_PUSH_SIMPLE);
		        	Push(DELTA_FIND_ID);
		        	Push(N_IDENTIFIER);
				}
		    }
		    else if (lookahead == typeInt || lookahead == typeShort ||
		             lookahead == typeLong || lookahead == typeFloat)
		    { // Константа
	    		Push(TRIAD_PUSH_CONST); // Добавил
		        Push(lookahead);
		    }
		    else if (lookahead == typeLeftBracket)
		    {
	    		Push(TRIAD_PUSH_CONST);
		        Push(typeRightBracket);
		        Push(N_EXPRESSION);
		        Push(typeLeftBracket);
		    }
		    else
		    {
		        scaner->PrintError("Ошибка в <элементарное выражение>", scaner->GetCurrentLex());
		    }
		    break;
		//TODO Разобраться с Push TRIAD_***
		case N_ELEM1: // <элем1> → () | ε
			if (lookahead == typeLeftBracket)
			{
				Push(DELTA_CALL_FUNCTION);
				Push(typeRightBracket); // ')'
				Push(typeLeftBracket);  // '('
			}
			else
			{
				eps();
			}
			break;

		default:
			type_lex lex;
			scaner->PrintError("Неизвестный нетерминал в ApplyRule", scaner->GetCurrentLex());
			break;
		}
}

int Diagram::Scan(type_lex lex) 
{
	return scaner->UseScaner(lex);
}

void Diagram::DeltaOperation(int delta)
{
	switch (delta)
	{
		case DELTA_START_DECLARE_DATA:
			translate->deltaStartDeclareData();
			Pop();
			break;

		case DELTA_END_DECLARE_DATA:
			translate->deltaEndDeclareData();
			Pop();
			break;

		case DELTA_SET_ID:
			translate->deltaSetId();
			Pop();
			break;

		case DELTA_FIND_ID:
			translate->deltaFindId();
			Pop();
			break;

		case DELTA_SET_FUNC:
			translate->deltaSetFunction();
			Pop();
			break;

		case DELTA_END_FUNC:
			translate->deltaEndFunction();
			Pop();
			break;

		case DELTA_ENTER_BLOCK:
			translate->deltaEnterBlock();
			Pop();
			break;

		case DELTA_EXIT_BLOCK:
			translate->deltaExitBlock();
			Pop();
			break;

		case DELTA_CHECK_TYPE:
			translate->deltaCheckType();
			Pop();
			break;

		case DELTA_CHECK_RETURN:
			translate->deltaCheckReturn();
			Pop();
			break;

		case DELTA_CALL_FUNCTION:
			translate->deltaCallFunction();
			Pop();
			break;

		case DELTA_SET_INT_CONST:
			translate->deltaSetIntConst();
			Pop();
			break;

		case DELTA_SET_FLOAT_CONST:
			translate->deltaSetFloatConst();
			Pop();
			break;

		case DELTA_EMPTY_STMT:
			translate->deltaEmptyStmt();
			Pop();
			break;

		case DELTA_ASSIGN_START:
			translate->deltaAssignStart();
			Pop();
			break;

		case DELTA_ASSIGN_END:
			translate->deltaAssignEnd();
			Pop();
			break;
		case DELTA_WHILE_START:
			translate->deltaWhileStart();
			Pop();
			break;

		case DELTA_WHILE_CONDITION:
			translate->deltaWhileCondition();
			Pop();
			break;

		case DELTA_WHILE_END:
			translate->deltaWhileEnd();
			Pop();
			break;

		case DELTA_PUSH_OPERAND:
			translate->deltaPushOperand();
			Pop();
			break;

		case DELTA_PUSH_OPERATOR:
			translate->deltaPushOperator();
			Pop();
			break;

		case DELTA_PROCESS_OPERATOR:
			translate->deltaProcessOperator();
			Pop();
			break;

		case DELTA_CHECK_BINARY_OP:
			translate->deltaCheckBinaryOp();
			Pop();
			break;

		case TRIAD_ADD:
			triadGenerator->deltaAdd();
			Pop();
			break;

		case TRIAD_MINUS:
			triadGenerator->deltaMinus();
			Pop();
			break;

		case TRIAD_MULT:
			triadGenerator->deltaMult();
			Pop();
			break;

		case TRIAD_DIV:
			triadGenerator->deltaDiv();
			Pop();
			break;

		case TRIAD_MOD:
			triadGenerator->deltaMod();
			Pop();
			break;

		case TRIAD_LESS:
			triadGenerator->deltaLess();
			Pop();
			break;

		case TRIAD_LESS_OR_EQUAL:
			triadGenerator->deltaLessEq();
			Pop();
			break;

		case TRIAD_MORE:
			triadGenerator->deltaMore();
			Pop();
			break;

		case TRIAD_MORE_OR_EQUAL:
			triadGenerator->deltaMoreEq();
			Pop();
			break;

		case TRIAD_SHIFT_LEFT:
			triadGenerator->deltaShiftLeft();
			Pop();
			break;

		case TRIAD_SHIFT_RIGHT:
			triadGenerator->deltaShiftRight();
			Pop();
			break;

		case TRIAD_EVAL:
			triadGenerator->deltaEval();
			Pop();
			break;

		case TRIAD_UNEVAL:
			triadGenerator->deltaUnEval();
			Pop();
			break;

		case TRIAD_PUSH_SIMPLE:
			triadGenerator->deltaPushOperand(false);
			Pop();
			break;

		case TRIAD_PUSH_CONST:
			triadGenerator->deltaPushOperand(true);
			Pop();
			break;

		case TRIAD_GOTO: //TODO Под вопросом его необходимость
			Pop();
			break;

		case TRIAD_INIT:
			translate->deltaSetInitValue();
			Pop();
			break;

		case TRIAD_ASSIGN:
			triadGenerator->deltaAssign();
			Pop();
			break;

		case TRIAD_INIT_FUN: //TODO
			triadGenerator->deltaInitFunction();
			Pop();
			break;

		case TRIAD_CALL_FUN:
			triadGenerator->deltaCallFunction();
			Pop();
			break;

		case TRIAD_FIND_FUN:
			translate->deltaFindFunc();
			Pop();
			break;

		default:
			std::cerr << "Warning: unknown delta operation " << delta << std::endl;
			break;
		}
}

void Diagram::PrintStack() {
	std::cout << "Стек: ";
	for (auto it = mag.rbegin(); it != mag.rend(); ++it) {
		std::cout << *it << " ";
	}
	std::cout << std::endl;
}

void Diagram::printTriad() {
	this->triadGenerator->printTriad();
}
