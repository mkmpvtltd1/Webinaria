#pragma once

namespace EditorApplication
{
	class CError
	{
	public:
		// Default constructor
		CError(void);

		// Virtual destructor
		virtual ~CError(void);

		// Static function provide error messge window dialog
		static void ErrMsg(HWND hMainWnd, LPTSTR szFormat,...);
	};
}
