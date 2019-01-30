#pragma once
#include "Export.h"
#include "BaseDevice.h"
#include "BaseOperator.h"

namespace UTS
{
    typedef enum _e_flow_status_
    {
        Initializing = 0,
        Ready,
        Testing,
        Stoping,
    } eFlowStatus;

    typedef struct _operator_info_
    {
        BaseOperator *pInstance;
        CString strFileName;
        int nIndex;
    } OPERATOR_INFO;

    class UTS_FRAMEWORK_API TestFlow
    {
    public:
        TestFlow(void);
        ~TestFlow(void);

        BOOL Initialize(HWND hDisplayWnd);
        void Destroy();

        BOOL DoInitialOperatorList();
        void Test();
        void Stop();
        void WaitTestFinish();

        eFlowStatus m_flowStatus;
        BaseDevice *m_pDevice;
        vector<OPERATOR_INFO> m_vecOpList;

        //------------------------------------------------------------------------------
        // test thread
        CWinThread *m_pThread;
        static BOOL m_bIsRunning;
        static UINT TestThreadProc(PVOID param);
        static void SaveThroughResult(LPCTSTR lpThroughResult); // fix 改善 #4: 白板分开测试导致资料也分开，需要一个总的履历表

    private:
        HMODULE m_hDvDllHandle;
        vector<HMODULE> m_vecOpDllHandle;
        vector<HMODULE> m_vecInitOpDllHandle;
        vector<OPERATOR_INFO> m_vecInitOpList;
        vector<HMODULE> m_vecFinalOpDllHandle;
        vector<OPERATOR_INFO> m_vecFinalOpList;
        CCriticalSection m_cs;

        BOOL InitDevice(HWND hDisplayWnd);
        BOOL InitInitOperator();
        BOOL InitTestOperator();
        BOOL InitFinalOperator();
        BOOL DoFinalOperatorList();
    };
}
