#pragma once

#include "OverlayElement.h"
#include "OverlayElementEditor.h"

[event_source(native), event_receiver(native)]
class OverlayElementEditorManager
{

private:
	OverlayElementEditor *currentEditor;

	void OnElementChanged();

public:
	OverlayElementEditorManager();
	~OverlayElementEditorManager();

	void ShowCurrentEditor();
	void DestroyCurrentEditor();
	void ShowEditorFor(HWND parent, OverlayElement *element);

public:
	__event void ElementChanged();

};
