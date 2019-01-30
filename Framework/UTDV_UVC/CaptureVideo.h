#pragma once
#include <atlbase.h>
#include <windows.h>
#include <dshow.h>
#include <Qedit.h> // ISampleGrabberCB


#ifndef SAFE_RELEASE
#define SAFE_RELEASE( x ) \
    if ( NULL != x ) \
            { \
            x->Release( ); \
            x = NULL; \
        }
#endif

class CVdoFrameHandler {
public:
    virtual void VdoFrameData(double dblSampleTime, BYTE * pBuffer, long lBufferSize) = 0 ; 
};

class CSampleGrabberCB : public ISampleGrabberCB 
{
public:
    long       lWidth ; 
    long       lHeight ; 
    CVdoFrameHandler * frame_handler ; 
    BOOL       bGrabVideo ; 
public:
    CSampleGrabberCB(){ 
        lWidth = 0 ; 
        lHeight = 0 ; 
        bGrabVideo = FALSE ; 
        frame_handler = NULL ; 
    } 
    STDMETHODIMP_(ULONG) AddRef() { return 2; }
    STDMETHODIMP_(ULONG) Release() { return 1; }
    STDMETHODIMP QueryInterface(REFIID riid, void ** ppv) {
        if ( riid == IID_ISampleGrabberCB || riid == IID_IUnknown ){ 
            *ppv = (void *) static_cast<ISampleGrabberCB*> ( this );
            return NOERROR;
        } 
        return E_NOINTERFACE;
    }
    STDMETHODIMP SampleCB( double SampleTime, IMediaSample * pSample ) {
        return 0;
    }
    STDMETHODIMP BufferCB( double dblSampleTime, BYTE * pBuffer, long lBufferSize ){
        if (!pBuffer) return E_POINTER;
        if (bGrabVideo && frame_handler) frame_handler->VdoFrameData(dblSampleTime, pBuffer, lBufferSize) ; 
        return 0;
    }
};

class CCaptureVideo : public CWnd 
{
    friend class CSampleGrabberCB;

public:
    void GrabVideoFrames(BOOL bGrabVideoFrames, CVdoFrameHandler * frame_handler); 
    HRESULT Open(int iDeviceID, HWND hWnd, long width, long height);
    HRESULT Close(); 
    int EnumDevices(HWND hList);
	HRESULT SetProcAmpProperty(long Property,long lValue,long Flags);
	HRESULT SetCameraControlProperty(long Property,long lValue,long Flags);
	HRESULT GetProcAmpProperty(long Property,long *lValue,long *Flags);
	HRESULT GetCameraControlProperty(long Property,long *lValue,long *Flags);
    CCaptureVideo();
    virtual ~CCaptureVideo();


protected:
    HWND      m_hWnd;
    IGraphBuilder *    m_pGB;
    ICaptureGraphBuilder2* m_pCapture;
    IBaseFilter*    m_pBF;
    IMediaControl*    m_pMC;
    IVideoWindow*    m_pVW;
    ISampleGrabber*    m_pGrabber;

protected:
    void FreeMediaType(AM_MEDIA_TYPE& mt);
    BOOL BindFilter(int deviceId, IBaseFilter **pFilter);
    void ResizeVideoWindow();
    HRESULT SetupVideoWindow();
    HRESULT InitCaptureGraphBuilder();
    void SetResolution(long width, long height);
};


