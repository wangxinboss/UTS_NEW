#include "stdafx.h"
#include "CaptureVideo.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CSampleGrabberCB mCB;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCaptureVideo::CCaptureVideo()
{
    ////COM Library Intialization
    //if (FAILED(CoInitialize(NULL))) /*, COINIT_APARTMENTTHREADED)))*/
    //{
    //    AfxMessageBox(_T("CCaptureVideo CoInitialize Failed!\r\n")); 
    //    return;
    //}
    m_hWnd = NULL;
    m_pVW = NULL;
    m_pMC = NULL;
    m_pGB = NULL;
    m_pBF = NULL; 
    m_pGrabber = NULL; 
    m_pCapture = NULL; 
}

CCaptureVideo::~CCaptureVideo()
{
    // Stop media playback
    // Stop media playback
    if (m_pMC)
    {
        m_pMC->StopWhenReady();
    }
    if (m_pVW)
    {
        m_pVW->put_Visible(OAFALSE);
        m_pVW->put_Owner(NULL);
    }
    SAFE_RELEASE(m_pMC);
    SAFE_RELEASE(m_pVW); 
    SAFE_RELEASE(m_pGB);
    SAFE_RELEASE(m_pBF);
    SAFE_RELEASE(m_pGrabber); 
    SAFE_RELEASE(m_pCapture);
    //CoUninitialize(); 
}
HRESULT CCaptureVideo::SetProcAmpProperty(long Property,long lValue,long Flags)
{
	HRESULT hr = S_OK;
	IAMVideoProcAmp *pVPA = NULL;
    hr = m_pBF->QueryInterface(IID_IAMVideoProcAmp, (void**)&pVPA);
    if (SUCCEEDED(hr))
    {
		long min, max, minstep, defaultValue, capflags;
        hr = pVPA->GetRange(CameraControl_Exposure,
            &min, // min
            &max, // max
            &minstep, // minstep
            &defaultValue, // default
            &capflags); // capflags
        pVPA->Set(Property,lValue,Flags);
    }
	return hr;
}

HRESULT CCaptureVideo::SetCameraControlProperty(long Property,long lValue,long Flags)
{
	HRESULT hr = S_OK;
	IAMCameraControl *pCameraControl;
     hr = m_pBF->QueryInterface(IID_IAMCameraControl, (void **)&pCameraControl); 
    if (SUCCEEDED(hr))
    {
		long min, max, minstep, defaultValue, capflags;
        hr = pCameraControl->GetRange(CameraControl_Exposure,
            &min, // min
            &max, // max
            &minstep, // minstep
            &defaultValue, // default
            &capflags); // capflags
        pCameraControl->Set(Property,lValue,Flags);
    }
	return hr;
}

HRESULT CCaptureVideo::GetProcAmpProperty(long Property,long *lValue,long *Flags)
{
	HRESULT hr = S_OK;
	IAMVideoProcAmp *pVPA = NULL;
    hr = m_pBF->QueryInterface(IID_IAMVideoProcAmp, (void**)&pVPA);
    if (SUCCEEDED(hr))
    {
		long min, max, minstep, defaultValue, capflags;
        hr = pVPA->GetRange(CameraControl_Exposure,
            &min, // min
            &max, // max
            &minstep, // minstep
            &defaultValue, // default
            &capflags); // capflags
        pVPA->Get(Property,lValue,Flags);
    }
	return hr;
}

HRESULT CCaptureVideo::GetCameraControlProperty(long Property,long *lValue,long *Flags)
{
	HRESULT hr = S_OK;
	IAMCameraControl *pCameraControl;
     hr = m_pBF->QueryInterface(IID_IAMCameraControl, (void **)&pCameraControl); 
    if (SUCCEEDED(hr))
    {
		long min, max, minstep, defaultValue, capflags;
        hr = pCameraControl->GetRange(CameraControl_Exposure,
            &min, // min
            &max, // max
            &minstep, // minstep
            &defaultValue, // default
            &capflags); // capflags
        pCameraControl->Get(Property,lValue,Flags);
    }
	return hr;
}

int CCaptureVideo::EnumDevices(HWND hList)
{
    if (!hList)
    {
        return -1;
    }
    int id = 0;

    //枚举视频扑捉设备
    ICreateDevEnum *pCreateDevEnum;
    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,IID_ICreateDevEnum, (void**)&pCreateDevEnum);

    if (hr != NOERROR)
    {
        return -1;
    }
    CComPtr<IEnumMoniker> pEm;
    hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEm, 0);
    if (hr != NOERROR)
    {
        return -1;
    }
    pEm->Reset();
    ULONG cFetched;
    IMoniker *pM;
    while (hr = pEm->Next(1, &pM, &cFetched), hr==S_OK)
    {
        IPropertyBag *pBag;
        hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
        if (SUCCEEDED(hr))
        {
            VARIANT var;
            var.vt = VT_BSTR;
            hr = pBag->Read(L"FriendlyName", &var, NULL);
            if (hr == NOERROR)
            {
                char str[2048]; 
                id++;
                WideCharToMultiByte(CP_ACP,0,var.bstrVal, -1, str, 2048, NULL, NULL);
                ::SendMessage(hList, CB_ADDSTRING, 0,(LPARAM)str);
                SysFreeString(var.bstrVal);
            }
            pBag->Release();
        }
        pM->Release();
    }
    return id;
}

HRESULT CCaptureVideo::Close() 
{
    // Stop media playback
    if (m_pMC)
    {
        m_pMC->StopWhenReady();
    }
    if (m_pVW)
    {
        m_pVW->put_Visible(OAFALSE);
        m_pVW->put_Owner(NULL);
    }
    SAFE_RELEASE(m_pMC);
    SAFE_RELEASE(m_pVW); 
    SAFE_RELEASE(m_pGB);
    SAFE_RELEASE(m_pBF);
    SAFE_RELEASE(m_pGrabber); 
    SAFE_RELEASE(m_pCapture);
    return S_OK ; 
}

HRESULT CCaptureVideo::Open(int iDeviceID, HWND hWnd, long width, long height)
{
    HRESULT hr;
    hr = InitCaptureGraphBuilder();
    if (FAILED(hr))
    {
        AfxMessageBox(_T("Failed to get video interfaces!"));
        return hr;
    }
    // Bind Device Filter. We know the device because the id was passed in
    if (!BindFilter(iDeviceID, &m_pBF))
    {
        return S_FALSE;
    }
    hr = m_pGB->AddFilter(m_pBF, L"Capture Filter");
    // hr = m_pCapture->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, 
    // m_pBF, NULL, NULL);
    // create a sample grabber
    hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_ISampleGrabber, (void**)&m_pGrabber);
    if (FAILED(hr))
    {
        AfxMessageBox(_T("Fail to create SampleGrabber, maybe qedit.dll is not registered?"));
        return hr;
    }
    CComQIPtr<IBaseFilter, &IID_IBaseFilter> pGrabBase(m_pGrabber);

    //设置视频格式
    AM_MEDIA_TYPE mt; 
    ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
    mt.majortype = MEDIATYPE_Video;
	mt.subtype = MEDIASUBTYPE_RGB24; // MEDIASUBTYPE_RGB24 ; 
    hr = m_pGrabber->SetMediaType(&mt);
    if (FAILED(hr))
    {
        AfxMessageBox(_T("Fail to set media type!"));
        return hr;
    }
    hr = m_pGB->AddFilter( pGrabBase, L"Grabber" );
    if (FAILED(hr))
    {
        AfxMessageBox(_T("Fail to put sample grabber in graph"));
        return hr;
    }

    // 设置分辨率
    SetResolution(width, height);

    //-------------------------------------------------------------------------
    // 禁止自动调焦
    IAMCameraControl *pCameraControl;
    hr = m_pBF->QueryInterface(IID_IAMCameraControl, (void **)&pCameraControl); 
    if (hr == S_OK)
    {
        long min, max, minstep, defaultFocusValue, capflags;
        hr = pCameraControl->GetRange(
            CameraControl_Focus,
            &min, // min
            &max, // max
            &minstep, // minstep
            &defaultFocusValue, // default
            &capflags); // capflags

        hr = pCameraControl->Set(
            CameraControl_Focus, // property
            defaultFocusValue, // value
            CameraControl_Flags_Manual); 
    }
    //-------------------------------------------------------------------------

    // try to render preview/capture pin
    hr = m_pCapture->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, m_pBF, pGrabBase, NULL);
    if (FAILED(hr))
    {
        hr = m_pCapture->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, m_pBF, pGrabBase, NULL);
    }

    if (FAILED(hr))
    {
        AfxMessageBox(_T("Can't build the graph"));
        return hr;
    }

    hr = m_pGrabber->GetConnectedMediaType(&mt);
    if (FAILED(hr))
    {
        AfxMessageBox(_T("Failt to read the connected media type"));
        return hr;
    }

    VIDEOINFOHEADER * vih = (VIDEOINFOHEADER*)mt.pbFormat;
    mCB.lWidth = vih->bmiHeader.biWidth;
    mCB.lHeight = vih->bmiHeader.biHeight;
    mCB.bGrabVideo = FALSE; 
    mCB.frame_handler = NULL; 
    FreeMediaType(mt);
    hr = m_pGrabber->SetBufferSamples(FALSE);
    hr = m_pGrabber->SetOneShot(FALSE);
    hr = m_pGrabber->SetCallback(&mCB, 1);

    //设置视频捕捉窗口
    m_hWnd = hWnd ; 
    SetupVideoWindow();
    hr = m_pMC->Run();//开始视频捕捉
    if (FAILED(hr))
    {
        AfxMessageBox(_T("Couldn't run the graph!"));
        return hr;
    }
    return S_OK;
}

BOOL CCaptureVideo::BindFilter(int deviceId, IBaseFilter **pFilter)
{
    if (deviceId < 0)
    {
        return FALSE;
    }
    // enumerate all video capture devices
    CComPtr<ICreateDevEnum> pCreateDevEnum;
    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
        IID_ICreateDevEnum, (void**)&pCreateDevEnum);
    if (hr != NOERROR)
    {
        return FALSE;
    }
    CComPtr<IEnumMoniker> pEm;
    hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,&pEm, 0);
    if (hr != NOERROR) 
    {
        return FALSE;
    }
    pEm->Reset();
    ULONG cFetched;
    IMoniker *pM;
    int index = 0;
    while (hr = pEm->Next(1, &pM, &cFetched), hr==S_OK, index <= deviceId)
    {
        IPropertyBag *pBag;
        hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
        if (SUCCEEDED(hr)) 
        {
            VARIANT var;
            var.vt = VT_BSTR;
            hr = pBag->Read(L"FriendlyName", &var, NULL);
            if (hr == NOERROR) 
            {
                if (index == deviceId)
                {
                    pM->BindToObject(0, 0, IID_IBaseFilter, (void**)pFilter);
                }
                SysFreeString(var.bstrVal);
            }
            pBag->Release();
        }
        pM->Release();
        index++;
    }
    return TRUE;
}

HRESULT CCaptureVideo::InitCaptureGraphBuilder()
{
    HRESULT hr;
    // 创建IGraphBuilder接口
    hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&m_pGB);
    // 创建ICaptureGraphBuilder2接口
    hr = CoCreateInstance (CLSID_CaptureGraphBuilder2 , NULL, CLSCTX_INPROC,
        IID_ICaptureGraphBuilder2, (void **)&m_pCapture);
    if (FAILED(hr))
    {
        return hr;
    }
    m_pCapture->SetFiltergraph(m_pGB);
    hr = m_pGB->QueryInterface(IID_IMediaControl, (void **)&m_pMC);
    if (FAILED(hr))
    {
        return hr;
    }
    hr = m_pGB->QueryInterface(IID_IVideoWindow, (LPVOID *)&m_pVW);
    if (FAILED(hr))
    {
        return hr;
    }
    return hr;
}
HRESULT CCaptureVideo::SetupVideoWindow()
{
    HRESULT hr;
    hr = m_pVW->put_Owner((OAHWND)m_hWnd);
    if (FAILED(hr))
    {
        return hr;
    }
    hr = m_pVW->put_WindowStyle(WS_CHILD | WS_CLIPCHILDREN);
    if (FAILED(hr))
    {
        return hr;
    }
    hr = m_pVW->put_WindowStyleEx(WS_EX_NOACTIVATE);
    if (FAILED(hr))
    {
        return hr;
    }
    ResizeVideoWindow();
    hr = m_pVW->put_Visible(OAFALSE);
    return hr;
}

void CCaptureVideo::ResizeVideoWindow()
{
    if (m_pVW)
    {
        //让图像充满整个窗口
        CRect rc;
        ::GetClientRect(m_hWnd, &rc);
        m_pVW->SetWindowPosition(0, 0, rc.right, rc.bottom);
    } 
}

void CCaptureVideo::FreeMediaType(AM_MEDIA_TYPE& mt)
{
    if (mt.cbFormat != 0)
    {
        CoTaskMemFree((PVOID)mt.pbFormat);
        // Strictly unnecessary but tidier
        mt.cbFormat = 0;
        mt.pbFormat = NULL;
    }
    if (mt.pUnk != NULL)
    {
        mt.pUnk->Release();
        mt.pUnk = NULL;
    }
}
void CCaptureVideo::GrabVideoFrames(BOOL bGrabVideoFrames, CVdoFrameHandler * frame_handler)
{
    mCB.frame_handler = frame_handler ; 
    mCB.bGrabVideo = bGrabVideoFrames ; 
}

void CCaptureVideo::SetResolution(long width, long height)
{
    IAMStreamConfig *pSC = NULL;   
    VIDEOINFOHEADER *vinfo = 0;      

    HRESULT hr = m_pCapture->FindInterface(&PIN_CATEGORY_CAPTURE,   
        &MEDIATYPE_Video, m_pBF, IID_IAMStreamConfig, (void **)&pSC);
    if (FAILED(hr))  
    {  
        AfxMessageBox(_T("fail"));
    }  
    AM_MEDIA_TYPE *mmt = NULL;
    pSC->GetFormat(&mmt);    //取得默认参数  
    VIDEOINFOHEADER *pvih = (VIDEOINFOHEADER*)mmt->pbFormat;  
    pvih->bmiHeader.biHeight = height;  //修改采集视频的高为height  
    pvih->bmiHeader.biWidth = width;   //修改采集视频的宽为width  
    pvih->bmiHeader.biSizeImage = height * width * 3;
    pvih->bmiHeader.biBitCount = 24;
    mmt->pbFormat = (unsigned char *)pvih;
    mmt->lSampleSize = pvih->bmiHeader.biSizeImage;
	mmt->subtype = MEDIASUBTYPE_YUY2;//yuy2

    pSC->SetFormat(mmt);   //重新设置参数  
}


