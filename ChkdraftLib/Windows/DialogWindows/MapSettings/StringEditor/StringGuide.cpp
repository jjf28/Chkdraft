#include "StringGuide.h"

enum_t(Id, u32, {
    TEXT_COLOR_FIRST = ID_FIRST,
    TEXT_COLOR_LAST = (TEXT_COLOR_FIRST+27)
});

StringGuideWindow::~StringGuideWindow()
{

}

bool StringGuideWindow::CreateThis(HWND hParent)
{
    if ( ClassWindow::RegisterWindowClass(0, NULL, NULL, CreateSolidBrush(RGB(0, 0, 0)), NULL, "StringGuide", NULL, false) &&
         ClassWindow::CreateClassWindow(0, "", WS_OVERLAPPED|WS_CHILD|WS_VISIBLE, 463, 7, 125, 365, hParent, NULL) )
    {
        HWND hStringGuide = getHandle();
        HDC hDC = GetDC(hStringGuide);
        SelectObject(hDC, defaultFont);
        SIZE strSize = { };
        for (size_t i = 0; i < strColors.size(); i++)
        {
            auto Color = strColors.at(i);
            GetTextExtentPoint32(hDC, icux::toUistring(std::get<2>(Color)).c_str(), (int)icux::toUistring(std::get<2>(Color)).length(), &strSize);
            colorPrefix[i].CreateThis(hStringGuide, 2, (s32)i*13, strSize.cx, 13, std::get<2>(Color), 0);
            color[i].CreateThis(hStringGuide, strSize.cx+3, (s32)i*13, 100, 13, std::get<1>(Color), Id::TEXT_COLOR_FIRST+i);
        }
        ReleaseDC(hDC);
        return true;
    }
    else
        return false;
}

LRESULT StringGuideWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_CTLCOLORSTATIC:
            {
                SetBkMode((HDC)wParam, TRANSPARENT);
                int id = GetDlgCtrlID((HWND)lParam);
                if ( id >= Id::TEXT_COLOR_FIRST && id <= Id::TEXT_COLOR_LAST )
                    SetTextColor((HDC)wParam, std::get<0>(strColors.at(id-Id::TEXT_COLOR_FIRST)));
                else
                    SetTextColor((HDC)wParam, RGB(16, 252, 24));

                return (LRESULT)CreateSolidBrush(RGB(0, 0, 0));
            }
            break;

        default:
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;
    }
    return 0;
}
