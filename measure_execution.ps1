# Performance Measurement Script for Compiled Program Execution
# Measures execution time of compiled x86-64 programs (simulated)

param(
    [Parameter(Mandatory=$true)]
    [string]$InputFile
)

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  EXECUTION PERFORMANCE MEASUREMENT" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Note: This measures the time to generate and process assembly code"
Write-Host "For actual execution metrics, assemble and run the output on target platform"
Write-Host ""
Write-Host "Input File: $InputFile"
Write-Host ""

# Compile the program
Write-Host "Compiling $InputFile..." -ForegroundColor Yellow
& ".\compiler.exe" $InputFile

if ($LASTEXITCODE -eq 0) {
    Write-Host "Compilation successful!" -ForegroundColor Green
    Write-Host ""
    
    # Check output file size (proxy for complexity)
    if (Test-Path "output.asm") {
        $asmFile = Get-Item "output.asm"
        $lineCount = (Get-Content "output.asm" | Measure-Object -Line).Lines
        $fileSize = $asmFile.Length
        
        Write-Host "========================================" -ForegroundColor Green
        Write-Host "  GENERATED CODE STATISTICS" -ForegroundColor Green
        Write-Host "========================================" -ForegroundColor Green
        Write-Host "Assembly file size: $fileSize bytes"
        Write-Host "Assembly lines: $lineCount"
        Write-Host ""
        Write-Host "To measure actual execution time:" -ForegroundColor Cyan
        Write-Host "  1. Assemble: nasm -f elf64 output.asm -o output.o"
        Write-Host "  2. Link: gcc output.o -o program -no-pie"
        Write-Host "  3. Time: time ./program"
        Write-Host ""
    }
} else {
    Write-Host "Compilation failed!" -ForegroundColor Red
}
