#include "areval.h"
#include <math.h>
#include <stdio.h>

char *LEX_NameOfType(LEX_TokenType type) {

  switch (type) {
  case PLUS:
    return "PLUS";
    break;
  case MINUS:
    return "MINUS";
    break;
  case MULT:
    return "MULT";
    break;
  case DIV:
    return "DIV";
    break;
  case LPAREN:
    return "LPAREN";
    break;
  case RPAREN:
    return "RPAREN";
    break;
  case NUMBER:
    return "NUMBER";
    break;
  case NOTYPE:
    return "NOTYPE";
  default:
    break;
  }
  return "";
}

char *LEX_ReadFile(int *length, char *filename) {
  // printf("jhjh");
  char *buffer;
  FILE *f = fopen(filename, "rb");
  if (f) {
    fseek(f, 0, SEEK_END);
    *length = ftell(f);
    fseek(f, 0, SEEK_SET);

    buffer = (char *)malloc(*length + 1);
    buffer[*length++] = ' ';
    if (buffer) {
      unsigned long res = fread(buffer, 1, *length, f);
      (void)res;
    }
    fclose(f);
  } else {
    printf("Unable to read input file! %s\n", filename);
    return "";
  }
  return buffer;
}

LEX_TokenType LEX_TypeOfChar(char c) {
  switch (c) {
  case '+':
    return PLUS;
    break;
  case '-':
    return MINUS;
    break;
  case '*':
    return MULT;
    break;
  case '/':
    return DIV;
    break;
  case '(':
    return LPAREN;
    break;
  case ')':
    return RPAREN;
    break;
  default:
    return NOTYPE;
    break;
  }
}

bool LEX_IsOperator(LEX_TokenType type) {
  return (type == PLUS || type == MINUS || type == MULT || type == DIV);
}

uint8_t LEX_Precedence(LEX_TokenType type) {
  if (type == PLUS || type == MINUS) {
    return 12;
  } else if (type == MULT || type == DIV) {
    return 13;
  } else if (type == LPAREN || type == RPAREN) {
    return 15;
  }
  return 0;
}

res LEX_GetNum(char *buffer, int length) {
  int i = 0;
  int res = 0;
  while (i < length) {
    if (buffer[i] < '0' || buffer[i] > '9') {
      printf("ERROR: Trying to parse non-numeric character as number:  %c!\n",
             buffer[i]);
      return 0;
    }
    res += (int)(buffer[i] - 0x30) * powf((float)10, (float)length - i - 1);
    ++i;
  }
  return res;
}

int mod(int a, int b) { return ((a % b) + b) % b; }

LEX_Token LEX_Peek(LEX_Prog *stack) {
  if (stack->length <= 0) {
    printf("Can't Peek empty stack !\n");
    return (LEX_Token){.type = NOTYPE, .num = 0};
  }
  return stack->data[mod(stack->length - 1, MAX_PROG_SIZE)];
}

LEX_Token LEX_Pop(LEX_Prog *stack) {
  if (stack->length <= 0) {
    printf("Can't Pop empty stack !\n");
    return (LEX_Token){.type = NOTYPE, .num = 0};
  }
  stack->length--;
  return stack->data[mod(stack->length, MAX_PROG_SIZE)];
}

void LEX_Push(LEX_Prog *stack, LEX_Token token) {
  if (stack->length < MAX_STACK_SIZE)
    stack->data[mod(stack->length, MAX_PROG_SIZE)] = token;
  stack->length++;
}

bool LEX_IsStrNum(char *str, int length) {
  for (int i = 0; i < length; ++i) {
    if (str[i] > '9' || str[i] < '0') {
      return false;
    }
  }
  return true;
}

LEX_TokenType LEX_GetOpType(char *buffer, int length) {
  if (length == 1) {
    return LEX_TypeOfChar(*buffer);
  }
  return NOTYPE;
}

bool is_char_num(char c) { return '0' <= c && c <= '9'; }

LEX_Prog LEX_Parse(char *buffer, int length) {
  LEX_Prog prog;
  prog.length = 0;
  char acc[32];
  for (int j = 0; j < 32; j++) {
    acc[j] = 0;
  }
  int acc_i = 0;
  for (int i = 0; i < length; ++i) {
    char toParse = buffer[i];
    if (acc_i > 0) {
      if (LEX_IsStrNum(&acc[acc_i - 1], 1)) {
        if (is_char_num(toParse)) {
          acc[acc_i] = toParse;
        } else if (toParse == ' ' || toParse == '\n') {
          int num = LEX_GetNum(acc, acc_i);
          LEX_Token tok = (LEX_Token){.type = NUMBER, .num = num};
          LEX_Push(&prog, tok);
          acc_i = 0;
          for (int j = 0; j < 32; j++) {
            acc[j] = 0;
          }
        } else if (toParse == '+') {
          int num = LEX_GetNum(acc, acc_i);
          LEX_Token tok1 = (LEX_Token){.type = NUMBER, .num = num};
          LEX_Push(&prog, tok1);
          acc_i = 0;
          for (int j = 0; j < 32; j++) {
            acc[j] = 0;
          }
          LEX_Token tok2 = (LEX_Token){.type = PLUS, .num = 0};
          LEX_Push(&prog, tok2);
        } else if (toParse == '-') {
          int num = LEX_GetNum(acc, acc_i);
          LEX_Token tok1 = (LEX_Token){.type = NUMBER, .num = num};
          LEX_Push(&prog, tok1);
          acc_i = 0;
          for (int j = 0; j < 32; j++) {
            acc[j] = 0;
          }
          LEX_Token tok2 = (LEX_Token){.type = MINUS, .num = 0};
          LEX_Push(&prog, tok2);
        } else if (toParse == '*') {
          int num = LEX_GetNum(acc, acc_i);
          LEX_Token tok1 = (LEX_Token){.type = NUMBER, .num = num};
          LEX_Push(&prog, tok1);
          acc_i = 0;
          for (int j = 0; j < 32; j++) {
            acc[j] = 0;
          }
          LEX_Token tok2 = (LEX_Token){.type = MULT, .num = 0};
          LEX_Push(&prog, tok2);
        } else if (toParse == '/') {
          int num = LEX_GetNum(acc, acc_i);
          LEX_Token tok1 = (LEX_Token){.type = NUMBER, .num = num};
          LEX_Push(&prog, tok1);
          acc_i = 0;
          for (int j = 0; j < 32; j++) {
            acc[j] = 0;
          }
          LEX_Token tok2 = (LEX_Token){.type = DIV, .num = 0};
          LEX_Push(&prog, tok2);
        } else if (toParse == '(') {
          int num = LEX_GetNum(acc, acc_i);
          LEX_Token tok1 = (LEX_Token){.type = NUMBER, .num = num};
          LEX_Push(&prog, tok1);
          acc_i = 0;
          for (int j = 0; j < 32; j++) {
            acc[j] = 0;
          }
          LEX_Token tok2 = (LEX_Token){.type = LPAREN, .num = 0};
          LEX_Push(&prog, tok2);
        } else if (toParse == ')') {
          int num = LEX_GetNum(acc, acc_i);
          LEX_Token tok1 = (LEX_Token){.type = NUMBER, .num = num};
          LEX_Push(&prog, tok1);
          acc_i = 0;
          for (int j = 0; j < 32; j++) {
            acc[j] = 0;
          }
          LEX_Token tok2 = (LEX_Token){.type = RPAREN, .num = 0};
          LEX_Push(&prog, tok2);
        }
      }

    } else {
      if (toParse == '+') {
        LEX_Token tok = (LEX_Token){.type = PLUS, .num = 0};
        LEX_Push(&prog, tok);
      } else if (toParse == '-') {
        LEX_Token tok = (LEX_Token){.type = MINUS, .num = 0};
        LEX_Push(&prog, tok);
      } else if (toParse == '*') {
        LEX_Token tok = (LEX_Token){.type = MULT, .num = 0};
        LEX_Push(&prog, tok);
      } else if (toParse == '/') {
        LEX_Token tok = (LEX_Token){.type = DIV, .num = 0};
        LEX_Push(&prog, tok);
      } else if (toParse == '(') {
        LEX_Token tok = (LEX_Token){.type = LPAREN, .num = 0};
        LEX_Push(&prog, tok);
      } else if (toParse == ')') {
        LEX_Token tok = (LEX_Token){.type = RPAREN, .num = 0};
        LEX_Push(&prog, tok);
      } else if (LEX_IsStrNum(&toParse, 1)) {
        acc[acc_i++] = toParse;
      }
    }
  }
  char toParse = ' ';
  if (acc_i > 0) {
    if (LEX_IsStrNum(&acc[acc_i - 1], 1)) {
      if (is_char_num(toParse)) {
        acc[acc_i] = toParse;
      } else if (toParse == ' ' || toParse == '\n') {
        int num = LEX_GetNum(acc, acc_i);
        LEX_Token tok = (LEX_Token){.type = NUMBER, .num = num};
        LEX_Push(&prog, tok);
        acc_i = 0;
        for (int j = 0; j < 32; j++) {
          acc[j] = 0;
        }
      } else if (toParse == '+') {
        int num = LEX_GetNum(acc, acc_i);
        LEX_Token tok1 = (LEX_Token){.type = NUMBER, .num = num};
        LEX_Push(&prog, tok1);
        acc_i = 0;
        for (int j = 0; j < 32; j++) {
          acc[j] = 0;
        }
        LEX_Token tok2 = (LEX_Token){.type = PLUS, .num = 0};
        LEX_Push(&prog, tok2);
      } else if (toParse == '-') {
        int num = LEX_GetNum(acc, acc_i);
        LEX_Token tok1 = (LEX_Token){.type = NUMBER, .num = num};
        LEX_Push(&prog, tok1);
        acc_i = 0;
        for (int j = 0; j < 32; j++) {
          acc[j] = 0;
        }
        LEX_Token tok2 = (LEX_Token){.type = MINUS, .num = 0};
        LEX_Push(&prog, tok2);
      } else if (toParse == '*') {
        int num = LEX_GetNum(acc, acc_i);
        LEX_Token tok1 = (LEX_Token){.type = NUMBER, .num = num};
        LEX_Push(&prog, tok1);
        acc_i = 0;
        for (int j = 0; j < 32; j++) {
          acc[j] = 0;
        }
        LEX_Token tok2 = (LEX_Token){.type = MULT, .num = 0};
        LEX_Push(&prog, tok2);
      } else if (toParse == '/') {
        int num = LEX_GetNum(acc, acc_i);
        LEX_Token tok1 = (LEX_Token){.type = NUMBER, .num = num};
        LEX_Push(&prog, tok1);
        acc_i = 0;
        for (int j = 0; j < 32; j++) {
          acc[j] = 0;
        }
        LEX_Token tok2 = (LEX_Token){.type = DIV, .num = 0};
        LEX_Push(&prog, tok2);
      } else if (toParse == '(') {
        int num = LEX_GetNum(acc, acc_i);
        LEX_Token tok1 = (LEX_Token){.type = NUMBER, .num = num};
        LEX_Push(&prog, tok1);
        acc_i = 0;
        for (int j = 0; j < 32; j++) {
          acc[j] = 0;
        }
        LEX_Token tok2 = (LEX_Token){.type = LPAREN, .num = 0};
        LEX_Push(&prog, tok2);
      } else if (toParse == ')') {
        int num = LEX_GetNum(acc, acc_i);
        LEX_Token tok1 = (LEX_Token){.type = NUMBER, .num = num};
        LEX_Push(&prog, tok1);
        acc_i = 0;
        for (int j = 0; j < 32; j++) {
          acc[j] = 0;
        }
        LEX_Token tok2 = (LEX_Token){.type = RPAREN, .num = 0};
        LEX_Push(&prog, tok2);
      }
    }

  } else {
    if (toParse == '+') {
      LEX_Token tok = (LEX_Token){.type = PLUS, .num = 0};
      LEX_Push(&prog, tok);
    } else if (toParse == '-') {
      LEX_Token tok = (LEX_Token){.type = MINUS, .num = 0};
      LEX_Push(&prog, tok);
    } else if (toParse == '*') {
      LEX_Token tok = (LEX_Token){.type = MULT, .num = 0};
      LEX_Push(&prog, tok);
    } else if (toParse == '/') {
      LEX_Token tok = (LEX_Token){.type = DIV, .num = 0};
      LEX_Push(&prog, tok);
    } else if (toParse == '(') {
      LEX_Token tok = (LEX_Token){.type = LPAREN, .num = 0};
      LEX_Push(&prog, tok);
    } else if (toParse == ')') {
      LEX_Token tok = (LEX_Token){.type = RPAREN, .num = 0};
      LEX_Push(&prog, tok);
    } else if (LEX_IsStrNum(&toParse, 1)) {
      acc[acc_i++] = toParse;
    }
  }
  return prog;
}

LEX_Prog LEX_InfixToRPN(LEX_Prog src) {
  LEX_Prog prog;
  prog.length = 0;
  LEX_Prog op_stack;
  op_stack.length = 0;
  for (int i = 0; i < src.length; i++) {
    LEX_Token token = src.data[i];
    if (LEX_IsOperator(token.type) && token.type != LPAREN &&
        token.type != RPAREN) {
      while (op_stack.length > 0) {
        LEX_Token peeked = LEX_Peek(&op_stack);
        if (LEX_Precedence(peeked.type) >= LEX_Precedence(token.type) &&
            peeked.type != LPAREN) {
          LEX_Token popped = LEX_Pop(&op_stack);
          LEX_Push(&prog, popped);
        } else {
          break;
        }
      }
      LEX_Push(&op_stack, token);
    } else if (token.type == LPAREN) {
      LEX_Push(&op_stack, token);
    } else if (token.type == RPAREN) {
      while (op_stack.length > 0) {
        LEX_Token peeked = LEX_Peek(&op_stack);
        if (peeked.type == LPAREN) {
          break;
        } else {
          LEX_Token popped = LEX_Pop(&op_stack);
          LEX_Push(&prog, popped);
        }
      }
      if (op_stack.length > 0) {
        (void)LEX_Pop(&op_stack);
      }
    } else {
      LEX_Push(&prog, token);
    }
  }
  while (op_stack.length > 0) {
    LEX_Token popped = LEX_Pop(&op_stack);
    LEX_Push(&prog, popped);
  }
  return prog;
}

res LEX_RPNToRes(LEX_Prog rpn) {
  LEX_Prog stack;

  stack.length = 0;
  for (int i = 0; i < rpn.length; ++i) {
    if (!LEX_IsOperator(rpn.data[i].type)) {
      LEX_Push(&stack, rpn.data[i]);
    } else {
      LEX_Token b = LEX_Pop(&stack);
      LEX_Token a = LEX_Pop(&stack);
      LEX_Token c;
      c.type = NUMBER;
      LEX_TokenType type = rpn.data[i].type;
      if (type == PLUS)
        c.num = a.num + b.num;
      else if (type == MINUS)
        c.num = a.num - b.num;
      else if (type == MULT)
        c.num = a.num * b.num;
      else if (type == DIV)
        c.num = a.num / b.num;
      LEX_Push(&stack, c);
    }
  }
  if (stack.length > 1) {
    printf("ERROR: Unhandled data on the stack.");
    return -1;
  }
  if (stack.length == 0) {
    printf("Couldn't evaluate program");
    return -1;
  } else {
    LEX_Token toRet = LEX_Pop(&stack);
    return toRet.num;
  }

  return 0;
}

res LEX_EvaluateProg(char *filename) {
  int length;
  char *buffer = LEX_ReadFile(&length, filename);
  LEX_Prog infix = LEX_Parse(buffer, length);
  LEX_Prog rpn = LEX_InfixToRPN(infix);
  res res = LEX_RPNToRes(rpn);
  return res;
}