#pragma once
#include "Export.h"

namespace UTS
{
    typedef enum _e_board_status_
    {
        Status_Initializing = 0,
        Status_Ready,
        Status_Testing,
        Status_Stop,
        Status_Pass,
        Status_Fail,
        Status_UserDefine,
    } eBoardStatus;

    typedef enum _e_board_item_list_
    {
        BIL_Init = 0,
        BIL_SetItemTesting,
        BIL_SetItemPass,
        BIL_SetItemFail
    } eBoardItemList;

    class UTS_FRAMEWORK_API UIBoardProtocol
    {
    public:
        void Initialize(HWND hBoardWnd);

        // set board msg
        void ShowMsg(LPCTSTR lpMsg);

        // set board sn
        void ShowSN(LPCTSTR lpSN);

        // set board status
        void ShowStatus(eBoardStatus status);
        void ShowStatus(LPCTSTR lpRes);

        // set board error msg
        void ShowErrorMsg(LPCTSTR lpErrorMsg);

        // set board item list, nItemIndex from 0
        void SetItemPass(int nItemIndex);
        void SetItemFail(int nItemIndex);
        void SetItemTesting(int nItemIndex);

        CString m_strResName;
        CString m_strMsg;
        CString m_strErrorMsg;

    private:
        HWND m_hBoardWnd;
    };
}
