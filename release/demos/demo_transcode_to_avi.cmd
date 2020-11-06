::::::::::::::::::::::::::::::::::::::::
:: Transcoding to AVI Demo
::::::::::::::::::::::::::::::::::::::::
@echo off
cd /d "%~dp0"

:: import CLSID constants
call ..\__clsid.inc.cmd

echo.
echo This demo transcodes a sample MP4 file to AVI.
echo.

echo Available Codecs:
echo =================
echo.
..\x64\dscmd -lc %CLSID_VideoCompressorCategory%
echo.
echo.

echo Please select a codec from the list, by entering its full name (as listed in the second column).
echo (Note: some might be incompatible with the AVI container)
echo.
set /p codec="Selected codec: "

echo.
echo ============================================================
echo Transcoding video to AVI with codec %codec% - please be patient...
echo ============================================================
echo.

:: make sure that also LAV's DLLs are found
set PATH=filters;%PATH%

..\x64\dscmd -p -q -nw ^
-g ^
%CLSID_LAVSplitterSource%=filters\LAVSplitter.ax;src=..\assets\bbb_360p_10sec.mp4,^
%CLSID_LAVVideoDecoder%=filters\LAVVideo.ax,^
%CLSID_VideoCompressorCategory%="%codec%";qual=1.0,^
%CLSID_AVIMux%,^
%CLSID_FileWriter%;dest=output.avi!^
0:1,1:2,2:3,3:4

echo.
echo.

if "%ERRORLEVEL%"=="0" (
	echo ============================================================
	echo The video was successfully transcoded to file "output.avi"
	echo ============================================================
) else (
	============================================================
	echo ERROR: Transcoding failed.
	============================================================
)

echo.
pause
