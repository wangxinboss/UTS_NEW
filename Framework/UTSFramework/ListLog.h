#pragma once
#include "Export.h"

namespace UTS
{
    class UTS_FRAMEWORK_API ListLog
    {
    public:
        BOOL Initialize(void);
        void Destroy(void);
        void ShowWindow(void);

        void Debug(LPCTSTR lpFormat, ...);
        void Info(LPCTSTR lpFormat, ...);
        void Warning(LPCTSTR lpFormat, ...);
        void Error(LPCTSTR lpFormat, ...);

        CString *m_LogList;
        int m_LogIndex;

    private:
        void Output(COLORREF color, LPCTSTR lpMsg);
    };
}
