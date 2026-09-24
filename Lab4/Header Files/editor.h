#pragma once

#include "framework.h"
#include "resource.h"
#include "painter.h"

#include <commctrl.h>

class EDITOR
{
private:
    HWND hToolbar = NULL;
    PAINTER painter;
    HINSTANCE hInst = GetModuleHandle(NULL);

public:
    void onCreate(HWND hWnd);
    void onSize(HWND hWnd);
    void onPaint(HWND hWnd);
    
    void onLButtonDown(HWND hWnd, WPARAM x, LPARAM y);
    void onMouseMove(HWND hWnd, WPARAM x, LPARAM y);
    void onLButtonUp(HWND hWnd, WPARAM x, LPARAM y);
    void onDestroy();

    LRESULT onCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT onNotify(HWND hWnd, WPARAM wParam, LPARAM lParam);

    static INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
};