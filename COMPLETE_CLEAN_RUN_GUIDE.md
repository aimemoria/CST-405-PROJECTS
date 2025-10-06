# CST-405 COMPLETE CLEAN & RUN GUIDE

## 🧹 STEP 1: COMPLETE CLEANUP (Start Fresh)

### In WSL Terminal:

```bash
# Navigate to project
cd /mnt/f/PROJECTS/CST\ 405/PROJECT/CST-405

# Clean ALL generated files
make -f Makefile_new clean

# Remove any leftover files
rm -f compiler *.o lex.yy.c parser.tab.c parser.tab.h parser.output
rm -f output.asm output.o program

# Verify everything is clean
ls -la

# You should only see source files (.c, .h, .l, .y, .src, .md, .txt, Makefile_new)
```

---

## 📥 STEP 2: GET LATEST CODE FROM GITHUB

```bash
# Make sure you have the latest version
git pull origin main

# Check status - should be clean
git status
```

---

## 🔨 STEP 3: BUILD THE COMPILER

```bash
# Build from scratch
make -f Makefile_new

# You should see:
# - Generating parser with Bison...
# - Compiling parser...
# - Generating lexer with Flex...
# - Compiling lexer...
# - Compiling AST module...
# - Compiling symbol table module...
# - Compiling semantic analyzer...
# - Compiling IR code generator...
# - Compiling code generator...
# - Compiling main compiler driver...
# - Linking compiler...
# - ✓ Compiler built successfully: compiler

# Verify compiler exists
ls -l compiler
```

---

## ✅ STEP 4: TEST BASIC PROGRAM

```bash
# Compile basic test
./compiler test_basic.src

# You should see:
# - Banner (CST-405 Complete Compiler System)
# - Phase 1 & 2: Lexical and Syntax Analysis
# - Phase 3: Semantic Analysis
# - Abstract Syntax Tree
# - Symbol Table
# - Phase 4: Intermediate Code Generation
# - Three-Address Code (TAC)
# - Phase 5: Assembly Code Generation
# - ✓ Compilation successful!

# Check assembly was generated
ls -l output.asm
```

---

## 🏃 STEP 5: ASSEMBLE AND RUN

```bash
# Assemble to object file
nasm -f elf64 output.asm -o output.o

# Link to executable (no warning now!)
gcc output.o -o program -no-pie

# Run the program
./program

# Expected Output:
# 30
# 10
# 20
```

---

## 🔄 STEP 6: TEST WHILE LOOPS (NEW FEATURE)

```bash
# Clean previous output
rm -f output.asm output.o program

# Compile while loop test
./compiler test_while.src

# Assemble and run
nasm -f elf64 output.asm -o output.o
gcc output.o -o program -no-pie
./program

# Expected Output:
# 0
# 1
# 2
# 3
# 4
# 10
```

---

## 🎯 STEP 7: TEST COMPLEX PROGRAM

```bash
# Clean previous output
rm -f output.asm output.o program

# Compile complex test
./compiler test_complex.src

# Assemble and run
nasm -f elf64 output.asm -o output.o
gcc output.o -o program -no-pie
./program

# Expected Output:
# 15
# 10
# 9
# 8
# 7
# 6
# 5
# 4
# 3
# 2
# 1
# 10
```

---

## 📹 STEP 8: PREPARE FOR VIDEO RECORDING

### Terminal Setup

```bash
# Clear screen for clean recording
clear

# Optional: Increase font size for visibility
# Right-click Ubuntu title bar → Properties → Font → Size 20
```

### Open These Files in Separate Windows

1. **VIDEO_SCRIPT.md** - Your speaking script
2. **test_while.src** - To show while loop code
3. **parser.y** - To show grammar
4. **A text editor** - For live viewing

---

## 🎬 VIDEO RECORDING COMMANDS (Copy-Paste Ready)

### Section 1: Introduction & Setup

```bash
# Show you're in the right place
pwd

# Show all files
ls -la

# Show a test program
cat test_while.src
```

### Section 2: Architecture

```bash
# Show file structure
echo "=== SOURCE FILES ==="
ls -1 *.c *.h *.l *.y

echo ""
echo "=== TEST PROGRAMS ==="
ls -1 *.src

# Show parser grammar (first 20 lines)
echo ""
echo "=== PARSER GRAMMAR ==="
head -n 20 parser.y
```

### Section 3: Build Compiler

```bash
# Clean first
make -f Makefile_new clean

# Build
make -f Makefile_new
```

### Section 4: Compile Test Program

```bash
# Compile while loop test
./compiler test_while.src
```

**⚠️ IMPORTANT:** Scroll up and point out:
- The AST structure (WHILE node with condition and body)
- Symbol table entries
- TAC code with labels (L0, L1)
- The control flow (if_false, goto)

### Section 5: Show Generated Assembly

```bash
# Show first 50 lines of assembly
head -n 50 output.asm
```

**Point out:**
- The .data section (format string)
- The .bss section (variables)
- The loop labels (L0:, L1:)
- The comparison (cmp)
- The conditional jump (je L1)
- The unconditional jump (jmp L0)

### Section 6: Assemble and Execute

```bash
# Assemble
nasm -f elf64 output.asm -o output.o

# Link
gcc output.o -o program -no-pie

# Run and show output
echo "=== PROGRAM OUTPUT ==="
./program
echo "=== END OUTPUT ==="
```

### Section 7: Demo Error Handling

```bash
# Create error file
echo "int x; y = 10; print(y);" > test_error.src

# Try to compile
./compiler test_error.src

# Point out the semantic error about undeclared variable
```

### Section 8: Show All Tests Work

```bash
# Quick test all programs
echo "=== TEST 1: BASIC ==="
./compiler test_basic.src && nasm -f elf64 output.asm -o output.o && gcc output.o -o program -no-pie && ./program

echo ""
echo "=== TEST 2: WHILE LOOPS ==="
./compiler test_while.src && nasm -f elf64 output.asm -o output.o && gcc output.o -o program -no-pie && ./program

echo ""
echo "=== TEST 3: COMPLEX ==="
./compiler test_complex.src && nasm -f elf64 output.asm -o output.o && gcc output.o -o program -no-pie && ./program
```

---

## 📋 VIDEO CHECKLIST

Before you start recording, verify:

- [ ] WSL terminal is open and working
- [ ] You're in the correct directory: `/mnt/f/PROJECTS/CST 405/PROJECT/CST-405`
- [ ] Everything is cleaned: `make -f Makefile_new clean`
- [ ] Latest code pulled: `git pull`
- [ ] Terminal font is large enough (size 18-20)
- [ ] VIDEO_SCRIPT.md is open in another window
- [ ] You've practiced the workflow once

**During recording, show:**

- [x] Project structure (files)
- [x] Source code (scanner, parser, test programs)
- [x] Build process
- [x] Compilation output (AST, symbol table, TAC)
- [x] Generated assembly code
- [x] Execution and correct output
- [x] Error handling
- [x] While loop feature working

---

## 🎯 KEY TALKING POINTS

### Introduction (1 min)
- "Complete 5-phase compiler"
- "All phases from lexing to assembly generation"
- "New feature: while loops with relational operators"

### Architecture (2 min)
- "Modular design with separate phases"
- "Lexer (Flex), Parser (Bison), Semantic analyzer"
- "Intermediate code generator, Assembly generator"
- "While loops integrated into ALL phases"

### AST → IR → Assembly (3 min) **CRITICAL SECTION**
- "AST captures hierarchical structure"
- "TAC linearizes with explicit control flow"
- "Labels for loop start and end"
- "IF_FALSE for exit condition"
- "GOTO for loop back"
- "Assembly maps directly from TAC"

### Demo (3 min)
- "Build from source"
- "Compile test program"
- "Show intermediate outputs"
- "Execute and verify"

---

## 🧪 TROUBLESHOOTING

### Problem: "command not found: make"

**Solution:**
```bash
sudo apt-get install make
```

### Problem: "No such file or directory: compiler"

**Solution:**
```bash
# Build didn't complete - rebuild
make -f Makefile_new clean
make -f Makefile_new
```

### Problem: "Permission denied: ./compiler"

**Solution:**
```bash
chmod +x compiler
```

### Problem: "cannot execute binary file"

**Solution:**
You're not in WSL - make sure you're in Ubuntu terminal, not Git Bash

### Problem: Line ending errors again

**Solution:**
```bash
# Convert all files
sed -i 's/\r$//' *.c *.h *.l *.y *.src Makefile_new
```

---

## ✨ FINAL CLEANUP AFTER VIDEO

```bash
# Clean all build artifacts
make -f Makefile_new clean

# Or manual cleanup
rm -f compiler *.o lex.yy.c parser.tab.* parser.output
rm -f output.asm output.o program test_error.src
```

---

## 🚀 ONE-COMMAND TEST (Verify Everything Works)

```bash
cd /mnt/f/PROJECTS/CST\ 405/PROJECT/CST-405 && \
make -f Makefile_new clean && \
make -f Makefile_new && \
./compiler test_while.src && \
nasm -f elf64 output.asm -o output.o && \
gcc output.o -o program -no-pie && \
./program
```

**Expected final output:**
```
0
1
2
3
4
10
```

**If you see this, EVERYTHING WORKS! 🎉**

---

## 📊 SUMMARY

**Your compiler:**
- ✅ 5 complete compilation phases
- ✅ While loops (NEW FEATURE)
- ✅ Full AST construction
- ✅ Symbol table with type checking
- ✅ TAC intermediate representation
- ✅ x86-64 assembly generation
- ✅ Executable programs
- ✅ 3000+ lines of code
- ✅ Extensively documented
- ✅ All on GitHub

**You're ready to record!** 🎬
