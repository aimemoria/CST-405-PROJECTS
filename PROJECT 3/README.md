# CST-405 Compiler Project

A complete compiler implementation translating a C-like programming language to x86-64 assembly.

## Features
- Variables and arrays
- Mathematical expressions (+, -, *, /, %) with precedence
- Control flow (while loops)
- Functions with parameters and return values
- Type checking and error reporting

## Build & Run
```bash
make
./compiler test_program.c
nasm -f elf64 output.asm -o output.o
gcc output.o -o program -no-pie
./program
```

## Compiler Phases
1. **Lexical Analysis** (scanner_new.l) - Tokenization
2. **Syntax Analysis** (parser.y) - Parsing & AST generation
3. **Symbol Table** (symtable.c/h) - Variable/function tracking
4. **Semantic Analysis** (semantic.c/h) - Type checking
5. **IR Generation** (ircode.c/h) - Three-address code
6. **Code Generation** (codegen.c/h) - x86-64 assembly

## Test Programs
- test_program.c - Basic features
- test_math.c - Mathematical expressions
- test_arrays.c - Array operations
- test_functions.c - Function calls

## Language Example
```c
int add(int a, int b) {
    return a + b;
}

int x;
int y;
x = 5;
y = 10;
print(add(x, y));  // Output: 15
```

## Author
CST-405 Compiler Design Project
