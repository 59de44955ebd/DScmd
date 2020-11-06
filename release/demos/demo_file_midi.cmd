::::::::::::::::::::::::::::::::::::::::
:: Play MIDI Demo
::::::::::::::::::::::::::::::::::::::::
@echo off
cd /d "%~dp0"

echo.
echo This demo plays a MIDI file at double speed, using the system's default graph.
echo Enter q or ESC to quit
echo.

..\x64\dscmd -k -nw -rate 2.0 -f ..\assets\test.mid

echo.
pause
