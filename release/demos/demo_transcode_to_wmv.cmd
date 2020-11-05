::::::::::::::::::::::::::::::::::::::::
:: Transcoding to WMV Demo
::::::::::::::::::::::::::::::::::::::::
@echo off
cd /d "%~dp0"

:: import CLSID constants
call __clsid.inc.cmd

echo.
echo ============================================================
echo Transcoding video to WMV - please be patient...
echo ============================================================
echo.

:: make sure that also LAV's DLLs are found
set PATH=filters;%PATH%

..\x64\dscmd -p -nw -q ^
-g ^
%CLSID_LAVSplitterSource%=filters\LAVSplitter.ax;src=assets\bbb_360p_10sec.mp4,^
%CLSID_LAVVideoDecoder%=filters\LAVVideo.ax,^
%CLSID_LAVAudioDecoder%=filters\LAVAudio.ax,^
%CLSID_WMASFWriter%;dest=output.wmv;dialog!^
0:1,0:2,2:3,1:3

echo.
echo.

if "%ERRORLEVEL%"=="0" (
	echo ============================================================
	echo The video was successfully transcoded to file "output.wmv"
	echo ============================================================
) else (
	============================================================
	echo ERROR: Transcoding failed.
	============================================================
)

echo.
pause
