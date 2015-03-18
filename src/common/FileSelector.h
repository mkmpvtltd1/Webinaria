#pragma once

enum SelectorAction
{
    SelectToOpen,
    SelectToSave
};

enum FileType
{
    FileTypeAvi,
    FileTypeFlv,
	FileTypeCut
};

class FileSelector
{

private:
    TCHAR m_FileName[MAX_PATH];

public:
    FileSelector();
    ~FileSelector();

    bool Select(HWND parent, SelectorAction action, const TCHAR *defaultPath = 0, FileType fileType = FileTypeAvi);

    const TCHAR* FileName() const;

};
