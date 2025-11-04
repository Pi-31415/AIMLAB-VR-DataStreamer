@echo off
REM =====================================================
REM AIMLAB VR - Data Directory Diagnostic
REM 
REM Author: Pi Ko (pi.ko@nyu.edu)
REM Date: 04 November 2025
REM Version: v1.0
REM 
REM Description:
REM Diagnoses issues with the data directory
REM =====================================================

echo.
echo ========================================
echo   Data Directory Diagnostic
echo ========================================
echo.

echo [CHECK 1] Current directory:
cd
echo.

echo [CHECK 2] Does 'data' exist?
if exist "data" (
    echo YES - data exists
) else (
    echo NO - data does not exist
)
echo.

echo [CHECK 3] Is 'data' a directory or file?
if exist "data\" (
    echo data is a DIRECTORY
) else (
    if exist "data" (
        echo data is a FILE (THIS IS THE PROBLEM!)
        echo.
        echo [FIX] Removing file and creating directory...
        del /F "data"
        mkdir "data"
        echo FIXED - data is now a directory
    ) else (
        echo data does not exist - creating directory...
        mkdir "data"
        echo CREATED - data directory created
    )
)
echo.

echo [CHECK 4] Contents of data directory:
if exist "data\" (
    dir /B data
    if errorlevel 1 (
        echo (empty directory)
    )
) else (
    echo Cannot list - data is not a directory
)
echo.

echo [CHECK 5] Can we write to data directory?
echo test > "data\test.txt" 2>nul
if exist "data\test.txt" (
    echo YES - data directory is writable
    del "data\test.txt"
) else (
    echo NO - data directory is NOT writable
)
echo.

echo [CHECK 6] Full path to data:
echo %CD%\data
echo.

echo ========================================
echo   Diagnostic complete!
echo ========================================
echo.

pause

