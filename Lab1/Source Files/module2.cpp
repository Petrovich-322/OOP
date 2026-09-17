#include "module2_resource.h"

static INT_PTR CALLBACK DialogMOD2(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static TCHAR* text = NULL;

	switch (message)
	{
		case WM_INITDIALOG:
			text = (TCHAR*)lParam;
			return 1;
		case WM_COMMAND: 
		{
			int wmId = LOWORD(wParam);
			switch (wmId)
			{
				case IDOK:
				{
					GetDlgItemText(hDlg, IDC_EDIT1, text, 256);
					EndDialog(hDlg, 1);
					return 1;
				}
				case IDCANCEL: 
					EndDialog(hDlg, 1);
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

int Func_MOD2(HWND hWnd, TCHAR* text)
{
	HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);

	INT_PTR res = DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG2), hWnd, DialogMOD2, (LPARAM)text);
	
	return res;
}