#include "StdAfx.h"
#include "UIBoardProtocol.h"


namespace UTS
{
    void UIBoardProtocol::Initialize(HWND hBoardWnd)
    {
        m_hBoardWnd = hBoardWnd;

        ::PostMessage(m_hBoardWnd, WM_BOARD_ITEMLIST, eBoardItemList::BIL_Init, 0);
    }

    void UIBoardProtocol::ShowMsg(LPCTSTR lpMsg)
    {
        m_strMsg = lpMsg;
        ::PostMessage(m_hBoardWnd, WM_BOARD_MSG, 0, 0);
    }

    void UIBoardProtocol::ShowSN(LPCTSTR lpSN)
    {
        uts.info.strSN = lpSN;
        ::PostMessage(m_hBoardWnd, WM_BOARD_SN, 0, 0);
    }

    void UIBoardProtocol::ShowStatus(eBoardStatus status)
    {
        ::PostMessage(m_hBoardWnd, WM_BOARD_STATUS, (WPARAM)status, 0);
    }

    void UIBoardProtocol::ShowStatus(LPCTSTR lpRes)
    {
        m_strResName = lpRes;
        ::PostMessage(m_hBoardWnd, WM_BOARD_STATUS, (WPARAM)(eBoardStatus::Status_UserDefine), 0);
    }

    void UIBoardProtocol::ShowErrorMsg(LPCTSTR lpErrorMsg)
    {
        m_strErrorMsg = lpErrorMsg;
        ::PostMessage(m_hBoardWnd, WM_BOARD_ERRORMSG, 0, 0);
    }

    void UIBoardProtocol::SetItemPass(int nItemIndex)
    {
        ::PostMessage(m_hBoardWnd, WM_BOARD_ITEMLIST, eBoardItemList::BIL_SetItemPass, nItemIndex);
    }

    void UIBoardProtocol::SetItemFail(int nItemIndex)
    {
        ::PostMessage(m_hBoardWnd, WM_BOARD_ITEMLIST, eBoardItemList::BIL_SetItemFail, nItemIndex);
    }

    void UIBoardProtocol::SetItemTesting(int nItemIndex)
    {
        ::PostMessage(m_hBoardWnd, WM_BOARD_ITEMLIST, eBoardItemList::BIL_SetItemTesting, nItemIndex);
    }
}


