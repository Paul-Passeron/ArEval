#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_NUM_SIZE 100
#define MAX_STACK_SIZE 100
#define MAX_PROG_SIZE 100

typedef enum { DEFAULT = 0, NUM, OP, FLAG_COUNT } Lex_Flag;

typedef enum {
  PLUS = 0,
  MINUS,
  MULT,
  DIV,
  NUMBER,
  LPAREN,
  RPAREN,
  NOTYPE,
  TYPE_COUNT
} LEX_TokenType;

typedef struct {
  LEX_TokenType type;
  int num;
} LEX_Token;

typedef struct {
  int length;
  LEX_Token data[MAX_PROG_SIZE];
} LEX_Prog;

typedef struct {
  uint8_t flag;
  uint8_t i;
  int l;

} LEX_State;

typedef int res;

char *LEX_ReadFile(int *length, char *filename);

char *LEX_NameOfType(LEX_TokenType type);

LEX_TokenType LEX_TypeOfChar(char c);

uint8_t getPrecedence(LEX_TokenType type);

LEX_Token LEX_Peek(LEX_Prog *stack);

LEX_Token LEX_Pop(LEX_Prog *stack);

void LEX_Push(LEX_Prog *stack, LEX_Token token);

LEX_Prog LEX_Parse(char *buffer, int length);

LEX_Prog LEX_InfixToRPN(LEX_Prog src);

res LEX_RPNToRes(LEX_Prog rpn); // MAybe call it execute or interprete

res LEX_GetNum(char *buffer, int length);
