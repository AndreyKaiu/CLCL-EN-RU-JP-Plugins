/*
 * tool_text
 *
 * quote.c
 *
 * Copyright (C) 1996-2003 by Nakashima Tomoaki. All rights reserved.
 *		http://www.nakka.com/
 *		nakka@nakka.com
 */



/* Include Files */
#define _INC_OLE
#include <windows.h>
#undef  _INC_OLE

#ifndef COUNT_CB 
#define COUNT_CB		25
#endif

#include "..\CLCLPlugin.h"
#include "File.h"
#include "Memory.h"
#include "Profile.h"
#include "resource.h"

/* Define */

/* Global Variables */
extern HINSTANCE hInst;
extern TCHAR ini_path[];

extern TCHAR quote_char[];

extern int op_count_cbQ;
extern TCHAR op_items_cbQ[COUNT_CB][1025];


static RECT rcPOS_Q;
BOOL GlobalParr; // global parameter


/* Local Function Prototypes */

/*
 * set_quote_proc - 引用符を設定
 */
BOOL CALLBACK set_quote_proc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{	
	static TOOL_EXEC_INFO *tei;

	switch (uMsg) {
	case WM_INITDIALOG:
		tei = (TOOL_EXEC_INFO*)lParam;
		if (tei == NULL || tei->cmd_line == NULL) GlobalParr = TRUE;

		SendDlgItemMessage(hDlg, IDC_COMBO_QUOTE_CHAR, (UINT)CB_RESETCONTENT, 0, 0);

		if (GlobalParr)
			SendDlgItemMessage(hDlg, IDC_COMBO_QUOTE_CHAR, WM_SETTEXT, 0, (LPARAM)quote_char);
		else
			SendDlgItemMessage(hDlg, IDC_COMBO_QUOTE_CHAR, WM_SETTEXT, 0, (LPARAM)tei->cmd_line);
		
		for (int i = 0; i < op_count_cbQ && i < COUNT_CB; ++i) {
			SendDlgItemMessage(hDlg, IDC_COMBO_QUOTE_CHAR, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)op_items_cbQ[i]);
		}

		if (!(rcPOS_Q.left == 0 && rcPOS_Q.bottom == 0 && rcPOS_Q.right == 0 && rcPOS_Q.top == 0)) {
			SetWindowPos(hDlg, HWND_TOP, rcPOS_Q.left, rcPOS_Q.top, 0, 0, SWP_NOSIZE);
		}

		break;

	case WM_CLOSE:
		GetWindowRect(hDlg, &rcPOS_Q);
		EndDialog(hDlg, FALSE);
		break;

	case WM_COMMAND:
		
		switch (LOWORD(wParam)) {
		case IDOK:
			GetWindowRect(hDlg, &rcPOS_Q);

			TCHAR txt[1025];
			int idxsel = SendDlgItemMessage(hDlg, IDC_COMBO_QUOTE_CHAR, CB_GETCURSEL, 0, 0);
			if (idxsel != CB_ERR) {
				SendDlgItemMessage(hDlg, IDC_COMBO_QUOTE_CHAR, CB_GETLBTEXT, (WPARAM)idxsel, (LPARAM)txt);
			}
			else
				SendDlgItemMessage(hDlg, IDC_COMBO_QUOTE_CHAR, WM_GETTEXT, (WPARAM)(BUF_SIZE - 1), (LPARAM)txt);


			if (*txt != TEXT('\0'))
			{
				int cnt = SendDlgItemMessage(hDlg, IDC_COMBO_QUOTE_CHAR, (UINT)CB_GETCOUNT, (WPARAM)0, (LPARAM)0);
				int f = -1;
				TCHAR ftxt[1025];
				for (int i = 0; i < cnt; ++i) {
					int lstr = SendDlgItemMessage(hDlg, IDC_COMBO_QUOTE_CHAR, CB_GETLBTEXTLEN, (WPARAM)i, (LPARAM)0);
					if (lstr < 1025) {
						SendDlgItemMessage(hDlg, IDC_COMBO_QUOTE_CHAR, CB_GETLBTEXT, (WPARAM)i, (LPARAM)ftxt);
						if (lstrcmp(txt, ftxt) == 0) {
							f = i;
							if (i > 0) {
								SendDlgItemMessage(hDlg, IDC_COMBO_QUOTE_CHAR, (UINT)CB_DELETESTRING, (WPARAM)i, (LPARAM)0); // del
								SendDlgItemMessage(hDlg, IDC_COMBO_QUOTE_CHAR, (UINT)CB_INSERTSTRING, (WPARAM)0, (LPARAM)txt); // insert 0
								SendDlgItemMessage(hDlg, IDC_COMBO_QUOTE_CHAR, (UINT)CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
							}
							break;
						}
					}
				}

				if (f == -1) {
					SendDlgItemMessage(hDlg, IDC_COMBO_QUOTE_CHAR, (UINT)CB_INSERTSTRING, (WPARAM)0, (LPARAM)txt); // insert 0
					SendDlgItemMessage(hDlg, IDC_COMBO_QUOTE_CHAR, (UINT)CB_DELETESTRING, (WPARAM)COUNT_CB, (LPARAM)0); // only COUNT_CB str
					SendDlgItemMessage(hDlg, IDC_COMBO_QUOTE_CHAR, (UINT)CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
				}

				op_count_cbQ = SendDlgItemMessage(hDlg, IDC_COMBO_QUOTE_CHAR, (UINT)CB_GETCOUNT, (WPARAM)0, (LPARAM)0);
			}

					
			
#ifdef UNICODE
			// For unicode to create a file if it does not exist
			if (!file_check_file(ini_path))
			{
				BYTE bt[2]; bt[0] = 0xFF; bt[1] = 0xFE; // UTF-16 LE BOM				
				file_write_buf(ini_path, bt, 2, NULL);
			}
#endif
			profile_initialize(ini_path, TRUE);
			TCHAR buf[1025];			
			profile_write_int(TEXT("quote"), TEXT("count_cbQ"), op_count_cbQ, ini_path);
			TCHAR itemname[256];
			for (int i = 0; i < op_count_cbQ && i < COUNT_CB; ++i) {
				wsprintf(itemname, TEXT("item_cbQ_%d"), i);
				SendDlgItemMessage(hDlg, IDC_COMBO_QUOTE_CHAR, (UINT)CB_GETLBTEXT, (WPARAM)i, (LPARAM)op_items_cbQ[i]);
				profile_write_string(TEXT("quote"), itemname, op_items_cbQ[i], ini_path);
			}
			


			if (GlobalParr) {
				idxsel = SendDlgItemMessage(hDlg, IDC_COMBO_QUOTE_CHAR, CB_GETCURSEL, 0, 0);
				if (idxsel != CB_ERR) {
					SendDlgItemMessage(hDlg, IDC_COMBO_QUOTE_CHAR, (UINT)CB_GETLBTEXT, (WPARAM)idxsel, (LPARAM)quote_char);
				}
				else
					SendDlgItemMessage(hDlg, IDC_COMBO_QUOTE_CHAR, (UINT)WM_GETTEXT, (WPARAM)(BUF_SIZE - 1), (LPARAM)quote_char);
						
				
				profile_write_string(TEXT("quote"), TEXT("char"), quote_char, ini_path);
			}
			else {									
				idxsel = SendDlgItemMessage(hDlg, IDC_COMBO_QUOTE_CHAR, CB_GETCURSEL, 0, 0);
				if (idxsel != CB_ERR) {
					SendDlgItemMessage(hDlg, IDC_COMBO_QUOTE_CHAR, (UINT)CB_GETLBTEXT, (WPARAM)idxsel, (LPARAM)tei->cmd_line);
				}
				else
					SendDlgItemMessage(hDlg, IDC_COMBO_QUOTE_CHAR, (UINT)WM_GETTEXT, (WPARAM)(BUF_SIZE - 1), (LPARAM)tei->cmd_line);
			}
			
			profile_flush(ini_path); profile_free();
			

			EndDialog(hDlg, TRUE);
			break;

		case IDCANCEL:
			GetWindowRect(hDlg, &rcPOS_Q);
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
 * item_quote - テキストに引用符を付加
 */
static int item_quote(DATA_INFO *di, const TCHAR *q_char)
{
	HANDLE ret;
	BYTE *from_mem, *to_mem;
	TCHAR *p, *r;
	int size;

	// コピー元ロック
	if ((from_mem = GlobalLock(di->data)) == NULL) {
		return TOOL_ERROR;
	}

	// 引用符の付加したサイズを取得
	size = lstrlen(q_char);
	for (p = (TCHAR *)from_mem; *p != TEXT('\0'); p++) {
#ifndef UNICODE
		if (IsDBCSLeadByte((BYTE)*p) == TRUE) {
			p++;
			size += 2;
			continue;
		}
#endif
		size++;
		if (*p == TEXT('\n') && *(p + 1) != TEXT('\0')) {
			size += lstrlen(q_char);
		}
	}
	size++;

	// コピー先確保
	if ((ret = GlobalAlloc(GHND, sizeof(TCHAR) * size)) == NULL) {
		GlobalUnlock(di->data);
		return TOOL_ERROR;
	}
	// コピー先ロック
	if ((to_mem = GlobalLock(ret)) == NULL) {
		GlobalFree(ret);
		GlobalUnlock(di->data);
		return TOOL_ERROR;
	}

	// 引用符の付加
	lstrcpy((TCHAR *)to_mem, q_char);
	r = (TCHAR *)to_mem + lstrlen(q_char);
	for (p = (TCHAR *)from_mem; *p != TEXT('\0'); p++) {
#ifndef UNICODE
		if (IsDBCSLeadByte((BYTE)*p) == TRUE) {
			*(r++) = *(p++);
			*(r++) = *p;
			continue;
		}
#endif
		*(r++) = *p;
		if (*p == TEXT('\n') && *(p + 1) != TEXT('\0')) {
			lstrcpy(r, q_char);
			r += lstrlen(q_char);
		}
	}
	*r = TEXT('\0');

	GlobalUnlock(ret);
	GlobalUnlock(di->data);

	GlobalFree(di->data);
	di->data = ret;
	di->size = sizeof(TCHAR) * size;
	return TOOL_DATA_MODIFIED;
}

/*
 * quote - テキストに引用符を付加
 */
__declspec(dllexport) int CALLBACK quote(const HWND hWnd, TOOL_EXEC_INFO *tei, TOOL_DATA_INFO *tdi)
{
	DATA_INFO *di;
	TCHAR *q_char = tei->cmd_line;
	int ret = TOOL_SUCCEED;

	if ((tei->cmd_line == NULL || *tei->cmd_line == TEXT('\0')) &&
		((tei->call_type & CALLTYPE_MENU) || (tei->call_type & CALLTYPE_VIEWER))) {
		if (DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOG_QUOTE), GetForegroundWindow(), set_quote_proc, 0) == FALSE) {
			return TOOL_CANCEL;
		}
	}
	if (q_char == NULL || *q_char == TEXT('\0')) {
		q_char = quote_char;
	}
	for (; tdi != NULL; tdi = tdi->next) {
		if (SendMessage(hWnd, WM_ITEM_CHECK, 0, (LPARAM)tdi->di) == -1) {
			continue;
		}
#ifdef UNICODE
		di = (DATA_INFO *)SendMessage(hWnd, WM_ITEM_GET_FORMAT_TO_ITEM, (WPARAM)TEXT("UNICODE TEXT"), (LPARAM)tdi->di);
#else
		di = (DATA_INFO *)SendMessage(hWnd, WM_ITEM_GET_FORMAT_TO_ITEM, (WPARAM)TEXT("TEXT"), (LPARAM)tdi->di);
#endif
		if (di != NULL && di->data != NULL) {
			ret |= item_quote(di, q_char);
		}
	}
	return ret;
}

/*
 * quote_property GLOBAL - プロパティ表示
 */
__declspec(dllexport) BOOL CALLBACK quote_property(const HWND hWnd, TOOL_EXEC_INFO *tei)
{
	GlobalParr = TRUE;
	DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOG_QUOTE), hWnd, set_quote_proc, (LPARAM)tei);

	return TRUE;
}

/*
 * quote_property - プロパティ表示
 */
__declspec(dllexport) BOOL CALLBACK quote_property2(const HWND hWnd, TOOL_EXEC_INFO* tei)
{
	GlobalParr = FALSE;
	DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOG_QUOTE), hWnd, set_quote_proc, (LPARAM)tei);
	return TRUE;
}


/* End of source */
