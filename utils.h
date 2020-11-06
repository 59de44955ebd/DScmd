#pragma once

#include <dshow.h>
#include <d3d9.h>
#include <Vmr9.h>
#include <evr.h>

#include <comdef.h>
#include <qnetwork.h>
#include <io.h>
#include <Dmodshow.h>
#include <initguid.h>
#include <Dmoreg.h>
#include <wmcodecdsp.h>

_COM_SMARTPTR_TYPEDEF(IBaseFilter, __uuidof(IBaseFilter));
_COM_SMARTPTR_TYPEDEF(IPropertyPage, __uuidof(IPropertyPage));

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) if (x) { x->Release(); x = NULL; }
#endif

#ifndef CHECK_HR
#define CHECK_HR(hr) if (FAILED(hr)) { goto done; }
#endif

//#define DBGS(X) OutputDebugStringA(X)

void GetErrorDesc(HRESULT hr, char *name, char *desc);

//#define DBGHR(X) if(FAILED(hr)){char name[64];char desc[128];GetErrorDesc(hr, name, desc);printf("%s: %ld - %s - %s (Line %ld)\n",X,hr,name,desc,__LINE__);}
#define DBGHR(_x_) ((void)0)

//######################################
// Additional Media Subtypes
//######################################

// {08E22ADA-B715-45ed-9D20-7B87750301D4}
DEFINE_GUID(MEDIASUBTYPE_MP4,
	0x08e22ada, 0xb715, 0x45ed, 0x9d, 0x20, 0x7b, 0x87, 0x75, 0x03, 0x01, 0xd4);

// {D2855FA9-61A7-4db0-B979-71F297C17A04}
DEFINE_GUID(MEDIASUBTYPE_Ogg,
	0xd2855fa9, 0x61a7, 0x4db0, 0xb9, 0x79, 0x71, 0xf2, 0x97, 0xc1, 0x7a, 0x4);

// {F2FAC0F1-3852-4670-AAC0-9051D400AC54}
DEFINE_GUID(MEDIASUBTYPE_FLV,
	0xf2fac0f1, 0x3852, 0x4670, 0xaa, 0xc0, 0x90, 0x51, 0xd4, 0x00, 0xac, 0x54);

// {1AC0BEBD-4D2B-45ad-BCEB-F2C41C5E3788}
DEFINE_GUID(MEDIASUBTYPE_Matroska,
	0x1ac0bebd, 0x4d2b, 0x45ad, 0xbc, 0xeb, 0xf2, 0xc4, 0x1c, 0x5e, 0x37, 0x88);

//######################################
// Filter categories
//######################################

// {33D9A761-90C8-11D0-BD43-00A0C911CE86}
DEFINE_GUID(CLSID_AudioCompressorCategory, 
	0x33d9a761, 0x90c8, 0x11d0, 0xbd, 0x43, 0x0, 0xa0, 0xc9, 0x11, 0xce, 0x86);

// {33D9A762-90C8-11d0-BD43-00A0C911CE86}
DEFINE_GUID(CLSID_AudioInputDeviceCategory, 
	0x33d9a762, 0x90c8, 0x11d0, 0xbd, 0x43, 0x0, 0xa0, 0xc9, 0x11, 0xce, 0x86);

// {E0F158E1-CB04-11d0-BD4E-00A0C911CE86}
DEFINE_GUID(CLSID_AudioRendererCategory,
	0xe0f158e1, 0xcb04, 0x11d0, 0xbd, 0x4e, 0x0, 0xa0, 0xc9, 0x11, 0xce, 0x86);

// {083863F1-70DE-11d0-BD40-00A0C911CE86}
DEFINE_GUID(CLSID_LegacyAmFilterCategory,
	0x083863F1, 0x70DE, 0x11d0, 0xBD, 0x40, 0x00, 0xA0, 0xC9, 0x11, 0xCE, 0x86);

// {7D22E920-5CA9-4787-8C2B-A6779BD11781}
DEFINE_GUID(CLSID_MediaEncoderCategory,
	0x7D22E920, 0x5CA9, 0x4787, 0x8C, 0x2B, 0xA6, 0x77, 0x9B, 0xD1, 0x17, 0x81);

// {236C9559-ADCE-4736-BF72-BAB34E392196}
DEFINE_GUID(CLSID_MediaMultiplexerCategory,
	0x236C9559, 0xADCE, 0x4736, 0xBF, 0x72, 0xBA, 0xB3, 0x4E, 0x39, 0x21, 0x96);

// {4EFE2452-168A-11d1-BC76-00C04FB9453B}
DEFINE_GUID(CLSID_MidiRendererCategory,
	0x4EfE2452, 0x168A, 0x11d1, 0xBC, 0x76, 0x0, 0xc0, 0x4F, 0xB9, 0x45, 0x3B);

// {33D9A760-90C8-11D0-BD43-00A0C911CE86}
DEFINE_GUID(CLSID_VideoCompressorCategory, 
	0x33d9a760, 0x90c8, 0x11d0, 0xbd, 0x43, 0x0, 0xa0, 0xc9, 0x11, 0xce, 0x86);

// {860BB310-5D01-11d0-BD3B-00A0C911CE86}
DEFINE_GUID(CLSID_VideoInputDeviceCategory, 
	0x860BB310, 0x5D01, 0x11d0, 0xBD, 0x3B, 0x00, 0xA0, 0xC9, 0x11, 0xCE, 0x86);

// {94297043-BD82-4DFD-B0DE-8177739C6D20}
DEFINE_GUID(CLSID_DMOWrapperFilter, 
	0x94297043, 0xbd82, 0x4dfd, 0xb0, 0xde, 0x81, 0x77, 0x73, 0x9c, 0x6d, 0x20);

//######################################
// Specific filters
//######################################

// {EBE1FB08-3957-47CAAF13-5827E5442E56}
DEFINE_GUID(IID_IDirectVobSub,
	0xEBE1FB08, 0x3957, 0x47ca, 0xAF, 0x13, 0x58, 0x27, 0xE5, 0x44, 0x2E, 0x56);

// {E2510970-F137-11CE-8B67-00AA00A3F1A6}
DEFINE_GUID(CLSID_AviMux, 
	0xe2510970, 0xf137, 0x11ce, 0x8b, 0x67, 0x00, 0xaa, 0x00, 0xa3, 0xf1, 0xa6);

// {93A22E7A-5091-45ef-BA61-6DA2 6156 A5D0}
DEFINE_GUID(CLSID_DirectVobSub, 
	0x93A22E7A, 0x5091, 0x45ef, 0xba, 0x61,  0x6d, 0xa2, 0x61, 0x56, 0xa5, 0xd0);

// {9852A670-F845-491B-9BE6-EBD841B8A613}
DEFINE_GUID(CLSID_DirectVobSubAutoload,
	0x9852A670, 0xF845, 0x491B, 0x9B, 0xE6, 0xEB, 0xD8, 0x41, 0xB8, 0xA6, 0x13);

// {8E14549A-DB61-4309-AFA1-3578E927E933}
DEFINE_GUID(CLSID_SpoutCam, 
	0x8E14549A, 0xDB61, 0x4309, 0xaf, 0xa1,  0x35, 0x78, 0xE9, 0x27, 0xE9, 0x33);

// {69168CC9-E263-46C7-9F6C-7BB51FFCA6CB}
DEFINE_GUID(CLSID_ScreenCam,
	0x69168CC9, 0xE263, 0x46C7, 0x9F, 0x6C, 0x7B, 0xB5, 0x1F, 0xFC, 0xA6, 0xCB);

// {B98D13E7-55DB-4385-A33D-09FD1BA26338}
DEFINE_GUID(CLSID_LAVSplitterSource,
	0xB98D13E7, 0x55DB, 0x4385, 0xA3, 0x3D, 0x09, 0xFD, 0x1B, 0xA2, 0x63, 0x38);

// {171252A0-8820-4AFE-9DF8-5C92B2D66B04}
DEFINE_GUID(CLSID_LAVSplitter,
	0x171252A0, 0x8820, 0x4AFE, 0x9D, 0xF8, 0x5C, 0x92, 0xB2, 0xD6, 0x6B, 0x04);

// {EE30215D-164F-4A92-A4EB-9D4C13390F9F}
DEFINE_GUID(CLSID_LAVVideoDecoder,
	0xEE30215D, 0x164F, 0x4A92, 0xA4, 0xEB, 0x9D, 0x4C, 0x13, 0x39, 0x0F, 0x9F);

// {E8E73B6B-4CB3-44A4-BE99-4F7BCB96E491}
DEFINE_GUID(CLSID_LAVAudioDecoder,
	0xE8E73B6B, 0x4CB3, 0x44A4, 0xBE, 0x99, 0x4F, 0x7B, 0xCB, 0x96, 0xE4, 0x91);


// TMP

// {081E5C27-E182-4DA0-8C3A-5F55D787A2AE}
DEFINE_GUID(IID_ICamSettings,
	0x81e5c27, 0xe182, 0x4da0, 0x8c, 0x3a, 0x5f, 0x55, 0xd7, 0x87, 0xa2, 0xae);

// {DE54BC4E-7717-4C9B-AC9A-8209B63F5EAD}
DEFINE_GUID(CLSID_ScreenCamPropertyPage,
	0xde54bc4e, 0x7717, 0x4c9b, 0xac, 0x9a, 0x82, 0x9, 0xb6, 0x3f, 0x5e, 0xad);

typedef struct tagVIDEOINFOHEADER2 
{ 
	RECT rcSource; 
	RECT rcTarget; 
	DWORD dwBitRate; 
	DWORD dwBitErrorRate; 
	REFERENCE_TIME AvgTimePerFrame; 
	DWORD dwInterlaceFlags; 
	DWORD dwCopyProtectFlags; 
	DWORD dwPictAspectRatioX; 
	DWORD dwPictAspectRatioY; 
	DWORD dwReserved1; 
	DWORD dwReserved2; 
	BITMAPINFOHEADER bmiHeader; 
} VIDEOINFOHEADER2;

enum LogLevel
{
	logError,
	logWarning,
	logInfo
};

void Log(UINT level, const char * msg...);

void StringToGuid(const WCHAR *guidstr, GUID *clsid);
void GuidToString(WCHAR *guidstr, GUID clsid);

HRESULT AddToRot(
	IUnknown *pUnkGraph, 
	DWORD *pdwRegister
);

void RemoveFromRot(DWORD pdwRegister);

HRESULT LoadGraphFile(
	IGraphBuilder *pGraph, 
	const WCHAR* wszName
);

HRESULT SaveGraphFile(
	IGraphBuilder *pGraph, 
	const WCHAR *wszPath
);

HRESULT FindFilterByGuid(
	IGraphBuilder *m_pGraphBuilder, 
	GUID guid, 
	IBaseFilter **pFilter
);

HRESULT GetPin(
	IBaseFilter *pFilter,
	PIN_DIRECTION PinDir,
	IPin **ppPin,
	GUID majortype = GUID_NULL
);

HRESULT GetUnconnectedPin(
	IBaseFilter *pFilter,
	PIN_DIRECTION PinDir,
	IPin **ppPin,
	GUID majortype = GUID_NULL
);

HRESULT GetConnectedPins(
	IBaseFilter *pFilter,
	PIN_DIRECTION PinDir,
	IPin **ppPin,
	IPin **ppPinTo,
	GUID majortype = GUID_NULL
);

HRESULT ShowFilterPropertyPages(
	IBaseFilter *pFilter, 
	HWND hWnd = NULL
);

HRESULT ShowFilterPropertyPageDirect(
	IBaseFilter *pFilter,
	const WCHAR *pPath,
	HWND hWnd = NULL
);

HRESULT ShowPinPropertyPages(
	IBaseFilter *pFilter, 
	HWND hWnd = NULL
);

HRESULT SetVideoCompressionQuality(
	IBaseFilter *pFilter, 
	double q
);

HRESULT AddFilterByCLSID(
	IGraphBuilder *pGraph,          // Pointer to the Filter Graph Manager.
	const GUID& clsid,              // CLSID of the filter to create.
	IBaseFilter **ppF,              // Receives a pointer to the filter.
	const WCHAR *wszName = NULL     // A name for the filter (can be NULL).
);

HRESULT CreateObjectFromPath(
	const WCHAR * pPath, 
	REFCLSID clsid, 
	IUnknown** ppUnk
);

HRESULT AddFilterByCLSIDAndPath(
	IGraphBuilder *pGraph,          // Pointer to the Filter Graph Manager
	GUID clsid,      				// CLSID of the filter to create  // GUID& clsid ???
	IBaseFilter **ppF,
	const WCHAR *pPath,
	const WCHAR *wszName = NULL     // A name for the filter (NULL -> default name)
);

HRESULT AddFilterCodec(
	IGraphBuilder *pGraph,
	GUID clsidCategory,
	const WCHAR *wszCodecName,
	IBaseFilter **ppF
);

HRESULT AddFilterDMO(
	IGraphBuilder *pGraph,         // Pointer to the Filter Graph Manager
	GUID clsid,			           // CLSID of the filter to create
	GUID clsidDmoCategory,         // CLSID of the DMO category (e.g. DMOCATEGORY_VIDEO_ENCODER)
	IBaseFilter **ppF,             // Receives a pointer to the filter
	const WCHAR *wszName = NULL    // A name for the filter, e.g. L"WMV9DMO Encoder" (NULL -> default name)
);

HRESULT GetCodecName(
	IMoniker* pMoniker, 
	WCHAR* wszCodecName
);

HRESULT GetPinMediaType(
	IPin *pPin,             // pointer to the pin
	REFGUID majorType,      // desired major type, or GUID_NULL = don't care
	REFGUID subType,        // desired subtype, or GUID_NULL = don't care
	REFGUID formatType      // desired format type, of GUID_NULL = don't care
	//AM_MEDIA_TYPE **ppmt  // Receives a pointer to the media type. (Can be NULL)
);

HRESULT ListFiltersInGraph(IGraphBuilder *pGraph);

const char * GetEventName(int e);

BOOL ResizeWindow(HWND hWnd, DWORD dwStyle, LONG w, LONG h);

const WCHAR * GetKnownFilterName(GUID clsid);

HRESULT FindFilterInterface(
	IGraphBuilder *pGraph, // Pointer to the Filter Graph Manager.
	REFGUID iid,           // IID of the interface to retrieve.
	void **ppUnk           // Receives the interface pointer.
);
