::::::::::::::::::::::::::::::::::::::::
:: SpoutCam Demo
::::::::::::::::::::::::::::::::::::::::
@echo off
cd /d "%~dp0"

:: import CLSID constants
call __clsid.inc.cmd

echo.
echo This demo loads and renders the virtual camera/Spout receiver "SpoutCam".
echo Enter q or ESC, or close the video window to quit.
echo.

..\x64\dscmd ^
-g ^
%CLSID_SpoutCam%=filters\SpoutCam64.ax,%CLSID_VideoMixingRenderer9%!!0

echo.
pause
