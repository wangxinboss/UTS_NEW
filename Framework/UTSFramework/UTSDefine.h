#pragma once

#include "UTSBase.h"
#include "UTS.h"
#pragma comment(lib, "UTSBase.lib")

//------------------------------------------------------------------------------
// User Message
#define WM_LISTLOG          (WM_USER + 0x101)
#define WM_BOARD_MSG        (WM_USER + 0x102)
#define WM_BOARD_SN         (WM_USER + 0x103)
#define WM_BOARD_STATUS     (WM_USER + 0x104)
#define WM_BOARD_ERRORMSG   (WM_USER + 0x105)
#define WM_BOARD_ITEMLIST   (WM_USER + 0x106)
#define WM_BOARD_ADJUST     (WM_USER + 0x107)


//------------------------------------------------------------------------------
#define PASS_STR    _T("PASS")
#define FAIL_STR    _T("FAIL")

//------------------------------------------------------------------------------
#define USERTYPE_OPERATOR   0
#define USERTYPE_PE         1
#define USERTYPE_SOFTWARE   2

