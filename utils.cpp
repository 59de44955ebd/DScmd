#include <iostream>

#include "utils.h"

#pragma comment(lib, "Comctl32.lib")

extern HRESULT hr;
extern UINT g_logLevel;

//######################################
// Specify verbosity : 0 = quiet, 1 = errors, 2 = warnings, 3 = infos (=default)
//######################################
void Log(UINT level, const char * msg...)
{
	if (level <= g_logLevel)
	{
		va_list argptr;
		va_start(argptr, msg);
		vfprintf(stderr, msg, argptr);
		va_end(argptr);
	}
}

//######################################
//
//######################################
void StringToGuid(const WCHAR *guidstr, GUID *clsid)
{
	unsigned long d1;
	unsigned short d2;
	unsigned short d3;
	int d4[8];
	if (guidstr[0] == L'{') guidstr++;
	swscanf_s(
		guidstr,
		L"%x-%hx-%hx-%02x%02x-%02x%02x%02x%02x%02x%02x",
		&d1, &d2, &d3, &d4[0], &d4[1], &d4[2], &d4[3], &d4[4], &d4[5], &d4[6], &d4[7]
	);

	clsid->Data1 = d1;
	clsid->Data2 = d2;
	clsid->Data3 = d3;
	for (int i = 0; i < 8; i++) clsid->Data4[i] = (unsigned char)d4[i];
}

//######################################
//
//######################################
void GuidToString(WCHAR *guidstr, GUID clsid)
{
	swprintf(guidstr, 39, L"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
		clsid.Data1,
		clsid.Data2,
		clsid.Data3,
		clsid.Data4[0],
		clsid.Data4[1],
		clsid.Data4[2],
		clsid.Data4[3],
		clsid.Data4[4],
		clsid.Data4[5],
		clsid.Data4[6],
		clsid.Data4[7]
	);
}

//######################################
// 
//######################################
HRESULT AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister)
{
	IMoniker * pMoniker = NULL;
	IRunningObjectTable *pROT = NULL;
	if (FAILED(GetRunningObjectTable(0, &pROT)))
	{
		return E_FAIL;
	}
	const size_t STRING_LENGTH = 256;
	WCHAR wsz[STRING_LENGTH];
	StringCchPrintfW(
		wsz, STRING_LENGTH,
		L"FilterGraph %08x pid %08x",
		(DWORD_PTR)pUnkGraph,
		GetCurrentProcessId()
	);
	hr = CreateItemMoniker(L"!", wsz, &pMoniker);
	if (SUCCEEDED(hr))
	{
		hr = pROT->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE, pUnkGraph, pMoniker, pdwRegister);
		pMoniker->Release();
	}
	pROT->Release();
	return hr;
}

//######################################
// 
//######################################
void RemoveFromRot(DWORD pdwRegister){
	IRunningObjectTable *pROT;
	if (SUCCEEDED(GetRunningObjectTable(0, &pROT))) {
		pROT->Revoke(pdwRegister);
		pROT->Release();
	}
}

//######################################
// 
//######################################
HRESULT LoadGraphFile(IGraphBuilder *pGraph, const WCHAR* wszName)
{
	if (!pGraph) return E_POINTER;

    IStorage *pStorage = NULL;
    if (S_OK != StgIsStorageFile(wszName))
    {
        return E_FAIL;
    }
    hr = StgOpenStorage(wszName, 0, STGM_TRANSACTED | STGM_READ | STGM_SHARE_DENY_WRITE, 0, 0, &pStorage);
    if (FAILED(hr))
    {
        return hr;
    }
    IPersistStream *pPersistStream = NULL;
    hr = pGraph->QueryInterface(IID_IPersistStream, reinterpret_cast<void**>(&pPersistStream));
    if (SUCCEEDED(hr))
    {
        IStream *pStream = NULL;
        hr = pStorage->OpenStream(L"ActiveMovieGraph", 0, STGM_READ | STGM_SHARE_EXCLUSIVE, 0, &pStream);
        if(SUCCEEDED(hr))
        {
            hr = pPersistStream->Load(pStream);
            pStream->Release();
        }
        pPersistStream->Release();
    }
    pStorage->Release();
    return hr;
}

//######################################
// 
//######################################
HRESULT SaveGraphFile(IGraphBuilder *pGraph, const WCHAR *wszPath) 
{
	if (!pGraph) return E_POINTER;

    const WCHAR wszStreamName[] = L"ActiveMovieGraph"; 
    IStorage *pStorage = NULL;
    hr = StgCreateDocfile(wszPath, STGM_CREATE | STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE, 0, &pStorage);
    if(FAILED(hr)) 
    {
        return hr;
    }

    IStream *pStream;
    hr = pStorage->CreateStream(wszStreamName, STGM_WRITE | STGM_CREATE | STGM_SHARE_EXCLUSIVE, 0, 0, &pStream);
    if (FAILED(hr)) 
    {
        pStorage->Release();    
        return hr;
    }

    IPersistStream *pPersist = NULL;
    pGraph->QueryInterface(IID_IPersistStream, (void**)&pPersist);
    hr = pPersist->Save(pStream, TRUE);
    pStream->Release();
    pPersist->Release();
    if (SUCCEEDED(hr)) 
    {
        hr = pStorage->Commit(STGC_DEFAULT);
    }
    pStorage->Release();
    return hr;
}

//######################################
//
//######################################
HRESULT FindFilterByGuid(IGraphBuilder *pGraph, GUID guid, IBaseFilter **pFilter)
{
	if (!pGraph) return E_POINTER;

	IEnumFilters *pEnumFilters;
	GUID clsid;
	hr = pGraph->EnumFilters(&pEnumFilters);
	if (FAILED(hr)) return hr;
	while (pEnumFilters->Next(1, pFilter, NULL) == S_OK)
	{
		(*pFilter)->GetClassID(&clsid);
		if (clsid == guid)
		{
			pEnumFilters->Release();
			return S_OK;
		}
		(*pFilter)->Release();
	}
	pEnumFilters->Release();
	return E_NOINTERFACE;
}

//######################################
//
//######################################
void _FreeMediaType(AM_MEDIA_TYPE& mt)
{
	if (mt.cbFormat != 0)
	{
		CoTaskMemFree((PVOID)mt.pbFormat);
		mt.cbFormat = 0;
		mt.pbFormat = NULL;
	}
	if (mt.pUnk != NULL)
	{
		// pUnk should not be used.
		mt.pUnk->Release();
		mt.pUnk = NULL;
	}
}

//######################################
// Delete a media type structure that was allocated on the heap.
//######################################
void _DeleteMediaType(AM_MEDIA_TYPE *pmt)
{
	if (pmt != NULL)
	{
		_FreeMediaType(*pmt);
		CoTaskMemFree(pmt);
	}
}

//######################################
// Given a pin, find a preferred media type 
//
// pPin         Pointer to the pin.
// majorType    Preferred major type (GUID_NULL = don't care).
// subType      Preferred subtype (GUID_NULL = don't care).
// formatType   Preferred format type (GUID_NULL = don't care).
// ppmt         Receives a pointer to the media type. Can be NULL.
//
// Note: If you want to check whether a pin supports a desired media type,
//       but do not need the format details, set ppmt to NULL.
//
//       If ppmt is not NULL and the method succeeds, the caller must
//       delete the media type, including the format block. 
//######################################
HRESULT GetPinMediaType(
	IPin *pPin,             // pointer to the pin
	REFGUID majorType,      // desired major type, or GUID_NULL = don't care
	REFGUID subType,        // desired subtype, or GUID_NULL = don't care
	REFGUID formatType     // desired format type, of GUID_NULL = don't care
	//AM_MEDIA_TYPE **ppmt    // Receives a pointer to the media type. (Can be NULL)
)
{
	if (!pPin) return E_POINTER;
	//*ppmt = NULL;

	IEnumMediaTypes *pEnum = NULL;
	AM_MEDIA_TYPE *pmt = NULL;
	BOOL bFound = FALSE;

	HRESULT hr = pPin->EnumMediaTypes(&pEnum);
	if (FAILED(hr))
	{
		return hr;
	}

	while (hr = pEnum->Next(1, &pmt, NULL), hr == S_OK)
	{
		if ((majorType == GUID_NULL) || (majorType == pmt->majortype))
		{
			if ((subType == GUID_NULL) || (subType == pmt->subtype))
			{
				if ((formatType == GUID_NULL) ||
					(formatType == pmt->formattype))
				{
					// Found a match. 
					//if (ppmt)
					//{
					//	*ppmt = pmt;  // Return it to the caller
					//}
					//else
					//{
					_DeleteMediaType(pmt);
					//}
					bFound = TRUE;
					break;
				}
			}
		}
		_DeleteMediaType(pmt);
	}

	SAFE_RELEASE(pEnum);

	if (SUCCEEDED(hr))
	{
		if (!bFound)
		{
			hr = VFW_E_NOT_FOUND;
		}
	}
	return hr;
}

//######################################
// Find a pin on a filter.
//######################################
HRESULT GetPin(
	IBaseFilter *pFilter,   // Pointer to the filter.
	PIN_DIRECTION PinDir,   // Direction of the pin to find.
	IPin **ppPin,            // Receives a pointer to the pin.
	GUID majortype
)
{
	if (!pFilter) return E_POINTER;

	*ppPin = NULL;
	IEnumPins *pEnum;
	IPin *pPin;
	hr = pFilter->EnumPins(&pEnum);
	if (FAILED(hr))
	{
		return hr;
	}
	while (pEnum->Next(1, &pPin, NULL) == S_OK)
	{
		PIN_DIRECTION ThisPinDir;
		pPin->QueryDirection(&ThisPinDir);
		if (ThisPinDir == PinDir)
		{
			if (majortype != GUID_NULL)
			{
				hr = GetPinMediaType(pPin, majortype, GUID_NULL, GUID_NULL);
				if (SUCCEEDED(hr))
				{
					pEnum->Release();
					*ppPin = pPin;
					return S_OK;
				}
			}
			else
			{
				pEnum->Release();
				*ppPin = pPin;
				return S_OK;
			}
		}
		pPin->Release();
	}
	pEnum->Release();

	// Did not find a matching pin.
	return E_FAIL;
}

//######################################
// Find an unconnected pin on a filter.
// This too is stolen from the DX9 SDK.
//######################################
HRESULT GetUnconnectedPin(
	IBaseFilter *pFilter,   // Pointer to the filter.
	PIN_DIRECTION PinDir,   // Direction of the pin to find.
	IPin **ppPin,            // Receives a pointer to the pin.
	GUID majortype
)
{
	if (!pFilter) return E_POINTER;

	*ppPin = NULL;
	IEnumPins *pEnum;
	IPin *pPin;
	hr = pFilter->EnumPins(&pEnum);
	if (FAILED(hr))
	{
		return hr;
	}
	while (pEnum->Next(1, &pPin, NULL) == S_OK)
	{
		PIN_DIRECTION ThisPinDir;
		pPin->QueryDirection(&ThisPinDir);
		if (ThisPinDir == PinDir)
		{
			IPin *pTmp = NULL;
			hr = pPin->ConnectedTo(&pTmp);
			if (SUCCEEDED(hr)) // Already connected - not the pin we want
			{
				pTmp->Release();
			}
			else // Unconnected - this is the pin we want
			{
				if (majortype != GUID_NULL)
				{
					hr = GetPinMediaType(pPin, majortype, GUID_NULL, GUID_NULL);
					if (SUCCEEDED(hr))
					{
						pEnum->Release();
						*ppPin = pPin;
						return S_OK;
					}
				}
				else
				{
					pEnum->Release();
					*ppPin = pPin;
					return S_OK;
				}
			}
		}
		pPin->Release();
	}
	pEnum->Release();

	// Did not find a matching pin.
	return E_FAIL;
}

//######################################
// 
//######################################
HRESULT GetConnectedPins(
	IBaseFilter *pFilter,   // Pointer to the filter.
	PIN_DIRECTION PinDir,   // Direction of the pin to find.
	IPin **ppPin,           // Receives a pointer to the pin.
	IPin **ppPinTo,
	GUID majortype
)
{
	if (!pFilter) return E_POINTER;

	*ppPin = NULL;
	IEnumPins *pEnum;
	IPin *pPin;
	hr = pFilter->EnumPins(&pEnum);
	if (FAILED(hr))
	{
		return hr;
	}
	while (pEnum->Next(1, &pPin, NULL) == S_OK)
	{
		PIN_DIRECTION ThisPinDir;
		pPin->QueryDirection(&ThisPinDir);
		if (ThisPinDir == PinDir)
		{
			IPin *pTmp = NULL;
			hr = pPin->ConnectedTo(&pTmp);
			if (SUCCEEDED(hr))  // connected - the pins we want
			{

				if (majortype != GUID_NULL)
				{
					hr = GetPinMediaType(pPin, majortype, GUID_NULL, GUID_NULL);
					if (SUCCEEDED(hr))
					{
						pEnum->Release();
						*ppPin = pPin;
						*ppPinTo = pTmp;
						return S_OK;
					}
				}
				else
				{
					pEnum->Release();
					*ppPin = pPin;
					*ppPinTo = pTmp;
					return S_OK;
				}
			}
		}
		pPin->Release();
	}
	pEnum->Release();

	// Did not find a matching pin.
	return E_FAIL;
}

//######################################
// Show the property pages for a filter.
// This is stolen from the DX9 SDK.
//######################################
HRESULT ShowFilterPropertyPages(IBaseFilter *pFilter, HWND hWnd)
{
	if (!pFilter) return E_POINTER;

	ISpecifyPropertyPages *pSpecifyPropertyPages;
	hr = pFilter->QueryInterface(IID_ISpecifyPropertyPages, reinterpret_cast<void**>(&pSpecifyPropertyPages));
	if (FAILED(hr)) return hr;

	// Get the filter's name and IUnknown pointer.
	FILTER_INFO FilterInfo;
	hr = pFilter->QueryFilterInfo(&FilterInfo);
	DBGHR("pFilter->QueryFilterInfo");

	IUnknown *pFilterUnk = reinterpret_cast<IUnknown*>(pFilter);

	// Show the page. 
	CAUUID caGUID;
	pSpecifyPropertyPages->GetPages(&caGUID);

	hr = OleCreatePropertyFrame(
		hWnd,                   // Parent window
		0, 0,                   // Reserved
		FilterInfo.achName,     // Caption for the dialog box
		1,                      // # of objects (just the filter)
		&pFilterUnk,            // Array of object pointers. 
		caGUID.cElems,          // Number of property pages
		caGUID.pElems,          // Array of property page CLSIDs
		0,                      // Locale identifier
		0, NULL                 // Reserved
	);
	DBGHR("pFilter->OleCreatePropertyFrame");

	// Clean up
	pSpecifyPropertyPages->Release();
	FilterInfo.pGraph->Release();
	CoTaskMemFree(caGUID.pElems);

	//if (FAILED(hr))
	//{
	//	// Try IAMVfwCompressDialog
	//	IAMVfwCompressDialogs *pCompressDialog;
	//	hr = pFilter->QueryInterface(IID_IAMVfwCompressDialogs, (void **)&pCompressDialog);
	//	if (SUCCEEDED(hr))
	//	{
	//		//VfwCompressDialog_Config	= 0x1,
	//		//VfwCompressDialog_About	= 0x2,
	//		//VfwCompressDialog_QueryConfig	= 0x4,
	//		//VfwCompressDialog_QueryAbout	= 0x8
	//		hr = pCompressDialog->ShowDialog(VfwCompressDialog_Config, 0);
	//	}
	//}

	//if (FAILED(hr))
	//{
	//	// Try IAMVfwCaptureDialogs
	//	IAMVfwCaptureDialogs *pCaptureDialogs;
	//	hr = pFilter->QueryInterface(IID_IAMVfwCaptureDialogs, (void **)&pCaptureDialogs);
	//	if (SUCCEEDED(hr))
	//	{
	//		//VfwCaptureDialog_Source	= 0x1,
	//		//VfwCaptureDialog_Format	= 0x2,
	//		//VfwCaptureDialog_Display	= 0x4
	//		hr = pCaptureDialogs->ShowDialog(VfwCaptureDialog_Source, 0);
	//	}
	//}

	return hr;
}

extern "C" {
	HRESULT WINAPI OleCreatePropertyFrameDirect(
		HWND hwndOwner,
		LPCOLESTR lpszCaption,
		LPUNKNOWN* ppUnk,
		IPropertyPage ** page,
		ULONG cPages
	);
}

//######################################
//
//######################################
HRESULT ShowFilterPropertyPagesDLL(IBaseFilter *pFilter, const WCHAR *pPath, HWND hwnd)
{
	if (!pFilter) return E_POINTER;

	ISpecifyPropertyPages *pSpecifyPropertyPages;
	hr = pFilter->QueryInterface(IID_ISpecifyPropertyPages, reinterpret_cast<void**>(&pSpecifyPropertyPages));
	if (FAILED(hr)) return hr;

	// Get the filter's name
	FILTER_INFO FilterInfo;
	hr = pFilter->QueryFilterInfo(&FilterInfo);
	DBGHR("pFilter->QueryFilterInfo");

	// Get the pages
	CAUUID caGUID;
	hr = pSpecifyPropertyPages->GetPages(&caGUID);
	DBGHR("pSpecifyPropertyPages->GetPages");

	IPropertyPagePtr * ppPages = new IPropertyPagePtr[caGUID.cElems];

	for (ULONG i = 0; i < caGUID.cElems; i++)
	{
		IUnknownPtr pUnk;
		hr = CreateObjectFromPath(pPath, caGUID.pElems[i], &pUnk);
		if (FAILED(hr)) 
			goto done;
		ppPages[i] = pUnk;
	}

	hr = OleCreatePropertyFrameDirect(
		hwnd,					// Parent window
		FilterInfo.achName,		// Caption for the dialog box
		(IUnknown **)&pFilter,  // Pointer to the filter
		(IPropertyPage **)ppPages,
		caGUID.cElems
	);
	DBGHR("OleCreatePropertyFrameDirect2");

	// Clean up
done:
	delete[] ppPages;
	pSpecifyPropertyPages->Release();
	FilterInfo.pGraph->Release();
	CoTaskMemFree(caGUID.pElems);

	return hr;
}

//######################################
// 
//######################################
HRESULT SetVideoCompressionQuality(IBaseFilter *pFilter, double q)
{
	if (!pFilter) return E_POINTER;

	IPin * pPin = NULL;
	hr = GetPin(pFilter, PINDIR_OUTPUT, &pPin);

	if (FAILED(hr)) return hr;

	IAMVideoCompression * pVideoCompression;
	hr = pPin->QueryInterface(IID_IAMVideoCompression, reinterpret_cast<void**>(&pVideoCompression));
	DBGHR("pPin->IID_IAMVideoCompression");
	if (SUCCEEDED(hr))
	{
		hr = pVideoCompression->put_Quality(q);
		DBGHR("pVideoCompression->put_Quality");

		//HRESULT put_KeyFrameRate(long KeyFrameRate);

		pVideoCompression->Release();
	}

	pPin->Release();

	return hr;
}

//######################################
// Show the property pages for a pin.
//######################################
HRESULT ShowPinPropertyPages(IBaseFilter *pFilter, HWND hWnd)
{
	if (!pFilter) return E_POINTER;

	// try first output pin
	IEnumPins *pEnum = 0;
	IPin *pPin = 0;
	hr = pFilter->EnumPins(&pEnum);
	if (SUCCEEDED(hr))
	{
		while (pEnum->Next(1, &pPin, NULL) == S_OK)
		{
			PIN_DIRECTION ThisPinDir;
			pPin->QueryDirection(&ThisPinDir);
			if (ThisPinDir == PINDIR_OUTPUT)
			{
				ISpecifyPropertyPages *pPinProp;
				hr = pPin->QueryInterface(IID_ISpecifyPropertyPages, reinterpret_cast<void**>(&pPinProp));
				if (SUCCEEDED(hr))
				{
					// Get the pins's name and IUnknown pointer.
					FILTER_INFO FilterInfo;
					hr = pFilter->QueryFilterInfo(&FilterInfo);

					IUnknown *pFilterUnk;
					pPin->QueryInterface(IID_IUnknown, reinterpret_cast<void**>(&pFilterUnk));

					// Show the page. 
					CAUUID caGUID;
					pPinProp->GetPages(&caGUID);

					OleCreatePropertyFrame(
						hWnd,                   // Parent window
						0, 0,                   // Reserved
						FilterInfo.achName,     // Caption for the dialog box
						1,                      // # of objects (just the filter)
						&pFilterUnk,            // Array of object pointers. 
						caGUID.cElems,          // Number of property pages
						caGUID.pElems,          // Array of property page CLSIDs
						0,                      // Locale identifier
						0, NULL                 // Reserved
					);

					// Clean up
					pPinProp->Release();
					pFilterUnk->Release();
					FilterInfo.pGraph->Release();
					CoTaskMemFree(caGUID.pElems);
				}

				pPin->Release();
				break;
			}
			pPin->Release();
		}
	}
	pEnum->Release();

	return hr;
}

//######################################
//
//######################################
HRESULT FindRegFilterByGUID(WCHAR *wfiltername, GUID guid)
{
	IFilterMapper *pFilterMapper = NULL;
	IEnumRegFilters	*pEnumRegFilters = NULL;
	REGFILTER *pRegFilter;

	hr = CoCreateInstance(CLSID_FilterMapper, NULL, CLSCTX_INPROC_SERVER, IID_IFilterMapper, reinterpret_cast<void**>(&pFilterMapper));
	if (FAILED(hr)) return hr;

	hr = pFilterMapper->EnumMatchingFilters(&pEnumRegFilters, 0, FALSE, GUID_NULL, GUID_NULL, FALSE, FALSE, GUID_NULL, GUID_NULL);
	if (SUCCEEDED(hr))
	{
		hr = E_FAIL;
		while (pEnumRegFilters->Next(1, &pRegFilter, NULL) == S_OK)
		{
			if (pRegFilter->Clsid == guid)
			{
				wcscpy_s(wfiltername, MAX_FILTER_NAME, pRegFilter->Name);
				hr = S_OK;
				break;
			}
		}
		SAFE_RELEASE(pEnumRegFilters);
	}
	SAFE_RELEASE(pFilterMapper);
	return hr;
}

//######################################
//
//######################################
HRESULT AddFilterByCLSID(
	IGraphBuilder *pGraph,		// Pointer to the Filter Graph Manager.
	const GUID& clsid,			// CLSID of the filter to create.
	IBaseFilter **ppF,			// Receives a pointer to the filter.
	const WCHAR * wszName		// A name for the filter (can be NULL).
)
{
	if (!pGraph || !ppF) return E_POINTER;

	*ppF = NULL;

	IBaseFilter *pFilter = NULL;
	hr = CoCreateInstance(
		clsid,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IBaseFilter,
		(void**)&pFilter);
	if (SUCCEEDED(hr))
	{
		if (wszName)
			hr = pGraph->AddFilter(pFilter, wszName);
		else
		{
			WCHAR tmp[MAX_FILTER_NAME];
			hr = FindRegFilterByGUID(tmp, clsid);
			hr = pGraph->AddFilter(pFilter, SUCCEEDED(hr)?tmp:NULL);
		}

		if (SUCCEEDED(hr))
		{
			*ppF = pFilter;
			(*ppF)->AddRef();
		}
		SAFE_RELEASE(pFilter);
	}
	return hr;
}

// define the prototype of the class factory entry point in a COM dll
typedef HRESULT(STDAPICALLTYPE* FN_DLLGETCLASSOBJECT)(REFCLSID clsid, REFIID iid, void** ppv);

//######################################
//
//######################################
HRESULT CreateObjectFromPath(const WCHAR * pPath, REFCLSID clsid, IUnknown** ppUnk)
{
	// load the target DLL directly
	HMODULE lib = LoadLibrary(pPath);
	if (!lib)
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}

	// the entry point is an exported function
	FN_DLLGETCLASSOBJECT fn = (FN_DLLGETCLASSOBJECT)GetProcAddress(lib, "DllGetClassObject");
	if (fn == NULL)
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}

	// create a class factory
	IUnknownPtr pUnk;
	hr = fn(clsid, IID_IUnknown, reinterpret_cast<void**>(&pUnk)); // was (void**)(IUnknown**)&pUnk)
	if (SUCCEEDED(hr))
	{
		IClassFactoryPtr pCF = pUnk;
		if (pCF == NULL)
		{
			hr = E_NOINTERFACE;
		}
		else
		{
			// ask the class factory to create the object
			hr = pCF->CreateInstance(NULL, IID_IUnknown, reinterpret_cast<void**>(ppUnk));
		}
	}
	return hr;
}

//	def _create_object_from_path (self, clsid, dll_filename, interface=IBaseFilter):
//		iclassfactory = self._raw_guid(IClassFactory._iid_)
//		my_dll = oledll.LoadLibrary(dll_filename)
//		factory_ptr = c_void_p(0)

//		hr = my_dll.DllGetClassObject(self._raw_guid(clsid), iclassfactory, byref(factory_ptr))
//		if hr!=S_OK:
//			raise COMError(hr, '', '')
//		ptr_icf = POINTER(IClassFactory)(factory_ptr.value)
//		unk = ptr_icf.CreateInstance()
//
//		# if ScreenCam or SpoutCam is loaded from local file, we also grab its property page
//		if clsid==CLSID_ScreenCam or clsid==CLSID_SpoutCam:
//			factory_ptr = c_void_p(0)

//			hr = my_dll.DllGetClassObject(
//					self._raw_guid(CLSID_SpoutCamPropertyPage if clsid==CLSID_SpoutCam else CLSID_ScreenCamPropertyPage),
//					iclassfactory,
//					byref(factory_ptr))
//			if hr!=S_OK:
//				raise COMError(hr, '', '')
//			ptr_icf = POINTER(IClassFactory)(factory_ptr.value)
//			unk2 = ptr_icf.CreateInstance()
//			self._page = unk2.QueryInterface(IPropertyPage)
//		return unk.QueryInterface(interface)


//######################################
//
//######################################
HRESULT AddFilterByCLSIDAndPath(
	IGraphBuilder *pGraph,  // Pointer to the Filter Graph Manager
	GUID clsid,      	    // CLSID of the filter to create
	IBaseFilter **ppF,      // Receives a pointer to the filter
	const WCHAR * pPath,    // Path of filter DLL
	const WCHAR * wszName   // A name for the filter (NULL -> default name)
)
{
	if (!pGraph || !ppF) return E_POINTER;
	*ppF = NULL;
	IUnknownPtr pUnk;
	hr = CreateObjectFromPath(pPath, clsid, &pUnk);
	if (SUCCEEDED(hr))
	{
		IBaseFilterPtr pFilter = pUnk;
		if (wszName)
		{
			hr = pGraph->AddFilter(pFilter, wszName);
		}
		else
		{
			hr = pGraph->AddFilter(pFilter, GetKnownFilterName(clsid));// L"Private Filter");
		}
		if (SUCCEEDED(hr)) *ppF = pFilter;
		else pFilter->Release();// ???
	}

	return hr;
}

//######################################
//
//######################################
HRESULT GetCodecName(IMoniker* pMoniker, WCHAR* wszCodecName)
{
	IPropertyBag *pPropertyBag = NULL;
	hr = pMoniker->BindToStorage(NULL, NULL, IID_IPropertyBag, reinterpret_cast<void**>(&pPropertyBag));
	if (SUCCEEDED(hr))
	{
		VARIANT var;
		VariantInit(&var);
		pPropertyBag->Read(L"FriendlyName", &var, NULL);
		wcscpy_s(wszCodecName, 512, var.bstrVal);
		VariantClear(&var);
	}
	return hr;
}

//######################################
//
//######################################
HRESULT FindCodecByName(IMoniker** ppMoniker, GUID cat_guid, const WCHAR* wszCodecName)
{
	ICreateDevEnum * pSysDevEnum = NULL;
	IEnumMoniker * pEnum = NULL;
	IMoniker * pMoniker = NULL;

	// System Device Enumerator
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, reinterpret_cast<void**>(&pSysDevEnum));
	if (FAILED(hr))	return hr;

	// Moniker Enumerator
	hr = pSysDevEnum->CreateClassEnumerator(cat_guid, &pEnum, 0);
	if (FAILED(hr))
	{
		pSysDevEnum->Release();
		return hr;
	}

	// Cycle throught IMoniker collection
	hr = E_FAIL;
	WCHAR buf[MAX_FILTER_NAME];
	while (pEnum->Next(1, &pMoniker, NULL) == S_OK)
	{
		hr = GetCodecName(pMoniker, (WCHAR *)buf);
		if (SUCCEEDED(hr) && lstrcmpi(wszCodecName, buf) == 0)
		{
			*ppMoniker = pMoniker; //???
			hr = S_OK;
			break;
		}
		pMoniker->Release(); // ???
	}

	//release
	pEnum->Release();
	pSysDevEnum->Release();
	return hr;
}

//######################################
//
//######################################
HRESULT AddFilterCodec(
	IGraphBuilder *pGraphBuilder,
	GUID cat_guid,
	const WCHAR *wfiltername,
	IBaseFilter **ppF
)
{
	if (!pGraphBuilder || !ppF) return E_POINTER;

	*ppF = 0;
	IBaseFilter * pBaseFilter = NULL;
	IMoniker *	pMoniker = NULL;
	hr = FindCodecByName(&pMoniker, cat_guid, wfiltername);
	if (FAILED(hr))	return hr;
	// Retrieve the IBaseFilter
	hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, reinterpret_cast<void**>(&pBaseFilter));
	pMoniker->Release(); // ???
	if (FAILED(hr))	return hr;
	hr = pGraphBuilder->AddFilter(pBaseFilter, wfiltername);
	if (SUCCEEDED(hr))
		*ppF = pBaseFilter;
	else
		pBaseFilter->Release();
	return hr;
}

//######################################
// Encoder Interfaces: If the DMO is a video encoder or an audio encoder, the output pin exposes the 
// IAMStreamConfig interface. If the DMO is a video encoder, the output pin also exposes the 
// IAMVideoCompression interface. In both cases, if the DMO supports the interface, the pin delegates to the DMO. 
// Otherwise, the pin provides its own implementation.
//######################################
HRESULT AddFilterDMO(
	IGraphBuilder *pGraph,  // Pointer to the Filter Graph Manager
	GUID clsid,			    // CLSID of the filter to create
	GUID dmoCategory,       // CLSID of the DMO category (e.g. DMOCATEGORY_VIDEO_ENCODER)
	IBaseFilter **ppF,      // Receives a pointer to the filter
	const WCHAR *wszName    // A name for the filter, e.g. L"WMV9DMO Encoder" (NULL -> default name)
)
{
	if (!pGraph || !ppF) return E_POINTER;
	
	*ppF = NULL;
	IBaseFilter *pBaseFilter = NULL;

	hr = CoCreateInstance(CLSID_DMOWrapperFilter, NULL, CLSCTX_INPROC, IID_IBaseFilter, reinterpret_cast<void**>(&pBaseFilter));
	if (SUCCEEDED(hr))
	{
		IDMOWrapperFilter *pWrapperFilter;
		hr = pBaseFilter->QueryInterface(&pWrapperFilter);
		if (SUCCEEDED(hr)) hr = pWrapperFilter->Init(clsid, dmoCategory);
		if (SUCCEEDED(hr)) hr = pGraph->AddFilter(pBaseFilter, wszName);
		if (SUCCEEDED(hr))
			*ppF = pBaseFilter;
		else
			pBaseFilter->Release();
	}

	return hr;
}

//######################################
//
//######################################
HRESULT ListFiltersInGraph(IGraphBuilder *pGraph)
{
	if (!pGraph) return E_POINTER;

	IEnumFilters *pEnumFilters;
	IBaseFilter *pFilter;
	GUID guid;
	WCHAR guidStr[39];
	FILTER_INFO FilterInfo;

	pGraph->EnumFilters(&pEnumFilters);
	Log(logInfo, "Filters in Graph:\n");
	while (pEnumFilters->Next(1, &pFilter, NULL) == S_OK)
	{
		hr = pFilter->GetClassID(&guid);
		GuidToString(guidStr, guid);
		hr = pFilter->QueryFilterInfo(&FilterInfo);
		Log(logInfo, "%ls %ls\n", guidStr, FilterInfo.achName);
		pFilter->Release();
	}
	pEnumFilters->Release();
	return S_OK;
}

//######################################
//
//######################################
const char * GetEventName(int e)
{
	switch (e)
	{
	case EC_COMPLETE: return "EC_COMPLETE";
	case EC_USERABORT: return "EC_USERABORT";
	case EC_ERRORABORT: return "EC_ERRORABORT";
	case EC_TIME: return "EC_TIME";
	case EC_REPAINT: return "EC_REPAINT";
	case EC_STREAM_ERROR_STOPPED: return "EC_STREAM_ERROR_STOPPED";
	case EC_STREAM_ERROR_STILLPLAYING: return "EC_STREAM_ERROR_STILLPLAYING";
	case EC_ERROR_STILLPLAYING: return "EC_ERROR_STILLPLAYING";
	case EC_PALETTE_CHANGED: return "EC_PALETTE_CHANGED";
	case EC_VIDEO_SIZE_CHANGED: return "EC_VIDEO_SIZE_CHANGED";
	case EC_QUALITY_CHANGE: return "EC_QUALITY_CHANGE";
	case EC_SHUTTING_DOWN: return "EC_SHUTTING_DOWN";
	case EC_CLOCK_CHANGED: return "EC_CLOCK_CHANGED";
	case EC_PAUSED: return "EC_PAUSED";
	case EC_OPENING_FILE: return "EC_OPENING_FILE";
	case EC_BUFFERING_DATA: return "EC_BUFFERING_DATA";
	case EC_FULLSCREEN_LOST: return "EC_FULLSCREEN_LOST";
	case EC_ACTIVATE: return "EC_ACTIVATE";
	case EC_NEED_RESTART: return "EC_NEED_RESTART";
	case EC_WINDOW_DESTROYED: return "EC_WINDOW_DESTROYED";
	case EC_DISPLAY_CHANGED: return "EC_DISPLAY_CHANGED";
	case EC_STARVATION: return "EC_STARVATION";
	case EC_OLE_EVENT: return "EC_OLE_EVENT";
	case EC_NOTIFY_WINDOW: return "EC_NOTIFY_WINDOW";
	case EC_STREAM_CONTROL_STOPPED: return "EC_STREAM_CONTROL_STOPPED";
	case EC_STREAM_CONTROL_STARTED: return "EC_STREAM_CONTROL_STARTED";
	case EC_END_OF_SEGMENT: return "EC_END_OF_SEGMENT";
	case EC_SEGMENT_STARTED: return "EC_SEGMENT_STARTED";
	case EC_LENGTH_CHANGED: return "EC_LENGTH_CHANGED";
	case EC_DEVICE_LOST: return "EC_DEVICE_LOST";
	case EC_SAMPLE_NEEDED: return "EC_SAMPLE_NEEDED";
	case EC_PROCESSING_LATENCY: return "EC_PROCESSING_LATENCY";
	case EC_SAMPLE_LATENCY: return "EC_SAMPLE_LATENCY";
	case EC_SCRUB_TIME: return "EC_SCRUB_TIME";
	case EC_STEP_COMPLETE: return "EC_STEP_COMPLETE";
	case EC_TIMECODE_AVAILABLE: return "EC_TIMECODE_AVAILABLE";
	case EC_EXTDEVICE_MODE_CHANGE: return "EC_EXTDEVICE_MODE_CHANGE";
	case EC_STATE_CHANGE: return "EC_STATE_CHANGE";
	case EC_GRAPH_CHANGED: return "EC_GRAPH_CHANGED";
	case EC_CLOCK_UNSET: return "EC_CLOCK_UNSET";
	case EC_VMR_RENDERDEVICE_SET: return "EC_VMR_RENDERDEVICE_SET";
	case EC_VMR_SURFACE_FLIPPED: return "EC_VMR_SURFACE_FLIPPED";
	case EC_VMR_RECONNECTION_FAILED: return "EC_VMR_RECONNECTION_FAILED";
	case EC_PREPROCESS_COMPLETE: return "EC_PREPROCESS_COMPLETE";
	case EC_CODECAPI_EVENT: return "EC_CODECAPI_EVENT";
	default: return "UNKNOWN EVENT";
	}
}

//######################################
// Provides nicer names for some known filters when the graph's filter list is printed.
// This only applies to non-registered filters loaded directly from DLL.
//######################################
const WCHAR * GetKnownFilterName(GUID clsid)
{
	if (clsid == CLSID_LAVSplitterSource) return L"LAV Splitter Source";
	if (clsid == CLSID_LAVSplitter) return L"LAV Splitter";
	if (clsid == CLSID_LAVVideoDecoder) return L"LAV Video Decoder";
	if (clsid == CLSID_LAVAudioDecoder) return L"LAV Audio Decoder";
	if (clsid == CLSID_DirectVobSub) return L"VSFilter";
	if (clsid == CLSID_DirectVobSubAutoload) return L"VSFilter (auto-loading version)";
	if (clsid == CLSID_SpoutCam) return L"SpoutCam";
	if (clsid == CLSID_ScreenCam) return L"ScreenCam";
	if (clsid == CLSID_LAMEDShowFilter) return L"LAME Audio Encoder";
	return NULL;
}

//######################################
// Resizes window according to requested width and height of its client rect,
// and then centers the window on the desktop.
//######################################
BOOL ResizeWindow(HWND hWnd, DWORD dwStyle, LONG w, LONG h)
{
	// find window rect for wanted client rect
	RECT r = { 0 , 0, w, h };
	BOOL ok = AdjustWindowRect(&r, dwStyle, FALSE);
	if (!ok) return ok;
	// position at screen center
	RECT dr;
	ok = GetClientRect(GetDesktopWindow(), &dr);
	if (!ok) return ok;
	return SetWindowPos(
		hWnd,
		NULL,
		dr.right / 2 - (r.right - r.left) / 2,
		dr.bottom / 2 - (r.bottom - r.top) / 2,
		r.right - r.left,
		r.bottom - r.top,
		0
	);
}

//######################################
//
//######################################
HRESULT FindFilterInterface(
	IGraphBuilder *pGraph, // Pointer to the Filter Graph Manager.
	REFGUID iid,           // IID of the interface to retrieve.
	void **ppUnk           // Receives the interface pointer.
)
{
	if (!pGraph || !ppUnk) return E_POINTER;

	HRESULT hr = S_OK;
	bool bFound = false;

	IEnumFilters *pEnum = NULL;
	IBaseFilter *pFilter = NULL;

	CHECK_HR(hr = pGraph->EnumFilters(&pEnum));

	// Query every filter for the interface.
	while (S_OK == pEnum->Next(1, &pFilter, 0))
	{
		hr = pFilter->QueryInterface(iid, ppUnk);
		SAFE_RELEASE(pFilter);
		if (SUCCEEDED(hr))
		{
			bFound = true;
			break;
		}
	}

done:
	SAFE_RELEASE(pEnum);
	SAFE_RELEASE(pFilter);

	return (bFound ? S_OK : VFW_E_NOT_FOUND);
}
