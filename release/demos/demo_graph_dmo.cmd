::::::::::::::::::::::::::::::::::::::::
:: DMO Demo
::::::::::::::::::::::::::::::::::::::::
@echo off
cd /d "%~dp0"

echo.
echo This demo plays a MP4's audio track, adding the DMO Echo effect.
echo The needed filters are loaded at runtime, so no global filter registration needed.
echo Enter q or ESC to quit.
echo.

:: import CLSID constants
call __clsid.inc.cmd

:: make sure that also LAV's DLLs are found
set PATH=filters;%PATH%

..\x64\dscmd -k -nw -loop ^
-g ^
%CLSID_LAVSplitterSource%=filters\LAVSplitter.ax;src=assets\bbb_360p_10sec.mp4,^
%CLSID_LAVAudioDecoder%=filters\LAVAudio.ax,^
%CLSID_DMOCATEGORY_AUDIO_EFFECT%=%CLSID_DMOCATEGORY_AUDIO_EFFECT_ECHO%,^
%CLSID_DefaultDirectSoundDevice%!0:1,1:2,2:3

echo.
pause
