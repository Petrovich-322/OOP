#include "framework.h"
#include "resource.h"
#include <string>
#include <commctrl.h>
#include <vector>
#include <sstream>
#include <cmath>

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

HWND hList = nullptr;

std::vector<double> ReadDataFromClipboard();

std::vector<double> data;


ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    data = ReadDataFromClipboard();

    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

    LoadStringW(hInstance, IDS_OBJECT3_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB1));

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = nullptr;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIcon          = LoadIcon(nullptr, IDI_APPLICATION);
    wcex.hIconSm        = LoadIcon(nullptr, IDI_APPLICATION);

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance;

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
      CW_USEDEFAULT, 0, 600, 400, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return 1;
}

std::vector<double> ReadDataFromClipboard()
{
    std::vector<double> data;
    
    if (OpenClipboard(nullptr))
    {
        HANDLE hMem = GetClipboardData(CF_UNICODETEXT);
        if (hMem != nullptr)
        {
            wchar_t* pText = (wchar_t*)GlobalLock(hMem);
            if (pText != nullptr)
            {
                std::wstring text(pText);
                GlobalUnlock(hMem); 

                std::wstringstream wss(text);
                std::wstring line;
                while (std::getline(wss, line, L'\n'))
                {
                    if (!line.empty())
                    {
                        try {
                            data.push_back(std::stod(line));
                        }
                        catch (...) {}
                    }
                }
            }
        }
        CloseClipboard();
    }
    return data;
}

void DrawData(HDC hdc, RECT& rect, const std::vector<double>& data)
{
    if(data.empty()) return;

    int margin = 50;
    int width = (rect.right - rect.left) - 2 * margin;
    int height = (rect.bottom - rect.top) - 2 * margin;

    double minY = data[0];
    double maxY = data[0];
    for (double val : data) {
        if (val < minY) minY = val;
        if (val > maxY) maxY = val;
    }

    HPEN hPenAxis = CreatePen(PS_SOLID, 2, RGB(100, 100, 100));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPenAxis);

    MoveToEx(hdc, margin, margin, nullptr);
    LineTo(hdc, margin, rect.bottom - margin);

    MoveToEx(hdc, margin, rect.bottom - margin, nullptr);
    LineTo(hdc, rect.right - margin, rect.bottom - margin);

    SelectObject(hdc, hOldPen);
    DeleteObject(hPenAxis);

    size_t n = data.size();
    
    HPEN hPenGraph = CreatePen(PS_SOLID, 2, RGB(0, 120, 215));
    SelectObject(hdc, hPenGraph);

    for (size_t i = 0; i < n; ++i)
    {
        double xNorm = (double)i / (n - 1);
        double yNorm = (data[i] - minY) / (maxY - minY);

        int screenX = margin + (int)(xNorm * width);
        int screenY = (rect.bottom - margin) - (int)(yNorm * height);

        Ellipse(hdc, screenX - 3, screenY - 3, screenX + 3, screenY + 3);

        if (i == 0) {
            MoveToEx(hdc, screenX, screenY, nullptr);
        } else {
            LineTo(hdc, screenX, screenY);
        }
    }

    DeleteObject(hPenGraph);
    
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_CREATE:
            break;
        case WM_COPYDATA:
        {
            COPYDATASTRUCT* pcds = (COPYDATASTRUCT*)lParam;
                if (pcds->dwData == 2)
                {
                    data = ReadDataFromClipboard();
                    InvalidateRect(hWnd, nullptr, TRUE);
                }
                return TRUE;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            RECT rect;
            GetClientRect(hWnd, &rect);

            DrawData(hdc, rect, data);

            EndPaint(hWnd, &ps);
            break;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
