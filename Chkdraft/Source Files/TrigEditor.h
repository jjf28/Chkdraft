#ifndef TRIGEDITOR_H
#define TRIGEDITOR_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include "Windows UI/WindowsUI.h"
#include "Triggers.h"
#include "Templates.h"
#include "Counters.h"
#include "CUWPs.h"
#include "Switches.h"

class TrigEditorWindow : public ClassWindow
{
	public:
		TriggersWindow triggersWindow;

		TrigEditorWindow();
		bool CreateThis(HWND hParent);
		bool DestroyThis();
		void ChangeTab(u32 tabId);
		void RefreshWindow();

	protected:
		void CreateSubWindows(HWND hWnd);
		void DoSize();

	private:
		u32 currTab;
		TabControl tabs;

		TemplatesWindow templatesWindow;
		CountersWindow countersWindow;
		CUWPsWindow cuwpsWindow;
		SwitchesWindow switchesWindow;

		BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif