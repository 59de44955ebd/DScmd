::::::::::::::::::::::::::::::::::::::::
:: SpoutCam Demo
::::::::::::::::::::::::::::::::::::::::
@echo off
cd /d "%~dp0"

echo.
echo This demo loads and renders the virtual camera "SpoutCam".
echo Enter q or ESC, or close the video window to quit.
echo.

:: import CLSID constants
call __clsid.inc.cmd

..\x64\dscmd -wc "SpoutCam Demo" ^
-g ^
%CLSID_SpoutCam%=filters\SpoutCam64.ax,^
%CLSID_ColorSpaceConverter%,^
%CLSID_VideoMixingRenderer9%!^
0:1,1:2

echo.
pause
