#include "module1_resource.h"

static INT_PTR CALLBACK DialogMOD1(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static TCHAR *text = NULL;
    switch (message)
    {
        case WM_INITDIALOG:
        {
            text = (TCHAR*)lParam;

            const TCHAR* groups[] = {
                _T("ІМ-51"),
                _T("ІМ-52"),
                _T("ІМ-53"),
                _T("ІМ-54"),
                _T("ІМ-55"),
                _T("ІМ-o51")
            };

            for (const TCHAR* group: groups)
            {
                SendDlgItemMessage(hDlg, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)group);
            }
                
            return 1;
        }
        case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);

            switch (wmId)
            {
                case IDOK:
                {
                    LRESULT index = SendDlgItemMessage(hDlg, IDC_LIST1, LB_GETCURSEL, 0, 0);

                    if (index == -1)
                    {
                        EndDialog(hDlg, 0);
                        return 1;
                        break;
                    }

                    SendDlgItemMessage(hDlg, IDC_LIST1, LB_GETTEXT, (WPARAM)index, (LPARAM)text);
                    EndDialog(hDlg, 1);
                    break;
                }
                case IDCANCEL:
                    EndDialog(hDlg, 1);
                    return 0;
                default:
                    break;
            }
            break;
        }
        case WM_CLOSE:
            EndDialog(hDlg, 1);
            return 0;
    }

    return 0;
}

int Func_MOD1 (HWND hWnd, TCHAR* text)
{
	HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);

	INT_PTR res = DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, DialogMOD1, (LPARAM)text);

    return res;
}