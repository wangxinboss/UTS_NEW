#include "StdAfx.h"
#include "UTTL_EEAIF.h"
#include "EEAInterface.h"
#include <afxmt.h>
#include "eea_automation_socketserver.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

const int DELAY_TIME = 200;
CWinThread *g_pMsgHandleThread = nullptr;
HWND g_hwndMsgWnd = nullptr;
CEvent g_evtWndCreated(FALSE, TRUE);
CEvent g_evtExit(FALSE, TRUE);
bool g_bTesting[STATION_CNT];
char g_MessageStr[256] = {0};
EEA_SocketDataSet g_DataSet = {0};

SocketServer* GetSocketServerInstance(void)
{
    static SocketServer *s_pServer = nullptr;
    if (nullptr == s_pServer)
    {
        s_pServer = new SocketServer;
    }

    return s_pServer;
}

UINT MsgHandleThreadProc(PVOID param)
{
	CString  strClass;
	CString  strWindow;

	strClass = _T("HIDEN_MSG_WINDOW_CLASS");
	strClass.AppendFormat(_T("_%s"),uts.info.strLineName);

	strWindow = _T("HIDEN_MSG_WINDOW");
	strWindow.AppendFormat(_T("_%s"),uts.info.strLineName);


    // 注册窗口类
    WNDCLASS wc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hInstance = theApp.m_hInstance;
    wc.lpfnWndProc = WndProc;
    wc.lpszClassName = strClass;
    wc.lpszMenuName = NULL;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    if (!RegisterClass(&wc)) 
    {
        return 1;
    }

    // 创建窗口
    g_hwndMsgWnd = CreateWindow(
        strClass,//_T("HIDEN_MSG_WINDOW_CLASS")
        strWindow,//_T("HIDEN_MSG_WINDOW")
        WS_SYSMENU, 0, 0, 100, 100,
        NULL, NULL, NULL, NULL);
    ShowWindow(g_hwndMsgWnd, SW_HIDE);
	//ShowWindow(g_hwndMsgWnd, SW_SHOW);
    UpdateWindow(g_hwndMsgWnd);
    // 触发创建成功事件
    g_evtWndCreated.SetEvent();

    g_evtExit.ResetEvent();
    // 主消息循环:
    MSG msg;
    while (true)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (WAIT_TIMEOUT != WaitForSingleObject(g_evtExit, 50))
        {
            DestroyWindow(g_hwndMsgWnd);
            break;
        }
		
    }

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int StrSize = 0;
    USES_CONVERSION;

    CStringA strKey;
    int nState;

    switch (message)
    {
    case SOCKETMESSAGE:
        memset(g_MessageStr, 0, ARRAY_SIZE(g_MessageStr));
        StrSize = strlen((char *)lParam);
        if (StrSize > 0)
        {
            memcpy(g_MessageStr, (char *)lParam, StrSize);	
        }
        uts.log.Debug(_T("Receive Msg: [%s]"), A2T(g_MessageStr));
        ParserEEASocketSet(g_MessageStr, g_DataSet);
        
        if (0 == strcmp(g_DataSet.CMD, "MCA_SetFunc_AtestStart")
            || 0 == strcmp(g_DataSet.CMD, "MCA_SetFunc_ATestStart"))
        {
            g_bTesting[STATION_A] = true;
        }
        else if (0 == strcmp(g_DataSet.CMD, "MCA_SetFunc_BtestStart")
            || 0 == strcmp(g_DataSet.CMD, "MCA_SetFunc_BTestStart"))
        {
            g_bTesting[STATION_B] = true;
        }
        else if (0 == strcmp(g_DataSet.CMD, "MCA_SetFunc_CtestStart")
            || 0 == strcmp(g_DataSet.CMD, "MCA_SetFunc_CTestStart"))
        {
            g_bTesting[STATION_C] = true;
        }
        else if (0 == strcmp(g_DataSet.CMD, "MCA_SetFunc_DtestStart")
            || 0 == strcmp(g_DataSet.CMD, "MCA_SetFunc_DTestStart"))
        {
            g_bTesting[STATION_D] = true;
        }
        else if (0 == strcmp(g_DataSet.CMD, "MCA_ReStartTest"))
        {
            uts.flow.Stop();
        }
        else if (0 == strcmp(g_DataSet.CMD, "MCA_ATestState"))
        {
            strKey.Format("Station%d_IsTesting", STATION_A);
            if (uts.info.mapShared[strKey.GetBuffer()] >= 0.0)
            {
                nState = 1;
            }
            else
            {
                nState = 0;
            }

            GetSocketServerInstance()->SendTestState(STATION_A, nState);
        }
        else if (0 == strcmp(g_DataSet.CMD, "MCA_BTestState"))
        {
            strKey.Format("Station%d_IsTesting", STATION_B);
            if (uts.info.mapShared[strKey.GetBuffer()] >= 0.0)
            {
                nState = 1;
            }
            else
            {
                nState = 0;
            }

            GetSocketServerInstance()->SendTestState(STATION_B, nState);
        }
        else if (0 == strcmp(g_DataSet.CMD, "MCA_CTestState"))
        {
            strKey.Format("Station%d_IsTesting", STATION_C);
            if (uts.info.mapShared[strKey.GetBuffer()] >= 0.0)
            {
                nState = 1;
            }
            else
            {
                nState = 0;
            }

            GetSocketServerInstance()->SendTestState(STATION_C, nState);
        }
        else if (0 == strcmp(g_DataSet.CMD, "MCA_DTestState"))
        {
            strKey.Format("Station%d_IsTesting", STATION_D);
            if (uts.info.mapShared[strKey.GetBuffer()] >= 0.0)
            {
                nState = 1;
            }
            else
            {
                nState = 0;
            }

            GetSocketServerInstance()->SendTestState(STATION_D, nState);
        }
        break;
	case SOCKETMESSAGE2:
		if (STATION_A == lParam)
		{
			g_bTesting[STATION_A] = true;
		}
		else if (STATION_B == lParam)
		{
			g_bTesting[STATION_B] = true;
		}
		else if (STATION_C == lParam)
		{
			g_bTesting[STATION_C] = true;
		}
		else if (STATION_D == lParam)
		{
			g_bTesting[STATION_D] = true;
		}
		break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

bool EEA_Initialize(int port)
{
    //------------------------------------------------------------------------------
    // 创建一个 含有处理消息的窗口 的线程
    g_evtWndCreated.ResetEvent();
    g_pMsgHandleThread = AfxBeginThread(MsgHandleThreadProc, NULL);
    g_pMsgHandleThread->m_bAutoDelete = FALSE;

    if (WAIT_TIMEOUT == WaitForSingleObject(g_evtWndCreated, 30000))
    {
        uts.log.Error(_T("[%s]Create thread window timeout."), UTS_FN);
        return false;
    }
	/*
    if (!GetSocketServerInstance()->Open(port, g_hwndMsgWnd))
    {
        uts.log.Error(_T("[%s]Socket open error. Retry..."), UTS_FN);
        Sleep(1000);
        if (!GetSocketServerInstance()->Open(port, g_hwndMsgWnd))
        {
            uts.log.Error(_T("[%s]Socket open error."), UTS_FN);
            return false;
        }
    }*/

    return true;
}

void EEA_UnInitialize(void)
{
    //-------------------------------------------------------------------------
    // 等待线程退出
    GetSocketServerInstance()->Close();
    g_evtExit.SetEvent();
    if (nullptr != g_pMsgHandleThread)
    {
        DWORD dwExitCode = 0;
        do 
        {
            Sleep(50);
            GetExitCodeThread(g_pMsgHandleThread->m_hThread, &dwExitCode);
        } while (dwExitCode == STILL_ACTIVE);
        if (nullptr != g_pMsgHandleThread)
        {
            delete g_pMsgHandleThread;
            g_pMsgHandleThread = nullptr;
        }
    }
}

void EEA_EndTest(eStation station, eResult result)
{
    GetSocketServerInstance()->SendEndTest(station, result);
    uts.log.Debug(_T("[%s]station = %d, result = %d"), UTS_FN, station, result);
}

void EEA_EndTestWithErrorCodeW(eStation station, eResult result, const wchar_t *pErrorCode)
{
    //char szBuffer[2048] = {0};
    //int nLength = 0;
    //char *p = szBuffer;

    //nLength = strlen("0XFEFE MCA_End");
    //memcpy(p, "0XFEFE MCA_End", nLength);
    //p += nLength;

    //nLength = 1;
    //char st = 'A' - 1 + station;
    //memcpy(p, &st, 1);
    //p += 1;

    //nLength = strlen("Test,");
    //memcpy(p, "Test,", nLength);
    //p += nLength;

    //char szResult[32] = {0};
    //itoa(result, szResult, 10);
    //nLength = strlen(szResult);
    //memcpy(p, szResult, nLength);
    //p += nLength;

    //nLength = 1;
    //char cm = ',';
    //memcpy(p, &st, 1);
    //p += 1;

    //nLength = wcslen(pErrorCode) * 2;
    //memcpy(p, pErrorCode, nLength);
    //p += nLength;

    //GetSocketServerInstance()->Send(szBuffer, p - szBuffer);

    CStringW strSend;
    switch(station)
    {
    case STATION_A:
        strSend = L"0XFEFE MCA_EndATest,";
        break;
    case STATION_B:
        strSend = L"0XFEFE MCA_EndBTest,";
        break;
    case STATION_C:
        strSend = L"0XFEFE MCA_EndCTest,";
        break;
    case STATION_D:
        strSend = L"0XFEFE MCA_EndDTest,";
        break;
    default:
        break;
    }
    strSend.AppendFormat(L"%d,%s 0XFDFD", result, pErrorCode);

    USES_CONVERSION;
    int nLen = GetSocketServerInstance()->Send(T2A(strSend.GetBuffer()), strSend.GetLength());
    uts.log.Debug(_T("[%s]station = %d, result = %d, errorcode=%s,[nLen=%d]"),
        UTS_FN, station, result, pErrorCode, nLen);
}

void EEA_EndTestWithErrorCodeA(eStation station, eResult result, const char *pErrorCode)
{

    //strcpy(SendString,"0XFEFE MCA_ EndATest,");
    //strcat(SendString,_itoa(_Result,_tmpStr,10));
    //strcat(SendString," 0XFDFD");
    //this->Send(SendString,256);	
}

int EEA_WaitStartTest(eStation station, int nTimeout, int *pbIsRunning)
{
    UTS::HighPrecisionTimer time;
    time.SetStartTime();

	uts.log.Debug(_T("EEA_WaitStartTest"));

    g_bTesting[station] = false;

    while (g_bTesting[station] == false)
    {
        if (!*pbIsRunning)
        {
            uts.log.Error(_T("[%s]User Stop."), UTS_FN);
            return RV_USER_STOP;
        }
        if (nTimeout >= 0)
        {
            if (time.GetPassTime() > nTimeout)
            {
                uts.log.Error(_T("[%s]Timeout."), UTS_FN);
                return RV_TIMEOUT;
            }
        }
        MySleep(DELAY_TIME);
    }
    uts.log.Debug(_T("[%s] success. station = %d"), UTS_FN, station);
    return RV_OK;
}

int EEA_WaitAllStartTest(int nTimeout, int *pbIsRunning, eStation &station)
{
    UTS::HighPrecisionTimer time;
    time.SetStartTime();

    for (int i = 1; i < STATION_CNT; i++)
    {
        g_bTesting[i] = false;
    }
    bool bStart = false;
    while (true)
    {
        for (int i = 1; i < STATION_CNT; i++)
        {
            if (g_bTesting[i])
            {
                station = (eStation)i;
                bStart = true;
            }
        }
        if (bStart)
        {
            break;
        }
        if (!*pbIsRunning)
        {
            uts.log.Error(_T("[%s]User Stop."), UTS_FN);
            return RV_USER_STOP;
        }
        if (nTimeout >= 0)
        {
            if (time.GetPassTime() > nTimeout)
            {
                uts.log.Error(_T("[%s]Timeout."), UTS_FN);
                return RV_TIMEOUT;
            }
        }
        MySleep(DELAY_TIME);
    }
    uts.log.Debug(_T("[%s] success. station = %d"), UTS_FN, station);
    return RV_OK;
}

void EEA_GetASn(char *sn)
{
    memcpy(sn, g_DataSet.Pram[0], 256);
}

void EEA_GetWSn(wchar_t *sn)
{
    USES_CONVERSION;
    memcpy(sn, A2T(g_DataSet.Pram[0]), 256);
}

void EEA_MoveRelaylensCylinder(eCylinderPos pos)
{
    GetSocketServerInstance()->SendMoveRelaylensCylinder(pos);
    uts.log.Debug(_T("[%s]pos = %d"), UTS_FN, pos);
}

int EEA_WaitRelaylensCylinder(eCylinderPos pos, int nTimeout, int *pbIsRunning)
{
    UTS::HighPrecisionTimer time;
    time.SetStartTime();

    while (true)
    {
        GetSocketServerInstance()->SendGetRelaylensCylinderPos();
        MySleep(DELAY_TIME);
        int nCurPos = GetSocketServerInstance()->MachineStatue.GetRelaylensCylinderState();
        if (pos == nCurPos)
        {
            uts.log.Debug(_T("[%s][nCurPos = want_to_pos = %d]"), UTS_FN, pos);
            break;
        }
        else
        {
            uts.log.Debug(_T("[%s][nCurPos = %d][want_to_pos = %d]"), UTS_FN, nCurPos, pos);
        }
        if (!*pbIsRunning)
        {
            uts.log.Error(_T("[%s]User Stop."), UTS_FN);
            return RV_USER_STOP;
        }
        if (nTimeout >= 0)
        {
            if (time.GetPassTime() > nTimeout)
            {
                uts.log.Error(_T("[%s]Timeout."), UTS_FN);
                return RV_TIMEOUT;
            }
        }
    }

    uts.log.Debug(_T("[%s] success. pos = %d"), UTS_FN, pos);
    return RV_OK;
}

void EEA_MoveBoardCylinder(eCylinderPos pos)
{
    GetSocketServerInstance()->SendMoveBoardCylinder(pos);
    uts.log.Debug(_T("[%s]pos = %d"), UTS_FN, pos);
}

int EEA_WaitBoardCylinder(eCylinderPos pos, int nTimeout, int *pbIsRunning)
{
    UTS::HighPrecisionTimer time;
    time.SetStartTime();

    while (true)
    {
        GetSocketServerInstance()->SendGetBoardCylinderPos();
        MySleep(DELAY_TIME);
        int nCurPos = GetSocketServerInstance()->MachineStatue.GetBoardCylinderState();
        if (pos == nCurPos)
        {
            uts.log.Debug(_T("[%s][nCurPos = want_to_pos = %d]"), UTS_FN, pos);
            break;
        }
        else
        {
            uts.log.Debug(_T("[%s][nCurPos = %d][want_to_pos = %d]"), UTS_FN, nCurPos, pos);
        }
        if (!*pbIsRunning)
        {
            uts.log.Error(_T("[%s]User Stop."), UTS_FN);
            return RV_USER_STOP;
        }
        if (nTimeout >= 0)
        {
            if (time.GetPassTime() > nTimeout)
            {
                uts.log.Error(_T("[%s]Timeout."), UTS_FN);
                return RV_TIMEOUT;
            }
        }
    }

    uts.log.Debug(_T("[%s] success. pos = %d"), UTS_FN, pos);
    return RV_OK;
}

void EEA_MovePushCylinder(eCylinderPos pos)
{
    GetSocketServerInstance()->SendMovePushCylinder(pos);
    uts.log.Debug(_T("[%s]pos = %d"), UTS_FN, pos);
}

int EEA_WaitPushCylinder(eCylinderPos pos, int nTimeout, int *pbIsRunning)
{
    UTS::HighPrecisionTimer time;
    time.SetStartTime();

    while (true)
    {
        GetSocketServerInstance()->SendGetPushCylinderPos();
        MySleep(DELAY_TIME);
        int nCurPos = GetSocketServerInstance()->MachineStatue.GetPushCylinderState();
        if (pos == nCurPos)
        {
            uts.log.Debug(_T("[%s][nCurPos = want_to_pos = %d]"), UTS_FN, pos);
            break;
        }
        else
        {
            uts.log.Debug(_T("[%s][nCurPos = %d][want_to_pos = %d]"), UTS_FN, nCurPos, pos);
        }
        if (!*pbIsRunning)
        {
            uts.log.Error(_T("[%s]User Stop."), UTS_FN);
            return RV_USER_STOP;
        }
        if (nTimeout >= 0)
        {
            if (time.GetPassTime() > nTimeout)
            {
                uts.log.Error(_T("[%s]Timeout."), UTS_FN);
                return RV_TIMEOUT;
            }
        }
    }

    uts.log.Debug(_T("[%s] success. pos = %d"), UTS_FN, pos);
    return RV_OK;
}

void EEA_MoveCanvas(int x, int y)
{
    GetSocketServerInstance()->SendMoveCanvas(x, y);
    uts.log.Debug(_T("[%s]x = %d, y = %d"), UTS_FN, x, y);
}

int EEA_WaitCanvas(int nTimeout, int *pbIsRunning)
{
    UTS::HighPrecisionTimer time;
    time.SetStartTime();

    while (true)
    {
        GetSocketServerInstance()->SendEndMoveCanvas();
        MySleep(DELAY_TIME);
        int nState = GetSocketServerInstance()->MachineStatue.GetMoveCanvasState();
        uts.log.Debug(_T("[%s][nState = %d]"), UTS_FN, nState);
        if (0 == nState)
        {
            break;
        }
        if (!*pbIsRunning)
        {
            uts.log.Error(_T("[%s]User Stop."), UTS_FN);
            return RV_USER_STOP;
        }
        if (nTimeout >= 0)
        {
            if (time.GetPassTime() > nTimeout)
            {
                uts.log.Error(_T("[%s]Timeout."), UTS_FN);
                return RV_TIMEOUT;
            }
        }
    }

    uts.log.Debug(_T("[%s] success."), UTS_FN);
    return RV_OK;
}

