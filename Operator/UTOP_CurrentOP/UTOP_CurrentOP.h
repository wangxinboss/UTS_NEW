// UTOP_CurrentOP.h : UTOP_CurrentOP DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CUTOP_CurrentOPApp
// �йش���ʵ�ֵ���Ϣ������� UTOP_CurrentOP.cpp
//

class CUTOP_CurrentOPApp : public CWinApp
{
public:
	CUTOP_CurrentOPApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUTOP_CurrentOPApp theApp;