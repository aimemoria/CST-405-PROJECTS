# Video Demonstration Guide

## Compiler Demonstration Script

### A) How the Code Works (2-3 minutes)

**Introduction**
Show the project structure: lexer (scanner_new.l), parser (parser.y), AST (ast.c/h), symbol table (symtable.c/h), semantic analyzer (semantic.c/h), IR generator (ircode.c/h), and code generator (codegen.c/h).

**Compilation Flow Demo**
Compile a simple program: `./compiler test_program.c`

Display each phase output:
1. Lexical analysis - show tokens generated
2. Syntax analysis - display the AST tree structure
3. Symbol table - show tracked variables and functions
4. Semantic analysis - demonstrate type checking results
5. IR generation - present Three-Address Code instructions
6. Code generation - show generated x86-64 assembly in output.asm

**Execution Demo**
Assemble and run: `nasm -f elf64 output.asm -o output.o`, `gcc output.o -o program -no-pie`, `./program`
Show the program output matching expected results.

### B) Individual Tasks Completed (1-2 minutes)

**My Contributions:**
- Implemented lexical analyzer with all tokens for variables, operators, control flow, and functions
- Developed parser grammar supporting expressions with precedence, control structures, and function definitions
- Created complete symbol table with hash table implementation for efficient lookup
- Built semantic analyzer performing comprehensive type checking and validation
- Designed IR generator producing optimized Three-Address Code
- Developed code generator targeting x86-64 assembly with proper calling conventions

### C) Team Integration Approach (1 minute)

**Component Integration:**
Our approach involved modular development where each compiler phase was implemented as an independent module with clear interfaces. The AST serves as the primary data structure connecting all phases. Symbol table is shared between semantic analysis and code generation. Each phase validates its input and provides clear error messages. Integration testing was performed incrementally, adding one feature at a time across all phases (variables, then arrays, then functions).

### D) Demonstration (2-3 minutes)

**Test Programs:**

**test_math.c** - Show arithmetic with precedence
**test_arrays.c** - Demonstrate array operations
**test_functions.c** - Display function calls with parameters

For each: compile, show intermediate outputs, assemble, run, verify correct results.

**Error Handling Demo:**
Show semantic error (undefined variable), syntax error (missing semicolon), type error (array as non-array) - demonstrate error messages with line numbers.

*Word Count: 297*
