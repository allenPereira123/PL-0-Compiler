// Allen Pereira
// Allec Pereira

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"

instruction *code;
symbol *symbol_table;
int codeidx = 0;
int token_num = 0;
int symbol_table_index = 0;
int procedurecount = 0;
char *errormsg[] = {
    "",                                                                                     // 0
    "Error : program must end with period",                                                 // 1
    "Error : const, var, and read keywords must be followed by identifier",                 // 2
    "Error : competing symbol declarations at the same level",                              // 3
    "Error : constants must be assigned with =",                                            // 4
    "Error : constants must be assigned an integer value",                                  // 5
    "Error : symbol declarations must be followed by a semicolon",                          // 6
    "Error : undeclared variable or constant in equation",                                  // 7
    "Error : only variable values may be altered",                                          // 8
    "Error : assignment statements must use :=",                                            // 9
    "Error : begin must be followed by end",                                                // 10
    "Error : if must be followed by then",                                                  // 11
    "Error : while must be followed by do",                                                 // 12
    "Error : condition must contain comparison operator",                                   // 13
    "Error : right parenthesis must follow left parenthesis",                               // 14
    "Error : arithmetic equations must contain operands, parentheses, numbers, or symbols", // 15
    "Error : undeclared procedure for call",                                                // 16
    "Error : parameters may only be specified by an identifier",                            // 17
    "Error : parameters must be declared",                                                  // 18
    "Error : cannot return from main"};                                                     // 19

int symbolTableSearch(char *name, int lexLevel, int kind);
void emit(int opcode, char op[], int l, int m);
int prodecure_decleration(lexeme *tokenList, int lexLevel);
int findprocedure(int index);
void block(lexeme *tokenList, int lexLevel, int param, int procedureIndex);
int constant_decleration(lexeme *tokenList, int lexLevel);
int symbol_table_check(char *str, int lexLevel);
void add_to_symbol_table(int kind, char *name, int val, int level, int addr, int mark, int param);
int var_decleration(lexeme *tokenList, int lexLevel, int param);
int procedure_decleration(lexeme *tokenList, int lexLevel);
void mark(int count);
void print_symbol_table();
void print_code();
void statement(lexeme *tokenList, int lexLevel);
void expression(lexeme *tokenList, int lexLevel);
void term(lexeme *tokenList, int lexLevel);
void factor(lexeme *tokenList, int lexLevel);
void condition(lexeme *tokenList, int lexLevel);

int symbolTableSearch(char *name, int lexLevel, int kind)
{

    int index = -1;

    for (int i = 0; i < symbol_table_index; i++)
    {
        if (strcmp(symbol_table[i].name, name) == 0 && symbol_table[i].kind == kind && symbol_table[i].level <= lexLevel && symbol_table[i].mark == 0)
            index = i;
    }

    return index;
}

void mark(int count)
{
    int i = symbol_table_index - 1;
    for (i; i >= 0; i--)
    {
        if (symbol_table[i].mark == 0 && count > 0)
        {
            symbol_table[i].mark = 1;
            count--;
        }
    }
}

int findprocedure(int index)
{
    int last_index = symbol_table_index - 1;
    for (int i = 0; i <= last_index; i++)
    {
        if (symbol_table[i].kind == 3 && symbol_table[i].val == index)
        {
            return i;
        }
    }

    return -1;
}

void print_symbol_table()
{
    for (int i = 0; i < symbol_table_index; i++)
    {
        printf("Symbol table index %d\n", i);
        printf("%d\n", symbol_table[i].kind);
        printf("%s\n", symbol_table[i].name);
        printf("%d\n", symbol_table[i].val);
        printf("%d\n", symbol_table[i].level);
        printf("%d\n", symbol_table[i].addr);
        printf("%d\n", symbol_table[i].mark);
        printf("%d\n", symbol_table[i].param);
        printf("\n\n");
    }
}

instruction *parse(lexeme *tokenList, int print)
{
    code = malloc(500 * sizeof(instruction));
    symbol_table = malloc(500 * sizeof(symbol));
    int numProc = 1;
    emit(7, "JMP", 0, 0);

    for (int i = 0; i < 500; i++)
    {
        if (tokenList[i].type == 30)
        {
            numProc++;
            emit(7, "JMP", 0, 0);
        }
    }

    add_to_symbol_table(3, "main", 0, 0, 0, 0, 0);

    procedurecount++;

    block(tokenList, 0, 0, 0);

    // .
    if (tokenList[token_num].type != 19)
    {
        printf("%s\n", errormsg[1]);
        exit(0);
    }

    // insert proper m value in jump instructions
    for (int i = 0; i < numProc; i++)
    {
        code[i].m = symbol_table[findprocedure(i)].addr;
    }

    // insert proper m value in call instructions
    for (int i = 0; i < codeidx; i++)
    {
        if (code[i].opcode == 5)
            code[i].m = symbol_table[findprocedure(code[i].m)].addr;
    }

    emit(9, "SYS", 0, 3);

    if (print)
    {
        print_code();
        printf("\n\n");
    }

    return code;
}

// checks for symbol table item with matching name and lex level that is unmarked
int symbol_table_check(char *str, int lexLevel)
{
    int index = symbol_table_index - 1;
    for (int i = 0; i <= index; i++)
    {
        if (strcmp(symbol_table[i].name, str) == 0 && symbol_table[i].level == lexLevel && symbol_table[i].mark == 0)
            return i;
    }

    return -1;
}

void add_to_symbol_table(int kind, char *name, int val, int level, int addr, int mark, int param)
{
    symbol_table[symbol_table_index].kind = kind;
    strcpy(symbol_table[symbol_table_index].name, name);
    symbol_table[symbol_table_index].val = val;
    symbol_table[symbol_table_index].level = level;
    symbol_table[symbol_table_index].addr = addr;
    symbol_table[symbol_table_index].mark = mark;
    symbol_table[symbol_table_index].param = param;
    symbol_table_index++;
}

void print_code()
{
    int line = 0;
    printf("Generated Assembly:\n");
    printf("Line\t\tOP\t\tL\t\tM\n");
    for (int i = 0; i < codeidx; i++)
    {
        printf("%d\t\t%s\t\t%d\t\t%d\n", line, code[i].op, code[i].l, code[i].m);
        line++;
    }
}

void block(lexeme *tokenList, int lexLevel, int param, int procedureIndex)
{
    int numConst = constant_decleration(tokenList, lexLevel);
    int numVars = var_decleration(tokenList, lexLevel, param);
    int numProc = procedure_decleration(tokenList, lexLevel);

    symbol_table[procedureIndex].addr = codeidx;

    emit(6, "INC", 0, 4 + numVars);
    statement(tokenList, lexLevel);

    mark(numConst + numVars + numProc);
}

int constant_decleration(lexeme *tokenList, int lexLevel)
{
    int numConst = 0;
    if (tokenList[token_num].type == 28)
    {
        do
        {
            numConst++;
            token_num++;

            // if token type is not an identifer
            if (tokenList[token_num].type != 2)
            {
                printf("%s\n", errormsg[2]);
                exit(0);
            }

            // checks if token has already been declared
            if (symbol_table_check(tokenList[token_num].name, lexLevel) != -1)
            {
                printf("%s\n", errormsg[3]);
                exit(0);
            }

            char ident_name[50];

            strcpy(ident_name, tokenList[token_num].name);

            token_num++;

            // checks if constant is assigned using "=" operator
            if (tokenList[token_num].type != 9)
            {
                printf("%s\n", errormsg[4]);
                exit(0);
            }

            token_num++;

            // checks if constant is being assigned an int value
            if (tokenList[token_num].type != 3)
            {
                printf("%s\n", errormsg[5]);
                exit(0);
            }

            add_to_symbol_table(1, ident_name, tokenList[token_num].value, lexLevel, 0, 0, 0);
            token_num++;
            // checks if token is ,
        } while (tokenList[token_num].type == 17);

        // checks if constant decleration ends with semi-colon
        if (tokenList[token_num].type != 18)
        {
            printf("%s\n", errormsg[6]);
            exit(0);
        }

        token_num++;
    }
    return numConst;
}

int var_decleration(lexeme *tokenList, int lexLevel, int param)
{
    int numVars;

    if (param == 1)
        numVars = 1;

    else
        numVars = 0;

    // if token is var
    if (tokenList[token_num].type == 29)
    {
        do
        {
            numVars++;
            token_num++;

            // checks if token is an identifier
            if (tokenList[token_num].type != 2)
            {
                printf("%s\n", errormsg[2]);
                exit(0);
            }

            // checks if identifier was declared previously
            if (symbol_table_check(tokenList[token_num].name, lexLevel) != -1)
            {
                printf("%s\n", errormsg[3]);
                exit(0);
            }

            add_to_symbol_table(2, tokenList[token_num].name, 0, lexLevel, numVars + 3, 0, 0);

            token_num++;

            // while token is ,
        } while (tokenList[token_num].type == 17);

        // checks if var decleration ends with semi-colon
        if (tokenList[token_num].type != 18)
        {
            printf("%s\n", errormsg[6]);
            exit(0);
        }

        token_num++;
    }
    return numVars;
}

int procedure_decleration(lexeme *tokenList, int lexLevel)
{
    int numProc = 0;

    if (tokenList[token_num].type == 30)
    {
        do
        {
            numProc++;
            token_num++;

            if (tokenList[token_num].type != 2)
            {
                printf("%s\n", errormsg[2]);
                exit(0);
            }

            if (symbol_table_check(tokenList[token_num].name, lexLevel) != -1)
            {
                printf("%s\n", errormsg[3]);
                exit(0);
            }

            int procIdx = symbol_table_index;

            add_to_symbol_table(3, tokenList[token_num].name, procedurecount, lexLevel, 0, 0, 0);
            procedurecount++;
            token_num++;

            // token = '('
            if (tokenList[token_num].type == 15)
            {
                token_num++;

                if (tokenList[token_num].type != 2)
                {
                    printf("%s\n", errormsg[17]);
                    exit(0);
                }

                add_to_symbol_table(2, tokenList[token_num].name, 0, lexLevel + 1, 3, 0, 0);

                symbol_table[symbol_table_index - 2].param = 1;
                token_num++;

                // token != )
                if (tokenList[token_num].type != 16)
                {
                    printf("%s\n", errormsg[14]);
                    exit(0);
                }

                token_num++;
                // token == ;
                if (tokenList[token_num].type != 18)
                {
                    printf("%s\n", errormsg[6]);
                    exit(0);
                }

                token_num++;

                block(tokenList, lexLevel + 1, 1, procIdx);
            }
            else
            {
                // ;
                if (tokenList[token_num].type != 18)
                {
                    printf("%s\n", errormsg[6]);
                    exit(0);
                }
                token_num++;
                block(tokenList, lexLevel + 1, 0, procIdx);
            }

            if (code[codeidx - 1].opcode != 2 && code[codeidx - 1].m != 0)
            {
                emit(1, "LIT", 0, 0);
                emit(2, "OPR", 0, 0);
            }

            if (tokenList[token_num].type != 18)
            {
                printf("%s\n", errormsg[6]);
                exit(0);
            }

            token_num++;

        } while (tokenList[token_num].type == 30);
    }

    return numProc;
}

void statement(lexeme *tokenList, int lexLevel)
{
    // if token is a identifier
    if (tokenList[token_num].type == 2)
    {
        int symIdx = symbolTableSearch(tokenList[token_num].name, lexLevel, 2);

        // checks if symbol is undeclared
        if (symIdx == -1)
        {
            printf("%s\n", errormsg[7]);
            exit(0);
        }

        token_num++;

        // checks if assignment statement uses becomessym (:=)
        if (tokenList[token_num].type != 20)
        {
            printf("%s\n", errormsg[9]);
            exit(0);
        }

        token_num++;

        expression(tokenList, lexLevel);
        emit(4, "STO", lexLevel - symbol_table[symIdx].level, symbol_table[symIdx].addr);

        return;
    }
    // call
    if (tokenList[token_num].type == 27)
    {
        token_num++;

        // checks if token is identifier
        if (tokenList[token_num].type != 2)
        {
            printf("%s\n", errormsg[2]);
            exit(0);
        }

        int symIdx = symbolTableSearch(tokenList[token_num].name, lexLevel, 3);

        // undeclared procedure
        if (symIdx == -1)
        {
            printf("%s\n", errormsg[16]);
            exit(0);
        }

        token_num++;

        // checks if token is (
        if (tokenList[token_num].type == 15)
        {
            token_num++;
            if (symbol_table[symIdx].param != 1)
            {
                printf("%s\n", errormsg[18]);
                exit(0);
            }

            expression(tokenList, lexLevel);

            if (tokenList[token_num].type != 16)
            {
                printf("%s\n", errormsg[14]);
                exit(0);
            }
            token_num++;
        }

        else
            emit(1, "LIT", 0, 0);

        emit(5, "CAL", lexLevel - symbol_table[symIdx].level, symbol_table[symIdx].val);
        return;
    }

    // if token is return symbol
    if (tokenList[token_num].type == 34)
    {
        if (lexLevel == 0)
        {
            printf("%s\n", errormsg[19]);
            exit(0);
        }

        token_num++;

        // (
        if (tokenList[token_num].type == 15)
        {
            token_num++;
            expression(tokenList, lexLevel);
            emit(2, "OPR", 0, 0);

            // )
            if (tokenList[token_num].type != 16)
            {
                printf("%s\n", errormsg[14]);
                exit(0);
            }

            token_num++;
        }

        else
        {
            emit(1, "LIT", 0, 0);
            emit(2, "OPR", 0, 0);
        }

        return;
    }
    // if token is begin symbol
    if (tokenList[token_num].type == 21)
    {
        do
        {
            token_num++;

            statement(tokenList, lexLevel);
        } while (tokenList[token_num].type == 18); // while token is semi-colon

        if (tokenList[token_num].type != 22)
        {
            printf("%s\n", errormsg[10]);
            exit(0);
        }

        token_num++;

        return;
    }
    // if statement
    if (tokenList[token_num].type == 23)
    {
        token_num++;

        condition(tokenList, lexLevel);
        int jpcIndex = codeidx;
        emit(8, "JPC", 0, 0);
        // checks for then token
        if (tokenList[token_num].type != 24)
        {
            printf("%s\n", errormsg[11]);
            exit(0);
        }
        token_num++;

        statement(tokenList, lexLevel);

        if (tokenList[token_num].type == 33)
        {
            token_num++;
            int jmpIndx = codeidx;
            emit(7, "JMP", 0, 0);
            code[jpcIndex].m = codeidx;
            statement(tokenList, lexLevel);
            code[jmpIndx].m = codeidx;
        }

        else
            code[jpcIndex].m = codeidx;

        return;
    }

    // while sym
    if (tokenList[token_num].type == 25)
    {
        token_num++;

        int loopIdx = codeidx;
        condition(tokenList, lexLevel);

        // checks if token is do symbol
        if (tokenList[token_num].type != 26)
        {
            printf("%s\n", errormsg[12]);
            exit(0);
        }

        token_num++;

        int jpcIdx = codeidx;
        emit(8, "JPC", 0, 0);

        statement(tokenList, lexLevel);
        emit(7, "JMP", 0, loopIdx);
        code[jpcIdx].m = codeidx;
        return;
    }

    // if read symbol
    if (tokenList[token_num].type == 32)
    {
        token_num++;

        if (tokenList[token_num].type != 2)
        {
            printf("%s\n", errormsg[2]);
            exit(0);
        }

        int symIdx = symbolTableSearch(tokenList[token_num].name, lexLevel, 2);
        if (symIdx == -1)
        {
            printf("%s\n", errormsg[7]);
            exit(0);
        }

        token_num++;

        emit(9, "SYS", 0, 2);
        emit(4, "STO", lexLevel - symbol_table[symIdx].level, symbol_table[symIdx].addr);
        return;
    }

    // token is write symbol
    if (tokenList[token_num].type == 31)
    {
        token_num++;

        expression(tokenList, lexLevel);
        emit(9, "SYS", 0, 1);
        return;
    }
}

void condition(lexeme *tokenList, int lexLevel)
{
    // token is ODD
    if (tokenList[token_num].type == 8)
    {
        token_num++;
        expression(tokenList, lexLevel);
        emit(2, "OPR", 0, 6);
    }
    else
    {
        expression(tokenList, lexLevel);
        // token is eql symbol
        if (tokenList[token_num].type == 9)
        {
            token_num++;

            expression(tokenList, lexLevel);
            emit(2, "OPR", 0, 8);
        }
        // token is NEQ
        else if (tokenList[token_num].type == 10)
        {
            token_num++;

            expression(tokenList, lexLevel);
            emit(2, "OPR", 0, 9);
        }

        // token is lesssym
        else if (tokenList[token_num].type == 11)
        {
            token_num++;

            expression(tokenList, lexLevel);
            emit(2, "OPR", 0, 10);
        }
        // token is leqsym
        else if (tokenList[token_num].type == 12)
        {
            token_num++;

            expression(tokenList, lexLevel);
            emit(2, "OPR", 0, 11);
        }
        // token is gtrsym
        else if (tokenList[token_num].type == 13)
        {
            token_num++;

            expression(tokenList, lexLevel);
            emit(2, "OPR", 0, 12);
        }
        // token is geqsym
        else if (tokenList[token_num].type == 14)
        {
            token_num++;

            expression(tokenList, lexLevel);
            emit(2, "OPR", 0, 13);
        }
        else
        {
            printf("%s\n", errormsg[13]);
            exit(0);
        }
    }
}

void expression(lexeme *tokenList, int lexLevel)
{
    // minus symbol
    if (tokenList[token_num].type == 5)
    {
        token_num++;

        term(tokenList, lexLevel);
        emit(2, "OPR", 0, 1);

        while (tokenList[token_num].type == 4 || tokenList[token_num].type == 5)
        {
            if (tokenList[token_num].type == 4)
            {
                token_num++;

                term(tokenList, lexLevel);
                emit(2, "OPR", 0, 2);
            }
            else
            {
                token_num++;

                term(tokenList, lexLevel);
                emit(2, "OPR", 0, 3);
            }
        }
    }

    else
    {
        // if token is '+'
        if (tokenList[token_num].type == 4)
        {
            token_num++;
        }

        term(tokenList, lexLevel);
        while (tokenList[token_num].type == 4 || tokenList[token_num].type == 5)
        {
            if (tokenList[token_num].type == 4)
            {
                token_num++;

                term(tokenList, lexLevel);
                emit(2, "OPR", 0, 2);
            }
            else
            {
                token_num++;

                term(tokenList, lexLevel);
                emit(2, "OPR", 0, 3);
            }
        }
    }
}

void term(lexeme *tokenList, int lexLevel)
{
    factor(tokenList, lexLevel);
    // if token is * or /  or %
    while (tokenList[token_num].type == 6 || tokenList[token_num].type == 7 || tokenList[token_num].type == 1)
    {

        if (tokenList[token_num].type == 6)
        {
            token_num++;

            factor(tokenList, lexLevel);
            emit(2, "OPR", 0, 4);
        }
        else if (tokenList[token_num].type == 7)
        {
            token_num++;

            factor(tokenList, lexLevel);
            emit(2, "OPR", 0, 5);
        }

        else
        {
            token_num++;

            factor(tokenList, lexLevel);
            emit(2, "OPR", 0, 7);
        }
    }
}

void factor(lexeme *tokenList, int lexLevel)
{
    if (tokenList[token_num].type == 2)
    {
        int symIdxV = symbolTableSearch(tokenList[token_num].name, lexLevel, 2);
        int symIdxC = symbolTableSearch(tokenList[token_num].name, lexLevel, 1);

        if (symIdxV == -1 && symIdxC == -1)
        {
            printf("%s\n", errormsg[7]);
            exit(0);
        }

        else if (symIdxC == -1 || (symIdxV != -1 && symbol_table[symIdxV].level > symbol_table[symIdxC].level))
            emit(3, "LOD", lexLevel - symbol_table[symIdxV].level, symbol_table[symIdxV].addr);

        else
            emit(1, "LIT", 0, symbol_table[symIdxC].val);

        token_num++;
    }

    // if token is a numbersym
    else if (tokenList[token_num].type == 3)
    {
        emit(1, "LIT", 0, tokenList[token_num].value);
        token_num++;
    }

    // if token is left '('
    else if (tokenList[token_num].type == 15)
    {
        token_num++;
        expression(tokenList, lexLevel);
        //checks if token is ')'
        if (tokenList[token_num].type != 16)
        {
            printf("%s\n", errormsg[14]);
            exit(0);
        }
        token_num++;
    }

    else if (tokenList[token_num].type == 27)
        statement(tokenList, lexLevel);

    else
    {
        printf("%s\n", errormsg[15]);
        exit(0);
    }
}

void emit(int opcode, char op[], int l, int m)
{
    code[codeidx].opcode = opcode;
    strcpy(code[codeidx].op, op);
    code[codeidx].l = l;
    code[codeidx].m = m;
    codeidx++;
}
