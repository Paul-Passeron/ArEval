#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_NUM_SIZE 100

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
  NOTYPE,
  RPAREN,
} LEX_TokenType;

typedef struct {
  LEX_TokenType type;
  uint16_t num;
} LEX_Token;

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
  for (i = 0; i < length; i++) {
    printtype(toks[i]);
  }
}

void fprintProg(LEX_Token *toks, int length, FILE *f) {
  int8_t i;
  for (i = 0; i < length; i++) {
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

int main(int argc, char *argv[]) {

  // Parsing command line args
  if (argc == 2 || argc == 3) {
    char *filename = argv[1];

    char *buffer;
    char *start = buffer;
    long length;
    FILE *f = fopen(filename, "rb");

    if (f) {

      fseek(f, 0, SEEK_END);
      length = ftell(f);
      fseek(f, 0, SEEK_SET);

      buffer = malloc(length + 1); // Append a newline char at teh eof.
      buffer[length] = '\n';

      if (buffer) {
        fread(buffer, 1, length, f);
      }
      fclose(f);
    } else {
      // printf("Unable to read input file! %s\n", filename);
      return 1;
    }
    if (!buffer) {
      printf("Got here\n");

      printf("Could not open file %s", filename);
    }

    char nume[MAX_NUM_SIZE];
    uint8_t i = 0;
    uint8_t flag = LEX_FLAG_DEFAULT;

    LEX_Token tokens[MAX_PROG_SIZE];
    int current_token = 0;
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
          tokens[current_token] = temp_tok;
          current_token++;
          flag = LEX_FLAG_DEFAULT;
          if (*buffer == '+') {
            flag = LEX_FLAG_OP;
            temp_tok.num = -1;
            temp_tok.type = PLUS;
            tokens[current_token] = temp_tok;
            current_token++;
          } else if (*buffer == '-') {
            flag = LEX_FLAG_OP;
            temp_tok.num = -1;
            temp_tok.type = MINUS;
            tokens[current_token] = temp_tok;
            current_token++;
          } else if (*buffer == '*') {
            flag = LEX_FLAG_OP;
            temp_tok.num = -1;
            temp_tok.type = MULT;
            tokens[current_token] = temp_tok;
            current_token++;
          } else if (*buffer == '/') {
            flag = LEX_FLAG_OP;
            temp_tok.num = -1;
            temp_tok.type = DIV;
            tokens[current_token] = temp_tok;
            current_token++;
          } else if (*buffer == '(') {
            flag = LEX_FLAG_OP;
            temp_tok.num = -1;
            temp_tok.type = LPAREN;
            tokens[current_token] = temp_tok;
            current_token++;
          } else if (*buffer == ')') {
            flag = LEX_FLAG_OP;
            temp_tok.num = -1;
            temp_tok.type = RPAREN;
            tokens[current_token] = temp_tok;
            current_token++;
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
            tokens[current_token] = temp_tok;
            current_token++;
          } else if (*buffer == '-') {
            flag = LEX_FLAG_OP;
            temp_tok.num = -1;
            temp_tok.type = MINUS;
            tokens[current_token] = temp_tok;
            current_token++;
          } else if (*buffer == '*') {
            flag = LEX_FLAG_OP;
            temp_tok.num = -1;
            temp_tok.type = MULT;
            tokens[current_token] = temp_tok;
            current_token++;
          } else if (*buffer == '/') {
            flag = LEX_FLAG_OP;
            temp_tok.num = -1;
            temp_tok.type = DIV;
            tokens[current_token] = temp_tok;
            current_token++;
          } else if (*buffer == '(') {
            flag = LEX_FLAG_OP;
            temp_tok.num = -1;
            temp_tok.type = LPAREN;
            tokens[current_token] = temp_tok;
            current_token++;
          } else if (*buffer == ')') {
            flag = LEX_FLAG_OP;
            temp_tok.num = -1;
            temp_tok.type = RPAREN;
            tokens[current_token] = temp_tok;
            current_token++;
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
          tokens[current_token] = temp_tok;
          current_token++;
        } else if (*buffer == '-') {
          flag = LEX_FLAG_OP;
          temp_tok.num = -1;
          temp_tok.type = MINUS;
          tokens[current_token] = temp_tok;
          current_token++;
        } else if (*buffer == '*') {
          flag = LEX_FLAG_OP;
          temp_tok.num = -1;
          temp_tok.type = MULT;
          tokens[current_token] = temp_tok;
          current_token++;
        } else if (*buffer == '/') {
          flag = LEX_FLAG_OP;
          temp_tok.num = -1;
          temp_tok.type = DIV;
          tokens[current_token] = temp_tok;
          current_token++;
        } else if (*buffer == '(') {
          flag = LEX_FLAG_OP;
          temp_tok.num = -1;
          temp_tok.type = LPAREN;
          tokens[current_token] = temp_tok;
          current_token++;
        } else if (*buffer == ')') {
          flag = LEX_FLAG_OP;
          temp_tok.num = -1;
          temp_tok.type = RPAREN;
          tokens[current_token] = temp_tok;
          current_token++;
        }
        break;

      default:
        printf("Unreachable\n");
        break;
      }

      ++buffer;
    }
    if (argc == 3) {
      FILE *fout = fopen(argv[2], "w");
      if (fout) {

        fprintf(fout, "-------------------------------\n");
        fprintProg(tokens, current_token, fout);
        fprintf(fout, "-------------------------------\n");
      } else {
        printf("Unable to find outpur file %s!\n", argv[2]);
      }
      fclose(fout);
    } else {
      printf("-------------------------------\n");
      printProg(tokens, current_token);
      printf("-------------------------------\n");
      printf("%d", current_token);
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
