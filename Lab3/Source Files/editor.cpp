#include "framework.h"
#include "editor.h"

#pragma comment(lib, "comctl32.lib")

EDITOR::EDITOR(HINSTANCE hInst)
{
    this->hInst = hInst;
}

void EDITOR::onCreate(HWND hWnd)
{
    TBBUTTON tbButtons[] = 
    {
        {0, ID_DOT_CHOOSE, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, 0},
        {1, ID_LINE_CHOOSE, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, 0},
        {2, ID_RECTANGLE_CHOOSE, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, 0},
        {3, ID_ELLIPSE_CHOOSE, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, 0}
    };
    
    hToolbar = CreateToolbarEx(
        hWnd,
        WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS,
        10001,
        4,
        hInst,
        IDB_TOOLBAR,
        tbButtons,
        4,
        16, 15,
        16, 15,
        sizeof(TBBUTTON)
    );
}

void EDITOR::onSize(HWND hWnd)
{
    if (hToolbar != NULL)
    {
        SendMessage(hToolbar, TB_AUTOSIZE, 0, 0);
    }
}

void EDITOR::onLButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    int startX = GET_X_LPARAM(lParam);
    int startY = GET_Y_LPARAM(lParam);
    painter.startDrawing(hWnd, startX, startY);
}

void EDITOR::onMouseMove(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    int endX = GET_X_LPARAM(lParam);
    int endY = GET_Y_LPARAM(lParam);
    painter.tempDrawing(hWnd, endX, endY);
}

void EDITOR::onLButtonUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    int endX = GET_X_LPARAM(lParam);
    int endY = GET_Y_LPARAM(lParam);
    painter.endDrawing(hWnd, endX, endY);
}

void EDITOR::onPaint(HWND hWnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    RECT rect;
    GetClientRect(hWnd, &rect);

    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    HDC tempDC = CreateCompatibleDC(hdc);
    HBITMAP memBitmap = CreateCompatibleBitmap(hdc, width, height);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(tempDC, memBitmap);

    HPEN defPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HPEN dashPen = CreatePen(PS_DASH, 1, RGB(0, 0, 0));

    FillRect(tempDC, &rect, (HBRUSH)(COLOR_WINDOW + 1));

    painter.DrawAll(tempDC, defPen);

    BitBlt(hdc, 0, 0, width, height, tempDC, 0, 0, SRCCOPY);
    
    SelectObject(tempDC, oldBitmap);
    
    DeleteObject(memBitmap);
    DeleteObject(dashPen);  
    DeleteObject(defPen);
    
    DeleteDC(tempDC);
    
    EndPaint(hWnd, &ps);
}

LRESULT EDITOR::onCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId = LOWORD(wParam);
    switch (wmId)
    {
    case ID_ACTIONS_WORK1:
        break;

    case ID_DOT_CHOOSE:
        painter.SetProtoShape(1);
        SetWindowText(hWnd, L"Dot");
        break;

    case ID_LINE_CHOOSE:
        painter.SetProtoShape(2);
        SetWindowText(hWnd, L"Line");
        break;
    
    case ID_RECTANGLE_CHOOSE:
        painter.SetProtoShape(3);
        SetWindowText(hWnd, L"Rectangle");
        break;
    
    case ID_ELLIPSE_CHOOSE:
        painter.SetProtoShape(4);
        SetWindowText(hWnd, L"Ellipse");
        break;

    case IDM_ABOUT:
        DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, AboutDlgProc);
        break;

    case IDM_EXIT:
        DestroyWindow(hWnd);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT EDITOR::onNotify(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    LPNMHDR lpnmhdr = (LPNMHDR)lParam;

    if (lpnmhdr->hwndFrom == hToolbar && lpnmhdr->code == NM_CUSTOMDRAW)
    {
        LPNMTBCUSTOMDRAW lpNMCustomDraw = (LPNMTBCUSTOMDRAW)lParam;
        
        if (lpNMCustomDraw->nmcd.dwDrawStage == CDDS_PREPAINT)
        {
            HBRUSH hBrush = CreateSolidBrush(RGB(220, 230, 242));
            FillRect(lpNMCustomDraw->nmcd.hdc, &lpNMCustomDraw->nmcd.rc, hBrush);
            DeleteObject(hBrush);

            return CDRF_DODEFAULT;
        }
    }

    if (lpnmhdr->code == TTN_NEEDTEXT)
    {
        LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT)lParam;
        switch (lpttt->hdr.idFrom)
        {
        case ID_DOT_CHOOSE:
            lstrcpy(lpttt->szText, L"i also can't see it");
            break;
        case ID_ELLIPSE_CHOOSE:
            lstrcpy(lpttt->szText, L"it should be an ellipse");
            break;
        case ID_LINE_CHOOSE:
            lstrcpy(lpttt->szText, L"possibly a line");
            break;
        case ID_RECTANGLE_CHOOSE:
            lstrcpy(lpttt->szText, L"i think it's rectangle");
            break;
        case IDM_ABOUT:
            lstrcpy(lpttt->szText, L"EEEEEE idk bro");
            break;
        default: 
            lstrcpy(lpttt->szText, L"Something strange");
            break;
        }
    }

    return 0;
}

INT_PTR CALLBACK EDITOR::AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}