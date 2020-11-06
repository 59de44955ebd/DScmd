:: This is not meant to be executed directly, but to be called by tests_win32.cmd and tests_x64.cmd
if "%ASSETS_DIR%"=="" exit /b

:: Tests
call:test_mediainfos
call:test_list_filters
call:test_list_filters_by_cat
call:test_play_midi
call:test_play_mp3
call:test_play_avi_legacy
call:test_play_avi_vmr7
call:test_play_avi_vmr9
call:test_play_avi_evr
call:test_win_rect_style
call:test_fullscreen
call:test_mp4_lav_render_pin
call:test_mp4_lav_full_graph
call:test_transcode_avi_compressor_name
call:test_transcode_avi_compressor_clsid
::call:test_transcode_to_mp3
call:test_transcode_to_wav
call:test_transcode_to_wmv
call:test_dmo_audio_effect
call:test_load_srt

exit /b

::######################################
:: Media Infos
::######################################
:test_mediainfos
dscmd -v 0 -i %ASSETS_DIR%\test.mp3 >nul
call:assert %ERRORLEVEL% "Media Infos"
exit /b

::######################################
:: List Filters
::######################################
:test_list_filters
dscmd -lf >nul
call:assert %ERRORLEVEL% "List Filters"
exit /b

::######################################
:: List Filters by Category
::######################################
:test_list_filters_by_cat
dscmd -lc %CLSID_VideoInputDeviceCategory% >nul
call:assert %ERRORLEVEL% "List Filters by Category"
exit /b

::######################################
:: Play MIDI
::######################################
:test_play_midi
dscmd -v 0 -nw -q -stop 500 -vol -10000 -f %ASSETS_DIR%\test.mid
call:assert %ERRORLEVEL% "Play MIDI File"
exit /b

::######################################
:: Play MP3
::######################################
:test_play_mp3
dscmd -v 0 -nw -q -stop 500 -vol -10000 -f %ASSETS_DIR%\test.mp3
call:assert %ERRORLEVEL% "Play MP3 File"
exit /b

::######################################
:: Play AVI - Legacy Renderer
::######################################
:test_play_avi_legacy
dscmd -v 0 -nw -q -r 0 -stop 500 -vol -10000 -f %ASSETS_DIR%\test.avi
call:assert %ERRORLEVEL% "Play AVI - Legacy Renderer"
exit /b

::######################################
:: Play AVI - VMR7 Renderer
::######################################
:test_play_avi_vmr7
dscmd -v 0 -nw -q -r 1 -stop 500 -vol -10000 -f %ASSETS_DIR%\test.avi
call:assert %ERRORLEVEL% "Play AVI - VMR7 Renderer"
exit /b

::######################################
:: Play AVI - VMR9 Renderer
::######################################
:test_play_avi_vmr9
dscmd -v 0 -nw -q -r 2 -stop 500 -vol -10000 -f %ASSETS_DIR%\test.avi
call:assert %ERRORLEVEL% "Play AVI - VMR9 Renderer"
exit /b

::######################################
:: Play AVI - EVR Renderer
::######################################
:test_play_avi_evr
dscmd -v 0 -nw -q -r 3 -stop 500 -vol -10000 -f %ASSETS_DIR%\test.avi
call:assert %ERRORLEVEL% "Play AVI - EVR Renderer"
exit /b

::######################################
:: Window Rect and Window Style (WS_OVERLAPPED=0)
::######################################
:test_win_rect_style
dscmd -v 0 -q -stop 500 -rect 100,100,300,300 -ws 0 -vol -10000 -f %ASSETS_DIR%\test.avi
call:assert %ERRORLEVEL% "Window Rect and Window Style"
exit /b

::######################################
:: Play Full Screen
::######################################
:test_fullscreen
dscmd -v 0 -q -stop 500 -fs -vol -10000 -f %ASSETS_DIR%\test.avi
call:assert %ERRORLEVEL% "Play Full Screen"
exit /b

::######################################
:: Play MP4 with LAV filters - Render Pin
::######################################
:test_mp4_lav_render_pin
dscmd -v 0 -nw -q -stop 500 ^
-g ^
%CLSID_LAVSplitterSource%=%FILTER_DIR%\LAVSplitter.ax;src=%ASSETS_DIR%\bbb_360p_10sec.mp4,^
%CLSID_LAVVideoDecoder%=%FILTER_DIR%\LAVVideo.ax^
!0:1!1
call:assert %ERRORLEVEL% "Play MP4 - Render Pin"
exit /b

::######################################
:: Play MP4 with LAV filters - Full Graph
::######################################
:test_mp4_lav_full_graph
dscmd -v 0 -nw -q -stop 500 -vol -10000 ^
-g ^
%CLSID_LAVSplitterSource%=%FILTER_DIR%\LAVSplitter.ax;src=%ASSETS_DIR%\bbb_360p_10sec.mp4,^
%CLSID_LAVVideoDecoder%=%FILTER_DIR%\LAVVideo.ax,^
%CLSID_VideoRendererDefault%,^
%CLSID_LAVAudioDecoder%=%FILTER_DIR%\LAVAudio.ax,^
%CLSID_DefaultDirectSoundDevice%!0:1,1:2,0:3,3:4
call:assert %ERRORLEVEL% "Play MP4 - Full Graph"
exit /b

::######################################
:: Transcode MP4 to MJPEG AVI
::######################################
:test_transcode_avi_compressor_name
dscmd -v 0 -nw -q -stop 500 -nc ^
-g ^
%CLSID_LAVSplitterSource%=%FILTER_DIR%\LAVSplitter.ax;src=%ASSETS_DIR%\bbb_360p_10sec.mp4,^
%CLSID_LAVVideoDecoder%=%FILTER_DIR%\LAVVideo.ax,^
%CLSID_VideoCompressorCategory%="MJPEG Compressor";qual=1.0,^
%CLSID_AVIMux%,^
%CLSID_FileWriter%;dest=output.avi!^
0:1,1:2,2:3,3:4
call:assert %ERRORLEVEL% "Transcode to AVI - Compressor by Name"
del output.avi 2>nul
exit /b

::######################################
:: Transcode MP4 to MJPEG AVI
::######################################
:test_transcode_avi_compressor_clsid
dscmd -v 0 -nw -q -stop 500 -nc ^
-g ^
%CLSID_LAVSplitterSource%=%FILTER_DIR%\LAVSplitter.ax;src=%ASSETS_DIR%\bbb_360p_10sec.mp4,^
%CLSID_LAVVideoDecoder%=%FILTER_DIR%\LAVVideo.ax,^
%CLSID_MJPEGCompressor%;qual=1.0,^
%CLSID_AVIMux%,^
%CLSID_FileWriter%;dest=output.avi!^
0:1,1:2,2:3,3:4
call:assert %ERRORLEVEL% "Transcode to AVI - Compressor by CLSID"
del output.avi 2>nul
exit /b

::######################################
:: Transcode MP4 to MP3
::######################################
:test_transcode_to_mp3
dscmd -v 0 -nw -q -stop 500 -nc ^
-g ^
%CLSID_LAVSplitterSource%=%FILTER_DIR%\LAVSplitter.ax;src=%ASSETS_DIR%\bbb_360p_10sec.mp4,^
%CLSID_LAVAudioDecoder%=%FILTER_DIR%\LAVAudio.ax,^
%CLSID_AudioCompressorCategory%="MPEG Layer-3",^
%CLSID_Dump%=%FILTER_DIR%\Dump.dll;dest=output.mp3!^
0:1,1:2,2:3
call:assert %ERRORLEVEL% "Transcode to MP3"
del output.mp3 2>nul
exit /b

::######################################
:: Transcode MP4 to WAV
::######################################
:test_transcode_to_wav
dscmd -v 0 -nw -q -stop 500 -nc ^
-g ^
%CLSID_LAVSplitterSource%=%FILTER_DIR%\LAVSplitter.ax;src=%ASSETS_DIR%\bbb_360p_10sec.mp4,^
%CLSID_LAVAudioDecoder%=%FILTER_DIR%\LAVAudio.ax,^
%CLSID_AudioCompressorCategory%="PCM",^
%CLSID_WAVDest%=%FILTER_DIR%\WavDest.dll,^
%CLSID_FileWriter%;dest=output.wav^
!0:1,1:2,2:3,3:4
call:assert %ERRORLEVEL% "Transcode to WAV"
del output.wav 2>nul
exit /b

::######################################
:: Transcode MP4 to WMV
::######################################
:test_transcode_to_wmv
dscmd -v 0 -nw -q -stop 500 -nc ^
-g ^
%CLSID_LAVSplitterSource%=%FILTER_DIR%\LAVSplitter.ax;src=%ASSETS_DIR%\bbb_360p_10sec.mp4,^
%CLSID_LAVVideoDecoder%=%FILTER_DIR%\LAVVideo.ax,^
%CLSID_LAVAudioDecoder%=%FILTER_DIR%\LAVAudio.ax,^
%CLSID_WMASFWriter%;dest=output.wmv!^
0:1,0:2,2:3,1:3
call:assert %ERRORLEVEL% "Transcode to WMV"
del output.wmv 2>nul
exit /b

::######################################
:: DMO Echo effect
::######################################
:test_dmo_audio_effect
dscmd -v 0 -q -nw -stop 500 -vol -10000 ^
-g ^
%CLSID_LAVSplitterSource%=%FILTER_DIR%\LAVSplitter.ax;src=%ASSETS_DIR%\bbb_360p_10sec.mp4,^
%CLSID_LAVAudioDecoder%=%FILTER_DIR%\LAVAudio.ax,^
%CLSID_DMOCATEGORY_AUDIO_EFFECT%=%CLSID_DMOCATEGORY_AUDIO_EFFECT_ECHO%,^
%CLSID_DefaultDirectSoundDevice%!0:1,1:2,2:3
call:assert %ERRORLEVEL% "DMO Echo effect"
exit /b

::######################################
:: DMO Echo effect
::######################################
:test_load_srt
dscmd -v 0 -q -nw -stop 500 -vol -10000 ^
-g ^
%CLSID_LAVSplitterSource%=%FILTER_DIR%\LAVSplitter.ax;src=%ASSETS_DIR%\bbb_360p_10sec.mp4,^
%CLSID_LAVVideoDecoder%=%FILTER_DIR%\LAVVideo.ax,^
%CLSID_VSFilter%=%FILTER_DIR%\VSFilter.dll;sub=%ASSETS_DIR%\test.srt,^
%CLSID_VideoRendererDefault%,^
%CLSID_LAVAudioDecoder%=%FILTER_DIR%\LAVAudio.ax,^
%CLSID_DefaultDirectSoundDevice%^
!0:1,1:2,2:3,0:4,4:5
call:assert %ERRORLEVEL% "Load SRT"
exit /b

::######################################
:: FUNCTION assert
::######################################
:assert
if "%1"=="0" (
..\..\helper\bg Print A "%~2: OK\n"
) else (
..\..\helper\bg Print C "%~2: FAILED\n"
)
exit /b
