::::::::::::::::::::::::::::::::::::::::
:: Screen Capture Recording Demo
::::::::::::::::::::::::::::::::::::::::
@echo off
cd /d "%~dp0"

echo.
echo This demo uses virtual camera "ScreenCam" to capture
echo the screen session as AVI file "capture.avi", using codec MJPEG.
echo Hit q or ESC to stop the recording.
echo.

:: import CLSID constants
call __clsid.inc.cmd

:: =filters\ScreenCam.dll
..\x64\dscmd -k -nw ^
-g ^
%CLSID_ScreenCam%,^
%CLSID_MJPEGCompressor%,^
%CLSID_AVIMux%,^
%CLSID_FileWriter%;dest=capture.avi!^
0:1,1:2,2:3

echo.
pause
