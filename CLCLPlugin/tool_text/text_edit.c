/*
 * tool_text
 *
 * to_lower.c
 *
 * Copyright (C) 1996-2003 by Nakashima Tomoaki. All rights reserved.
 *		http://www.nakka.com/
 *		nakka@nakka.com
 */



/* Include Files */
#define _INC_OLE
#include <windows.h>
#undef  _INC_OLE

#include "..\CLCLPlugin.h"

#include "resource.h"
#include "Font.h"

/* Define */
#define ID_SELECT_TIMER					1

/* Global Variables */
extern HINSTANCE hInst;
extern TCHAR ini_path[];
extern TCHAR text_edit_font_name[BUF_SIZE];
extern int text_edit_font_size;
extern int text_edit_font_weight;
extern int text_edit_font_italic;
extern int text_edit_font_charset;



/* Local Function Prototypes */

static HFONT list_font = NULL;

static RECT rcPOS_E;


/*
 * set_edit_proc - テキスト編集プロシージャ
 */
BOOL CALLBACK set_edit_proc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	RECT window_rect;
	DATA_INFO *di;
	BYTE *mem;
	int len;

	switch (uMsg) {
	case WM_INITDIALOG:

		if (*text_edit_font_name != TEXT('\0')) {
			if (list_font != NULL) {				
				DeleteObject(list_font);
			}
			list_font = font_create(text_edit_font_name, text_edit_font_size, text_edit_font_charset,
				text_edit_font_weight, (text_edit_font_italic == 0) ? FALSE : TRUE, FALSE);
			if (list_font != NULL) {				
				SendDlgItemMessage(hDlg, IDC_EDIT_TEXT, WM_SETFONT, (WPARAM)list_font, MAKELPARAM(FALSE, 0));						
			}
			else {				
				SendDlgItemMessage(hDlg, IDC_EDIT_TEXT, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0));		
			}
		}
		else {			
			SendDlgItemMessage(hDlg, IDC_EDIT_TEXT, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0));			
		}
					
		di = (DATA_INFO *)lParam;
		// メモリのロック
		if ((mem = GlobalLock(di->data)) != NULL) {
			SendDlgItemMessage(hDlg, IDC_EDIT_TEXT, WM_SETTEXT, 0, (LPARAM)mem);
			GlobalUnlock(di->data);
		}
		SetWindowLong(hDlg, GWL_USERDATA, lParam);
		SetTimer(hDlg, ID_SELECT_TIMER, 1, NULL);

		if (!(rcPOS_E.left == 0 && rcPOS_E.bottom == 0 && rcPOS_E.right == 0 && rcPOS_E.top == 0)) {
			SetWindowPos(hDlg, HWND_TOP, rcPOS_E.left, rcPOS_E.top, rcPOS_E.right-rcPOS_E.left+1, rcPOS_E.bottom-rcPOS_E.top+1, 0);
		}
		break;

	case WM_SIZE:		
		GetWindowRect(hDlg, (LPRECT)&window_rect);
		SetWindowPos(GetDlgItem(hDlg, IDC_EDIT_TEXT), 0, 0, 0,
			(window_rect.right - window_rect.left) - (GetSystemMetrics(SM_CXFRAME) * 2),
			(window_rect.bottom - window_rect.top) - GetSystemMetrics(SM_CYSIZE) - (GetSystemMetrics(SM_CXFRAME) * 2) - 35,
			SWP_NOZORDER);
		SetWindowPos(GetDlgItem(hDlg, IDOK), 0,
			(window_rect.right - window_rect.left) - (GetSystemMetrics(SM_CXFRAME) * 2) - 220,
			(window_rect.bottom - window_rect.top) - GetSystemMetrics(SM_CYSIZE) - (GetSystemMetrics(SM_CXFRAME) * 2) - 30,
			0, 0, SWP_NOZORDER | SWP_NOSIZE);
		SetWindowPos(GetDlgItem(hDlg, IDCANCEL), 0,
			(window_rect.right - window_rect.left) - (GetSystemMetrics(SM_CXFRAME) * 2) - 100,
			(window_rect.bottom - window_rect.top) - GetSystemMetrics(SM_CYSIZE) - (GetSystemMetrics(SM_CXFRAME) * 2) - 30,
			0, 0, SWP_NOZORDER | SWP_NOSIZE);
		InvalidateRect(GetDlgItem(hDlg, IDOK), NULL, FALSE);
		UpdateWindow(GetDlgItem(hDlg, IDOK));
		InvalidateRect(GetDlgItem(hDlg, IDCANCEL), NULL, FALSE);
		UpdateWindow(GetDlgItem(hDlg, IDCANCEL));
		
		break;

	case WM_CLOSE:
		GetWindowRect(hDlg, &rcPOS_E);
		EndDialog(hDlg, FALSE);
		break;

	case WM_TIMER:
		// タイマー
		switch (wParam) {
		case ID_SELECT_TIMER:
			KillTimer(hDlg, wParam);
			SendDlgItemMessage(hDlg, IDC_EDIT_TEXT, EM_SETSEL, 0, 0);
			break;
		}
		break;

	case WM_COMMAND:
		
		switch (LOWORD(wParam)) {
		case IDOK:
			GetWindowRect(hDlg, &rcPOS_E);

			di = (DATA_INFO *)GetWindowLong(hDlg, GWL_USERDATA);
			if (di == NULL) {
				EndDialog(hDlg, FALSE);
				break;
			}
			GlobalFree(di->data);

			len = SendDlgItemMessage(hDlg, IDC_EDIT_TEXT, WM_GETTEXTLENGTH, 0, 0) + 1;
			di->data = GlobalAlloc(GPTR, sizeof(TCHAR) * len);
			if (di->data != NULL) {
				if ((mem = GlobalLock(di->data)) != NULL) {
					*mem = TEXT('\0');
					SendDlgItemMessage(hDlg, IDC_EDIT_TEXT, WM_GETTEXT, len, (LPARAM)mem);
					GlobalUnlock(di->data);
					di->size = sizeof(TCHAR) * len;
				}
			}
			EndDialog(hDlg, TRUE);
			break;

		case IDCANCEL:
			GetWindowRect(hDlg, &rcPOS_E);
			EndDialog(hDlg, FALSE);
			break;
		}
		break;

	default:
		return FALSE;
	}
	return TRUE;
}

/*
 * text_edit - テキストを小文字に変換
 */
__declspec(dllexport) int CALLBACK text_edit(const HWND hWnd, TOOL_EXEC_INFO *tei, TOOL_DATA_INFO *tdi)
{
	DATA_INFO *di;

	if (tdi == NULL) {
		return TOOL_SUCCEED;
	}

	// 形式選択
#ifdef UNICODE
	di = (DATA_INFO *)SendMessage(hWnd, WM_ITEM_GET_FORMAT_TO_ITEM, (WPARAM)TEXT("UNICODE TEXT"), (LPARAM)tdi->di);
#else
	di = (DATA_INFO *)SendMessage(hWnd, WM_ITEM_GET_FORMAT_TO_ITEM, (WPARAM)TEXT("TEXT"), (LPARAM)tdi->di);
#endif
	if (di == NULL || di->data == NULL) {
		return TOOL_SUCCEED;
	}

	if (DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOG_TEXT_EDIT), GetForegroundWindow(), set_edit_proc, (LPARAM)di) == FALSE) {
		return TOOL_CANCEL;
	}
	return TOOL_DATA_MODIFIED;
}



// 2023-02-16 kaiu@mail.ru NOT DISPLAYED IN DLL (look at the file .def)
/*
 * text_edit_property - プロパティ表示
 */
__declspec(dllexport) BOOL CALLBACK text_edit_property(const HWND hWnd, TOOL_EXEC_INFO *tei)
{
	return FALSE;
}
/* End of source */
