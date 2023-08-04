#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_NUM_SIZE 100
#define MAX_STACK_SIZE 100
#define MAX_PROG_SIZE 100

#define LEX_FLAG_DEFAULT 0
#define LEX_FLAG_NUM 1
#define LEX_FLAG_OP 2

typedef uint8_t bool;
#define false 0;
#define true 1;

typedef enum {
  PLUS,
  MINUS,
  MULT,
  DIV,
  NUMBER,
  LPAREN,
  RPAREN,
  NOTYPE,
} LEX_TokenType;

typedef struct {
  LEX_TokenType type;
  int num;
} LEX_Token;

char *readFile(int *length, char *filename) {
  char *buffer;
  FILE *f = fopen(filename, "rb");
  if (f) {
    fseek(f, 0, SEEK_END);
    *length = ftell(f);
    fseek(f, 0, SEEK_SET);

    buffer = (char *)malloc(*length + 1);
    buffer[*length] = '\n';
    *length = *length + 1;
    if (buffer) {
      fread(buffer, 1, *length, f);
    }
    fclose(f);
  } else {
    printf("Unable to read input file! %s\n", filename);
    return "";
  }

  return buffer;
}

void printtype(LEX_Token tok) {
  LEX_TokenType type = tok.type;
  if (type == PLUS) {
    printf("PLUS\n");
    return;
  }
  if (type == MINUS) {
    printf("MINUS\n");
    return;
  }
  if (type == MULT) {
    printf("MULT\n");
    return;
  }
  if (type == DIV) {
    printf("DIV\n");
    return;
  }
  if (type == LPAREN) {
    printf("LPAREN\n");
    return;
  }
  if (type == RPAREN) {
    printf("RPAREN\n");
  }
  if (type == NUMBER) {
    printf("NUMBER: %d\n", tok.num);
    return;
  }
}

void fprinttype(LEX_Token tok, FILE *f) {
  LEX_TokenType type = tok.type;
  if (type == PLUS) {
    fprintf(f, "PLUS\n");
    return;
  }
  if (type == MINUS) {
    fprintf(f, "MINUS\n");
    return;
  }
  if (type == MULT) {
    fprintf(f, "MULT\n");
    return;
  }
  if (type == DIV) {
    fprintf(f, "DIV\n");
    return;
  }
  if (type == LPAREN) {
    fprintf(f, "LPAREN\n");
    return;
  }
  if (type == RPAREN) {
    fprintf(f, "RPAREN\n");
  }
  if (type == NUMBER) {
    fprintf(f, "NUMBER: %d\n", tok.num);
    return;
  }
}

void printProg(LEX_Token *toks, int length) {
  int8_t i;
  for (i = 0; i < length && i < MAX_PROG_SIZE; i++) {
    printtype(toks[i]);
  }
}

void fprintProg(LEX_Token *toks, int length, FILE *f) {
  int8_t i;
  for (i = 0; i < length && i < MAX_PROG_SIZE; i++) {
    fprinttype(toks[i], f);
  }
}

uint64_t parseNum(char *num, uint8_t length) {
  uint64_t res = 0;
  for (uint8_t i = 0; i < length; ++i) {
    uint64_t digit = (uint64_t)num[i] - 0x30;

    int puiss = (uint64_t)powl(10, length - i - 1);
    res = res + (digit * puiss);
  }

  return res;
}

bool isOperator(LEX_Token tok) {
  LEX_TokenType type = tok.type;
  if (type == NUMBER || type == LPAREN || type == RPAREN || type == NOTYPE) {
    return false;
  }
  return true;
}

typedef struct {
  int length;
  LEX_Token toks[MAX_STACK_SIZE];
} LEX_TokenStack;

bool LEX_Push(LEX_TokenStack *stack, LEX_Token tok) {
  if (stack->length == MAX_STACK_SIZE) {
    return false;
  }
  stack->toks[stack->length] = tok;
  stack->length++;
  return true;
}

LEX_Token LEX_Pop(LEX_TokenStack *stack) {
  if (stack->length == 0) {
    // Throw some sort of error maybe ? Or return an invalid token
    return (LEX_Token){.type = NOTYPE, .num = -1};
  }
  stack->length--;
  return stack->toks[stack->length];
}

LEX_Token LEX_Peek(LEX_TokenStack *stack) {
  if (stack->length == 0) {
    // Throw some sort of error maybe ? Or return an invalid token
    return (LEX_Token){.type = NOTYPE, .num = -1};
  }
  return stack->toks[stack->length - 1];
}

int getPrecedence(LEX_Token tok) {

  if (tok.type == PLUS || tok.type == MINUS) {
    return 12;
  } else if (tok.type == MULT || tok.type == DIV) {
    return 13;
  } else if (tok.type == LPAREN || tok.type == RPAREN) {
    return 15;
  }
  return 0;
}

#define prec getPrecedence

void infixToRPN(LEX_Token *dst, LEX_Token *toks, int *length, int or_length) {
  int i = 0;
  int j = 0;
  LEX_TokenStack stack;
  stack.length = 0;
  while (i < or_length) {

    if (toks[i].type == NOTYPE) {
      break;
    } else if (toks[i].type == NUMBER) {
      dst[j] = toks[i];
      j++;
    } else if (toks[i].type == LPAREN) {
      LEX_Push(&stack, toks[i]);
    } else if (toks[i].type == RPAREN) {
      while (LEX_Peek(&stack).type != LPAREN && stack.length > 0) {
        dst[j] = LEX_Pop(&stack);
        j++;
      }
      LEX_Pop(&stack);
    } else if (isOperator(toks[i])) {
      if (LEX_Peek(&stack).type == LPAREN) {
        LEX_Push(&stack, toks[i]);

      } else {
        while (prec(LEX_Peek(&stack)) > prec(toks[i]) && stack.length > 0) {
          dst[j] = LEX_Pop(&stack);
          j++;
        }
        LEX_Push(&stack, toks[i]);
      }
    }
    ++i;
  }
  while (stack.length > 0) {
    dst[j] = LEX_Pop(&stack);
    j++;
  }
  *length = j;
}

int RPNToRes(LEX_Token *toks, int length) {

  LEX_TokenStack stack;
  stack.length = 0;
  for (int i = 0; i < length; i++) {
    LEX_Token tok = toks[i];
    if (!isOperator(tok)) {
      LEX_Push(&stack, tok);
    } else if (tok.type == PLUS) {
      LEX_Token toka = LEX_Pop(&stack);
      LEX_Token tokb = LEX_Pop(&stack);
      int n = toka.num + tokb.num;
      LEX_Token tokc = (LEX_Token){.type = NUMBER, .num = toka.num + tokb.num};
      LEX_Push(&stack, tokc);
    } else if (tok.type == MINUS) {
      LEX_Token toka = LEX_Pop(&stack);
      LEX_Token tokb = LEX_Pop(&stack);
      int n = toka.num + tokb.num;
      LEX_Token tokc = (LEX_Token){.type = NUMBER, .num = toka.num - tokb.num};
      LEX_Push(&stack, tokc);
    } else if (tok.type == DIV) {
      LEX_Token tokb = LEX_Pop(&stack);
      LEX_Token toka = LEX_Pop(&stack);
      int n = toka.num + tokb.num;
      LEX_Token tokc = (LEX_Token){.type = NUMBER, .num = toka.num / tokb.num};
      LEX_Push(&stack, tokc);
    } else if (tok.type == MULT) {
      LEX_Token toka = LEX_Pop(&stack);
      LEX_Token tokb = LEX_Pop(&stack);
      int n = toka.num + tokb.num;
      LEX_Token tokc = (LEX_Token){.type = NUMBER, .num = toka.num * tokb.num};
      LEX_Push(&stack, tokc);
    }
  }
  return LEX_Pop(&stack).num;
}

void parseBuffer(char *buffer, LEX_Token *tokens, int *length) {

  char nume[MAX_NUM_SIZE];
  uint8_t i = 0;
  uint8_t flag = LEX_FLAG_DEFAULT;

  // LEX_Token tokens[MAX_PROG_SIZE];
  LEX_Token temp_tok;
  while (*buffer) {
    switch (flag) {
    case LEX_FLAG_NUM:
      if ('0' <= *buffer && *buffer <= '9') {
        nume[i] = *buffer;
        i++;
      } else {
        int number = parseNum(nume, i);
        temp_tok.num = number;
        temp_tok.type = NUMBER;
        tokens[*length] = temp_tok;
        *length = *length + 1;
        flag = LEX_FLAG_DEFAULT;
        if (*buffer == '+') {
          flag = LEX_FLAG_OP;
          temp_tok.num = -1;
          temp_tok.type = PLUS;
          tokens[*length] = temp_tok;
          *length = *length + 1;
        } else if (*buffer == '-') {
          flag = LEX_FLAG_OP;
          temp_tok.num = -1;
          temp_tok.type = MINUS;
          tokens[*length] = temp_tok;
          *length = *length + 1;
        } else if (*buffer == '*') {
          flag = LEX_FLAG_OP;
          temp_tok.num = -1;
          temp_tok.type = MULT;
          tokens[*length] = temp_tok;
          *length = *length + 1;
        } else if (*buffer == '/') {
          flag = LEX_FLAG_OP;
          temp_tok.num = -1;
          temp_tok.type = DIV;
          tokens[*length] = temp_tok;
          *length = *length + 1;
        } else if (*buffer == '(') {
          flag = LEX_FLAG_OP;
          temp_tok.num = -1;
          temp_tok.type = LPAREN;
          tokens[*length] = temp_tok;
          *length = *length + 1;
        } else if (*buffer == ')') {
          flag = LEX_FLAG_OP;
          temp_tok.num = -1;
          temp_tok.type = RPAREN;
          tokens[*length] = temp_tok;
          *length = *length + 1;
        }
      }
      break;

    case LEX_FLAG_OP:
      i = 0;
      if (*buffer == ' ' || *buffer == '\n') {
        flag = LEX_FLAG_DEFAULT;
      } else if ('0' <= *buffer && *buffer <= '9') {
        flag = LEX_FLAG_NUM;
        nume[i] = *buffer;
        i++;
      } else {
        if (*buffer == '+') {
          flag = LEX_FLAG_OP;
          temp_tok.num = -1;
          temp_tok.type = PLUS;
          tokens[*length] = temp_tok;
          *length = *length + 1;
        } else if (*buffer == '-') {
          flag = LEX_FLAG_OP;
          temp_tok.num = -1;
          temp_tok.type = MINUS;
          tokens[*length] = temp_tok;
          *length = *length + 1;
        } else if (*buffer == '*') {
          flag = LEX_FLAG_OP;
          temp_tok.num = -1;
          temp_tok.type = MULT;
          tokens[*length] = temp_tok;
          *length = *length + 1;
        } else if (*buffer == '/') {
          flag = LEX_FLAG_OP;
          temp_tok.num = -1;
          temp_tok.type = DIV;
          tokens[*length] = temp_tok;
          *length = *length + 1;
        } else if (*buffer == '(') {
          flag = LEX_FLAG_OP;
          temp_tok.num = -1;
          temp_tok.type = LPAREN;
          tokens[*length] = temp_tok;
          *length = *length + 1;
        } else if (*buffer == ')') {
          flag = LEX_FLAG_OP;
          temp_tok.num = -1;
          temp_tok.type = RPAREN;
          tokens[*length] = temp_tok;
          *length = *length + 1;
        }
      }
      break;

    case LEX_FLAG_DEFAULT:
      i = 0;
      if ('0' <= *buffer && *buffer <= '9') {
        flag = LEX_FLAG_NUM;
        nume[i] = *buffer;
        i++;
      } else if (*buffer == '+') {
        flag = LEX_FLAG_OP;
        temp_tok.num = -1;
        temp_tok.type = PLUS;
        tokens[*length] = temp_tok;
        *length = *length + 1;
      } else if (*buffer == '-') {
        flag = LEX_FLAG_OP;
        temp_tok.num = -1;
        temp_tok.type = MINUS;
        tokens[*length] = temp_tok;
        *length = *length + 1;
      } else if (*buffer == '*') {
        flag = LEX_FLAG_OP;
        temp_tok.num = -1;
        temp_tok.type = MULT;
        tokens[*length] = temp_tok;
        *length = *length + 1;
      } else if (*buffer == '/') {
        flag = LEX_FLAG_OP;
        temp_tok.num = -1;
        temp_tok.type = DIV;
        tokens[*length] = temp_tok;
        *length = *length + 1;
      } else if (*buffer == '(') {
        flag = LEX_FLAG_OP;
        temp_tok.num = -1;
        temp_tok.type = LPAREN;
        tokens[*length] = temp_tok;
        *length = *length + 1;
      } else if (*buffer == ')') {
        flag = LEX_FLAG_OP;
        temp_tok.num = -1;
        temp_tok.type = RPAREN;
        tokens[*length] = temp_tok;
        *length = *length + 1;
      }
      break;

    default:
      printf("Unreachable\n");
      break;
    }

    ++buffer;
  }
}

int main(int argc, char *argv[]) {
  if (argc == 2 || argc == 3) {
    char *filename = argv[1];
    int length;
    char *buffer = readFile(&length, filename);
    LEX_Token tokens[MAX_PROG_SIZE];
    int current_token;
    parseBuffer(buffer, tokens, &current_token);
    if (!buffer) {
      printf("Could not open file %s", filename);
    }
    LEX_Token rpnTokens[MAX_PROG_SIZE];
    for (size_t i = 0; i < MAX_PROG_SIZE; ++i) {
      rpnTokens[i] = (LEX_Token){.type = NOTYPE, .num = -1};
    }
    int length_rpn;
    infixToRPN(rpnTokens, tokens, &length_rpn, current_token);

    if (argc == 3) {
      FILE *fout = fopen(argv[2], "w");
      if (fout) {
        fprintf(fout, "-------------------------------\n");
        fprintProg(rpnTokens, length_rpn, fout);
        fprintf(fout, "-------------------------------\n");
        int result = RPNToRes(rpnTokens, length_rpn);
        fprintf(fout, "RESULTAT: %d\n", result);
        fprintf(fout, "-------------------------------\n");
      } else {
        printf("Unable to find output file %s!\n", argv[2]);
      }
      fclose(fout);
    } else {
      printf("-------------------------------\n");
      printProg(rpnTokens, length_rpn);
      int result = RPNToRes(rpnTokens, length_rpn);
      printf("-------------------------------\n");
      printf("RESULTAT: %d\n", result);
      printf("-------------------------------\n");
    }
  } else if (argc > 3) {
    printf("Too many arguments. Syntax:\n./main.bin <input file> "
           "or\n./main.bin <input file> <output file>\n");
    return 1;
  } else if (argc < 2) {
    printf("Too few arguments. Syntax:\n./main.bin <input file> or\n./main.bin "
           "<input file> <output file>\n");
    return 1;
  }
  return 0;
}
