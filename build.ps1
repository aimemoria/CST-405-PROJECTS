# Build script for CST-405 Compiler - PowerShell version
# This properly handles paths with spaces

$ErrorActionPreference = "Stop"
# Use C:\CST405BUILD if we're there, otherwise use current location
$CURRENT_DIR = if (Test-Path "C:\CST405BUILD") { "C:\CST405BUILD" } else { Get-Location }
$BISON_BIN = Join-Path $CURRENT_DIR "bison-temp\bin"
$BISON_PKGDATADIR = Join-Path $CURRENT_DIR "bison-temp\share\bison"

# Add bison to PATH
$env:PATH = "$BISON_BIN;$env:PATH"
$env:BISON_PKGDATADIR = $BISON_PKGDATADIR

Write-Host "========================================================"
Write-Host "Building CST-405 Compiler"
Write-Host "========================================================"

Write-Host ""
Write-Host "Generating parser with Bison..."
& "$BISON_BIN\bison.exe" -d -v parser.y
if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: Bison failed"
    exit 1
}

Write-Host "Generating lexer with Flex..."
flex scanner_new.l
if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: Flex failed"
    exit 1
}

Write-Host ""
Write-Host "Compiling source files..."
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

Write-Host ""
Write-Host "Linking compiler..."
gcc -Wall -g -o compiler.exe compiler.o parser.tab.o lex.yy.o ast.o symtable.o semantic.o ircode.o optimizer.o codegen.o codegen_mips.o diagnostics.o security.o

if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: Linking failed"
    exit 1
}

Write-Host ""
Write-Host "========================================================"
Write-Host "[OK] Compiler built successfully: compiler.exe"
Write-Host "========================================================"
