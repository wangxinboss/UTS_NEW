#include <windows.h>

#if defined(LSCDLL_EXPORTS)
#define DLLFUNC extern "C" __declspec(dllexport)
#elif defined(__cplusplus)
#define DLLFUNC extern "C" __declspec(dllimport)
#else
#define DLLFUNC __declspec(dllimport)
#endif




//unsigned short *m_pBuffer;
DLLFUNC bool initialize(int iWidth, int iHeight, int iOrder, 
					   int m_nPedestal, int m_nShadingPower, int m_nLensPolyfitOrder, int m_bLensModeling, float m_fAlphaStart, float m_fAlphaEnd, float m_fSeed, 
					   float m_fTargetRI_10, float m_fTargetRI_20, float m_fTargetRI_30, float m_fTargetRI_40, float m_fTargetRI_50, 
					   float m_fTargetRI_60, float m_fTargetRI_70, float m_fTargetRI_80, float m_fTargetRI_90, float m_fTargetRI_100);
DLLFUNC bool Processing(unsigned short *imbuff, int mode);
DLLFUNC void Release();
DLLFUNC bool GetCurrData(char *lpPath);
DLLFUNC bool SetInitData(char *lpPath);
DLLFUNC bool DataOutCDE(char *lpFolderPath);
DLLFUNC bool GetSRAMFileFormat(char *str, int maxlen, int &outlen);
DLLFUNC bool GetOTPFileFormat(char *str, bool b24Mhz, bool b19p2Mhz, bool b13Mhz, bool bSet1, bool bSet2, unsigned int *pAlpha, int maxlen,int &outlen);

#ifdef SPEED_INFO
DLLFUNC void GetSpeedInfo(double & data1, double & data2, double & data3, int *data4);
#endif