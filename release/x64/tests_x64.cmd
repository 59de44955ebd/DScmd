@echo off
cd /d "%~dp0"

set ASSETS_DIR=..\assets
set FILTER_DIR=filters
set PATH=%FILTER_DIR%;%PATH%

:: import CLSID constants
call ..\__clsid.inc.cmd

..\..\helper\bg Print E "\nRunning tests for dscmd.exe - x64 (64-bit)\n\n"

:: run all tests
call ..\__tests.cmd

echo.
pause
exit /b
