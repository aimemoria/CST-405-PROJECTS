# Comprehensive Benchmark Script
# Runs all test files and measures compilation performance

Write-Host ""
Write-Host "+==========================================================+" -ForegroundColor Cyan
Write-Host "|        COMPILER PERFORMANCE BENCHMARK SUITE              |" -ForegroundColor Cyan
Write-Host "+==========================================================+" -ForegroundColor Cyan
Write-Host ""

# Get all test files
$testFiles = Get-ChildItem -Filter "test_*.c" | Sort-Object Name

if ($testFiles.Count -eq 0) {
    Write-Host "No test files found!" -ForegroundColor Red
    exit 1
}

Write-Host "Found $($testFiles.Count) test files" -ForegroundColor Green
Write-Host ""

$results = @()

foreach ($testFile in $testFiles) {
    Write-Host "Testing: $($testFile.Name)" -ForegroundColor Yellow
    Write-Host ("-" * 60)
    
    # Measure compilation time
    $startTime = Get-Date
    & ".\compiler.exe" $testFile.Name 2>&1 | Out-Null
    $endTime = Get-Date
    $elapsed = ($endTime - $startTime).TotalMilliseconds
    
    $result = [PSCustomObject]@{
        TestFile = $testFile.Name
        FileSize = $testFile.Length
        CompileTime = $elapsed
        Status = if ($LASTEXITCODE -eq 0) { "SUCCESS" } else { "FAILED" }
    }
    
    $results += $result
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "  [OK] Compiled in $([math]::Round($elapsed, 2)) ms" -ForegroundColor Green
    } else {
        Write-Host "  [FAIL] Compilation failed" -ForegroundColor Red
    }
    Write-Host ""
}

# Generate summary report
Write-Host ""
Write-Host "+==========================================================+" -ForegroundColor Cyan
Write-Host "|                   BENCHMARK SUMMARY                      |" -ForegroundColor Cyan
Write-Host "+==========================================================+" -ForegroundColor Cyan
Write-Host ""

$results | Format-Table -AutoSize

$successCount = ($results | Where-Object { $_.Status -eq "SUCCESS" }).Count
$totalTime = ($results | Measure-Object -Property CompileTime -Sum).Sum
$avgTime = if ($successCount -gt 0) { $totalTime / $successCount } else { 0 }

Write-Host ""
Write-Host "Total tests: $($results.Count)"
Write-Host "Successful: $successCount" -ForegroundColor Green
Write-Host "Failed: $($results.Count - $successCount)" -ForegroundColor $(if (($results.Count - $successCount) -eq 0) { "Green" } else { "Red" })
Write-Host "Total compilation time: $([math]::Round($totalTime, 2)) ms"
Write-Host "Average compilation time: $([math]::Round($avgTime, 2)) ms"
Write-Host ""

# Save results to CSV
$results | Export-Csv -Path "benchmark_results.csv" -NoTypeInformation
Write-Host "Results saved to: benchmark_results.csv" -ForegroundColor Cyan
