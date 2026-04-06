#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define maxText 100000
#define maxLex 100
#define maxKeyword 16
#define maxNumber 10

typedef char type_mod[maxText];
typedef char type_lex[maxLex];

// Терминалы

#define typeId 1

// Ключевые слова
#define typeShort 10
#define typeLong 11
#define typeInt 12
#define typeFloat 13
#define typeWhile 14
#define typeReturn 15
#define typeMain 16

// Константы
#define constInt 20
#define constFloat 21

// Специальные символы
#define typePoint 30
#define typeComma 31
#define typeSemicolon 32
#define typeLeftBracket 33
#define typeRightBracket 34
#define typeLeftBrace 35
#define typeRightBrace 36

// Операторы
#define typeEval 40
#define typeUnEq 41
#define typeEq 42
#define typeMore 43
#define typeMoreOrEq 44
#define typeLess 45
#define typeLessOrEq 46
#define typeBitwiseRight 47
#define typeBitwiseLeft 48
#define typeMul 49
#define typeDiv 50
#define typeMod 51
#define typePlus 52
#define typeMinus 53

// Служебные
#define typeEnd 100
#define typeError 199

// Граница терминалов
#define MAX_TYPE_TERMINAL 200

// Не терминалы
#define N_PROGRAM 300
#define N_DESCRIPTION 301
#define N_TYPE 302
#define N_LIST 303
#define N_VARIABLE 304
#define N_VARIABLE1 305
#define N_DESCRIPTION1 306
#define N_ASSIGNMENT 307
#define N_EXPRESSION 308
#define N_EXPRESSION1 309
#define N_COMPOSITE_OPERATOR 310
#define N_OPERATORS_AND_DESCRIPTIONS 311
#define N_OPERATOR 312
#define N_EMPTY_OPERATOR 313
#define N_FUNCTION_CALL 314
#define N_COMPARISON 315
#define N_COMPARISON1 316
#define N_BITWISE_SHIFT 317
#define N_BITWISE_SHIFT1 318
#define N_SUMMAND 319
#define N_SUMMAND1 320
#define N_MULTIPLIER 321
#define N_MULTIPLIER1 322
#define N_UNARY_OPERATION 323
#define N_SIGN 324
#define N_ELEMENTARY_EXPRESSION 325
#define N_ELEM1 326
#define N_CYCLE 327
#define N_RETURN 328
#define N_IDENTIFIER 329
#define N_CONSTANT 330
#define N_INT_CONSTANT 331
#define N_INT_CONSTANT1 332
#define N_FLOAT_CONSTANT 333

#define DELTA_START_DECLARE_DATA 400
#define DELTA_END_DECLARE_DATA 401
#define DELTA_SET_ID 402
#define DELTA_FIND_ID 403
#define DELTA_SET_FUNC 404
#define DELTA_END_FUNC 405
#define DELTA_ENTER_BLOCK 406
#define DELTA_EXIT_BLOCK 407
#define DELTA_CHECK_TYPE 408
#define DELTA_CHECK_RETURN 409
#define DELTA_CALL_FUNCTION 410
#define DELTA_SET_INT_CONST 411
#define DELTA_SET_FLOAT_CONST 412
#define DELTA_EMPTY_STMT 413
#define DELTA_ASSIGN_START 414
#define DELTA_ASSIGN_END 415
#define DELTA_WHILE_START 416
#define DELTA_WHILE_CONDITION 417
#define DELTA_WHILE_END 418
#define DELTA_PUSH_OPERAND 419
#define DELTA_PUSH_OPERATOR 420
#define DELTA_PROCESS_OPERATOR 421
#define DELTA_CHECK_BINARY_OP 422

#define TRIAD_ADD 500
#define TRIAD_MINUS 501
#define TRIAD_MULT 502
#define TRIAD_DIV 503
#define TRIAD_MOD 504
#define TRIAD_LESS 505
#define TRIAD_LESS_OR_EQUAL 506
#define TRIAD_MORE 507
#define TRIAD_MORE_OR_EQUAL 508
#define TRIAD_SHIFT_LEFT 509
#define TRIAD_SHIFT_RIGHT 510
#define TRIAD_EVAL 511
#define TRIAD_UNEVAL 512
#define TRIAD_PUSH_SIMPLE 513
#define TRIAD_PUSH_CONST 514
#define TRIAD_GOTO 515
#define TRIAD_INIT 516
#define TRIAD_ASSIGN 517
#define TRIAD_INIT_FUN 518
#define TRIAD_CALL_FUN 519
#define TRIAD_FIND_FUN 520