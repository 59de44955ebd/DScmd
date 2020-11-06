::::::::::::::::::::::::::::::::::::::::
:: Transcoding to WAV Demo
::::::::::::::::::::::::::::::::::::::::
@echo off
cd /d "%~dp0"

:: import CLSID constants
call ..\__clsid.inc.cmd

echo.
echo ============================================================
echo Transcoding video to WAV - please be patient...
echo ============================================================
echo.

:: make sure that also LAV's DLLs are found
set PATH=filters;%PATH%

..\x64\dscmd -p -nw -q -nc ^
-g ^
%CLSID_LAVSplitterSource%=filters\LAVSplitter.ax;src=..\assets\bbb_360p_10sec.mp4,^
%CLSID_LAVAudioDecoder%=filters\LAVAudio.ax,^
%CLSID_AudioCompressorCategory%="PCM",^
%CLSID_WAVDest%=filters\WavDest.dll,^
%CLSID_FileWriter%;dest=output.wav^
!0:1,1:2,2:3,3:4

echo.
echo.

if "%ERRORLEVEL%"=="0" (
	echo ============================================================
	echo The video was successfully transcoded to file "output.wav"
	echo ============================================================
) else (
	============================================================
	echo ERROR: Transcoding failed.
	============================================================
)

echo.
pause
