# ArEval
Simple proof of concept arithmetic evaluator written in C.

## Quickstart:
Compile main.c file using the following command:
```console
 cc main.c areval.c -o main c -lm
```
or 
```console
 clang main.c areval.c -o main c -lm
```

And then run the main executable with the filename of your program :

```console
./main <filename>
```

Ex:
test.ar:
```python
(1+7)*(8*9-5)/3-2
```
```console
./main test.ar
RESULT: 176
```