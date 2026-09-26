#include "table.h"
#include "editor.h"
#include "resource.h"

static HWND hTableDlg = NULL;

static INT_PTR CALLBACK TableBox(HWND hTb, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTER& painter = PAINTER::GetInstance();
    const auto& shapes = painter.GetShapes();
    
	switch(message)
	{
		case WM_INITDIALOG:
            {
                initTableColumns(hTb);
                updateTableWindow();
                return (INT_PTR)TRUE;
            }
            break;   
        case WM_NOTIFY:
            {
                LPNMHDR lpnmhdr = (LPNMHDR)lParam;

                if (lpnmhdr->idFrom == IDC_TABLE_LIST && lpnmhdr->code == NM_CLICK)
                {
                    LPNMITEMACTIVATE pnmItem = (LPNMITEMACTIVATE)lParam;
                    int rowIndex = pnmItem->iItem;
                    int colIndex = pnmItem->iSubItem;

                    painter.RemoveShapeByIndex(rowIndex);
                    updateTableWindow();
                    
                    HWND hMainWnd = GetParent(hTb);
                    InvalidateRect(hMainWnd, NULL, TRUE);
                    
                    return (INT_PTR)TRUE;
                }
            }
            break;
		case WM_CLOSE:
            DestroyWindow(hTb);
            hTableDlg = NULL;
			return TRUE;
	}

	return FALSE;
}

void updateTableWindow()
{
    if (hTableDlg == NULL)
        return;

    PAINTER& painter = PAINTER::GetInstance();
    const auto& shapes = painter.GetShapes();

    WCHAR textBuffer[64];
    wsprintfW(textBuffer, L"Shapes count: %d", shapes.size());
    SetDlgItemTextW(hTableDlg, IDC_SHAPES_COUNT_TEXT, textBuffer);

    HWND hListView = GetDlgItem(hTableDlg, IDC_TABLE_LIST);
    if (hListView == NULL)
        return;

    SendMessage(hListView, WM_SETREDRAW, FALSE, 0);
    ListView_DeleteAllItems(hListView);

    LVITEMW lvi = { 0 };
    WCHAR buf[32];

    for (size_t i = 0; i < shapes.size(); ++i)
    {
        if (shapes[i] == nullptr) continue;

        wsprintfW(buf, L"%d", i + 1);
        lvi.mask = LVIF_TEXT;
        lvi.iItem = i;
        lvi.iSubItem = 0;
        lvi.pszText = buf;
        ListView_InsertItem(hListView, &lvi);

        ListView_SetItemText(hListView, i, 1, (LPWSTR)shapes[i]->GetName());

        wsprintfW(buf, L"%d", shapes[i]->GetX1());
        ListView_SetItemText(hListView, i, 2, buf);

        wsprintfW(buf, L"%d", shapes[i]->GetY1());
        ListView_SetItemText(hListView, i, 3, buf);

        wsprintfW(buf, L"%d", shapes[i]->GetX2());
        ListView_SetItemText(hListView, i, 4, buf);

        wsprintfW(buf, L"%d", shapes[i]->GetY2());
        ListView_SetItemText(hListView, i, 5, buf);
    }

    SendMessage(hListView, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(hListView, NULL, TRUE);
}

void initTableColumns(HWND hTb)
{
    HWND hListView = GetDlgItem(hTb, IDC_TABLE_LIST);
    if (!hListView) return;

    ListView_SetExtendedListViewStyle(hListView, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    LVCOLUMNW lvc = { 0 };
    lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    const wchar_t* headers[] = { L"№", L"Shape", L"X1", L"Y1", L"X2", L"Y2" };
    int widths[] = { 30, 80, 45, 45, 45, 45 };

    for (int i = 0; i < 6; i++)
    {
        lvc.iSubItem = i;
        lvc.pszText = (LPWSTR)headers[i];
        lvc.cx = widths[i];
        ListView_InsertColumn(hListView, i, &lvc);
    }
}

HWND tableBoxFunc (HWND hWnd)
{
	if (hTableDlg != NULL)
    {
        SetFocus(hTableDlg);
        return hTableDlg;
    }
	HINSTANCE hInstance = GetModuleHandle(NULL);

	hTableDlg = CreateDialogW(hInstance, MAKEINTRESOURCEW(IDD_TABLEBOX), hWnd, TableBox);

	if (hTableDlg != NULL)
    {
        ShowWindow(hTableDlg, SW_SHOW);
        UpdateWindow(hTableDlg);
    }

    return hTableDlg;
}