#include "framework.h"
#include "resource.h"
#include <string>
#include <commctrl.h>
#include <random>

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

HWND hList = nullptr;

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

    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

    LoadStringW(hInstance, IDS_OBJECT2_TITLE, szTitle, MAX_LOADSTRING);
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

void FillTableWithRandomData(HWND hList, int n, double min, double max)
{
    ListView_DeleteAllItems(hList);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(min, max);

    for (int i = 0; i < n; ++i)
    {
        double randomNum = dis(gen);

        LVITEMW lvItem = { 0 };
        lvItem.mask = LVIF_TEXT;
        lvItem.iItem = i;
        lvItem.iSubItem = 0;
        
        std::wstring indexStr = std::to_wstring(i + 1);
        lvItem.pszText = (LPWSTR)indexStr.c_str();
        
        int rowIndex = ListView_InsertItem(hList, &lvItem);

        std::wstring valueStr = std::to_wstring(randomNum);
        ListView_SetItemText(hList, rowIndex, 1, (LPWSTR)valueStr.c_str());
    }
}

std::wstring PrepareDataForObject3(HWND hList)
{
    std::wstring rawData = L"";
    int rowCount = ListView_GetItemCount(hList);

    for (int i = 0; i < rowCount; ++i)
    {
        wchar_t valueBuf[64] = { 0 };
        ListView_GetItemText(hList, i, 1, valueBuf, 64);
        
        rawData += std::wstring(valueBuf) + L"\n";
    }
    return rawData;
}

void CopyTextToClipboard(HWND hWnd, const std::wstring& text)
{
    if (text.empty()) return;

    if (OpenClipboard(hWnd))
    {
        EmptyClipboard();

        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, (text.size() + 1) * sizeof(wchar_t));
        if (hMem != nullptr)
        {
            void* pMem = GlobalLock(hMem);
            if (pMem != nullptr)
            {
                memcpy(pMem, text.c_str(), (text.size() + 1) * sizeof(wchar_t));
                GlobalUnlock(hMem);
                SetClipboardData(CF_UNICODETEXT, hMem);
            }
        }
        CloseClipboard();
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int n = 10, min = -5.0, max = 5.0;

    switch (message)
    {
        case WM_CREATE:
        {
            HANDLE hReadyEvent = OpenEventW(EVENT_MODIFY_STATE, FALSE, L"Local\\Object2ReadyEvent");
            if (hReadyEvent != NULL) {
                SetEvent(hReadyEvent);
                CloseHandle(hReadyEvent);
            }

            RECT rcClient;
            GetClientRect(hWnd, &rcClient);

            hList = CreateWindowExW(
                0, WC_LISTVIEWW, L"",
                WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_EDITLABELS,
                0, 0, rcClient.right, rcClient.bottom,
                hWnd, (HMENU)1001, hInst, nullptr
            );

            ListView_SetExtendedListViewStyle(hList, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

            LVCOLUMNW lvc = { 0 };
            lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

            lvc.iSubItem = 0;
            lvc.pszText = (LPWSTR)L"№";
            lvc.cx = 80;
            ListView_InsertColumn(hList, 0, &lvc);

            lvc.iSubItem = 1;
            lvc.pszText = (LPWSTR)L"Випадкове значення";
            lvc.cx = 250;
            ListView_InsertColumn(hList, 1, &lvc);
            break;
        }
        case WM_SIZE:
        {
            if (hList != nullptr) {
                RECT rcClient;
                GetClientRect(hWnd, &rcClient);
                MoveWindow(hList, 0, 0, rcClient.right, rcClient.bottom, TRUE);
            }
            break;
        }
        case WM_COPYDATA:
        {
            COPYDATASTRUCT* pcds = (COPYDATASTRUCT*)lParam;
                if (pcds->dwData == 1)
                {
                    wchar_t* receivedStr = (wchar_t*)pcds->lpData;
                    std::wstring data(receivedStr);

                    size_t firstNewLine = data.find(L'\n');
                    size_t secondNewLine = data.find(L'\n', firstNewLine + 1);

                    if (firstNewLine != std::wstring::npos && secondNewLine != std::wstring::npos) {
                        n   = std::stod(data.substr(0, firstNewLine));
                        min = std::stod(data.substr(firstNewLine + 1, secondNewLine - (firstNewLine + 1)));
                        max = std::stod(data.substr(secondNewLine + 1));
                    }
                    
                    if(hList != nullptr) 
                    {
                        FillTableWithRandomData(hList, n, min, max);
                    }

                    std::wstring dataForClipboard = PrepareDataForObject3(hList);
                    CopyTextToClipboard(hWnd, dataForClipboard);
                }
                return TRUE;
        }
        case WM_COMMAND:
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
