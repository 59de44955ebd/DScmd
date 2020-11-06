::::::::::::::::::::::::::::::::::::::::
:: Legacy Renderer Demo
::::::::::::::::::::::::::::::::::::::::
@echo off
cd /d "%~dp0"

echo.
echo This demo plays a MP4 video by creating an explicit graph (using Legacy VideoRenderer).
echo The needed filters are loaded at runtime, so no global filter registration needed.
echo Enter q or ESC, or close the video window to quit.
echo.

:: import CLSID constants
call ..\__clsid.inc.cmd

:: make sure that also LAV's DLLs are found
set PATH=filters;%PATH%

..\x64\dscmd -wc "Legacy VideoRenderer Demo" ^
-g ^
%CLSID_LAVSplitterSource%=filters\LAVSplitter.ax;src=..\assets\bbb_360p_10sec.mp4,^
%CLSID_LAVVideoDecoder%=filters\LAVVideo.ax,^
%CLSID_VideoRenderer%,^
%CLSID_LAVAudioDecoder%=filters\LAVAudio.ax,^
%CLSID_DefaultDirectSoundDevice%!0:1,1:2,0:3,3:4

echo.
pause
