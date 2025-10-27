# PowerShell Build Script for CST-405 Compiler
# Run this in VS Code PowerShell terminal

Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host "CST-405 Compiler - PowerShell Build" -ForegroundColor Cyan
Write-Host "========================================`n" -ForegroundColor Cyan

# Check for required tools
Write-Host "Checking for required tools..." -ForegroundColor Yellow

$gcc = Get-Command gcc -ErrorAction SilentlyContinue
$flex = Get-Command flex -ErrorAction SilentlyContinue
$bison = Get-Command bison -ErrorAction SilentlyContinue
$winbison = Get-Command win_bison -ErrorAction SilentlyContinue

if (-not $gcc) {
    Write-Host "ERROR: GCC not found. Please install TDM-GCC or MinGW" -ForegroundColor Red
    exit 1
}
Write-Host "? GCC found: $($gcc.Source)" -ForegroundColor Green

if (-not $flex) {
    Write-Host "ERROR: Flex not found. Please install Flex" -ForegroundColor Red
    exit 1
}
Write-Host "? Flex found: $($flex.Source)" -ForegroundColor Green

if ($bison) {
    $BISON = "bison"
    Write-Host "? Bison found: $($bison.Source)" -ForegroundColor Green
} elseif ($winbison) {
    $BISON = "win_bison"
    Write-Host "? Win Bison found: $($winbison.Source)" -ForegroundColor Green
} else {
    Write-Host "ERROR: Bison not found. Please install Bison or WinFlexBison" -ForegroundColor Red
    Write-Host "Install with: choco install winflexbison3" -ForegroundColor Yellow
    exit 1
}

# Clean old files
Write-Host "`nCleaning old files..." -ForegroundColor Yellow
Remove-Item -Path lex.yy.c, parser.tab.c, parser.tab.h, parser.output, *.o, compiler.exe -ErrorAction SilentlyContinue
Write-Host "? Clean complete" -ForegroundColor Green

# Generate parser
Write-Host "`nGenerating parser with $BISON..." -ForegroundColor Yellow
& $BISON -d -v parser.y
if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: Bison failed" -ForegroundColor Red
    exit 1
}
Write-Host "? Parser generated" -ForegroundColor Green

# Generate lexer
Write-Host "`nGenerating lexer with Flex..." -ForegroundColor Yellow
& flex scanner_new.l
if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: Flex failed" -ForegroundColor Red
    exit 1
}
Write-Host "? Lexer generated" -ForegroundColor Green

# Compile all modules
Write-Host "`nCompiling modules..." -ForegroundColor Yellow

$sources = @(
    "parser.tab.c",
    "lex.yy.c",
    "compiler.c",
    "ast.c",
    "symtable.c",
    "semantic.c",
    "ircode.c",
    "codegen.c"
)

foreach ($src in $sources) {
    Write-Host "  Compiling $src..." -ForegroundColor Gray
    & gcc -Wall -g -c $src
    if ($LASTEXITCODE -ne 0) {
        Write-Host "ERROR: Compilation of $src failed" -ForegroundColor Red
        exit 1
    }
}

Write-Host "? All modules compiled" -ForegroundColor Green

# Link compiler
Write-Host "`nLinking compiler..." -ForegroundColor Yellow
& gcc -o compiler.exe parser.tab.o lex.yy.o compiler.o ast.o symtable.o semantic.o ircode.o codegen.o
if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: Linking failed" -ForegroundColor Red
    exit 1
}

Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host "? BUILD SUCCESSFUL!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "`nCompiler: compiler.exe" -ForegroundColor Yellow
Write-Host "`nUsage:" -ForegroundColor Yellow
Write-Host "  .\compiler.exe test_program.c" -ForegroundColor White
Write-Host "`nTest programs:" -ForegroundColor Yellow
Write-Host "  test_program.c  - Basic features" -ForegroundColor White
Write-Host "  test_math.c     - Math operators" -ForegroundColor White
Write-Host "  test_arrays.c   - Array operations" -ForegroundColor White
Write-Host "  test_functions.c - Functions" -ForegroundColor White
Write-Host ""
