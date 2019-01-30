// LSC DLL Header

#if defined(LSCDLL_EXPORTS)
#define DLLFUNC extern "C" __declspec(dllexport)
#elif defined(__cplusplus)
#define DLLFUNC extern "C" __declspec(dllimport)
#else
#define DLLFUNC __declspec(dllimport)
#endif

// Set parameters
DLLFUNC void SetParameter( int m_nPedestal, int m_nWidth, int m_nHeight, int m_nGr, int m_nR, 
   int m_nB, int m_nGb, double m_dbAlpha, double m_dbSeed, BOOL m_bLens, BOOL m_bScooby );

// Run LSC
DLLFUNC BOOL Run( BYTE *pRawBuffer, BYTE *pTrgBuffer, BOOL m_bSimulatedResults, 
	CString strWorkDir, BOOL b2ndSet, BOOL *is_SINGULAR, BOOL *bSEED_overflow);

DLLFUNC BOOL SaveOTPSetfile(const char* _Filename);

DLLFUNC BOOL SaveSRAMSetfile(const char* _Filename);

DLLFUNC int GetOTPSetfileSize();

DLLFUNC int GetOTPSetfile(char *szOTP);

DLLFUNC int GetSRAMSetfileSize();

DLLFUNC int GetSRAMSetfile(char *szSRAM);
