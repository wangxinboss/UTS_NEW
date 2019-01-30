#pragma once
#include "Export.h"

namespace UTS
{
    class UTS_FRAMEWORK_API Keyboard
    {
        /*
    public:
        // 同步等待SN
        void WaitSyncSN(HINSTANCE hInstance, BOOL *pbIsRunning);

        // 异步等待VirtualKey
        void WaitAsyncVirtualKeyBegin(HINSTANCE hInstance, BOOL *pbIsRunning, DWORD dwVirtualKey);
        void WaitAsyncVirtualKeyEnd(void);

    private:
        void WaitSync(HINSTANCE hInstance, BOOL *pbIsRunning, HOOKPROC hookproc);
        */

    public:
        Keyboard();
        ~Keyboard();

        void BeginListen();
        void EndListen();

        void OnKeyReturn();
        BOOL IsReturnPress();
        void WaitReturn(BOOL *pbIsRunning);

        void OnKeySpace();
        BOOL IsSpacePress();
        void WaitSpace(BOOL *pbIsRunning);

        void OnKeyChar(BYTE vkCode);
        void OnKeyBackspace();  // fix bug #6: SN输入后不能删除或者更改

    private:
        BOOL m_bListening;      // 正在监听按键
        BOOL m_bReturnPress;    // 回车键按下
        BOOL m_bSpacePress;     // 空格键按下
        BOOL m_bWaitingReturn;  // 正在等待回车键
    };
}
