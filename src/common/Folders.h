#pragma once

class Folders
{

private:
    Folders();

    static bool BuildPathFromBase(TCHAR *path, const TCHAR *base, const TCHAR *subPath);

public:
    static const TCHAR* My();                                   //  Returns path to module which is currently executing.
    static const TCHAR* Home();                                 //  Returns path to Webinaria files in My Documents.
    static const TCHAR* RecorderUI();                           //  Returns path to Recorder UI.
    static const TCHAR* EditorUI();                             //  Returns path to Editor UI.

    static const TCHAR* FromMine(TCHAR *path, const TCHAR *subPath);         //  Builds a path using My() as a base.
    static const TCHAR* FromHome(TCHAR *path, const TCHAR *subPath);         //  Builds a path using Home() as a base.
    static const TCHAR* FromRecorderUI(TCHAR *path, const TCHAR *subPath);   //  Builds a path using RecorderUI() as a base.
    static const TCHAR* FromEditorUI(TCHAR *path, const TCHAR *subPath);     //  Builds a path using EditorUI() as a base.

    static bool EnsurePathExists(const TCHAR *path);
    static bool EnsureHomeFolderExists();						//	Creates Webinaria's folder under My Documents and brands it with application icon.

};
