#ifndef STREAMEDITCONTROL_H
#define STREAMEDITCONTROL_H
#include "../../CommonFiles/CommonFiles.h"
#include "../WindowsLib/WindowsUi.h"
#include "../IcuLib/SimpleIcu.h"
#include <deque>
#include <ostream>
#include <streambuf>

class StreamEditControl : public WinLib::ClassWindow, public std::ostream, public std::streambuf
{
    public:
        StreamEditControl();
        virtual ~StreamEditControl();
        bool CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, bool readOnly, u64 id);

    protected:
        virtual int sync();
        virtual int overflow(int c);
        void SizeSubWindows();
        void ToggleLineNumbers();
        virtual void ContextMenu(int x, int y);
        virtual LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    
    private:
        bool showLineNumbers;
        u64 lineNumber;
        std::deque<size_t> lineStart;
        icux::uistring prevLines;
        icux::uistring lineNumbersString;
        std::string currLine;
        WinLib::RichEditControl lineNumbers;
        WinLib::RichEditControl richText;

        enum_t(ContextMenuItem, UINT, {
            ToggleLineNumbers = 1
        });
};

#endif