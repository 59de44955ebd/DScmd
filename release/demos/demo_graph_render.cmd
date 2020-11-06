::::::::::::::::::::::::::::::::::::::::
:: Render Audio Demo
::::::::::::::::::::::::::::::::::::::::
@echo off
cd /d "%~dp0"

echo.
echo This demo plays a MP4 video by creating an explicit graph for the video stream,
echo but lets the system automatically render the audio stream.
echo Enter q or ESC, or close the video window to quit.
echo.

:: import CLSID constants
call ..\__clsid.inc.cmd

..\x64\dscmd -wc "Render Audio Demo" ^
-g ^
%CLSID_LAVSplitterSource%=filters\LAVSplitter.ax;src=..\assets\bbb_360p_10sec.mp4,^
%CLSID_LAVVideoDecoder%=filters\LAVVideo.ax,^
%CLSID_VideoMixingRenderer9%,^
%CLSID_LAVAudioDecoder%=filters\LAVAudio.ax!^
0:1,1:2,0:3!^
3

pause
