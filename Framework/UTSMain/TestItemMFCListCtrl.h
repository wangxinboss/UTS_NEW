#pragma once


// CTestItemMFCListCtrl
#define MAX_ROW_COUNT   128

class CTestItemMFCListCtrl : public CMFCListCtrl
{
	DECLARE_DYNAMIC(CTestItemMFCListCtrl)

public:
	CTestItemMFCListCtrl();
    virtual ~CTestItemMFCListCtrl();
    
    // Support for individual cells text/background colors:
    virtual COLORREF OnGetCellTextColor(int nRow, int nColum);
    virtual COLORREF OnGetCellBkColor(int nRow, int nColum);
    virtual HFONT OnGetCellFont(int nRow, int nColum, DWORD dwData = 0);

    void SetRowBkColor(int nRow, COLORREF color);
    //void Set

protected:
	DECLARE_MESSAGE_MAP()

private:
    CFont m_fontItem;
    COLORREF m_RowBkColor[MAX_ROW_COUNT];
};


