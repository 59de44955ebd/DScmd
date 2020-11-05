::::::::::::::::::::::::::::::::::::::::
:: ScreenCam Demo
::::::::::::::::::::::::::::::::::::::::
@echo off
cd /d "%~dp0"

:: import CLSID constants
call __clsid.inc.cmd

echo.
echo This demo loads and renders the virtual camera "ScreenCam".
echo Enter q or ESC, or close the video window to quit.
echo.

..\x64\dscmd -g %CLSID_ScreenCam%=filters\ScreenCam.dll,%CLSID_VideoMixingRenderer9%!!0

echo.
pause
