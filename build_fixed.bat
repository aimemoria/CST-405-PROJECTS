@echo off
REM Build script for CST-405 Compiler - Fixed for paths with spaces
SETLOCAL EnableDelayedExpansion

REM Get the current directory
SET "CURRENT_DIR=%~dp0"
SET "BISON_BIN=%CURRENT_DIR%bison-temp\bin"
SET "BISON_PKGDATADIR=%CURRENT_DIR%bison-temp\share\bison"

REM Add bison to PATH
SET "PATH=%BISON_BIN%;%PATH%"

echo ════════════════════════════════════════════════════════
echo Building CST-405 Compiler
echo ════════════════════════════════════════════════════════

echo.
echo Generating parser with Bison...
"%BISON_BIN%\bison.exe" -d -v parser.y
if errorlevel 1 (
    echo ERROR: Bison failed
    exit /b 1
)

echo Generating lexer with Flex...
flex scanner_new.l
if errorlevel 1 (
    echo ERROR: Flex failed
    exit /b 1
)

echo.
echo Compiling source files...
gcc -Wall -g -c compiler.c
gcc -Wall -g -c parser.tab.c
gcc -Wall -g -c lex.yy.c
gcc -Wall -g -c ast.c
gcc -Wall -g -c symtable.c
gcc -Wall -g -c semantic.c
gcc -Wall -g -c ircode.c
gcc -Wall -g -c optimizer.c
gcc -Wall -g -c codegen.c
gcc -Wall -g -c codegen_mips.c
gcc -Wall -g -c diagnostics.c
gcc -Wall -g -c security.c

echo.
echo Linking compiler...
gcc -Wall -g -o compiler.exe compiler.o parser.tab.o lex.yy.o ast.o symtable.o semantic.o ircode.o optimizer.o codegen.o codegen_mips.o diagnostics.o security.o

if errorlevel 1 (
    echo ERROR: Linking failed
    exit /b 1
)

echo.
echo ════════════════════════════════════════════════════════
echo ✓ Compiler built successfully: compiler.exe
echo ════════════════════════════════════════════════════════
ENDLOCAL
