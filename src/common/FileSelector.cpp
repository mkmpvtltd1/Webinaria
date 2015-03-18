#include "StdAfx.h"
#include "FileSelector.h"

#include "Folders.h"

///
/// FileSelector.
///

FileSelector::FileSelector()
{
}

FileSelector::~FileSelector()
{
}

bool FileSelector::Select(HWND parent, SelectorAction action, const TCHAR *defaultPath/* = 0*/, FileType fileType/* = FileTypeAvi*/)
{
    OPENFILENAME ofn = { 0 };

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = parent;
	ofn.nMaxFile = sizeof(m_FileName) / sizeof(m_FileName[0]);
    ofn.lpstrInitialDir = Folders::Home();

    switch (fileType)
    {
	case FileTypeAvi:
        _tcscpy(m_FileName, _T("Recording.avi"));
        ofn.lpstrDefExt = _T("avi");
	    ofn.lpstrFilter = _T("Recordings (*.avi)\0*.avi\0All Files\0*.*\0\0");
		break;

	case FileTypeFlv:
        _tcscpy(m_FileName, _T("Recording.flv"));
        ofn.lpstrDefExt = _T("flv");
	    ofn.lpstrFilter = _T("Recordings (*.flv)\0*.flv\0All Files\0*.*\0\0");
		break;

	case FileTypeCut:
        _tcscpy(m_FileName, _T("Recording.rcut"));
        ofn.lpstrDefExt = _T("rcut");
	    ofn.lpstrFilter = _T("Recording Cuts (*.rcut)\0*.rcut\0All Files\0*.*\0\0");
		break;

	default:
		return false;
    }

	if (defaultPath)
	{
		_tcscpy(m_FileName, defaultPath);
	}

	ofn.lpstrFile = m_FileName;
	ofn.nFilterIndex = 1;

    if (SelectToOpen == action)
    {
	    ofn.Flags = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        return GetOpenFileName(&ofn);
    }
    else
    {
	    ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;

        return GetSaveFileName(&ofn);
    }
}

const TCHAR* FileSelector::FileName() const
{
    return m_FileName;
}
