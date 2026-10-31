@echo off
chcp 65001 > nul
echo ========================================================
echo   BIEN DICH HE THONG MO PHONG ^& TOI UU CHI PHI DA DAM MAY
echo ========================================================
echo.

set GXX=g++
where g++ >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo [LOI] Khong tim thay g++ trong PATH. Hay cai dat MinGW / GCC!
    pause
    exit /b 1
)

echo Dang bien dich toan bo chuong trinh tich hop...
g++ -std=c++17 -Wall -Wextra ^
    main.cpp ^
    tv2/PricingAdapters.cpp ^
    tv3/CostCalculator.cpp ^
    tv3/SimulationEngine.cpp ^
    tv3/MultiCloudOptimizationService.cpp ^
    tv3/TestSuite.cpp ^
    tv4/CheapestProviderStrategy.cpp ^
    tv4/RightSizingStrategy.cpp ^
    tv4/PricingModelStrategy.cpp ^
    tv5/ConsoleUI.cpp ^
    -o multi_cloud_optimizer.exe

if %ERRORLEVEL% equ 0 (
    echo [THANH CONG] Da tao multi_cloud_optimizer.exe!
    echo Chay chuong trinh: .\multi_cloud_optimizer.exe
) else (
    echo [LOI] Bien dich that bai!
    pause
    exit /b 1
)
