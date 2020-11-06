::::::::::::::::::::::::::::::::::::::::
:: Transcoding to MP3 Demo
::::::::::::::::::::::::::::::::::::::::
@echo off
cd /d "%~dp0"

:: import CLSID constants
call ..\__clsid.inc.cmd

echo.
echo ============================================================
echo Transcoding video to MP3 - please be patient...
echo ============================================================
echo.

:: make sure that also LAV's DLLs are found
set PATH=filters;%PATH%

..\x64\dscmd -p -nw -q ^
-g ^
%CLSID_LAVSplitterSource%=filters\LAVSplitter.ax;src=..\assets\bbb_360p_10sec.mp4,^
%CLSID_LAVAudioDecoder%=filters\LAVAudio.ax,^
%CLSID_AudioCompressorCategory%="MPEG Layer-3";dialog,^
%CLSID_Dump%=filters\Dump.dll;dest=output.mp3!^
0:1,1:2,2:3

echo.
echo.

if "%ERRORLEVEL%"=="0" (
	echo ============================================================
	echo The video was successfully transcoded to file "output.mp3"
	echo ============================================================
) else (
	============================================================
	echo ERROR: Transcoding failed.
	============================================================
)

echo.
pause
