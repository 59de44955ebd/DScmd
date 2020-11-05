::::::::::::::::::::::::::::::::::::::::
:: VMR-9 URL Demo
::::::::::::::::::::::::::::::::::::::::
@echo off
cd /d "%~dp0"

echo.
echo This demo plays a video loaded via HTTPS by creating an explicit graph (using VMR-9 as renderer).
echo The needed filters are loaded at runtime, so no global filter registration needed.
echo Enter q or ESC, or close the video window to quit.
echo.

:: import CLSID constants
call __clsid.inc.cmd

:: make sure that also LAV's DLLs are found
set PATH=filters;%PATH%

..\x64\dscmd -wc "VMR-9 URL Demo" ^
-g ^
%CLSID_LAVSplitterSource%=filters\LAVSplitter.ax;src=https://valentin.dasdeck.com/tmp/test.mp4,^
%CLSID_LAVVideoDecoder%=filters\LAVVideo.ax,^
%CLSID_VideoMixingRenderer9%,^
%CLSID_LAVAudioDecoder%=filters\LAVAudio.ax,^
%CLSID_DefaultDirectSoundDevice%!^
0:1,1:2,0:3,3:4

echo.
pause
