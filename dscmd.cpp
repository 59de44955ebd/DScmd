#include <stdio.h>

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <utility>
#include <cstdarg>

#include "dscmd.h"
#include "utils.h"

#include "version.h"

// DirectVobSub/VSFilter
#include "VSFilter/IDirectVobSub.h"

#define EXIT(X) {exitCode = X; goto clean_up_and_exit;}

#define IDT_CHECK_PROGRESS 100

// EXIT CODES
enum
{
	ERROR_NONE,
	ERROR_INITWINDOW_FAILED,
	ERROR_COCREATEINSTANCE_FAILED,
	ERROR_NO_MEDIACONTROL,
	ERROR_NO_MEDIASEEKING,
	ERROR_LOAD_GRAPH_FAILED,
	ERROR_RENDER_FAILED,
	ERROR_CONNECTING_FILTERS_FAILED,
	ERROR_RENDER_FILTER_FAILED
};

//LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//######################################
// Globals
//######################################
HRESULT hr;
BOOL ok;

HINSTANCE g_hInst;
HWND g_hWnd;

WCHAR g_szTitle[] = L"DScmd";
WCHAR g_szWindowClass[] = L"DSCMD";

BOOL g_fullscreen = FALSE;
RECT g_rect;
LONGLONG g_starttime = 0;
BOOL g_bLoop = FALSE;
BOOL g_bProgress = FALSE;
LONGLONG g_duration = 0;

UINT g_logLevel = logInfo;

IVideoWindow * g_pVideoWindow = NULL; // LegacyRenderer
IVMRWindowlessControl * g_pIVMRWindowlessControl = NULL; // VMR7
IVMRWindowlessControl9 * g_pIVMRWindowlessControl9 = NULL; // VMR9 = Default
IMFVideoDisplayControl * g_pIMFVideoDisplayControl = NULL; // EVR
UINT g_iRenderer = 2;

long g_lAspectX=0, g_lAspectY=0;

IBasicAudio * g_pBasicAudio = NULL;
IMediaEventEx * g_pMediaEvent = NULL;
IMediaSeeking * g_pMediaSeeking = NULL;
IMediaControl * g_pMediaControl = NULL;

//IFilterGraph * g_pFilterGraph = NULL;
IGraphBuilder * g_pGraphBuilder = NULL;

HANDLE g_hStdin = NULL;
HANDLE g_hThread = NULL;
//BOOL g_bConsoleCloseFlag = FALSE;

BOOL g_bQuitWhenComplete = FALSE;

BOOL g_bHasGraph = FALSE;

DWORD g_dwStyle = WS_OVERLAPPEDWINDOW;

//######################################
//
//######################################
BOOL CmdOptionExists(WCHAR** begin, WCHAR** end, const std::wstring& option)
{
	return std::find(begin, end, option) != end;
}

//######################################
//
//######################################
WCHAR * CmdOptionGet(WCHAR ** begin, WCHAR ** end, const std::wstring & option)
{
	WCHAR ** itr = std::find(begin, end, option);
	if (itr != end && ++itr != end)
	{
		return *itr;
	}
	return 0;
}

//######################################
//
//######################################
std::vector<std::wstring> Explode(std::wstring const & s, WCHAR delim)
{
	std::vector<std::wstring> result;
	std::wistringstream iss(s);
	for (std::wstring token; std::getline(iss, token, delim); )
	{
		result.push_back(std::move(token));
	}
	return result;
}

//######################################
//
//######################################
HRESULT InitVmr9(IBaseFilter *pVmr9)
{
	IVMRFilterConfig9 * pIVMRFilterConfig9;
	hr = pVmr9->QueryInterface(IID_IVMRFilterConfig9, reinterpret_cast<void**>(&pIVMRFilterConfig9));
	DBGHR("QueryInterface(IID_IVMRFilterConfig9)");
	if (SUCCEEDED(hr))
	{
		hr = pIVMRFilterConfig9->SetRenderingMode(VMR9Mode_Windowless);
		DBGHR("pIVMRFilterConfig9->SetRenderingMode");
		pIVMRFilterConfig9->Release();
	}

	hr = pVmr9->QueryInterface(IID_IVMRWindowlessControl9, reinterpret_cast<void**>(&g_pIVMRWindowlessControl9));
	DBGHR("QueryInterface(IID_IVMRWindowlessControl9)");
	if (SUCCEEDED(hr))
	{
		RECT cr;
		GetClientRect(g_hWnd, &cr);
		RECT r = { 0, 0, cr.right - cr.left, cr.bottom - cr.top };
		hr = g_pIVMRWindowlessControl9->SetVideoPosition(NULL, &r);
		hr = g_pIVMRWindowlessControl9->SetVideoClippingWindow(g_hWnd);
	}

	IVMRAspectRatioControl9 * pIVMRAspectRatioControl9;
	hr = pVmr9->QueryInterface(IID_IVMRAspectRatioControl9, reinterpret_cast<void**>(&pIVMRAspectRatioControl9));
	if (SUCCEEDED(hr))
	{
		hr = pIVMRAspectRatioControl9->SetAspectRatioMode(VMR_ARMODE_LETTER_BOX);
		pIVMRAspectRatioControl9->Release();
	}

	return hr;
}

//######################################
//
//######################################
HRESULT InitVmr7(IBaseFilter *pVmr7)
{
	IVMRFilterConfig * pIVMRFilterConfig;
	hr = pVmr7->QueryInterface(IID_IVMRFilterConfig, reinterpret_cast<void**>(&pIVMRFilterConfig));
	DBGHR("QueryInterface(IID_IVMRFilterConfig)");
	if (SUCCEEDED(hr))
	{
		hr = pIVMRFilterConfig->SetRenderingMode(VMRMode_Windowless);
		DBGHR("pIVMRFilterConfig->SetRenderingMode");
		pIVMRFilterConfig->Release();
	}

	hr = pVmr7->QueryInterface(IID_IVMRWindowlessControl, reinterpret_cast<void**>(&g_pIVMRWindowlessControl));
	DBGHR("QueryInterface(IID_IVMRWindowlessControl)");
	if (SUCCEEDED(hr))
	{
		RECT cr;
		::GetClientRect(g_hWnd, &cr);
		RECT r = { 0, 0, cr.right - cr.left, cr.bottom - cr.top };
		hr = g_pIVMRWindowlessControl->SetVideoPosition(NULL, &r);
		hr = g_pIVMRWindowlessControl->SetVideoClippingWindow(g_hWnd);
	}

	IVMRAspectRatioControl * pIVMRAspectRatioControl;
	hr = pVmr7->QueryInterface(IID_IVMRAspectRatioControl, reinterpret_cast<void**>(&pIVMRAspectRatioControl));
	if (SUCCEEDED(hr))
	{
		hr = pIVMRAspectRatioControl->SetAspectRatioMode(VMR_ARMODE_LETTER_BOX);
		pIVMRAspectRatioControl->Release();
	}

	return hr;
}

//######################################
//
//######################################
HRESULT InitEvr(IBaseFilter *pEvr)
{
	IMFGetService *pGS = NULL;
	hr = pEvr->QueryInterface(IID_PPV_ARGS(&pGS));
	DBGHR("pEvr->QueryInterface");
	if (SUCCEEDED(hr))
	{
		hr = pGS->GetService(MR_VIDEO_RENDER_SERVICE, IID_PPV_ARGS(&g_pIMFVideoDisplayControl));
		DBGHR("pGS->GetService");
		if (SUCCEEDED(hr))
		{
			RECT cr;
			::GetClientRect(g_hWnd, &cr);
			RECT r = { 0, 0, cr.right - cr.left, cr.bottom - cr.top };
			hr = g_pIMFVideoDisplayControl->SetVideoPosition(NULL, &r);
			hr = g_pIMFVideoDisplayControl->SetVideoWindow(g_hWnd);

			hr = g_pIMFVideoDisplayControl->SetAspectRatioMode(MFVideoARMode_PreservePicture);
			DBGHR("g_pIMFVideoDisplayControl->SetAspectRatioMode");
		}
		pGS->Release();
	}

	return hr;
}

//######################################
//
//######################################
HRESULT InitLegacyRenderer(IBaseFilter *pLegacyRenderer)
{
	hr = g_pGraphBuilder->QueryInterface(IID_IVideoWindow, reinterpret_cast<void**>(&g_pVideoWindow));
	DBGHR("QueryInterface(IID_IVideoWindow)");
	if (FAILED(hr)) return hr;

	hr = g_pVideoWindow->put_WindowStyle(WS_CHILD);
	DBGHR("g_pVideoWindow->put_WindowStyle");

	hr = g_pVideoWindow->put_Owner((OAHWND)g_hWnd);
	DBGHR("g_pVideoWindow->put_Owner");

	RECT cr;
	::GetClientRect(g_hWnd, &cr);
	hr = g_pVideoWindow->SetWindowPosition(0, 0, cr.right - cr.left, cr.bottom - cr.top);
	DBGHR("g_pVideoWindow->SetWindowPosition");

	hr = g_pVideoWindow->put_MessageDrain((OAHWND)g_hWnd);
	DBGHR("g_pVideoWindow->put_MessageDrain");

	// get aspect ratio
	IBasicVideo2 * pBasicVideo2 = NULL;
	hr = g_pGraphBuilder->QueryInterface(IID_IBasicVideo2, reinterpret_cast<void**>(&pBasicVideo2));
	DBGHR("QueryInterface(IID_IBasicVideo2)");
	if (SUCCEEDED(hr))
	{
		pBasicVideo2->GetPreferredAspectRatio(&g_lAspectX, &g_lAspectY); // e.g. 16 9
		pBasicVideo2->Release();
	}
	return hr;
}

//######################################
//
//######################################
HRESULT CheckAndInitRenderer(BOOL bDisconnect=FALSE)
{
	IPin *pPin;
	IPin *pPinTo;

	IBaseFilter * pRenderer = NULL;

	//######################################
	// VMR9
	//######################################
	hr = FindFilterByGuid(g_pGraphBuilder, CLSID_VideoMixingRenderer9, &pRenderer);
	if (SUCCEEDED(hr))
	{
		//log(logInfo, "VMR9 found\n");
		if (bDisconnect)
		{
			hr = GetConnectedPins(pRenderer, PINDIR_INPUT, &pPin, &pPinTo);
			if (SUCCEEDED(hr))
			{
				// DISCONNECT
				hr = g_pGraphBuilder->Disconnect(pPin);
				hr = g_pGraphBuilder->Disconnect(pPinTo);
				DBGHR("g_pGraphBuilder->Disconnect");

				hr = InitVmr9(pRenderer);
				DBGHR("InitVmr9");

				// RECONNECT
				hr = g_pGraphBuilder->ConnectDirect(pPinTo, pPin, NULL);
				DBGHR("g_pGraphBuilder->ConnectDirect");

				pPin->Release();
				pPinTo->Release();
				pRenderer->Release();
				return hr;
			}
		}

		hr = InitVmr9(pRenderer);
		DBGHR("InitVmr9");

		pRenderer->Release();
		return hr;
	}

	//######################################
	// VMR7
	//######################################
	hr = FindFilterByGuid(g_pGraphBuilder, CLSID_VideoMixingRenderer, &pRenderer);
	if (SUCCEEDED(hr))
	{
		//log(logInfo, "VMR7 found\n");
		if (bDisconnect)
		{
			hr = GetConnectedPins(pRenderer, PINDIR_INPUT, &pPin, &pPinTo);
			if (SUCCEEDED(hr))
			{
				// DISCONNECT
				hr = g_pGraphBuilder->Disconnect(pPin);
				hr = g_pGraphBuilder->Disconnect(pPinTo);
				DBGHR("g_pGraphBuilder->Disconnect");

				hr = InitVmr7(pRenderer);
				DBGHR("InitVmr7");

				// RECONNECT
				hr = g_pGraphBuilder->ConnectDirect(pPinTo, pPin, NULL);
				DBGHR("g_pGraphBuilder->ConnectDirect");

				pPin->Release();
				pPinTo->Release();
				pRenderer->Release();
				return hr;
			}
		}

		hr = InitVmr7(pRenderer);
		DBGHR("InitVmr7");

		pRenderer->Release();
		return hr;
	}

	//######################################
	// EVR
	//######################################
	hr = FindFilterByGuid(g_pGraphBuilder, CLSID_EnhancedVideoRenderer, &pRenderer);
	if (SUCCEEDED(hr))
	{
		//log(logInfo, "EVR found\n");
		if (bDisconnect)
		{
			hr = GetConnectedPins(pRenderer, PINDIR_INPUT, &pPin, &pPinTo);
			if (SUCCEEDED(hr))
			{
				// DISCONNECT
				hr = g_pGraphBuilder->Disconnect(pPin);
				hr = g_pGraphBuilder->Disconnect(pPinTo);
				DBGHR("g_pGraphBuilder->Disconnect");

				hr = InitEvr(pRenderer);
				DBGHR("InitEvr");

				// RECONNECT
				hr = g_pGraphBuilder->ConnectDirect(pPinTo, pPin, NULL);
				DBGHR("g_pGraphBuilder->ConnectDirect");

				pPin->Release();
				pPinTo->Release();
				pRenderer->Release();
				return hr;
			}
		}

		hr = InitEvr(pRenderer);
		DBGHR("InitEvr");

		pRenderer->Release();
		return hr;
	}

	//######################################
	// Legacy VideoRenderer
	//######################################
	hr = FindFilterByGuid(g_pGraphBuilder, CLSID_VideoRenderer, &pRenderer);
	if (SUCCEEDED(hr))
	{
		//log(logInfo, "Legacy VideoRenderer found\n");
		hr = InitLegacyRenderer(pRenderer);
		DBGHR("InitLegacyRenderer");
		pRenderer->Release();
	}

	return hr;
}

//######################################
//
//######################################
void ToggleFullscreen()
{
	RECT r;
	long style = GetWindowLong(g_hWnd, GWL_STYLE);

	if (g_fullscreen)
	{
		SetWindowLong(
			g_hWnd,
			GWL_STYLE,
			style | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME
		);
		r = g_rect;
		g_fullscreen = FALSE;
	}
	else
	{
		ok = GetWindowRect(g_hWnd, &g_rect);
		ok = GetWindowRect(GetDesktopWindow(), &r);
		ok = SetWindowLong(
			g_hWnd,
			GWL_STYLE,
			style & ~WS_CAPTION & ~WS_SYSMENU & ~WS_THICKFRAME
		);
		g_fullscreen = TRUE;
	}

	ok = SetWindowPos(
		g_hWnd,
		NULL,
		r.left,
		r.top,
		r.right - r.left,
		r.bottom - r.top,
		0
	);
}

//######################################
// 
//######################################
HRESULT GetVideoSize(LONG * pWidth, LONG * pHeight)
{
	hr = E_FAIL;
	if (g_pIVMRWindowlessControl9) // VMR9
	{
		LONG vw, vh, aw, ah;
		hr = g_pIVMRWindowlessControl9->GetNativeVideoSize(&vw, &vh, &aw, &ah);
		if (SUCCEEDED(hr))
		{
			*pWidth = vw;
			*pHeight = vh;
		}
	}
	else if (g_pIVMRWindowlessControl) // VMR9
	{
		LONG vw, vh, aw, ah;
		hr = g_pIVMRWindowlessControl->GetNativeVideoSize(&vw, &vh, &aw, &ah);
		if (SUCCEEDED(hr))
		{
			*pWidth = vw;
			*pHeight = vh;
		}
	}
	else if (g_pIMFVideoDisplayControl) // EVR
	{
		SIZE szVideo, szARVideo;
		hr = g_pIMFVideoDisplayControl->GetNativeVideoSize(&szVideo, &szARVideo);
		if (SUCCEEDED(hr))
		{
			*pWidth = szVideo.cx;
			*pHeight = szVideo.cy;
		}
	}
	else if (g_pVideoWindow) // Legacy VideoRenderer
	{
		IBasicVideo * pBasicVideo = NULL;
		hr = g_pGraphBuilder->QueryInterface(IID_IBasicVideo, reinterpret_cast<void**>(&pBasicVideo));
		if (SUCCEEDED(hr))
		{
			LONG vw, vh;
			hr = pBasicVideo->GetVideoSize(&vw, &vh);
			*pWidth = vw;
			*pHeight = vh;
			pBasicVideo->Release();
		}
	}
	return hr;
}

//######################################
// 
//######################################
void PrintMediaInfos()
{
	printf("Media Infos:\n");

	//IUnknown * pUnk;
	//hr = FindFilterInterface(g_pGraphBuilder, IID_IFileSourceFilter, reinterpret_cast<void**>(&pUnk));
	//if (SUCCEEDED(hr))
	//{
	//	IFileSourceFilter *pFileSourceFilter = reinterpret_cast<IFileSourceFilter*>(pUnk);


	IFileSourceFilter * pFileSourceFilter;
	hr = FindFilterInterface(g_pGraphBuilder, IID_IFileSourceFilter, reinterpret_cast<void**>(&pFileSourceFilter));
	DBGHR("IFileSourceFilter");
	if (SUCCEEDED(hr))
	{
		AM_MEDIA_TYPE mt;
		WCHAR *pszFileName;
		pFileSourceFilter->GetCurFile(
			&pszFileName,
			&mt
		);
		const char * type;
		if (mt.subtype == MEDIASUBTYPE_Avi)
			type = "AVI";
		else if (mt.subtype == MEDIASUBTYPE_MP4)
			type = "MP4";
		else if (mt.subtype == MEDIASUBTYPE_MPEG1System)
			type = "MPEG1";
		else if (mt.subtype == MEDIASUBTYPE_FLV)
			type = "FLV";
		else if (mt.subtype == MEDIASUBTYPE_Matroska)
			type = "Matroska";

		else if (mt.subtype == MEDIASUBTYPE_MPEG1Audio)
			type = "MPEG1 Audio";
		else if (mt.subtype == MEDIASUBTYPE_WAVE)
			type = "WAV";
		else if (mt.subtype == MEDIASUBTYPE_AIFF)
			type = "AIFF";
		else if (mt.subtype == MEDIASUBTYPE_Ogg)
			type = "Ogg";
		else if (mt.subtype == MEDIATYPE_Midi)
			type = "Midi";
		else
		{
			IBaseFilter *pFilter;
			hr = FindFilterByGuid(g_pGraphBuilder, CLSID_WMAsfReader, &pFilter);
			if (SUCCEEDED(hr))
			{
				type = "WMV";
				pFilter->Release();
			}
			else type = "Other";
		}
			
		printf("  Media type: %s\n", type);
		pFileSourceFilter->Release(); //???
	}

	//hr = FindFilterInterface(g_pGraphBuilder, IID_IAMStreamSelect, (void**)&pUnk);
	//if (SUCCEEDED(hr))
	//{
	//	IAMStreamSelect *pStreamSelect = (IAMStreamSelect*)pUnk;

	IAMStreamSelect *pStreamSelect;
	hr = FindFilterInterface(g_pGraphBuilder, IID_IAMStreamSelect, reinterpret_cast<void**>(&pStreamSelect));
	DBGHR("IAMStreamSelect");
	if (SUCCEEDED(hr))
	{
		DWORD dwStreams;
		hr = pStreamSelect->Count(&dwStreams);
		printf("  Streams: %ld\n", dwStreams);
		for (DWORD index = 0; index < dwStreams; index++)
		{
			AM_MEDIA_TYPE *pmt;
			WCHAR *pszName;
			DWORD dwFlags;
			LCID lcid;
			hr = pStreamSelect->Info(
				index,
				&pmt,
				&dwFlags,
				&lcid,
				NULL,
				&pszName,
				NULL,
				NULL
			);

			const char * type;
			if (pmt->majortype == MEDIATYPE_Video)
				type = "Video";
			else if (pmt->majortype == MEDIATYPE_Audio)
				type = "Audio";
			else if (pmt->majortype == MEDIATYPE_Midi)
				type = "Midi";
			else
				type = "Other";
			printf("  Stream %ld: %s\n", index, type);
		}
		pStreamSelect->Release(); //???
	}

	LONGLONG time;
	hr = g_pMediaSeeking->GetDuration(&time); // REFERENCE_TIME
	DBGHR("g_pMediaSeeking->GetDuration");
	if (SUCCEEDED(hr))
	{
		printf("  Duration: %ld ms\n", (long)(time / 10000));
	}

	LONG width, height;
	if (SUCCEEDED(GetVideoSize(&width, &height)))
	{
		printf("  Width: %ld\n", width);
		printf("  Height: %ld\n", height);
	}

	IBasicVideo * pBasicVideo = NULL;
	hr = g_pGraphBuilder->QueryInterface(IID_IBasicVideo, reinterpret_cast<void**>(&pBasicVideo));
	DBGHR("IBasicVideo");
	if (SUCCEEDED(hr))
	{
		IEnumFilters *pEnumFilters;
		IBaseFilter *pFilter;
		IPin *pPin;
		IPin *pPinTo;
		g_pGraphBuilder->EnumFilters(&pEnumFilters);
		while (pEnumFilters->Next(1, &pFilter, NULL) == S_OK)
		{
			hr = GetConnectedPins(
				pFilter,
				PINDIR_OUTPUT,
				&pPin,
				&pPinTo,
				MEDIATYPE_Video
			);
			pFilter->Release();
			if (SUCCEEDED(hr))
			{
				//FILTER_INFO FilterInfo;
				//hr = pFilter->QueryFilterInfo(&FilterInfo);
				//printf("Filter: %ls\n", FilterInfo.achName);
				AM_MEDIA_TYPE mt;
				pPin->ConnectionMediaType(&mt);
				if (mt.formattype == FORMAT_VideoInfo || mt.formattype == FORMAT_MPEGVideo)
				{
					VIDEOINFOHEADER * info = reinterpret_cast<VIDEOINFOHEADER*>(mt.pbFormat);
					printf("  Fps: %.3f\n", (double)(10000000. / info->AvgTimePerFrame));
					//std::cout << "  Compression: " <<
					//	char(info->bmiHeader.biCompression) <<
					//	char(info->bmiHeader.biCompression >> 8) <<
					//	char(info->bmiHeader.biCompression >> 16) <<
					//	char(info->bmiHeader.biCompression >> 24) << '\n';
				}
				else if (mt.formattype == FORMAT_VideoInfo2 || mt.formattype == FORMAT_MPEG2_VIDEO)
				{
					VIDEOINFOHEADER2 * info = reinterpret_cast<VIDEOINFOHEADER2*>(mt.pbFormat);
					printf("  Fps: %.3f\n", (double)(10000000. / info->AvgTimePerFrame));
					//std::cout << "  Compression: " <<
					//	char(info->bmiHeader.biCompression) << 
					//	char(info->bmiHeader.biCompression >> 8) <<
					//	char(info->bmiHeader.biCompression >> 16) <<
					//	char(info->bmiHeader.biCompression >> 24) << '\n';
				}

				//else if (mt.formattype == FORMAT_WaveFormatEx)
				//{
				//	WAVEFORMATEX * info = reinterpret_cast<WAVEFORMATEX*>(mt.pbFormat);
				//	//info->nChannels
				//	//info->nSamplesPerSec
				//	//info->wBitsPerSample
				//	//info->wFormatTag WAVE_FORMAT_PCM WAVE_FORMAT_MPEG MPEGLAYER3WAVEFORMAT
				//}

				pPin->Release();
				pPinTo->Release();
				break;
			}
		}
		pEnumFilters->Release();
	}
}

//######################################
// 
//######################################
HWND InitWindow(HINSTANCE hInstance)
{
	HWND hWnd = CreateWindowW(
		g_szWindowClass,
		g_szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);
	if (!hWnd)
	{
		return NULL;
	}
	SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)GetStockObject(BLACK_BRUSH));
	//ShowWindow(hWnd, nCmdShow);
	//UpdateWindow(hWnd);
	return hWnd;
}

//######################################
//
//######################################
void LoadFileInResource(int name, int type, DWORD& size, const char*& data)
{
	HMODULE handle = ::GetModuleHandle(NULL);
	HRSRC rc = ::FindResource(handle, MAKEINTRESOURCE(name),
		MAKEINTRESOURCE(type));
	HGLOBAL rcData = ::LoadResource(handle, rc);
	size = ::SizeofResource(handle, rc);
	data = static_cast<const char*>(::LockResource(rcData));
}

//######################################
// 
//######################################
void PrintHelp()
{
	printf("\n%s v%s - %s\n\n", VER_PRODUCTNAME, VER_VERSION_STRING, VER_FILEDESCRIPTION);
	DWORD size = 0;
	const char* data = NULL;
	LoadFileInResource(IDR_MYTEXTFILE, TEXTFILE, size, data);
	std::cout << data << "\0";
}

//######################################
// 
//######################################
void HandleKey(UINT_PTR key)
{
	switch (key)
	{
	case VK_SPACE:
	case 'P':
		{
			OAFilterState state;
			g_pMediaControl->GetState(INFINITE, &state);
			if (state == State_Running)
				g_pMediaControl->Pause();
			else
				g_pMediaControl->Run();
		}
		break;
	case 'R':
		g_pMediaSeeking->SetPositions(&g_starttime, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);
		break;
	case VK_ESCAPE:
	case 'Q':
		g_pMediaControl->Stop();
		PostQuitMessage(0);
		break;
	case 'H':
		PrintHelp();
		break;
	case VK_DOWN:
		if (g_pBasicAudio)
		{
			long vol; // -10000 = silent, 0 = maximum
			g_pBasicAudio->get_Volume(&vol);
			//vol = (vol + 10000)*0.9 - 10000;
			vol = (long)(vol * 0.9 - 1000);
			g_pBasicAudio->put_Volume(vol);
		}
		break;
	case VK_UP:
		if (g_pBasicAudio)
		{
			long vol; // -10000 = silent, 0 = maximum
			g_pBasicAudio->get_Volume(&vol);
			//vol = (vol + 10000)*1.1 - 10000;
			vol = (long)(vol * 1.1 + 1000);
			g_pBasicAudio->put_Volume(vol);
		}
		break;
	case 'Y':
		if (g_pBasicAudio)
		{
			long bal; // -10000 to 10000
			g_pBasicAudio->get_Balance(&bal);
			g_pBasicAudio->put_Balance(bal - 1000);
		}
		break;
	case 'X':
		if (g_pBasicAudio)
		{
			long bal; // -10000 to 10000
			g_pBasicAudio->get_Balance(&bal);
			g_pBasicAudio->put_Balance(bal + 1000);
		}
		break;
	case 'M':
		if (g_pBasicAudio)
			g_pBasicAudio->put_Volume(-10000);
		break;
	case VK_ADD:
		{
			double dRate;
			g_pMediaSeeking->GetRate(&dRate);
			g_pMediaSeeking->SetRate(dRate * 2);
		}
		break;
	case VK_SUBTRACT:
		{
			double dRate;
			g_pMediaSeeking->GetRate(&dRate);
			g_pMediaSeeking->SetRate(dRate / 2);
		}
		break;
	case 'C':
	{
		CURSORINFO info;
		info.cbSize = sizeof(CURSORINFO);
		GetCursorInfo(&info);
		ShowCursor(info.flags == 0);
	}
	break;
	case 0x30: // 0
	case 0x31:
	case 0x32:
	case 0x33:
	case 0x34:
	case 0x35:
	case 0x36:
	case 0x37:
	case 0x38:
	case 0x39: // 9
	{
		IEnumFilters *pEnumFilters;
		IBaseFilter *pFilter;
		g_pGraphBuilder->EnumFilters(&pEnumFilters);
		int i = 0;
		while (pEnumFilters->Next(1, &pFilter, NULL) == S_OK)
		{
			if (i == key - 0x30)
			{
				hr = ShowFilterPropertyPages(pFilter, g_hWnd);
				if (FAILED(hr))
					ShowPinPropertyPages(pFilter, g_hWnd);
			}
			pFilter->Release();
			i++;
		}
		pEnumFilters->Release();
	}
	break;
	//	LEFT/RIGHT: Seek one frame backwards/forward (if supported by loaded graph)
	case VK_LEFT:
		{
			LONGLONG time;
			g_pMediaSeeking->GetCurrentPosition(&time);
			time -= 100 * 10000; // 100 ms   frame_to_currtime(frames);
			g_pMediaSeeking->SetPositions(&time, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);
		}
		break;
	case VK_RIGHT:
		{
			LONGLONG time;
			g_pMediaSeeking->GetCurrentPosition(&time);
			time += 100 * 10000; // 100 ms   frame_to_currtime(frames);
			g_pMediaSeeking->SetPositions(&time, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);
		}
		break;
		//	PAGEUP/PAGEDOWN					Seek 30 frames backwards/forward (if supported by loaded graph)
	case VK_PRIOR: // PAGE UP
		{
			LONGLONG time;
			g_pMediaSeeking->GetCurrentPosition(&time);
			time -= 1000 * 10000; // 1000 ms
			g_pMediaSeeking->SetPositions(&time, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);
		}
		break;
	case VK_NEXT: // PAGE DOWN
		{
			LONGLONG time;
			g_pMediaSeeking->GetCurrentPosition(&time);
			time += 1000 * 10000; // 1000 ms
			g_pMediaSeeking->SetPositions(&time, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);
		}
		break;
	case 'F':
		ToggleFullscreen();
		break;
	case 'T': // todo: toggle?
		SetWindowPos(g_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		break;
	case 'D':
	{
		IUnknown * pUnk;
		hr = FindFilterInterface(g_pGraphBuilder, IID_IDirectVobSub, reinterpret_cast<void**>(&pUnk));
		if (SUCCEEDED(hr))
		{
			IDirectVobSub* pDirectVobSub = (IDirectVobSub*)pUnk;
			bool bHide;
			pDirectVobSub->get_HideSubtitles(&bHide);
			pDirectVobSub->put_HideSubtitles(!bHide);
			pDirectVobSub->Release(); //???
		}
	}
	break;
	case 'O':
	{
		//OFN_EXPLORER
		DWORD flags = OFN_FILEMUSTEXIST | 0x00800000; // OFN_ENABLERESIZING = 0x00800000
		WCHAR szFile[MAX_PATH];

		OPENFILENAMEW ofn;
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = g_hWnd;
		ofn.lpstrFilter = NULL; // "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";

		//ofn.lpstrFile = NULL; // default FileName;
		ofn.lpstrFile = szFile;
		ofn.lpstrFile[0] = 0;

		ofn.nMaxFile = MAX_PATH;
		ofn.lpfnHook = NULL;
		ofn.Flags = flags;
		ofn.lpstrDefExt = L"mp4";
		ofn.lpstrTitle = L"Open Media File";
		if (GetOpenFileNameW(&ofn))
		{
			g_pMediaControl->Stop();

			// Remove all filters
			IEnumFilters *pEnumFilters;
			IBaseFilter *pFilter;
			g_pGraphBuilder->EnumFilters(&pEnumFilters);
			while (pEnumFilters->Next(1, &pFilter, NULL) == S_OK)
			{
				hr = g_pGraphBuilder->RemoveFilter(pFilter);
				pFilter->Release();
				pEnumFilters->Reset();
			}
			pEnumFilters->Release();

			// force redaw, in case we switched from video to audio only
			InvalidateRect(g_hWnd, NULL, TRUE);
			UpdateWindow(g_hWnd);

			//############################################
			IBaseFilter *pRenderer = NULL;

			switch (g_iRenderer)
			{
				case 0: // Legacy VideoRenderer
				{
					//printf("Legacy VideoRenderer\n");
					hr = CoCreateInstance(CLSID_VideoRenderer, 0, CLSCTX_INPROC_SERVER, IID_IBaseFilter, reinterpret_cast<void**>(&pRenderer));
					if (FAILED(hr)) return;
					hr = g_pGraphBuilder->AddFilter(pRenderer, L"Video Renderer");
					DBGHR("g_pGraphBuilder->AddFilter");
					// must be initalized *after* rendering the graph
				}
				break;
				case 1: // VMR7
				{
					//printf("VMR7\n");
					hr = CoCreateInstance(CLSID_VideoMixingRenderer, 0, CLSCTX_INPROC_SERVER, IID_IBaseFilter, reinterpret_cast<void**>(&pRenderer));
					if (FAILED(hr)) return;
					hr = g_pGraphBuilder->AddFilter(pRenderer, L"Video Mixing Renderer");
					DBGHR("g_pGraphBuilder->AddFilter");

					hr = InitVmr7(pRenderer);
					DBGHR("InitVmr7");
				}
				break;
				case 3: // EVR
				{
					//printf("EVR\n");
					hr = CoCreateInstance(CLSID_EnhancedVideoRenderer, 0, CLSCTX_INPROC_SERVER, IID_IBaseFilter, reinterpret_cast<void**>(&pRenderer));
					if (FAILED(hr)) return;
					hr = g_pGraphBuilder->AddFilter(pRenderer, L"Enhanced Video Renderer");
					DBGHR("g_pGraphBuilder->AddFilter");

					hr = InitEvr(pRenderer);
					DBGHR("InitEvr");
				}
				break;
				default: // VMR9
				{
					//printf("VMR9\n");
					hr = CoCreateInstance(CLSID_VideoMixingRenderer9, 0, CLSCTX_INPROC_SERVER, IID_IBaseFilter, reinterpret_cast<void**>(&pRenderer));
					if (FAILED(hr)) return;
					hr = g_pGraphBuilder->AddFilter(pRenderer, L"Video Mixing Renderer 9");
					DBGHR("g_pGraphBuilder->AddFilter");

					hr = InitVmr9(pRenderer);
					DBGHR("InitVmr9");
				}
			}

			hr = g_pGraphBuilder->RenderFile(ofn.lpstrFile, NULL);
			DBGHR("g_pGraphBuilder->RenderFile");
			if (FAILED(hr))
			{
				SAFE_RELEASE(g_pIVMRWindowlessControl9);
				SAFE_RELEASE(g_pIVMRWindowlessControl);
				SAFE_RELEASE(g_pIMFVideoDisplayControl);
				SAFE_RELEASE(g_pVideoWindow);
				g_pGraphBuilder->RemoveFilter(pRenderer);
				pRenderer->Release();
				return;
			}

			// check if renderer is connected, if not remove it
			IPin *pPin;
			IPin *pPinTo;
			hr = GetConnectedPins(
				pRenderer,
				PINDIR_INPUT,
				&pPin,
				&pPinTo
			);
			if (SUCCEEDED(hr))
			{
				// has video
				pPin->Release();
				pPinTo->Release();

				if (g_iRenderer == 0)
				{
					hr = InitLegacyRenderer(pRenderer);
					DBGHR("InitLegacyRenderer");
				}
			}
			else
			{
				// no video
				SAFE_RELEASE(g_pIVMRWindowlessControl9);
				SAFE_RELEASE(g_pIVMRWindowlessControl);
				SAFE_RELEASE(g_pIMFVideoDisplayControl);
				SAFE_RELEASE(g_pVideoWindow);

				hr = g_pGraphBuilder->RemoveFilter(pRenderer);
			}
			pRenderer->Release();

			g_pMediaControl->Run();
		}
	}
	break;
	case 'U':
	{
		IUnknown * pUnk;
		hr = FindFilterInterface(g_pGraphBuilder, IID_IDirectVobSub, reinterpret_cast<void**>(&pUnk));
		if (SUCCEEDED(hr))
		{
			IDirectVobSub* pDirectVobSub = (IDirectVobSub*)pUnk;
			DWORD flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | 0x00800000; // OFN_ENABLERESIZING = 0x00800000
			OPENFILENAMEW ofn;
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = g_hWnd;
			ofn.lpstrFilter = NULL; // "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
			ofn.lpstrFile = NULL; // default FileName;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpfnHook = NULL;
			ofn.Flags = flags;
			ofn.lpstrDefExt = L"srt";
			ofn.lpstrTitle = L"Open Subtitle File";
			if (GetOpenFileNameW(&ofn))
			{
				pDirectVobSub->put_FileName(ofn.lpstrFile);
			}
			pDirectVobSub->Release(); //???
		}
	}
	break;

	case 'I':
		PrintMediaInfos();
		break;

	case 'L':
		ListFiltersInGraph(g_pGraphBuilder);
		break;

	}//switch
}

//##########################################################
//
//##########################################################
DWORD WINAPI ConsoleThread(void*)
{
	DWORD numEvents;
	INPUT_RECORD irInBuf[128];
	while (TRUE)//!g_bConsoleCloseFlag)
	{
		ok = ReadConsoleInput(
			g_hStdin,
			irInBuf,
			128,
			&numEvents
		);
		if (ok)
		{
			for (DWORD i = 0; i < numEvents; i++)
			{
				if (irInBuf[i].EventType == KEY_EVENT)
				{
					if (irInBuf[i].Event.KeyEvent.bKeyDown)
					{
						for (int j = 0; j < irInBuf[i].Event.KeyEvent.wRepeatCount; j++)
						{
							if (
								irInBuf[i].Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE ||
								irInBuf[i].Event.KeyEvent.wVirtualKeyCode == 'Q'
							)
							{
								PostMessage(g_hWnd, WM_DESTROY, 0, 0);
								return 0;
							}
							HandleKey(irInBuf[i].Event.KeyEvent.wVirtualKeyCode);
						}
					}
				}
			}
		}
	}
	return 0;
}

//######################################
// 
//######################################
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
	{
		if (g_pIVMRWindowlessControl9) // VMR9
		{
			RECT cr;
			ok = GetClientRect(g_hWnd, &cr);
			RECT r = { 0, 0, cr.right - cr.left, cr.bottom - cr.top };
			hr = g_pIVMRWindowlessControl9->SetVideoPosition(NULL, &r);
			DBGHR("g_pIVMRWindowlessControl9->SetVideoPosition");
		}
		else if (g_pIVMRWindowlessControl) // VMR7
		{
			RECT cr;
			ok = GetClientRect(g_hWnd, &cr);
			RECT r = { 0, 0, cr.right - cr.left, cr.bottom - cr.top };
			hr = g_pIVMRWindowlessControl->SetVideoPosition(NULL, &r);
			DBGHR("g_pIVMRWindowlessControl->SetVideoPosition");
		}
		else if (g_pIMFVideoDisplayControl) // EVR
		{
			RECT cr;
			::GetClientRect(g_hWnd, &cr);
			RECT r = { 0, 0, cr.right - cr.left, cr.bottom - cr.top };
			hr = g_pIMFVideoDisplayControl->SetVideoPosition(NULL, &r);
			DBGHR("g_pIMFVideoDisplayControl->SetVideoPosition");
		}
		else if (g_pVideoWindow) // Legacy VideoRenderer
		{
			RECT cr;
			::GetClientRect(g_hWnd, &cr);
			long w = cr.right - cr.left;
			long h = cr.bottom - cr.top;
			if (g_lAspectX > 0 && g_lAspectY > 0)
			{
				// keep aspect ratio
				if (w / h <= g_lAspectX / g_lAspectY)
				{
					long h_new = w * g_lAspectY / g_lAspectX;
					hr = g_pVideoWindow->SetWindowPosition(0, (h - h_new) / 2, w, h_new);
				}
				else
				{
					long w_new = h * g_lAspectX / g_lAspectY;
					hr = g_pVideoWindow->SetWindowPosition((w - w_new) / 2, 0, w_new, h);
				}
			}
			else
			{ 
				hr = g_pVideoWindow->SetWindowPosition(0, 0, w, h);
			}
			DBGHR("g_pVideoWindow->SetWindowPosition");
		}
		break;
	}

	case WM_KEYDOWN:
		HandleKey(wParam);
		break;

	case WM_LBUTTONDBLCLK:
	{
		ToggleFullscreen();
		break;
	}

	case WM_TIMER:
		{
			LONGLONG time;
			//long ms;
			hr = g_pMediaSeeking->GetCurrentPosition(&time);
			DBGHR("g_pMediaSeeking->GetCurrentPosition");
			if (SUCCEEDED(hr))
			{
				if (g_duration)
				{
					printf("Progress: %ld %%\n", (long)(100 * time / g_duration));
				}
			}
		}
		break;

	case WM_USER:
		{
			long EventCode;
			LONG_PTR Param1, Param2;

			while (g_pMediaEvent->GetEvent(&EventCode, &Param1, &Param2, 0) != E_ABORT)
			{
				if (EventCode == EC_COMPLETE)
				{
					if (g_bLoop && g_pMediaSeeking)
					{
						// go back to start
						hr = g_pMediaSeeking->SetPositions(&g_starttime, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);
						DBGHR("g_pMediaSeeking->SetPositions");
					}
					else
					{
						hr = g_pMediaControl->Stop();
						if (g_bProgress && g_duration)
						{
							printf("Progress: 100 %%\n");
						}
						if (g_bQuitWhenComplete)
							PostQuitMessage(0); //quit
						else
						{
							g_bHasGraph = FALSE;

							// force redaw
							InvalidateRect(g_hWnd, NULL, TRUE);
							UpdateWindow(g_hWnd);
						}
					}
				}

				if (g_logLevel >= logInfo)
					Log(logInfo, "Event: %s\n", GetEventName(EventCode));

				g_pMediaEvent->FreeEventParams(EventCode, Param1, Param2);
			}
		}
		break;

	case WM_DESTROY:
		g_pMediaControl->Stop();
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

//######################################
//  Registers the window class
//######################################
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_DBLCLKS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DSCMD));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = g_szWindowClass;
	wcex.lpszClassName = g_szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_DSCMD));
	return RegisterClassExW(&wcex);
}

//######################################
//
//######################################
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
	int exitCode = ERROR_NONE;
	int argc;
	LPWSTR * argv = CommandLineToArgvW(
		lpCmdLine,
		&argc
	);
	WCHAR * tmp;

	//######################################
	// Help
	//######################################
	if (CmdOptionExists(argv, argv + argc, L"-h"))
	{
		PrintHelp();
		return 0;
	}

	CoInitialize(NULL);

	//######################################
	// List Filters
	//######################################
	if (CmdOptionExists(argv, argv + argc, L"-lf"))
	{
		IFilterMapper * pFilterMapper;
		IEnumRegFilters	* pEnumRegFilters;
		hr = CoCreateInstance(CLSID_FilterMapper, NULL, CLSCTX_INPROC_SERVER, IID_IFilterMapper, (LPVOID*)&pFilterMapper);
		if (SUCCEEDED(hr))
		{
			hr = pFilterMapper->EnumMatchingFilters(&pEnumRegFilters, 0, FALSE, GUID_NULL, GUID_NULL, FALSE, FALSE, GUID_NULL, GUID_NULL);
			if (SUCCEEDED(hr))
			{
				REGFILTER * pRegFilter;
				WCHAR guidStr[39];
				while (pEnumRegFilters->Next(1, &pRegFilter, NULL) == S_OK)
				{
					GuidToString(guidStr, pRegFilter->Clsid);
					std::wcout << guidStr << L" " << pRegFilter->Name << L"\n";
				}
				pEnumRegFilters->Release();
			}
			pFilterMapper->Release();
		}
		CoUninitialize();
		return 0;
	}

	//######################################
	// List Category
	// e.g. dscmd -lc {860BB310-5D01-11D0-BD3B-00A0C911CE86}
	//######################################
	tmp = CmdOptionGet(argv, argv + argc, L"-lc");
	if (tmp)
	{
		ICreateDevEnum * pSysDevEnum;
		IEnumMoniker * pEnum;
		IMoniker * pMoniker;
		GUID category_guid;
		IBaseFilter *pBaseFilter;
		WCHAR filterName[512];
		GUID filter_guid;
		WCHAR guidStr[39];

		StringToGuid(tmp, &category_guid);

		hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&pSysDevEnum);
		if (SUCCEEDED(hr))
		{
			hr = pSysDevEnum->CreateClassEnumerator(category_guid, &pEnum, 0);
			if (SUCCEEDED(hr))
			{
				while (pEnum->Next(1, &pMoniker, NULL) == S_OK)
				{
					hr = GetCodecName(pMoniker, filterName);
					hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)&pBaseFilter);
					if (SUCCEEDED(hr))
					{
						hr = pBaseFilter->GetClassID(&filter_guid);
						GuidToString(guidStr, filter_guid);
						pBaseFilter->Release();
					}
					std::wcout << guidStr << L" " << filterName << L"\n";
					pMoniker->Release();
				}
				pEnum->Release();
			}
			pSysDevEnum->Release();
		}
		CoUninitialize();
		return 0;
	}

	//######################################
	// Verbosity
	//######################################
	tmp = CmdOptionGet(argv, argv + argc, L"-v");
	if (tmp)
	{
		g_logLevel = _wtoi(tmp); // 0..3
	}

	//######################################
	// Create GraphBuilder
	//######################################
	// A) Create GraphBuilder directly
	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&g_pGraphBuilder);
	if (FAILED(hr))
	{
		EXIT(ERROR_COCREATEINSTANCE_FAILED);
	}

	// B) Create FilterGraph, then query interface?
	//hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IFilterGraph, (void **)&g_pFilterGraph);
	//if (FAILED(hr))
	//{
	//	EXIT(ERROR_COCREATEINSTANCE_FAILED);
	//}
	//hr = g_pFilterGraph->QueryInterface(IID_IGraphBuilder, (void **)&g_pGraphBuilder);
	//DBGHR("QueryInterface(IID_IGraphBuilder)");

	//######################################
	// Query interfaces
	//######################################

	hr = g_pGraphBuilder->QueryInterface(IID_IMediaControl, (void **)&g_pMediaControl);
	DBGHR("QueryInterface(IID_IMediaControl)");
	if (FAILED(hr))
	{
		EXIT(ERROR_NO_MEDIACONTROL);
	}

	hr = g_pGraphBuilder->QueryInterface(IID_IMediaSeeking, (void**)&g_pMediaSeeking);
	DBGHR("QueryInterface(IID_IMediaSeeking)");
	if (FAILED(hr))
	{
		EXIT(ERROR_NO_MEDIASEEKING);
	}

	//######################################
	// Print media infos and exit
	//######################################
	tmp = CmdOptionGet(argv, argv + argc, L"-i");
	if (tmp)
	{
		hr = g_pGraphBuilder->RenderFile(tmp, NULL);
		if (SUCCEEDED(hr))
		{
			hr = CheckAndInitRenderer(TRUE);
			PrintMediaInfos();
		}
		else
			Log(logError, "Failed to render file\n");
		goto clean_up_and_exit;
	}

	//######################################
	// Init window
	//######################################
	g_hInst = hInstance;
	MyRegisterClass(hInstance);
	nCmdShow = SW_SHOW;
	g_hWnd = InitWindow(hInstance);
	if (!g_hWnd)
	{
		EXIT(ERROR_INITWINDOW_FAILED);
	}

	//######################################
	// No Window
	//######################################
	if (CmdOptionExists(argv, argv + argc, L"-nw"))
	{
		nCmdShow = SW_HIDE;
	}
	else
	{
		//######################################
		// Window Style
		//######################################
		tmp = CmdOptionGet(argv, argv + argc, L"-ws");
		if (tmp)
		{
			g_dwStyle = (DWORD)_wtol(tmp);
			ok = SetWindowLong(
				g_hWnd,
				GWL_STYLE,
				g_dwStyle
			);
		}
	}

	//######################################
	// Preferred video renderer
	//######################################
	tmp = CmdOptionGet(argv, argv + argc, L"-r");
	if (tmp)
	{
		g_iRenderer = _wtoi(tmp);
		if (g_iRenderer > 3) g_iRenderer = 3;
	}

	//######################################
	// Loop
	//######################################
	if (CmdOptionExists(argv, argv + argc, L"-loop"))
	{
		g_bLoop = TRUE;
	}

	//######################################
	// Progress
	//######################################
	if (CmdOptionExists(argv, argv + argc, L"-p"))
	{
		g_bProgress = TRUE;
	}

	//######################################
	// Quit when complete
	//######################################
	if (CmdOptionExists(argv, argv + argc, L"-q"))
	{
		g_bQuitWhenComplete = TRUE;
	}

	//######################################
	// dscmd foo.mp4
	// dscmd -f foo.mp4 ...
	//######################################
	if (wcslen(lpCmdLine) > 0 && argc == 1 && argv[0][0] != '-')
		tmp = argv[0];
	else
		tmp = CmdOptionGet(argv, argv + argc, L"-f"); // Media File

	if (tmp)
	{
		IBaseFilter *pRenderer = 0;

		switch (g_iRenderer)
		{ 
			case 0: // Legacy VideoRenderer
			{
				//printf("Legacy VideoRenderer\n");
				HRESULT hr = CoCreateInstance(CLSID_VideoRenderer, 0, CLSCTX_INPROC_SERVER, IID_IBaseFilter, reinterpret_cast<void**>(&pRenderer));
				hr = g_pGraphBuilder->AddFilter(pRenderer, L"Video Renderer");
				DBGHR("g_pGraphBuilder->AddFilter");
				// must be initalized *after* rendering the graph
			}
			break;
			case 1: // VMR7
			{
				//printf("VMR7\n");
				HRESULT hr = CoCreateInstance(CLSID_VideoMixingRenderer, 0, CLSCTX_INPROC_SERVER, IID_IBaseFilter, reinterpret_cast<void**>(&pRenderer));
				hr = g_pGraphBuilder->AddFilter(pRenderer, L"Video Mixing Renderer");
				DBGHR("g_pGraphBuilder->AddFilter");

				hr = InitVmr7(pRenderer);
				DBGHR("InitVmr7");
			}
			break;
			case 3: // EVR
			{
				//printf("EVR\n");
				HRESULT hr = CoCreateInstance(CLSID_EnhancedVideoRenderer, 0, CLSCTX_INPROC_SERVER, IID_IBaseFilter, reinterpret_cast<void**>(&pRenderer));
				hr = g_pGraphBuilder->AddFilter(pRenderer, L"Enhanced Video Renderer");
				DBGHR("g_pGraphBuilder->AddFilter");

				hr = InitEvr(pRenderer);
				DBGHR("InitEvr");
			}
			break;
			default: // VMR9
			{
				//printf("VMR9\n");
				HRESULT hr = CoCreateInstance(CLSID_VideoMixingRenderer9, 0, CLSCTX_INPROC_SERVER, IID_IBaseFilter, reinterpret_cast<void**>(&pRenderer));
				hr = g_pGraphBuilder->AddFilter(pRenderer, L"Video Mixing Renderer 9");
				DBGHR("g_pGraphBuilder->AddFilter");

				hr = InitVmr9(pRenderer);
				DBGHR("InitVmr9");
			}
		}

		hr = g_pGraphBuilder->RenderFile(tmp, NULL);
		DBGHR("g_pGraphBuilder->RenderFile");

		// TEST: check if renderer is connected, if not remove it
		IPin *pPin;
		IPin *pPinTo;
		hr = GetConnectedPins(
			pRenderer,
			PINDIR_INPUT,
			&pPin,
			&pPinTo
		);
		if (SUCCEEDED(hr))
		{
			// has video
			pPin->Release();
			pPinTo->Release();

			if (g_iRenderer == 0)
			{
				hr = InitLegacyRenderer(pRenderer);
				DBGHR("InitLegacyRenderer");
			}
		}
		else
		{
			// no video
			SAFE_RELEASE(g_pIVMRWindowlessControl9);
			SAFE_RELEASE(g_pIVMRWindowlessControl);
			SAFE_RELEASE(g_pIMFVideoDisplayControl);
			SAFE_RELEASE(g_pVideoWindow);

			hr = g_pGraphBuilder->RemoveFilter(pRenderer);
		}

		pRenderer->Release();

		if (FAILED(hr))
		{
			EXIT(ERROR_RENDER_FAILED);
		}

		g_bHasGraph = TRUE;
	}
	else
	{
		//######################################
		// Graph
		//######################################
		tmp = CmdOptionGet(argv, argv + argc, L"-g");
		if (tmp)
		{
			Log(logInfo, "Starting DirectShow graph...\n");

			std::wstring graph(tmp);
			auto parts = Explode(graph, L'!');

			//######################################
			// filter list
			//######################################
			auto filters = Explode(parts[0], L',');

			std::vector<IBaseFilter *> pFilterList;

			for (const auto& filter : filters)
			{
				auto filterConf = Explode(filter, L';');

				auto filterParts = Explode(filterConf[0], L'=');

				GUID filterGuid;
				StringToGuid(filterParts[0].c_str(), &filterGuid);

				Log(logInfo, "Adding filter %ls\n", filterParts[0].c_str());

				IBaseFilter *pFilter;

				//%CLSID_VideoCompressorCategory%="<codec name>"
				if (
					filterGuid == CLSID_AudioCompressorCategory ||
					filterGuid == CLSID_AudioInputDeviceCategory ||
					filterGuid == CLSID_AudioRendererCategory ||
					filterGuid == CLSID_LegacyAmFilterCategory ||
					filterGuid == CLSID_MediaEncoderCategory ||
					filterGuid == CLSID_MediaMultiplexerCategory ||
					filterGuid == CLSID_MidiRendererCategory ||
					filterGuid == CLSID_VideoCompressorCategory ||
					filterGuid == CLSID_VideoInputDeviceCategory
				)
				{
					hr = AddFilterCodec(g_pGraphBuilder, filterGuid, filterParts[1].c_str(), &pFilter);
					DBGHR("AddFilterCodec");
					if (FAILED(hr))
						Log(logError, "Adding filter failed\n");
				}

				else if (
					filterGuid == DMOCATEGORY_AUDIO_DECODER ||
					filterGuid == DMOCATEGORY_AUDIO_EFFECT ||
					filterGuid == DMOCATEGORY_AUDIO_ENCODER ||
					filterGuid == DMOCATEGORY_VIDEO_DECODER ||
					filterGuid == DMOCATEGORY_VIDEO_EFFECT ||
					filterGuid == DMOCATEGORY_VIDEO_ENCODER ||
					filterGuid == DMOCATEGORY_AUDIO_CAPTURE_EFFECT
				)
				{
					GUID clsid;
					StringToGuid(filterParts[1].c_str(), &clsid);
					hr = AddFilterDMO(
						g_pGraphBuilder,
						clsid,
						filterGuid,
						&pFilter
					);
					DBGHR("AddFilterDMO");
					if (FAILED(hr))
						Log(logError, "Adding DMO filter failed\n");
				}

				else if (filterParts.size() > 1)
				{
					hr = AddFilterByCLSIDAndPath(g_pGraphBuilder, filterGuid, &pFilter, filterParts[1].c_str(), NULL);
					DBGHR("AddFilterByCLSIDAndPath");
					if (FAILED(hr))
						Log(logError, "Loading filter from DLL failed\n");
				}

				else
				{
					hr = AddFilterByCLSID(g_pGraphBuilder, filterGuid, &pFilter);
					DBGHR("AddFilterByCLSID");
					if (FAILED(hr))
						Log(logError, "Adding filter failed\n");

					// if AVIMux, use its IMediaSeeking interface instead
					if (filterGuid == CLSID_AviMux)
					{
						SAFE_RELEASE(g_pMediaSeeking);

						hr = pFilter->QueryInterface(IID_IMediaSeeking, (void**)&g_pMediaSeeking);
						DBGHR("QueryInterface(IID_IMediaSeeking)");
					}
				}
					
				if (FAILED(hr)) continue; // ???

				pFilterList.push_back(pFilter);

				for (std::size_t i = 1; i < filterConf.size(); ++i)
				{
					if (filterConf[i].find(L"src=") == 0)
					{
						IFileSourceFilter * pFileSourceFilter;
						hr = pFilter->QueryInterface(IID_IFileSourceFilter, (void**)&pFileSourceFilter);
						DBGHR("IID_IFileSourceFilter");
						if (SUCCEEDED(hr))
						{
							hr = pFileSourceFilter->Load(filterConf[i].substr(4).c_str(), NULL);
							DBGHR("pFileSourceFilter->Load");
						}
					}

					else if (filterConf[i].find(L"dest=") == 0)
					{
						IFileSinkFilter * pFileSinkFilter;
						hr = pFilter->QueryInterface(IID_IFileSinkFilter, (void**)&pFileSinkFilter);
						DBGHR("IID_IFileSinkFilter");
						if (SUCCEEDED(hr))
						{
							hr = pFileSinkFilter->SetFileName(filterConf[i].substr(5).c_str(), NULL);
							DBGHR("pFileSinkFilter->SetFileName");
						}
					}

					else if (filterConf[i].find(L"sub=") == 0)
					{
						IDirectVobSub * pDirectVobSub;
						hr = pFilter->QueryInterface(IID_IDirectVobSub, (void**)&pDirectVobSub);
						DBGHR("IID_IFileSinkFilter");
						if (SUCCEEDED(hr))
						{
							hr = pDirectVobSub->put_FileName((WCHAR *)filterConf[i].substr(4).c_str());
							DBGHR("pDirectVobSub->put_FileName");
						}
					}

					else if (filterConf[i].find(L"qual=") == 0)
					{
						double q = _wtof(filterConf[i].substr(5).c_str());
						hr = SetVideoCompressionQuality(pFilter, q);
						DBGHR("SetVideoCompressionQuality");
					}
		
					if (filterConf[i].find(L"dialog") == 0)
					{
						if (filterParts.size() > 1)
						{
							// The filter was loaded from DLL, so we also load its dialog from DLL
							hr = ShowFilterPropertyPageDirect(pFilter, filterParts[1].c_str(), g_hWnd);
							DBGHR("ShowFilterPropertyPageDirect");
						}
						else
						{
							hr = ShowFilterPropertyPages(pFilter, g_hWnd);
							DBGHR("ShowFilterPropertyPages");	
						}
					}
					if (filterConf[i].find(L"pindialog") == 0)
					{
						hr = ShowPinPropertyPages(pFilter, g_hWnd);
						DBGHR("ShowPinPropertyPages");
					}
				}
			}

			//######################################
			// connect-list
			//######################################
			if (parts.size() > 1)
			{
				//std::wcout << parts[1] << L"\n"; // 0:1

				//0:1,1:2,0:3
				auto connections = Explode(parts[1], L',');
				for (const auto& conn : connections)
				{
					//std::wcout << conn << L"\n"; // 0:1
					auto pairs = Explode(conn, L':');
					if (pairs.size() > 1)
					{
						size_t from_index = _wtoi(pairs[0].c_str());
						size_t to_index = _wtoi(pairs[1].c_str());

						Log(logInfo, "Connecting filters %ld -> %ld\n", from_index, to_index);

						if (from_index >= 0 && from_index < pFilterList.size() && to_index >= 0 && to_index < pFilterList.size())
						{
							IPin *pOut = 0;
							IPin *pIn = 0;

							// Find an input pin on the downstream filter
							hr = GetUnconnectedPin(pFilterList[to_index], PINDIR_INPUT, &pIn);
							if (SUCCEEDED(hr))
							{
								hr = GetUnconnectedPin(pFilterList[from_index], PINDIR_OUTPUT, &pOut);
								if (SUCCEEDED(hr))
								{
									hr = g_pGraphBuilder->Connect(pOut, pIn);
									//DBGHR("Connect");
									pOut->Release();
								}

								if (FAILED(hr))
								{
									// try video
									hr = GetUnconnectedPin(pFilterList[from_index], PINDIR_OUTPUT, &pOut, MEDIATYPE_Video);
									if (SUCCEEDED(hr))
									{
										hr = g_pGraphBuilder->Connect(pOut, pIn);
										//DBGHR("Connect");
										pOut->Release();
									}
								}

								if (FAILED(hr))
								{
									// try audio
									hr = GetUnconnectedPin(pFilterList[from_index], PINDIR_OUTPUT, &pOut, MEDIATYPE_Audio);
									if (SUCCEEDED(hr))
									{
										hr = g_pGraphBuilder->Connect(pOut, pIn);
										//DBGHR("Connect");
										pOut->Release();
									}
								}

								pIn->Release();
							}

							if (FAILED(hr))
							{
								Log(logError, "Connecting filters %ld -> %ld failed\n", from_index, to_index);
								EXIT(ERROR_CONNECTING_FILTERS_FAILED);
							}
						}
					}
				}
			}

			//######################################
			// render-list
			//######################################
			if (parts.size() > 2)
			{
				auto render = Explode(parts[2], L',');
				for (const auto& r : render)
				{
					size_t render_index = _wtoi(r.c_str());
					if (render_index >= 0 && render_index < pFilterList.size())
					{
						Log(logInfo, "Rendering filter %ld...\n", render_index);

						//######################################
						// try to render all output pins
						//######################################
						IEnumPins *pEnumPins = NULL;
						IPin *pPin = NULL;
						PIN_DIRECTION ThisPinDir;
						BOOL bRendered = FALSE; // at least one pin successfully rendered?
						hr = pFilterList[render_index]->EnumPins(&pEnumPins);
						if (FAILED(hr))
						{
							Log(logError, "Rendering filter %ld failed\n", render_index);
							EXIT(ERROR_RENDER_FILTER_FAILED);
						}
						while (pEnumPins->Next(1, &pPin, NULL) == S_OK)
						{
							pPin->QueryDirection(&ThisPinDir);
							if (ThisPinDir == PINDIR_OUTPUT)
							{
								hr = g_pGraphBuilder->Render(pPin);
								if (SUCCEEDED(hr)) bRendered = TRUE;
							}
							pPin->Release();
						}
						pEnumPins->Release();
						if (!bRendered)
						{
							Log(logError, "Rendering filter %ld failed\n", render_index);
							EXIT(ERROR_RENDER_FILTER_FAILED);
						}
					}
				}
			}
		
			//######################################
			// Find and init renderer (for Legacy VideoRenderer only works after filters are connected?)
			//######################################
			hr = CheckAndInitRenderer(TRUE);

			g_bHasGraph = TRUE;
		}

		else
		{
			//######################################
			// -grf
			//######################################
			tmp = CmdOptionGet(argv, argv + argc, L"-grf");
			if (tmp)
			{
				hr = LoadGraphFile(g_pGraphBuilder, tmp);
				DBGHR("LoadGraphFile");
				if (FAILED(hr))
				{
					// clean up and exit
					EXIT(ERROR_LOAD_GRAPH_FAILED);
				}

				// Find and init renderer
				hr = CheckAndInitRenderer(TRUE);

				g_bHasGraph = TRUE;
			}
		}
	}

	if (g_bHasGraph)
	{
		//######################################
		// list names of all filters in graph
		//######################################
		if (g_logLevel >= logInfo)
			ListFiltersInGraph(g_pGraphBuilder);

		//######################################
		// No Clock
		//######################################
		tmp = CmdOptionGet(argv, argv + argc, L"-nc");
		if (tmp)
		{
			IMediaFilter * pMediaFilter;
			hr = g_pGraphBuilder->QueryInterface(IID_IMediaFilter, (void **)&pMediaFilter);
			DBGHR("QueryInterface(IID_IMediaFilter)");
			if (SUCCEEDED(hr))
			{
				//If this parameter is NULL, the filter graph does not use a reference clock, and all filters run as quickly as possible.
				hr = pMediaFilter->SetSyncSource(NULL);
				DBGHR("pMediaFilter->SetSyncSource");
				pMediaFilter->Release();
			}
		}

		//######################################
		// Save graph as GRF file
		//######################################
		tmp = CmdOptionGet(argv, argv + argc, L"-save");
		if (tmp)
		{
			hr = SaveGraphFile(g_pGraphBuilder, tmp);
			DBGHR("SaveGraphFile");
		}

		//######################################
		// Register graph
		//######################################
		if (CmdOptionExists(argv, argv + argc, L"-reg"))
		{
			DWORD dwRegister;
			hr = AddToRot(g_pGraphBuilder, &dwRegister);
			DBGHR("AddToRot");
		}

		//######################################
		// Rate (0.5, 1, 2)
		//######################################
		tmp = CmdOptionGet(argv, argv + argc, L"-rate");
		if (tmp)
		{
			double dRate = _wtof(tmp);
			hr = g_pMediaSeeking->SetRate(dRate);
			DBGHR("g_pMediaSeeking->SetRate");
		}

		//######################################
		// Start Time
		//######################################
		tmp = CmdOptionGet(argv, argv + argc, L"-start");
		if (tmp)
		{
			long ms = _wtol(tmp); // time / 10000
			g_starttime = ms * 10000; // ???
			//hr = MsToTime(g_pMediaSeeking, ms, &g_starttime);
			//DBGHR("ms_to_currtime");
			//if (SUCCEEDED(hr))
			//{
			hr = g_pMediaSeeking->SetPositions(&g_starttime, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);
			DBGHR("m_pMediaSeeking->SetPositions");
			//}
		}

		//######################################
		// Stop Time
		//######################################
		tmp = CmdOptionGet(argv, argv + argc, L"-stop");
		if (tmp)
		{
			long ms = _wtol(tmp);
			LONGLONG time;
			time = ms * 10000; // ???
			//hr = MsToTime(g_pMediaSeeking, ms, &time);
			//DBGHR("MsToTime");
			//if (SUCCEEDED(hr))
			//{
			hr = g_pMediaSeeking->SetPositions(NULL, AM_SEEKING_NoPositioning, &time, AM_SEEKING_AbsolutePositioning);
			DBGHR("m_pMediaSeeking->SetPositions");
			//}
		}

		// audio
		hr = g_pGraphBuilder->QueryInterface(IID_IBasicAudio, (void**)&g_pBasicAudio);
		if (g_pBasicAudio)
		{
			//######################################
			// Volume (-10000 = silent, 0 = maximum)
			//######################################
			tmp = CmdOptionGet(argv, argv + argc, L"-vol");
			if (tmp)
			{
				long vol = _wtoi(tmp);
				hr = g_pBasicAudio->put_Volume(vol);
				DBGHR("g_pBasicAudio->put_Volume");
			}

			//######################################
			// Balance (-10000 to 10000)
			//######################################
			tmp = CmdOptionGet(argv, argv + argc, L"-bal");
			if (tmp)
			{
				long bal = _wtoi(tmp);
				hr = g_pBasicAudio->put_Balance(bal);
				DBGHR("g_pBasicAudio->put_Balance");
			}
		}
	}

	// options that only apply for visible player window
	if (nCmdShow == SW_SHOW)
	{
		//######################################
		// Rect
		//######################################
		tmp = CmdOptionGet(argv, argv + argc, L"-rect");
		if (tmp)
		{
			auto parts = Explode(tmp, L',');
			if (parts.size() > 3)
			{
				int x, y, w, h;
				x = _wtoi(parts[0].c_str());
				y = _wtoi(parts[1].c_str());
				w = _wtoi(parts[2].c_str());
				h = _wtoi(parts[3].c_str());

				// find window rect for wanted client rect
				RECT r = { x , y, x+w, y+h };
				ok = AdjustWindowRect(&r, g_dwStyle, FALSE);
				ok = SetWindowPos(
					g_hWnd,
					NULL,
					r.left,
					r.top,
					r.right - r.left,
					r.bottom - r.top,
					0
				);
			}
		}
		else
		{
			LONG width, height;
			if (SUCCEEDED(GetVideoSize(&width, &height)))
			{
				ResizeWindow(g_hWnd, g_dwStyle, width, height);
			}
		}

		//######################################
		// Full Screen
		//######################################
		if (CmdOptionExists(argv, argv + argc, L"-fs"))
		{
			ToggleFullscreen();
		}

		//######################################
		// Hide Cursor
		//######################################
		if (CmdOptionExists(argv, argv + argc, L"-hc"))
		{
			ShowCursor(FALSE);
		};

		//######################################
		// Window Caption
		//######################################
		tmp = CmdOptionGet(argv, argv + argc, L"-wc");
		if (tmp) SetWindowTextW(g_hWnd, tmp);

		//######################################
		// Always on Top
		//######################################
		if (CmdOptionExists(argv, argv + argc, L"-top"))
		{
			ok = SetWindowPos(g_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		}

		// show the window
		ok = ShowWindow(g_hWnd, SW_SHOW);
	}

	hr = g_pGraphBuilder->QueryInterface(IID_IMediaEventEx, (void**)&g_pMediaEvent);
	DBGHR("QueryInterface(IID_IMediaEventEx)");
	if (SUCCEEDED(hr))
	{
		hr = g_pMediaEvent->SetNotifyWindow((OAHWND)g_hWnd, WM_USER, NULL);
		DBGHR("m_pMediaEvent->SetNotifyWindow");
	}

	if (g_bProgress)
		SetTimer(g_hWnd, IDT_CHECK_PROGRESS, 1000, (TIMERPROC)NULL);

	if (g_bHasGraph)
	{
		hr = g_pMediaSeeking->GetDuration(&g_duration);

		hr = g_pMediaControl->Run();
		DBGHR("m_pMediaControl->Run");
	}

	//######################################
	// Keyboard control in CMD shell
	//######################################
	if (CmdOptionExists(argv, argv + argc, L"-k"))
	{
		g_hStdin = GetStdHandle(STD_INPUT_HANDLE);
		if (g_hStdin == INVALID_HANDLE_VALUE)
		{
			Log(logError, "GetStdHandle failed.\n");
		}
		else
		{
			// create new thread for listening to console input
			g_hThread = CreateThread(0, 0, ConsoleThread, NULL, 0, NULL);
		}
	}

	//######################################
    // Main message loop
	//######################################

	MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
    }

clean_up_and_exit:
	SAFE_RELEASE(g_pIVMRWindowlessControl9);
	SAFE_RELEASE(g_pIVMRWindowlessControl);
	SAFE_RELEASE(g_pIMFVideoDisplayControl);
	SAFE_RELEASE(g_pVideoWindow);

	SAFE_RELEASE(g_pBasicAudio);
	SAFE_RELEASE(g_pMediaEvent);
	SAFE_RELEASE(g_pMediaSeeking);
	SAFE_RELEASE(g_pMediaControl);

	SAFE_RELEASE(g_pGraphBuilder);
	//SAFE_RELEASE(g_pFilterGraph);

	CoUninitialize();

	//printf("EXIT OK\n");
    return exitCode;
}
