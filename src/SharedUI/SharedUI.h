// SharedUI.h : main header file for the SharedUI DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CSharedUIApp
// See SharedUI.cpp for the implementation of this class
//

class CSharedUIApp : public CWinApp
{
public:
	CSharedUIApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
