# DScmd
DirectShow command-line player

```
Usage:
======

DScmd -h                         Print this help text and exit
DScmd -i <file>                  Print media infos and exit
DScmd -lf                        List Filters: list all registered DirectShow filters (CLSID and name) and exit
DScmd -lc <clsid>                List Category: list registered DirectShow filters of specified filter category and exit
DScmd <file>                     Play media file with system defaults
DScmd -f <file> <options>        Play media file with system defaults, using specified options
DScmd -g <graph> <options>       Create and play specified DirectShow filter graph (see details below)
DScmd -grf <file> <options>      Load and play a GRF file


Options:
========

-k                               Activate keyboard control in CMD window (see key list below)
-v <int verbosity>               Specify verbosity : 0=quiet, 1=errors, 2=warnings, 3=infos (default=3)
-r <int renderer>                Specifiy video renderer: 0=Legacy renderer, 1=VMR7, 2=VMR9, 3=EVR (default=2)
-nw                              No Window: Don't show a window (e.g. when playing only audio files)
-nc                              No Clock: Deactivate clock (render as fast as possible
-p                               Print progress status updates (once per second)
-q                               Quit after playback
-fs                              Start in full screen mode
-top                             Keep window always on top
-hc                              Hide mouse cursor (window mode only)
-loop                            Loop forever
-rect <left,top,width,height>    Specify rect of video window
-wc <string>                     Window Caption: specify custom caption (title) of video window (default=\"DScmd\")
-ws <int style>                  Window Style: specify custom style of video window (default=WS_TILEDWINDOW)
-rate <float rate>               Specify playback rate (default=1.0)
-start <int milliseconds>        Specify the start time in media file
-stop <int milliseconds>         Specify the stop time in media file
-vol <int volume>                Specify the audio volume (-10000=silent, 0=maximum)
-bal <int balance>               Specify the audio balance (-10000 to 10000)
-reg                             Register rendered graph in system, so other DirectShow apps can read it
-save <grf file>                 Save rendered graph as GRF file


Graph string:
=============

A graph string has the form "<filter list>!<connect list>!<render list>", where <connect list> and <render list>
are optional. <connect list> can also be empty, i.e. you can use <filter list>!!<render list>.

Example (note: actually a single line, "^" only works in a batch script):

dscmd -g ^
{B98D13E7-55DB-4385-A33D-09FD1BA26338}=filters\LAVSplitter.ax,;src=big_buck_bunny.mp4^
{EE30215D-164F-4A92-A4EB-9D4C13390F9F}=filters\LAVVideo.ax,^
{51B4ABF3-748F-4E3B-A276-C828330E926A},^
{E8E73B6B-4CB3-44A4-BE99-4F7BCB96E491}=filters\LAVAudio.ax!^
0:1,1:2,0:3!^
3

The above example string does the following:
It creates a graph based on the filters LAV Splitter Source, LAV Video Decoder, LAV Audio Decoder and VMR-9.
The 3 LAV filters are dynamically loaded from provided binary DLL files.
Splitter, video decoder and VMR-9 are connected explicitely, whereas the audio decoder is rendered by the system.


Filter list:
============

A list of filter items, separated by comma.
Each item starts with a CLSID, optionally followed by "=<additional info>", optionally followed by option strings, separated by semicolon.

Additional infos are:
<filter CLSID>=<filter DLL>      Allows to dynamically load non-registered filters directly from AX/DLL file
<filter category CLSID>=<name>   Allows to specify VCM/ACM codecs, video/audio input devices etc. by name
<DMO category CLSID>=<CLSID>     Allows to load DMOs specified by CLSID into the DMOWrapper filter

Available options are:
src=<file/URL>                   Media file or URL, for source filters only
dest=<file>                      Destination file, for sink filters only
sub=<file>                       Load this (e.g. SRT or SUB) subtitles file into VSFilter/DirectVobSub filter
qual=<float>                     Allows to specify the compression quality (in range 0.0 to 1.0) for a audio/video compressor filter
dialog                           Show the filter's settings dialog, if available
pindialog                        Show the filter's output pin settings dialog, if available


Connect list:
=============

A list of integer pairs, separated by comma.
Each pair has the format <int a>:<int b>, which means that the 2 filters with index a and b should be connected.
The indexes refer to the positions in the list that was passed as <filter list>.


Render list:
============

A list of integers, separated by comma, which specify the indexes of filters the DirectShow system should try to
render automatically. The indexes refer to the positions in the list that was passed as <filter-list>.


Keyboard control:
=================

Note: in window mode (default) keystrokes are accepted by the window. Use -k to activate keyboard control in CMD shell.

+/-                              Double/half playback rate (if supported by loaded graph)
0-9                              Show config dialog for <n>th filter in graph (if supported by filter)
LEFT/RIGHT                       Seek 100 ms backwards/forward (if supported by loaded graph)
PAGEUP/PAGEDOWN                  Seek 1 second backwards/forward (if supported by loaded graph)
UP/DOWN                          Volume up/down (10% step)
c                                Toggle hide/show cursor (window mode only)
d                                Toggle CC/subs visibility (if a CC/subs file is loaded via VSFilter/DirectVobSub)
f                                Toggle full screen (if a video is loaded)
h                                Print help text
i                                Show media infos
l                                Print list of filters (CLSID and name) in current filter graph
m                                Mute (set volume to -10000)
o                                Load media file via open file dialog
p, SPACE                         Toggle pause/play
q, ESC                           Quit
r                                Rewind
t                                Make window stay-on-top (window mode only)
u                                Load subtitle file via open file dialog (if VSFilter/DirectVobSub in filter graph)
y/x                              Step audio balance to left/right channel (5% step, if supported by graph)

```