::::::::::::::::::::::::::::::::::::::::
:: Subtitles Demo (using DirectVobSub/VSFilter.dll)
::::::::::::::::::::::::::::::::::::::::
@echo off
cd /d "%~dp0"

echo.
echo This demo plays a MP4 video and loads a (dummy) SRT subtitles file.
echo The needed filters are loaded at runtime, so no global filter registration needed.
echo Enter q or ESC, or close the video window to quit.
echo.

:: import CLSID constants
call ..\__clsid.inc.cmd

:: make sure that also LAV's DLLs are found
set PATH=filters;%PATH%

set FILTER_DIR=..\x64\filters

..\x64\dscmd -wc "Subtitles Demo" -loop ^
-g ^
%CLSID_LAVSplitterSource%=%FILTER_DIR%\LAVSplitter.ax;src=..\assets\bbb_360p_10sec.mp4,^
%CLSID_LAVVideoDecoder%=%FILTER_DIR%\LAVVideo.ax,^
%CLSID_VSFilter%=%FILTER_DIR%\VSFilter.dll;sub=..\assets\test.srt,^
%CLSID_VideoRendererDefault%,^
%CLSID_LAVAudioDecoder%=%FILTER_DIR%\LAVAudio.ax,^
%CLSID_DefaultDirectSoundDevice%^
!0:1,1:2,2:3,0:4,4:5

echo.
pause
