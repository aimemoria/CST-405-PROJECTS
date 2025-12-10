# Performance Measurement Script for Compiler
# Measures compilation time for test programs

param(
    [Parameter(Mandatory=$true)]
    [string]$InputFile,
    
    [string[]]$CompilerFlags = @()
)

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  COMPILER PERFORMANCE MEASUREMENT" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Input File: $InputFile"
Write-Host "Compiler Flags: $($CompilerFlags -join ' ')"
Write-Host ""

# Start timing
$startTime = Get-Date

# Run the compiler
$compilerArgs = @($InputFile) + $CompilerFlags
& ".\compiler.exe" @compilerArgs

# End timing
$endTime = Get-Date
$elapsed = ($endTime - $startTime).TotalSeconds

Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host "  PERFORMANCE RESULTS" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host "Total Compilation Time: $([math]::Round($elapsed, 4)) seconds" -ForegroundColor Yellow
Write-Host ""

# Return the elapsed time
return $elapsed
