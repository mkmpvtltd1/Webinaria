#include "stdafx.h"
#include "OverlayElementEditorManager.h"

#include "TextElementEditor.h"

OverlayElementEditorManager::OverlayElementEditorManager()
	: currentEditor(0)
{
}

OverlayElementEditorManager::~OverlayElementEditorManager()
{
	DestroyCurrentEditor();
}

void OverlayElementEditorManager::DestroyCurrentEditor()
{
	if (currentEditor)
	{
		__unhook(&OverlayElementEditor::ElementChanged, currentEditor, &OverlayElementEditorManager::OnElementChanged, this);
		delete currentEditor;
		currentEditor = 0;
	}
}

void OverlayElementEditorManager::ShowCurrentEditor()
{
	if (currentEditor)
		currentEditor->Show();
}

void OverlayElementEditorManager::ShowEditorFor(HWND parent, OverlayElement *element)
{
	DestroyCurrentEditor();

	if (element)
	{
		switch (element->GetElementType())
		{
		case ElementType_Text:
			currentEditor = new TextElementEditor(parent, element);
			break;
		}

		if (currentEditor)
		{
			__hook(&OverlayElementEditor::ElementChanged, currentEditor, &OverlayElementEditorManager::OnElementChanged, this);
			currentEditor->Show();
		}
	}
}

void OverlayElementEditorManager::OnElementChanged()
{
	__raise ElementChanged();
}
