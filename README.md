# CST-405 Complete C Compiler

A full-featured compiler for a C-like programming language with lexical analysis, parsing, semantic analysis, optimization, and code generation for both MIPS and x86-64 architectures.

## Features

### Language Support
- Variables and arrays
- Mathematical expressions with proper precedence (+, -, *, /, %)
- Decision control structures (if, if-else, nested if)
- Loop constructs (while, for, do-while)
- Boolean/relational operations (<, >, <=, >=, ==, !=)
- Functions with parameters and return values
- Print statements

### Compiler Capabilities
- **6 Complete Compiler Phases**: Lexical analysis through code generation
- **Dual Code Generation**: MIPS (QtSpim/MARS) and x86-64 (NASM)
- **Code Optimization**: 6 optimization techniques including constant folding, dead code elimination
- **Security Analysis**: Buffer overflow, integer overflow, division by zero detection
- **Enhanced Diagnostics**: 4-level severity system with logging support
- **Comprehensive Testing**: 11+ test programs included

---

## Quick Start

### Build the Compiler
```bash
make clean
make
```

### Compile a Program
```bash
# Generate x86-64 assembly (default)
./compiler test_basic.c

# Generate MIPS assembly (for QtSpim/MARS)
./compiler test_basic.c --mips

# Verbose mode with all phases displayed
./compiler test_basic.c --verbose

# Save diagnostics to log file
./compiler test_basic.c --log output.log
```

### Command-Line Options
| Option | Description |
|--------|-------------|
| `--mips` | Generate MIPS assembly instead of x86-64 |
| `--verbose` or `-v` | Enable verbose output showing all phases |
| `--log <file>` | Write diagnostics to specified log file |
| `--Werror` | Treat warnings as errors |
| `--no-warnings` | Suppress warning messages |

---

## Compiler Phases

### Phase 1: Lexical Analysis
- **File**: `scanner_new.l`
- **Tool**: Flex
- **Function**: Tokenization of source code
- **Features**: Line/column tracking, error reporting

### Phase 2: Syntax Analysis
- **File**: `parser.y`
- **Tool**: Bison
- **Function**: Parse tree and AST construction
- **Features**: Symbol table generation, grammar validation

### Phase 3: Semantic Analysis
- **Files**: `semantic.c`, `semantic.h`
- **Function**: Type checking and semantic validation
- **Features**: Variable declaration checking, scope analysis, type inference

### Phase 4: Intermediate Code Generation
- **Files**: `ircode.c`, `ircode.h`
- **Function**: Three-Address Code (TAC) generation
- **Output**: `output.ir`

### Phase 5: Code Optimization
- **Files**: `optimizer.c`, `optimizer.h`
- **Techniques**:
  1. Constant folding (3 + 5 → 8)
  2. Dead code elimination
  3. Copy propagation
  4. Peephole optimization
  5. Flow optimization
  6. Algebraic simplification (x * 1 → x)

### Phase 6: Code Generation
- **MIPS**: `codegen_mips.c`, `codegen_mips.h` → `output_mips.asm`
- **x86-64**: `codegen.c`, `codegen.h` → `output.asm`

### Additional: Security Analysis
- **Files**: `security.c`, `security.h`
- **Checks**: Buffer overflow, integer overflow, division by zero, uninitialized variables, infinite loops

### Additional: Enhanced Diagnostics
- **Files**: `diagnostics.c`, `diagnostics.h`
- **Features**: 4-level severity (NOTE, WARNING, ERROR, FATAL), logging, safe memory management

---

## Output Files

| File | Description |
|------|-------------|
| `output.asm` | x86-64 NASM assembly code (default) |
| `output_mips.asm` | MIPS assembly code (with --mips flag) |
| `output.ir` | Three-Address Code (intermediate representation) |
| `*.log` | Diagnostic log file (with --log flag) |

---

## Testing

### Basic Test
```bash
./compiler test_basic.c
```

### Test All Features
```bash
# Test loops
./compiler test_loops.c

# Test conditionals
./compiler test_if_else.c

# Test arrays
./compiler test_arrays.c

# Test functions
./compiler test_functions.c

# Test security analysis
./compiler test_security.c

# Test comprehensive (all features)
./compiler test_comprehensive.c --verbose
```

### Test MIPS Code Generation
```bash
./compiler test_basic.c --mips
cat output_mips.asm
```

### Verify Optimization
```bash
./compiler test_basic.c --verbose 2>&1 | grep "optimization"
```

---

## Test Programs Included

- `test_basic.c` - Basic arithmetic and variables
- `test_loops.c` - While, for, do-while loops
- `test_if_else.c` - Conditional statements
- `test_arrays.c` - Array operations
- `test_functions.c` - Function declarations and calls
- `test_nested_loops.c` - Nested loop structures
- `test_order_of_operations.c` - Expression precedence
- `test_comprehensive.c` - All features combined
- `test_security.c` - Security vulnerability detection
- `test_for.c` - For loop specific tests
- `test_do_while.c` - Do-while loop tests

---

## Language Example

```c
int factorial(int n) {
    int result;
    int i;
    result = 1;
    i = 1;

    while (i <= n) {
        result = result * i;
        i = i + 1;
    }

    return result;
}

int main() {
    int x;
    x = 5;
    print(factorial(x));  // Output: 120
    return 0;
}
```

---

## Building and Running Programs

### For x86-64 (Linux/macOS)
```bash
# Compile to assembly
./compiler program.c

# Assemble (requires NASM)
nasm -f elf64 output.asm -o output.o

# Link (requires GCC)
gcc output.o -o program -no-pie

# Run
./program
```

### For MIPS (QtSpim/MARS Simulator)
```bash
# Compile to MIPS assembly
./compiler program.c --mips

# Open output_mips.asm in QtSpim or MARS
# Run in the simulator
```

---

## Project Structure

```
CST-405-PROJECTS/
├── compiler.c              # Main compiler driver
├── scanner_new.l           # Lexical analyzer (Flex)
├── parser.y                # Syntax analyzer (Bison)
├── ast.c / ast.h           # Abstract Syntax Tree
├── semantic.c / semantic.h # Semantic analysis
├── ircode.c / ircode.h     # IR code generation
├── optimizer.c / optimizer.h     # Code optimizer
├── codegen.c / codegen.h   # x86-64 code generator
├── codegen_mips.c / codegen_mips.h  # MIPS code generator
├── diagnostics.c / diagnostics.h    # Diagnostics system
├── security.c / security.h # Security analysis
├── Makefile                # Build system
├── test_*.c                # Test programs (11 files)
└── README.md               # This file
```

---

## Requirements

- **GCC** - C compiler
- **Flex** - Lexical analyzer generator
- **Bison** - Parser generator
- **Make** - Build automation
- **NASM** (optional) - For assembling x86-64 output
- **QtSpim/MARS** (optional) - For running MIPS output

### Installation (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install gcc flex bison make nasm
```

### Installation (macOS)
```bash
brew install gcc flex bison make nasm
```

---

## Makefile Commands

```bash
make          # Build the compiler
make clean    # Remove generated files
make info     # Show compiler information
make help     # Show available commands
```

---

## Compilation Examples

### Example 1: Basic Compilation
```bash
./compiler test_basic.c
# Creates: output.asm, output.ir
```

### Example 2: MIPS with Verbose Output
```bash
./compiler test_loops.c --mips --verbose
# Shows all phases, creates: output_mips.asm, output.ir
```

### Example 3: Production Mode (Warnings as Errors)
```bash
./compiler program.c --Werror
# Strict error checking
```

### Example 4: Full Logging
```bash
./compiler test_comprehensive.c --verbose --log compile.log
# All diagnostics saved to compile.log
```

---

## Expected Output

### Successful Compilation
```
+============================================================+
|           CST-405 COMPLETE COMPILER SYSTEM                 |
+============================================================+

Input file: test_basic.c
Output file: output.asm
Target: x86-64 (NASM)

+============================================================+
| PHASE 1 & 2: LEXICAL AND SYNTAX ANALYSIS                  |
+============================================================+
[OK] Lexical analysis complete
[OK] Syntax analysis complete
[OK] Abstract Syntax Tree (AST) constructed

+============================================================+
| PHASE 3: SEMANTIC ANALYSIS                                 |
+============================================================+
[OK] SUCCESS: No semantic errors detected

+============================================================+
| PHASE 4: INTERMEDIATE CODE GENERATION                      |
+============================================================+
Generated 25 TAC instructions
[OK] Intermediate code saved to: output.ir

+============================================================+
| PHASE 5: CODE OPTIMIZATION                                 |
+============================================================+
[OPTIMIZER] Total optimizations: 12

+============================================================+
| PHASE 5.5: SECURITY ANALYSIS                               |
+============================================================+
✓ No security issues detected!

+============================================================+
| PHASE 6: ASSEMBLY CODE GENERATION                          |
+============================================================+
[CODEGEN] Assembly generation complete

+============================================================+
|                   COMPILATION SUMMARY                     |
+============================================================+
|  Status:           [OK] SUCCESS                           |
|  Lexical errors:   0                                      |
|  Syntax errors:    0                                      |
|  Semantic errors:  0                                      |
|  Optimization:     Enabled                                |
|  Code generated:   Yes                                    |
+============================================================+

[OK] Compilation successful!
[OK] Assembly code written to: output.asm
```

---

## Error Reporting

The compiler provides detailed error messages with line and column information:

```
[semantic:10:5] error: Variable 'x' used before declaration
[security:15:3] warning: Array 'arr' access with index 10 is out of bounds [0..9]
[semantic:20:8] error: Division by zero detected
```

---

## Security Analysis Features

The compiler detects potential security vulnerabilities:

1. **Buffer Overflow**: Array access out of bounds
2. **Integer Overflow**: Arithmetic operations that may overflow
3. **Division by Zero**: Division or modulo by constant zero
4. **Uninitialized Variables**: Variables used before assignment
5. **Infinite Loops**: Loops with constant true conditions

Example security report:
```
╔════════════════════════════════════════════════════╗
║           SECURITY ANALYSIS REPORT                ║
╠════════════════════════════════════════════════════╣
║ Buffer Overflow Risks:      1                     ║
║ Integer Overflow Risks:     0                     ║
║ Division by Zero Risks:     0                     ║
║ Unsafe Array Accesses:      0                     ║
║ Infinite Loop Risks:        0                     ║
╠════════════════════════════════════════════════════╣
║ Total Security Issues:      1                     ║
╚════════════════════════════════════════════════════╝
```

---

## Troubleshooting

### Issue: `compiler: command not found`
**Solution**: Build the compiler first
```bash
make
```

### Issue: `flex: not found` or `bison: not found`
**Solution**: Install flex and bison
```bash
# Ubuntu/Debian
sudo apt-get install flex bison

# macOS
brew install flex bison
```

### Issue: No output files generated
**Solution**: Check for compilation errors
```bash
./compiler test_basic.c --verbose
```

---

## Course Information

**Course**: CST-405 Compiler Design
**Project**: Complete C Compiler Implementation
**Features**: 6 compiler phases, dual code generation, optimization, security analysis

---

## License

Educational project for CST-405 Compiler Design course.

---

## Contact

For issues or questions related to this compiler project, please refer to the course materials or contact the instructor.
