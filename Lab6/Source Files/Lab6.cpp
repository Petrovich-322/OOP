#include "framework.h"
#include "resource.h"
#include <string>

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

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

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
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
    wcex.lpszMenuName   = NULL;
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

void LaunchProcesses(const wchar_t processName[]) {
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    CreateProcess(processName, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void SendDataToObject(HWND hWndTarget, const std::wstring& data, int id = 1) {
    COPYDATASTRUCT cds;
    
    if (!IsWindow(hWndTarget)) 
    {
        return;
    }
    
    cds.dwData = id;
    cds.cbData = (data.size() + 1) * sizeof(wchar_t);
    cds.lpData = (void*)data.c_str();

    SendMessage(hWndTarget, WM_COPYDATA, (WPARAM)NULL, (LPARAM)&cds);
}

INT_PTR CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    double n = 10, min = -5.0, max = 5.0;

    switch (message) {  
        case WM_INITDIALOG:
            SetDlgItemText(hDlg, 1001, _T("10"));
            SetDlgItemText(hDlg, 1002, _T("-10"));
            SetDlgItemText(hDlg, 1003, _T("10"));
            return TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == ID_BTN_OK) {
                TCHAR buf[32];
                
                GetDlgItemText(hDlg, 1001, buf, 32); 
                n = _tstof(buf);
                
                GetDlgItemText(hDlg, 1002, buf, 32); 
                min = _tstof(buf);
                
                GetDlgItemText(hDlg, 1003, buf, 32); 
                max = _tstof(buf);

                EndDialog(hDlg, ID_BTN_OK);
                
                HWND hWndObject2 = FindWindow(NULL, L"Object2");
                HWND hWndObject3 = FindWindow(NULL, L"Object3");

                if (hWndObject2 != NULL) {
                    SendDataToObject(hWndObject2, std::to_wstring(n) + L"\n" + std::to_wstring(min) + L"\n" + std::to_wstring(max));
                }
                else 
                {
                    HANDLE hReadyEvent = CreateEventW(NULL, TRUE, FALSE, L"Local\\Object2ReadyEvent");
                    ResetEvent(hReadyEvent);
                    LaunchProcesses(L"Object2.exe");
                    DWORD waitResult = WaitForSingleObject(hReadyEvent, 1000);

                    if (waitResult == WAIT_OBJECT_0) {
                        hWndObject2 = FindWindow(NULL, L"Object2");
                        SendDataToObject(hWndObject2, std::to_wstring(n) + L"\n" + std::to_wstring(min) + L"\n" + std::to_wstring(max));
                    }
                }

                Sleep(100);
                
                if(hWndObject3 == NULL) {
                    LaunchProcesses(L"Object3.exe");
                }
                else 
                {
                    SendDataToObject(hWndObject3, L"", 2);
                }
                return TRUE;
            }
            if (LOWORD(wParam) == ID_BTN_CANCEL) {
                EndDialog(hDlg, ID_BTN_CANCEL);
                return TRUE;
            }
            break;
        case WM_CLOSE:
            EndDialog(hDlg, 0);
            return TRUE;
        }
    return FALSE;
}

HWND hButton = NULL;
int btnWidth = 120;
int btnHeight = 35;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    RECT rect;
    GetClientRect(hWnd, &rect);
    
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    switch (message)
    {
        case WM_CREATE:
            hButton = CreateWindowEx(
                0, L"BUTTON", L"Start",
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                width/2 - btnWidth/2, height/2 - btnHeight/2, btnWidth, btnHeight,
                hWnd, (HMENU)IDM_START,
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL
            );
            break;
        case WM_SIZE: 
        {
            if (hButton != NULL) {
                int btnX = (width - btnWidth) / 2;
                int btnY = (height - btnHeight) / 2;

                MoveWindow(hButton, btnX, btnY, btnWidth, btnHeight, TRUE);
            }
            break;
        }
        case WM_COMMAND:
            {
                int wmId = LOWORD(wParam);
                switch (wmId)
                {
                    case IDM_START:
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_PARAMS_DIALOG), hWnd, DlgProc);
                        break;
                    default:
                        return DefWindowProc(hWnd, message, wParam, lParam);
                }
            }
            break;
        case WM_DESTROY:
        {   
            HWND hWndObject2 = FindWindow(NULL, L"Object2");
            if(hWndObject2 != NULL) {
                SendMessage(hWndObject2, WM_CLOSE, 0, 0);
            }
            
            HWND hWndObject3 = FindWindow(NULL, L"Object3");
            if(hWndObject3 != NULL) {
                SendMessage(hWndObject3, WM_CLOSE, 0, 0);
            }
            
            PostQuitMessage(0);
        }
        break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
