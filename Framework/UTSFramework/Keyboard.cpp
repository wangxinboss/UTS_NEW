#include "StdAfx.h"
#include "Keyboard.h"


namespace UTS
{
    /*
    HHOOK g_hook = nullptr;

#pragma region WaitSyncSN
    BOOL g_bContinue = FALSE;

    // 等待SN后，输入回车
    LRESULT CALLBACK WaitSNProc(INT nCode, WPARAM wParam, LPARAM lParam)
    {
        DWORD vkCode = (DWORD)wParam;
        DWORD nKeyRelease = lParam & 0x80000000;

        switch (nCode)
        {  
        case HC_ACTION:
            if (nKeyRelease != 0)
            {
                if ((vkCode >= 0x30 && vkCode <= 0x39)      // 0-9
                    || (vkCode >= 0x41 && vkCode <= 0x5A)   // A-Z
                    )  // a-z
                {
                    uts.info.strSN.AppendChar((TCHAR)vkCode);
                    uts.board.ShowSN(uts.info.strSN);
                }
                else if (VK_RETURN == vkCode)
                {
                    if (uts.info.strSN == EMPTY_STR)
                    {
                        break;
                    }
                    g_bContinue = TRUE;
                }
            }
            break;
        default:
            break;  
        }

        return CallNextHookEx(g_hook, nCode, wParam, lParam);
    }

    void Keyboard::WaitSync(HINSTANCE hInstance, BOOL *pbIsRunning, HOOKPROC hookproc)
    {
        g_bContinue = FALSE;

        g_hook = (HHOOK)SetWindowsHookEx(
            WH_KEYBOARD, hookproc, hInstance, 0);  
        while (!g_bContinue && *pbIsRunning)
        {
            Sleep(50);
        }

        if (g_hook)
        {  
            BOOL result = UnhookWindowsHookEx((HHOOK)g_hook); // 卸载钩子  
            if (result)
            {
                g_hook = NULL;  
            }
        }
    }

    void Keyboard::WaitSyncSN(HINSTANCE hInstance, BOOL *pbIsRunning)
    {
        uts.log.Debug(_T("WaitSyncSN Begin."));
        WaitSync(hInstance, pbIsRunning, WaitSNProc);
        uts.log.Debug(_T("WaitSyncSN End."));
    }
#pragma endregion

#pragma region WaitAsyncVirtualKey
    DWORD g_dwVirtualKey = 0;
    BOOL* g_pbGetKey = FALSE;

    LRESULT CALLBACK WaitVirtualKeyProc(INT nCode, WPARAM wParam, LPARAM lParam)
    {
        DWORD vkCode = (DWORD)wParam;
        DWORD nKeyRelease = lParam & 0x80000000;

        switch (nCode)
        {  
        case HC_ACTION:
            if (nKeyRelease != 0)
            {
                if (g_dwVirtualKey == vkCode)
                {
                    *g_pbGetKey = TRUE;
                }
            }
            break;
        default:
            break;  
        }

        return CallNextHookEx(g_hook, nCode, wParam, lParam);
    }

    void Keyboard::WaitAsyncVirtualKeyBegin(HINSTANCE hInstance, BOOL *pbGetKey, DWORD dwVirtualKey)
    {
        uts.log.Debug(_T("WaitAsyncVirtualKeyBegin"));
        g_dwVirtualKey = dwVirtualKey;
        g_pbGetKey = pbGetKey;

        g_hook = (HHOOK)SetWindowsHookEx(
            WH_KEYBOARD, (HOOKPROC)WaitVirtualKeyProc, hInstance, 0);  
    }

    void Keyboard::WaitAsyncVirtualKeyEnd(void)
    {
        if (g_hook)
        {  
            BOOL result = UnhookWindowsHookEx((HHOOK)g_hook); // 卸载钩子  
            if (result)
            {
                g_hook = NULL;  
            }
        }
        uts.log.Debug(_T("WaitAsyncVirtualKeyEnd"));
    }
    */
    Keyboard::Keyboard()
    {
        m_bListening = FALSE;
        m_bReturnPress = FALSE;
        m_bSpacePress = FALSE;
        m_bWaitingReturn = FALSE;
    }

    Keyboard::~Keyboard()
    {

    }

    void Keyboard::BeginListen()
    {
        m_bListening = TRUE;
        m_bReturnPress = FALSE;
        m_bSpacePress = FALSE;
        m_bWaitingReturn = FALSE;
    }

    void Keyboard::EndListen()
    {
        m_bListening = FALSE;
        m_bWaitingReturn = FALSE;
    }

    void Keyboard::OnKeyReturn()
    {
        if (m_bListening)
        {
            m_bReturnPress = TRUE;
        }
    }

    void Keyboard::OnKeySpace()
    {
        if (m_bListening)
        {
            m_bSpacePress = TRUE;
        }
    }

    void Keyboard::OnKeyChar(BYTE vkCode)
    {
        if (m_bListening)
        {
            if (m_bWaitingReturn)
            {
                uts.info.strSN.AppendChar(vkCode);
                uts.board.ShowSN(uts.info.strSN);
            }
        }
    }

    //------------------------------------------------------------------------------
    // fix bug #6: SN输入后不能删除或者更改
    void Keyboard::OnKeyBackspace()
    {
        if (m_bListening)
        {
            if (m_bWaitingReturn)
            {
                uts.info.strSN = uts.info.strSN.Left(uts.info.strSN.GetLength() - 1);
                uts.board.ShowSN(uts.info.strSN);
            }
        }
    }
    //------------------------------------------------------------------------------

    BOOL Keyboard::IsReturnPress()
    {
        return m_bReturnPress;
    }

    BOOL Keyboard::IsSpacePress()
    {
        return m_bSpacePress;
    }

    void Keyboard::WaitSpace(BOOL *pbIsRunning)
    {
        if (m_bListening)
        {
            while (*pbIsRunning)
            {
                if (m_bSpacePress)
                {
                    break;
                }
                else
                {
                    Sleep(1);
                }
            }
        }
    }

    void Keyboard::WaitReturn(BOOL *pbIsRunning)
    {
        if (m_bListening)
        {
            m_bWaitingReturn = TRUE;
            while (*pbIsRunning)
            {
                if (m_bReturnPress)
                {
                    break;
                }
                else
                {
                    Sleep(1);
                }
            }
        }
    }

#pragma endregion
}
