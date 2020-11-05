::::::::::::::::::::::::::::::::::::::::
:: Play Graph Fullscreen Demo
::::::::::::::::::::::::::::::::::::::::
@echo off
cd /d "%~dp0"

:: import CLSID constants
call __clsid.inc.cmd

..\x64\dscmd -fs -hc -q ^
-g ^
%CLSID_LAVSplitterSource%=filters\LAVSplitter.ax;src=assets\bbb_360p_10sec.mp4,^
%CLSID_LAVVideoDecoder%=filters\LAVVideo.ax,^
%CLSID_VideoMixingRenderer9%,^
%CLSID_LAVAudioDecoder%=filters\LAVAudio.ax,^
%CLSID_DefaultDirectSoundDevice%!^
0:1,1:2,0:3,3:4

pause
