//------------------------------------------------------------------------------
// File: Setup.cpp
//
// Desc: DirectShow sample code - implementation of RGBFilters sample filters
//
// Copyright (c) 2000-2002  Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

//#include <windows.h>
#include <streams.h>
//#include <atlbase.h>
//#include <qedit.h>


#include "iRGBFilters.h"
#include "RGBFilters_i.c"

#include "TransSmpte\WebinariaLogoFilter.h"
#include "TransSmpte\TransSmpte.h"
#include "TransSmpte\NotesAdder.h"
#include "TransSmpte\AudioSkipper.h"
#include "TransSmpte\Adder.h"
#include "TransSmpte\TimeRemover.h"
#include "TransSmpte\Synchronizer.h"

// List of class IDs and creator functions for the class factory. This
// provides the link between the OLE entry point in the DLL and an object
// being created. The class factory will call the static CreateInstance.
// We provide a set of filters in this one DLL.

CFactoryTemplate g_Templates[] = 
{
	{ L"WebinariaLogo",		&CLSID_CWebinariaLogoFilter,    CWebinariaLogoFilter::CreateInstance,   NULL, &sudWebinLogo },
    { L"RemoveBackGround",  &CLSID_CRemoveBackground,		CRemoveBackground::CreateInstance,		NULL, &sudTransSmpte },
	{ L"Adder",				&CLSID_CAdder,					CAdder::CreateInstance,					NULL, &sudAdder },
	{ L"NotesAdder",		&CLSID_CNotesAdder,				CNotesAdder::CreateInstance,			NULL, &sudTransNotes},
	{ L"TimeRemover",		&CLSID_CTimeRemover,			CTimeRemover::CreateInstance,			NULL, &sudTimeRemover},
	{ L"Synchronizer",		&CLSID_CSynchronizer,			CSynchronizer::CreateInstance,			NULL, &sudSynchronizer},
	{ L"AudioSkipper",		&CLSID_CAudioSkipper,			CAudioSkipper::CreateInstance,			NULL, &sudAudioSkipper}
};
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);

//
// DllRegisterServer
//
STDAPI DllRegisterServer()
{
    return AMovieDllRegisterServer2( TRUE );

} // DllRegisterServer

//
// DllUnregisterServer
//
STDAPI DllUnregisterServer()
{
    return AMovieDllRegisterServer2( FALSE );

} // DllUnregisterServer