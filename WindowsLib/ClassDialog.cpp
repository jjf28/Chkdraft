#include "ClassDialog.h"
#include <CommCtrl.h>
#include <SimpleIcu.h>

namespace WinLib {

    ClassDialog::ClassDialog() : allowEditNotify(true)
    {
        WindowClassName().clear();
    }

    ClassDialog::~ClassDialog()
    {
        if ( getHandle() != NULL )
            DestroyDialog();
    }

    bool ClassDialog::SetDialogItemText(int nIDDlgItem, const std::string &text)
    {
        return SetDlgItemText(getHandle(), nIDDlgItem, icux::toUistring(text).c_str()) != 0;
    }

    bool ClassDialog::CreateModelessDialog(LPCTSTR lpTemplateName, HWND hWndParent)
    {
        if ( getHandle() == NULL )
            setHandle(CreateDialogParam(GetModuleHandle(NULL), lpTemplateName, hWndParent, (DLGPROC)SetupDialogProc, (LPARAM)this));

        if ( getHandle() != NULL )
            return true;

        return false;
    }

    INT_PTR ClassDialog::CreateDialogBox(LPCTSTR lpTemplateName, HWND hWndParent)
    {
        return DialogBoxParam(GetModuleHandle(NULL), lpTemplateName, hWndParent, (DLGPROC)SetupDialogProc, (LPARAM)this);
    }

    bool ClassDialog::DestroyDialog()
    {
        if ( getHandle() != NULL && EndDialog(getHandle(), IDCLOSE) != 0 )
        {
            setHandle(NULL);
            allowEditNotify = true;
            return true;
        }
        else
            return false;
    }

    BOOL ClassDialog::DlgNotify(HWND, WPARAM, NMHDR*)
    {
        return FALSE;
    }

    void ClassDialog::NotifyTreeSelChanged(LPARAM)
    {

    }

    void ClassDialog::NotifyButtonClicked(int, HWND)
    {

    }

    void ClassDialog::NotifyEditUpdated(int, HWND)
    {

    }

    void ClassDialog::NotifyEditFocused(int idFrom, HWND hWndFrom)
    {

    }

    void ClassDialog::NotifyEditFocusLost(int, HWND)
    {

    }

    void ClassDialog::NotifyComboSelChanged(int idFrom, HWND hWndFrom)
    {

    }

    void ClassDialog::NotifyComboEditUpdated(int idFrom, HWND hWndFrom)
    {

    }

    void ClassDialog::NotifyComboEditFocused(int idFrom, HWND hWndFrom)
    {

    }

    void ClassDialog::NotifyComboEditFocusLost(int idFrom, HWND hWndFrom)
    {

    }

    void ClassDialog::NotifyWindowHidden()
    {

    }

    void ClassDialog::NotifyWindowShown()
    {

    }

    BOOL ClassDialog::DlgCommand(HWND, WPARAM, LPARAM)
    {
        return FALSE;
    }

    BOOL ClassDialog::DlgProc(HWND, UINT, WPARAM, LPARAM)
    {
        return FALSE;
    }

    void ClassDialog::SendNotifyEditUpdated(int idFrom, HWND hWndFrom)
    {
        if ( allowEditNotify )
        {
            allowEditNotify = false;
            NotifyEditUpdated(idFrom, hWndFrom);
            allowEditNotify = true;
        }
    }

    BOOL CALLBACK ClassDialog::SetupDialogProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if ( msg == WM_INITDIALOG )
        {
            LONG_PTR classPtr = lParam;
            SetWindowLongPtr(hWnd, DWLP_USER, classPtr);
            if ( GetWindowLongPtr(hWnd, DWLP_USER) == classPtr && classPtr != 0 && SetWindowLong(hWnd, DWLP_DLGPROC, (LONG_PTR)ForwardDlgProc) != 0 )
            {
                ((ClassDialog*)classPtr)->setHandle(hWnd); // Preload the window handle
                return ((ClassDialog*)classPtr)->DlgProc(hWnd, msg, wParam, lParam);
            }
            else
                return FALSE;
        }
        else
            return FALSE;
    }

    BOOL CALLBACK ClassDialog::ForwardDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        ClassDialog* classDialog = (ClassDialog*)GetWindowLong(hWnd, DWLP_USER);
        switch ( msg )
        {
            case WM_SHOWWINDOW:
            {
                if ( (BOOL)wParam == TRUE )
                    classDialog->NotifyWindowShown();
                else
                {
                    HWND hEditFocused = GetFocus();
                    int editCtrlId = GetDlgCtrlID(hEditFocused);
                    classDialog->NotifyEditFocusLost(editCtrlId, hEditFocused);
                    classDialog->NotifyComboEditFocusLost(editCtrlId, hEditFocused);
                    classDialog->NotifyWindowHidden();
                }
            }
            break;

            case WM_NOTIFY:
            {
                switch ( ((NMHDR*)lParam)->code )
                {
                    case TVN_SELCHANGED: classDialog->NotifyTreeSelChanged(((NMTREEVIEW*)lParam)->itemNew.lParam); break;
                }
                return classDialog->DlgNotify(hWnd, wParam, (NMHDR*)lParam);
            }
            break;

            case WM_COMMAND:
            {
                switch ( HIWORD(wParam) )
                {
                    case BN_CLICKED: classDialog->NotifyButtonClicked(LOWORD(wParam), (HWND)lParam); break;
                    case EN_UPDATE: classDialog->SendNotifyEditUpdated(LOWORD(wParam), (HWND)lParam); break;
                    case EN_SETFOCUS: classDialog->NotifyEditFocused(LOWORD(wParam), (HWND)lParam); break;
                    case EN_KILLFOCUS: classDialog->NotifyEditFocusLost(LOWORD(wParam), (HWND)lParam); break;
                    case CBN_SELCHANGE: classDialog->NotifyComboSelChanged(LOWORD(wParam), (HWND)lParam); break;
                    case CBN_EDITCHANGE: classDialog->NotifyComboEditUpdated(LOWORD(wParam), (HWND)lParam); break;
                    case CBN_SETFOCUS: classDialog->NotifyComboEditFocused(LOWORD(wParam), (HWND)lParam); break;
                    case CBN_KILLFOCUS: classDialog->NotifyComboEditFocusLost(LOWORD(wParam), (HWND)lParam); break;
                }
                return classDialog->DlgCommand(hWnd, wParam, lParam);
            }
            break;
        }
        return classDialog->DlgProc(hWnd, msg, wParam, lParam);
    }

}
