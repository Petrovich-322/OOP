#include "framework.h"
#include "main_resource.h"

#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// static TCHAR text[128] = _T("");

void work1(HWND hWnd);
void startDrawing(HWND hWnd, int startX, int startY);
void endDrawing(HWND hWnd, int endX, int endY);
void tempDrawing(HWND hWnd, int endX, int endY);

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
    
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_BAR_CLASSES;
    InitCommonControlsEx(&icex);
    
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
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LAB1);
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

class SHAPE
{
    protected: 
        int x1, y1, x2, y2;
    public:
        void Set(int x1, int y1, int x2, int y2)
        {
            this->x1 = x1;
            this->y1 = y1;
            this->x2 = x2;
            this->y2 = y2;
        }

        virtual ~SHAPE() {}
        virtual SHAPE* Clone() = 0;
        virtual void Show(HDC hdc, HPEN hPen) {};
};

class LINE: public SHAPE
{
    public:
        void Show(HDC hdc, HPEN hPen) override
        {
            HPEN oldPen = (HPEN)SelectObject(hdc, hPen);
            int oldBkMode = SetBkMode(hdc, TRANSPARENT);

            MoveToEx(hdc, x1, y1, NULL);
            LineTo(hdc, x2, y2);

            SelectObject(hdc, oldPen);
            SetBkMode(hdc, oldBkMode);
        }

        SHAPE* Clone() override 
        {
            return new LINE();
        }
};

class DOT: public SHAPE
{
    private:
        const int radius = 3;
    public:
        void Show(HDC hdc, HPEN hPen) override
        {
            SetPixel(hdc, x1, y1, RGB(0, 0, 0));
        }

        SHAPE* Clone() override 
        {
            return new DOT();
        }
};

class RECTANGLE: public SHAPE
{    
    public:
        void Show(HDC hdc, HPEN hPen) override
        {
            HBRUSH orangeBrush = CreateSolidBrush(RGB(255, 128, 0));
            HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, orangeBrush); 
            HPEN oldPen = (HPEN)SelectObject(hdc, hPen);
            
            Rectangle(hdc, x1, y1, x2, y2);

            SelectObject(hdc, oldPen);
            SelectObject(hdc, oldBrush);
            DeleteObject(orangeBrush);
        }

        SHAPE* Clone() override 
        {
            return new RECTANGLE();
        }
};

class ELLIPSE: public SHAPE
{
    public:
        void Show(HDC hdc, HPEN hPen) override
        {
            HPEN oldPen = (HPEN)SelectObject(hdc, hPen);

            const int dx = abs(x1 - x2), dy = abs(y1 - y2);

            Ellipse(hdc, x1 - dx, y1 - dy, x1 + dx, y1 + dy);

            SelectObject(hdc, oldPen);
        }

        SHAPE* Clone() override 
        {
            return new ELLIPSE();
        }
};

static int startX = 0, startY = 0, shapeCount = 0;
static bool isDrawing = FALSE;

static LINE protoLine;
static RECTANGLE protoRectangle;
static DOT protoDot;
static ELLIPSE protoEllipse;

static SHAPE* shapes[MAX_SHAPES];
static SHAPE* protoShape = &protoDot;
static SHAPE* shape = nullptr;

static HWND hToolbar = NULL;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    
    case WM_CREATE:
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
        break;
    case WM_NOTIFY:
        {
            LPNMHDR lpnmhdr = (LPNMHDR)lParam;

            if(lpnmhdr->hwndFrom == hToolbar && lpnmhdr->code == NM_CUSTOMDRAW)
            {
                LPNMTBCUSTOMDRAW lpNMCustomDraw = (LPNMTBCUSTOMDRAW)lParam;
                
                if(lpNMCustomDraw->nmcd.dwDrawStage == CDDS_PREPAINT)
                {
                    HBRUSH hBrush = CreateSolidBrush(RGB(220, 230, 242));
                    
                    FillRect(lpNMCustomDraw->nmcd.hdc, &lpNMCustomDraw->nmcd.rc, hBrush);
                
                    DeleteObject(hBrush);

                    return CDRF_DODEFAULT;
                }
            }
        }
        break;
        
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
            
            case ID_ACTIONS_WORK1:
                work1(hWnd);
                break;
        
            case ID_LINE_CHOOSE:
                protoShape = &protoLine;
                SetWindowText(hWnd, L"Line");
                break;
            
            case ID_DOT_CHOOSE:
                protoShape = &protoDot;
                SetWindowText(hWnd, L"Dot");
                break;
            
            case ID_RECTANGLE_CHOOSE:
                protoShape = &protoRectangle;
                SetWindowText(hWnd, L"Rectangle");
                break;
            
            case ID_ELLIPSE_CHOOSE:
                protoShape = &protoEllipse;
                SetWindowText(hWnd, L"Ellipse");
                break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
                break;
            }
        }
        break;
    case WM_PAINT:
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

            for (int i = 0; i < shapeCount; i++) {
                shapes[i]->Show(tempDC, defPen);
            }

            if(shape != nullptr)
            {
                shape->Show(tempDC, defPen);
            }

            BitBlt(hdc, 0, 0, width, height, tempDC, 0, 0, SRCCOPY);
            
            SelectObject(tempDC, oldBitmap);
            
            DeleteObject(memBitmap);
            DeleteObject(dashPen);  
            DeleteObject(defPen);
            
            DeleteDC(tempDC);
            
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_LBUTTONDOWN:
        {
            startX = GET_X_LPARAM(lParam);
            startY = GET_Y_LPARAM(lParam);
            
            startDrawing(hWnd, startX, startY);
        }
        break;
    case WM_MOUSEMOVE:
        {
            int endX = GET_X_LPARAM(lParam);
            int endY = GET_Y_LPARAM(lParam);
            
            tempDrawing(hWnd, endX, endY);
        }
        break;
    case WM_LBUTTONUP:
        {
            int endX = GET_X_LPARAM(lParam);
            int endY = GET_Y_LPARAM(lParam);
            
            endDrawing(hWnd, endX, endY);
        }
        break;

    case WM_DESTROY:
        
        for (int i = 0; i < shapeCount; i++) {
            delete shapes[i];
            shapes[i] = nullptr;
        }
        
        shapeCount = 0;
        
        if (shape != nullptr) {
            delete shape;
            shape = nullptr;
        }
        
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void work1(HWND hWnd)
{
    InvalidateRect(hWnd, NULL, TRUE);
    return;
}

void startDrawing(HWND hWnd, int startX, int startY)
{
    shape = protoShape->Clone();
    shape->Set(startX, startY, startX, startY);
    
    isDrawing = TRUE;
}

void tempDrawing(HWND hWnd, int endX, int endY)
{
    if (isDrawing && shape != nullptr)
    {   
        shape->Set(startX, startY, endX, endY);
            
        InvalidateRect(hWnd, NULL, FALSE);
    }
}

void endDrawing(HWND hWnd, int endX, int endY) 
{
    if(isDrawing)
        {
            if(shapeCount < MAX_SHAPES)
            {
                SHAPE* finalShape = protoShape->Clone();
                finalShape->Set(startX, startY, endX, endY);
                shapes[shapeCount++] = finalShape;
            }

            if (shape != nullptr) {
                delete shape;
                shape = nullptr;
            }

            isDrawing = FALSE;
            InvalidateRect(hWnd, NULL, TRUE);
        }
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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