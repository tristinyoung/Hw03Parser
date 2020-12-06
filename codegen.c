#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lex.h"
#include "parser.h"
#include "codegen.h"

#define TOKEN list[next].tokType
#define CODE_SIZE 500

int next = 0;
int cx = 0;
symbol *table;
lexeme *list;
instruction *code;

void printCode(void);
int lookup(char *str);
void program1(void);
void block1(int lexL);
void statement1(int lexL);
void condition1(int lexL);
void expression1(int regToEndUpIn, int lexL);
void term1(int regToEndUpIn, int lexL);
void factor1(int regToEndUpIn, int lexL);
void emit(int op, int r, int l, int m);

void printCode(void)
{
	int i;
	printf("\n");
	for (i = 1; i < cx; i++)
	{
		printf("%d %d %d %d %d\n", i, code[i].op, code[i].r, code[i].l, code[i].m);
	}
}

int lookup(char *str)
{
	int tx;

	for (tx = 0; tx <= 500; tx++)
	{
		if (strcmp(str, table[tx].name) == 0)
			return tx;
	}

	return (tx = -1);
}

void program1(void)
{
	code = emit(code, 7, 0, 0, 1);
	code = block1(table, list, code);
	code = emit(code, 9, 0, 0, 3);
	return code;
}

void block1(int lexL)
{
	int numVars = 0;
	if (TOKEN == constsym)
	{
		do
		{
			lexL1 += 4;
		} while (TOKEN == commasym);
		lexL1++;
	}
	if (TOKEN == varsym)
	{
		do
		{
			numVars++;
			lexL1 += 2;
		} while (TOKEN == commasym);
		lexL1++;
	}
	code = emit(code, 6, 0, 0, 3 + numVars);
	code = statement1(table, list, code);
	return code;
}

void statement1(int lexL)
{
	int tx, temp, temp2;
	if (TOKEN == identsym)
	{
		tx = lookup(list[lexL1].lex, table);
		//code[lexL1].op = identsym;
		lexL1 += 2;
		code = expression1(0, table, list, code);
		code = emit(code, 4, 0, 0, table[tx].val);
	}
	if (TOKEN == beginsym)
	{
		lexL1++;
		code = statement1(table, list, code);
		while (TOKEN == semicolonsym)
		{
			lexL1++;
			code = statement1(table, list, code);
		}
		lexL1++;
	}
	if (TOKEN == ifsym)
	{
		lexL1++;
		code = condition1(table, list, code);
		temp = cx;
		code = emit(code, 8, 0, 0, 0);
		lexL1++;
		code = statement1(table, list, code);
		code[temp].m = cx;
	}
	if (TOKEN == whilesym)
	{
		lexL1++;
		temp = cx;
		code = condition1(table, list, code);
		lexL1++;
		temp2 = cx;
		code = emit(code, 8, 0, 0, 0);
		code = statement1(table, list, code);
		code = emit(code, 7, 0, 0, temp);
		code[temp2].m = cx;
	}
	if (TOKEN == readsym)
	{
		lexL1++;
		tx = lookup(list[lexL1].lex, table);
		lexL1++;
		code = emit(code, 9, 0, 0, 2);
		code = emit(code, 4, 0, 0, table[tx].addr);
	}
	if (TOKEN == writesym)
	{
		lexL1++;
		tx = lookup(list[lexL1].lex, table);
		if (TOKEN == varsym)
		{
			code = emit(code, 3, 0, 0, table[tx].addr);
			code = emit(code, 9, 0, 0, 1);
		}
		if (TOKEN == constsym)
		{
			code = emit(code, 1, 0, 0, table[tx].val);
			code = emit(code, 9, 0, 0, 1);
		}
		lexL1++;
	}
	return code;
}

void condition1(int lexL)
{
	if (TOKEN == oddsym)
	{
		lexL1++;
		code = expression1(0, table, list, code);
		code = emit(code, 15, 0, 0, 0);
	}
	else
	{
		code = expression1(0, table, list, code);
		if (TOKEN == eqsym)
		{
			lexL1++;
			code = expression1(1, table, list, code);
			code = emit(code, 17, 0, 0, 1);
		}
		if (TOKEN == neqsym)
		{
			lexL1++;
			code = expression1(1, table, list, code);
			code = emit(code, 18, 0, 0, 1);
		}
		if (TOKEN == lessym)
		{
			lexL1++;
			code = expression1(1, table, list, code);
			code = emit(code, 19, 0, 0, 1);
		}
		if (TOKEN == leqsym)
		{
			lexL1++;
			code = expression1(1, table, list, code);
			code = emit(code, 20, 0, 0, 1);
		}
		if (TOKEN == gtrsym)
		{
			lexL1++;
			code = expression1(1, table, list, code);
			code = emit(code, 21, 0, 0, 1);
		}
		if (TOKEN == geqsym)
		{
			lexL1++;
			code = expression1(1, table, list, code);
			code = emit(code, 22, 0, 0, 1);
		}
	}
	return code;
}

void expression1(int regToEndUpIn, int lexL)
{
	if (TOKEN == plussym)
	{
		lexL1++;
	}
	if (TOKEN == minussym)
	{
		lexL1++;
		code = term1(regToEndUpIn, table, list, code);
		code = emit(code, 10, regToEndUpIn, 0, 0);
		while (TOKEN == plussym || TOKEN == minussym)
		{
			if (TOKEN == plussym)
			{
				lexL1++;
				code = term1(regToEndUpIn+1, table, list, code);
				code = emit(code, 11, regToEndUpIn, regToEndUpIn, regToEndUpIn+1);
			}
			if (TOKEN == minussym)
			{
				lexL1++;
				code = term1(regToEndUpIn+1, table, list, code);
				code = emit(code, 12, regToEndUpIn, regToEndUpIn, regToEndUpIn+1);
			}
		}
		return code;
	}
	code = term1(regToEndUpIn, table, list, code);
	while (TOKEN == plussym || TOKEN == minussym)
	{
		if (TOKEN == plussym)
		{
			lexL1++;
			code = term1(regToEndUpIn+1, table, list, code);
			code = emit(code, 11, regToEndUpIn, regToEndUpIn, regToEndUpIn+1);
		}
		if (TOKEN == minussym)
		{
			lexL1++;
			code = term1(regToEndUpIn+1, table, list, code);
			code = emit(code, 12, regToEndUpIn, regToEndUpIn, regToEndUpIn+1);
		}
	}
	return code;
}

void term1(int regToEndUpIn, int lexL)
{
	code = factor1(regToEndUpIn, table, list, code);
	while (TOKEN == multsym || TOKEN == slashsym)
	{
		if (TOKEN == multsym)
		{
			lexL1++;
			code = factor1(regToEndUpIn + 1, table, list, code);
			code = emit(code, 13, regToEndUpIn, regToEndUpIn, regToEndUpIn + 1);
		}
		if (TOKEN == slashsym)
		{
			lexL1++;
			code = factor1(regToEndUpIn + 1, table, list, code);
			code = emit(code, 14, regToEndUpIn, regToEndUpIn, regToEndUpIn + 1);
		}
	}
	return code;
}

void factor1(int regToEndUpIn, int lexL)
{
	int tx;
	if (TOKEN == identsym)
	{
		tx = lookup(list[lexL1].lex, table);
		if (TOKEN == constsym)
		{
			code = emit(code, 1, regToEndUpIn, 0, table[tx].val);
		}
		if (TOKEN == varsym)
		{
			code = emit(code, 3, regToEndUpIn, 0, table[tx].addr);
		}
		lexL1++;
	}
	else if (TOKEN == numbersym)
	{
		code = emit(code, 1, regToEndUpIn, 0, atoi(list[lexL1].lex));
		lexL1++;
	}
	else
	{
		lexL1++;
		code = expression1(regToEndUpIn, table, list, code);
		lexL1++;
	}
	return code;
}

void emit(int op, int r, int l, int m)
{
	if (cx > CODE_SIZE)
		printf("error\n");
	else
	{
		code[cx].op = op;
		code[cx].r = r;
		code[cx].l = l;
		code[cx].m = m;
		cx++;
	}
	return code;
}

instruction *generate_code(symbol *t, lexeme *l)
{
	instruction *c = malloc(500 * sizeof(instruction));
	code = c;
	table = t;
	list = l;
	program1();
	return code;
}
