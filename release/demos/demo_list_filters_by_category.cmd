::::::::::::::::::::::::::::::::::::::::
:: List Filters by Category Demo
::::::::::::::::::::::::::::::::::::::::
@echo off
cd /d "%~dp0"

:: import CLSID constants
call __clsid.inc.cmd

echo.

echo Audio Compressor:
echo =================
..\x64\dscmd -lc %CLSID_AudioCompressorCategory%
echo.
echo.

echo Audio Input Device:
echo ===================
echo.
..\x64\dscmd -lc %CLSID_AudioInputDeviceCategory%
echo.
echo.

echo Audio Renderer:
echo ===============
echo.
..\x64\dscmd -lc %CLSID_AudioRendererCategory%
echo.
echo.

echo Media Encoder:
echo ==============
echo.
..\x64\dscmd -lc %CLSID_MediaEncoderCategory%
echo.
echo.

echo Media Multiplexer:
echo ==================
echo.
..\x64\dscmd -lc %CLSID_MediaMultiplexerCategory%
echo.
echo.

echo MIDI Renderer:
echo ==============
echo.
..\x64\dscmd -lc %CLSID_MidiRendererCategory%
echo.
echo.

echo Video Compressor:
echo =================
echo.
..\x64\dscmd -lc %CLSID_VideoCompressorCategory%
echo.
echo.

echo Video Input Devices:
echo ====================
echo.
..\x64\dscmd -lc %CLSID_VideoInputDeviceCategory%
echo.
echo.

echo Legacy Filter:
echo ==============
echo.
..\x64\dscmd -lc %CLSID_LegacyAmFilterCategory%
echo.
echo.

pause
