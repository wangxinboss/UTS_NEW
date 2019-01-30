#ifndef _EEA_INTERFACE_H_
#define _EEA_INTERFACE_H_

#ifdef UTS_EEA_EXPORTS
#define UTS_EEA_API __declspec(dllexport)
#else
#define UTS_EEA_API __declspec(dllimport)
#endif

enum eStation
{
    STATION_A = 0,
    STATION_B,
    STATION_C,
    STATION_D,
    STATION_CNT,
};

enum eResult
{
    RST_PASS = 0,
    RST_NG,
};

enum eCylinderPos
{
    CP_Home = 0,
    CP_End,
};

enum eReturnValue
{
    RV_OK = 0,
    RV_USER_STOP,
    RV_NG,
    RV_TIMEOUT,
};

const int INFINITE_TIMEOUT = -1;    // nTimeout: millisecond

EXTERN_C
{
    UTS_EEA_API bool EEA_Initialize(int port);
    UTS_EEA_API void EEA_UnInitialize(void);

    //-------------------------------------------------------------------------
    // test operation
    UTS_EEA_API void EEA_EndTest(eStation station, eResult result);
    UTS_EEA_API void EEA_EndTestWithErrorCodeA(eStation station, eResult result, const char *pErrorCode);
    UTS_EEA_API void EEA_EndTestWithErrorCodeW(eStation station, eResult result, const wchar_t *pErrorCode);
    UTS_EEA_API int  EEA_WaitStartTest(eStation station, int nTimeout, int *pbIsRunning );       // 等待指定站别开始
    UTS_EEA_API int  EEA_WaitAllStartTest(int nTimeout, int *pbIsRunning, eStation &station);   // 等待所有站别开始，返回开始站别
    UTS_EEA_API void EEA_GetASn(char *sn);
    UTS_EEA_API void EEA_GetWSn(wchar_t *sn);

    //-------------------------------------------------------------------------
    // hardware operation
    UTS_EEA_API void EEA_MoveRelaylensCylinder(eCylinderPos pos);
    UTS_EEA_API int  EEA_WaitRelaylensCylinder(eCylinderPos pos, int nTimeout, int *pbIsRunning);
    UTS_EEA_API void EEA_MoveBoardCylinder(eCylinderPos pos);
    UTS_EEA_API int  EEA_WaitBoardCylinder(eCylinderPos pos, int nTimeout, int *pbIsRunning);
    UTS_EEA_API void EEA_MovePushCylinder(eCylinderPos pos);
    UTS_EEA_API int  EEA_WaitPushCylinder(eCylinderPos pos, int nTimeout, int *pbIsRunning);
    UTS_EEA_API void EEA_MoveCanvas(int x, int y);
    UTS_EEA_API int EEA_WaitCanvas(int nTimeout, int *pbIsRunning);
};

#endif  /* _EEA_INTERFACE_H_ */

