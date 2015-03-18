// JPEG.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved )
{
    return TRUE;
}

DLL_SPEC void _stdcall SaveAsJPEG(const wchar_t * File)
{
   CImage myimage;
   wchar_t * FileName = new wchar_t[wcslen(File) + 5];
   wcscpy(FileName, File);
   wcscat(FileName, L".jpg");
   // Load existing image
   myimage.Load(File); 
   // Save an image in JPEG format
   myimage.Save(FileName);
}