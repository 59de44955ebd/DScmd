::::::::::::::::::::::::::::::::::::::::
:: Transcoding to MP3 Demo
::::::::::::::::::::::::::::::::::::::::
@echo off
cd /d "%~dp0"

:: import CLSID constants
call ..\__clsid.inc.cmd

echo.
echo ============================================================
echo Transcoding WAV to MP3 - please be patient...
echo ============================================================
echo.

..\x64\dscmd -p -nw -q ^
-g ^
%CLSID_AsyncReader%;src=..\assets\test.wav,^
%CLSID_WaveParser%,^
%CLSID_LAMEAudioEncoder%=filters\lame.ax;dialog,^
%CLSID_FileWriter%;dest=output.mp3!^
0:1,1:2,2:3

echo.
echo.

if "%ERRORLEVEL%"=="0" (
	echo ============================================================
	echo The WAV was successfully transcoded to file "output.mp3"
	echo ============================================================
) else (
	============================================================
	echo ERROR: Transcoding failed.
	============================================================
)

echo.
pause
