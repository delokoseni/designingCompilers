#include "Scaner.h"
#include <fstream>
#include <sstream>
#include <iostream>

/**
* Список допустимых ключевых слов в программе
*/
type_lex keyword[maxKeyword] =
{
    "int", "short", "long", "float", "while", "main", "return"
};

/**
* Список идентификаторов, которые соответствуют допустимым ключевым словам в программе
*/
int indexKeyword[maxKeyword] =
{
    typeInt, typeShort, typeLong, typeFloat, typeWhile, typeMain, typeReturn
};

//Ошибка если файл пустой
void Scaner::PutUK(size_t uk) 
{
    if (uk < text.size()) 
    {
        this->uk = uk;
    }
    else if(text.size() != 0)
    {
        std::cerr << "Ошибка: индекс выходит за пределы размера текста." << std::endl;
        exit(1);
    }
}

size_t Scaner::GetUK()
{
    return uk;
}

void Scaner::GetData(const std::string& filename)
{
    std::ifstream file(filename);
    if (file.is_open())
    {
        std::stringstream buffer;
        buffer << file.rdbuf();
        text = buffer.str(); // Сохраняем текст из файла
        text += '\n';

        // Определяем позиции перевода строки
        for (size_t i = 0; i < text.size(); ++i) {
            if (text[i] == '\n') {
                lineBreakPositions.push_back(i);
            }
        }

        std::cout << "Текст программы:" << std::endl << text << std::endl;
        std::cout << std::endl << "Результат работы сканера:" << std::endl;
    }
    else
    {
        PrintError("Ошибка открытия файла.", filename);
    }
}

void Scaner::PrintError(const std::string& error, const std::string& lexeme)
{
    int line = 1;
    int pos = uk;

    for (size_t i = 0; i < lineBreakPositions.size(); ++i) {
        if (lineBreakPositions[i] < uk) {
            line++;
        }
        else {
            pos = uk - (i == 0 ? 0 : lineBreakPositions[i - 1] + 1);
            break;
        }
    }

    std::cout << "Ошибка: " << error << " на строке " << line << ", позиция " << pos;
    if (!lexeme.empty())
    {
        std::cout << ", найдено: " << lexeme;
    }
    std::cout << std::endl;
    exit(1);
}

int Scaner::UseScaner(type_lex lex)
{
    int i = 0;
    lex[0] = '\0';

    while (true)
    {
        // Пропускаем пробелы
        while (uk < text.size() && (text[uk] == ' ' || text[uk] == '\t' || text[uk] == '\n'))
        {
            uk++;
        }

        // Конец программы 
        if (uk >= text.size())
        {
            lex[i] = '\0';
            strcpy_s(currentLex, lex);
            return typeEnd;
        }

        // Обработка комментариев
        if (text[uk] == '/')
        {
            uk++;
            if (uk < text.size() && text[uk] == '/')
            {
                while (uk < text.size() && text[uk] != '\n')
                {
                    uk++;
                }
                continue;
            }
            else
            {
                // Деление
                lex[i++] = '/';
                lex[i++] = '\0';
                strcpy_s(currentLex, lex);
                return typeDiv;
            }
        }

        // Обработка чисел
        if (isdigit(text[uk]))
        {
            while (uk < text.size() && isdigit(text[uk]) && i < maxNumber - 1)
            {
                lex[i++] = text[uk++];
            }
            if (i == maxNumber - 1 && uk < text.size() && isdigit(text[uk])) {
                PrintError("Константа превышает максимальную длину лексемы: ", lex);
                strcpy_s(currentLex, lex);
                return typeError;
            }

            // Обработка вещественных чисел
            if (uk < text.size() && text[uk] == '.')
            {
                lex[i++] = text[uk++];
                while (uk < text.size() && isdigit(text[uk]) && i < maxNumber - 1)
                {
                    lex[i++] = text[uk++];
                }
                lex[i] = '\0';
                if (i == maxNumber - 1 && uk < text.size() && isdigit(text[uk]))
                {
                    while (uk < text.size() && isdigit(text[uk]))
                    {
                        uk++;
                    }
                    PrintError("Константа превышает максимальную длину лексемы: ", lex);
                    strcpy_s(currentLex, lex);
                    return typeError;
                }
                strcpy_s(currentLex, lex);
                return typeFloat;
            }

            lex[i] = '\0';
            strcpy_s(currentLex, lex);
            return typeInt;
        }

        // Обработка точки
        if (text[uk] == '.')
        {
            lex[i++] = text[uk++];
            if (uk < text.size() && isdigit(text[uk]))
            {
                while (uk < text.size() && isdigit(text[uk]) && i < maxNumber - 1)
                {
                    lex[i++] = text[uk++];
                }
                lex[i] = '\0';
                if (i == maxNumber - 1 && uk < text.size() && isdigit(text[uk]))
                {
                    while (uk < text.size() && isdigit(text[uk]))
                    {
                        uk++;
                    }
                    strcpy_s(currentLex, lex);
                    PrintError("Константа превышает максимальную длину лексемы: ", lex);
                    return typeError;
                }
                return typeFloat;
            }
            else
            {
                lex[i] = '\0';
                strcpy_s(currentLex, lex);
                return typePoint;
            }
        }

        // Обработка идентификаторов
        if (isalpha(text[uk]) || text[uk] == '_')
        {
            while (uk < text.size() && (isalnum(text[uk]) || text[uk] == '_') && i < maxLex - 1)
            {
                lex[i++] = text[uk++];
            }
            lex[i] = '\0';

            if (i == maxLex - 1 && (uk < text.size() && (isalnum(text[uk]) || text[uk] == '_')))
            {
                while (uk < text.size() && (isalnum(text[uk]) || text[uk] == '_'))
                {
                    uk++;
                }
                PrintError("Идентификатор превышает максимальную длину лексемы.", lex);
                strcpy_s(currentLex, lex);
                return typeError;
            }

            // Проверяем, является ли идентификатор ключевым словом
            for (int j = 0; j < maxKeyword; j++)
            {
                if (strcmp(lex, keyword[j]) == 0)
                {
                    strcpy_s(currentLex, lex);
                    return indexKeyword[j];
                }
            }
            strcpy_s(currentLex, lex);
            return typeId;
        }

        // Обработка специальных символов
        switch (text[uk])
        {
        case ',':
            uk++; lex[i++] = ','; lex[i] = '\0'; return typeComma;
        case ';':
            uk++; lex[i++] = ';'; lex[i] = '\0'; return typeSemicolon;
        case '(':
            uk++; lex[i++] = '('; lex[i] = '\0'; return typeLeftBracket;
        case ')':
            uk++; lex[i++] = ')'; lex[i] = '\0'; return typeRightBracket;
        case '{':
            uk++; lex[i++] = '{'; lex[i] = '\0'; return typeLeftBrace;
        case '}':
            uk++; lex[i++] = '}'; lex[i] = '\0'; return typeRightBrace;
        case '+':
            uk++; lex[i++] = '+'; lex[i] = '\0'; return typePlus;
        case '-':
            uk++; lex[i++] = '-'; lex[i] = '\0'; return typeMinus;
        case '*':
            uk++; lex[i++] = '*'; lex[i] = '\0'; return typeMul;
        case '%':
            uk++; lex[i++] = '%'; lex[i] = '\0'; return typeMod;
        case '=':
            uk++;
            lex[i++] = '=';
            if (uk < text.size() && text[uk] == '=')
            {
                uk++;
                lex[i++] = '=';
                lex[i] = '\0';
                strcpy_s(currentLex, lex);
                return typeEq;
            }
            else
            {
                lex[i] = '\0';
                strcpy_s(currentLex, lex);
                return typeEval;
            }
        case '!':
            uk++;
            lex[i++] = '!';
            if (uk < text.size() && text[uk] == '=')
            {
                uk++;
                lex[i++] = '=';
                lex[i] = '\0';
                strcpy_s(currentLex, lex);
                return typeUnEq;
            }
            else
            {
                lex[i] = '\0';
                PrintError("Неожидаемый символ: ", lex);
                strcpy_s(currentLex, lex);
                return typeError;
            }
        case '>':
            uk++;
            lex[i++] = '>';
            if (uk < text.size() && text[uk] == '=')
            {
                uk++;
                lex[i++] = '=';
                lex[i] = '\0';
                strcpy_s(currentLex, lex);
                return typeMoreOrEq;
            }
            else if (uk < text.size() && text[uk] == '>')
            {
                uk++;
                lex[i++] = '>';
                lex[i] = '\0';
                strcpy_s(currentLex, lex);
                return typeBitwiseRight;
            }
            else
            {
                lex[i] = '\0';
                strcpy_s(currentLex, lex);
                return typeMore;
            }
        case '<':
            uk++;
            lex[i++] = '<';
            if (uk < text.size() && text[uk] == '=')
            {
                uk++;
                lex[i++] = '=';
                lex[i] = '\0';
                strcpy_s(currentLex, lex);
                return typeLessOrEq;
            }
            else if (uk < text.size() && text[uk] == '<')
            {
                uk++;
                lex[i++] = '<';
                lex[i] = '\0';
                strcpy_s(currentLex, lex);
                return typeBitwiseLeft;
            }
            else
            {
                lex[i] = '\0';
                strcpy_s(currentLex, lex);
                return typeLess;
            }
        default:
            lex[i++] = text[uk++];
            lex[i] = '\0';
            PrintError("Лексическая ошибка в строке ", lex);
            strcpy_s(currentLex, lex);
            return typeError;
        }
    }
}

Scaner::Scaner(const std::string& filename)
{
    GetData(filename);
    PutUK(0);
}

char* Scaner::GetCurrentLex()
{
    return currentLex;
}