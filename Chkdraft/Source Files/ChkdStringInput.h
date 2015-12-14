#ifndef CHKDSTRINGINPUT_H
#define CHKDSTRINGINPUT_H
#include "Mapping Core/MappingCore.h"
#include "Windows UI/WindowsUI.h"

class ChkdStringInputDialog : public ClassDialog
{
	public:
		static bool GetChkdString(ChkdString &str, ChkdString &initialString, HWND hParent);

	protected:
		ChkdStringInputDialog();
		bool InternalGetChkdString(ChkdString &str, ChkdString &initialString, HWND hParent);
		BOOL DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
		BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		bool gotChkdString;
		ChkdString newString;
		ChkdString initialString;
		EditControl editString;
};

#endif