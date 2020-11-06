::::::::::::::::::::::::::::::::::::::::
:: Play MP4 Demo
::::::::::::::::::::::::::::::::::::::::
@echo off
cd /d "%~dp0"

echo.
echo This demo only works if needed filters to play AVC/AAC MP4 are registered in the system.
echo If not, you might first run "__register_lav_run_as_admin.cmd" in the "filters folder.
echo Enter q or ESC, or close the video window to quit.
echo.

..\x64\dscmd -wc "Play MP4 Demo" -f ..\assets\bbb_360p_10sec.mp4

echo.
pause
