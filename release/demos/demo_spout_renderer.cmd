::::::::::::::::::::::::::::::::::::::::
:: SpoutRenderer Demo
::::::::::::::::::::::::::::::::::::::::
@echo off
cd /d "%~dp0"

:: import CLSID constants
call __clsid.inc.cmd

echo.
echo This demo plays a MP4 video, and at the same time shares it as OpenGL texture using filter "SpoutRenderer".
echo To demonstrate the texture sharing, a second player is opened that receives the texture using SpoutCam.
echo Enter q or ESC, or close the video window to quit.
echo.

:: first open SpoutCam
start /MIN demo_spout_cam.cmd

:: make sure that also LAV's DLLs are found
set PATH=filters;%PATH%

:: now play the SpoutRenderer graph
..\x64\dscmd -wc "SpoutRenderer Demo" ^
-g ^
%CLSID_LAVSplitterSource%=filters\LAVSplitter.ax;src=assets\bbb_360p_10sec.mp4,^
%CLSID_LAVVideoDecoder%=filters\LAVVideo.ax,^
%CLSID_InfinitePinTeeFilter%,^
%CLSID_ColorSpaceConverter%,^
%CLSID_VideoMixingRenderer9%,^
%CLSID_SpoutRenderer%=filters\SpoutRenderer.ax^
!0:1,1:2,2:3,3:4,2:5

echo.
pause
