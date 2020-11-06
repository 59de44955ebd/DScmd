@echo off
cd /d "%~dp0"

set ASSETS_DIR=..\assets
set FILTER_DIR=filters
set PATH=%FILTER_DIR%;%PATH%

:: import CLSID constants
call ..\__clsid.inc.cmd

echo Running tests for dscmd.exe - Win32 (32-bit)
echo.

:: run all tests
call ..\__tests.cmd

echo.
pause
exit /b
