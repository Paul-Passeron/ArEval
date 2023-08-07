#include "areval.h"

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Not enough arguments !");
    return -1;
  }
  int length;
  char *filename = argv[1];
  char *buffer = LEX_ReadFile(&length, filename);
  LEX_Prog infix = LEX_Parse(buffer, length);
  LEX_Prog rpn = LEX_InfixToRPN(infix);
  res res = LEX_RPNToRes(rpn);
  printf("RESULT: %d\n", res);
  return 0;
}
