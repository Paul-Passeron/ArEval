#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_NUM_SIZE 10

#define MAX_PROG_SIZE 100

#define LEX_FLAG_DEFAULT 0
#define LEX_FLAG_NUM 1
#define LEX_FLAG_OP 2

typedef uint8_t bool;
#define false 0;
#define true 1;

typedef enum { PLUS, MINUS, MULT, DIV, NUMBER, NOTYPE } LEX_TokenType;

typedef struct {
  LEX_TokenType type;
  uint16_t num;
} LEX_Token;

void printtype(LEX_TokenType type) {
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
  if (type == NUMBER) {
    printf("NUMBER\n");
    return;
  }
  return;
}

uint64_t parseNum(char *num, uint8_t length) {
  uint64_t res = 0;
  for (uint8_t i = 0; i < length; ++i) {
    int digit = (int)num[i] - 0x30;

    int puiss = (int)powf(10, length - i - 1);
    res = res + (digit * puiss);
  }

  return res;
}

long loadFile(char **buffer, char *filename) {
  long length;

  return length;
}

int main(void) {
  char *buffer;
  char *filename = "test.ar";
  long length;
  FILE *f = fopen(filename, "rb");

  if (f) {

    fseek(f, 0, SEEK_END);
    length = ftell(f);
    fseek(f, 0, SEEK_SET);

    buffer = malloc(length);

    if (buffer) {
      fread(buffer, 1, length, f);
    }
    fclose(f);
  }
  if (!buffer) {
    printf("Got here\n");

    printf("Could not open file %s", filename);
  }
  char *start = buffer;

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
        if (*buffer == '+') {
          flag = LEX_FLAG_OP;
          temp_tok.num = -1;
          temp_tok.type = PLUS;
        } else if (*buffer == '-') {
          flag = LEX_FLAG_OP;
          temp_tok.num = -1;
          temp_tok.type = MINUS;
        } else if (*buffer == '*') {
          flag = LEX_FLAG_OP;
          temp_tok.num = -1;
          temp_tok.type = MULT;
        } else if (*buffer == '/') {
          flag = LEX_FLAG_OP;
          temp_tok.num = -1;
          temp_tok.type = DIV;
        }
      }
      break;

    case LEX_FLAG_OP:
      i = 0;
      if (*buffer == ' ' || *buffer == '\n') {
        flag = LEX_FLAG_DEFAULT;
      }
      break;

    case LEX_FLAG_DEFAULT:
      if ('0' <= *buffer && *buffer <= '9') {
        flag = LEX_FLAG_NUM;
        nume[i] = *buffer;
        i++;
      }
      if (*buffer == '+') {
      }

      break;

    default:
      printf("Unreachable\n");
      break;
    }

    ++buffer;
  }
  free(start);
  return 0;
}
