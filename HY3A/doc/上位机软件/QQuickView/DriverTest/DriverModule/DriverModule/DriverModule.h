
// DriverModule.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CDriverModuleApp:
// �йش����ʵ�֣������ DriverModule.cpp
//

class CDriverModuleApp : public CWinApp
{
public:
	CDriverModuleApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CDriverModuleApp theApp;