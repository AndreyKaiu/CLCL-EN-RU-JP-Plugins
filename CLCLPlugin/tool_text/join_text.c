/*
 * tool_text
 *
 * join_text.c
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
#include "Memory.h"
#include "File.h"
#include "resource.h"
#include "Profile.h"
#include "String.h"

/* Define */

/* Global Variables */
extern HINSTANCE hInst;
extern TCHAR ini_path[];

extern int join_text_add_return;
extern int join_text_add_space; // IDC_CHECK_ADD_SPACE
extern int join_text_reverse;

static RECT rcPOS_JT;
BOOL GlobalParr; // global parameter

/* Local Function Prototypes */


BOOL CALLBACK set_join_text_proc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);


/*
 * get_text_item - テキストデータを含むアイテムを取得
 */
static TOOL_DATA_INFO *get_text_item(TOOL_DATA_INFO *tdi)
{
	// Format selection - 形式選択
	if (tdi->di->type == TYPE_ITEM) {
#ifdef UNICODE
		for (tdi = tdi->child; tdi != NULL && lstrcmpi(tdi->di->format_name, TEXT("UNICODE TEXT")) != 0; tdi = tdi->next)
			;
#else
		for (tdi = tdi->child; tdi != NULL && lstrcmpi(tdi->di->format_name, TEXT("TEXT")) != 0; tdi = tdi->next)
			;
#endif
		if (tdi == NULL) {
			return NULL;
		}
	} else if (tdi->di->type == TYPE_DATA) {
#ifdef UNICODE
		if (lstrcmpi(tdi->di->format_name, TEXT("UNICODE TEXT")) != 0) {
#else
		if (lstrcmpi(tdi->di->format_name, TEXT("TEXT")) != 0) {
#endif
			return NULL;
		}
	} else {
		return NULL;
	}
	if (tdi->di->data == NULL) {
		return NULL;
	}
	return tdi;
}

/*
 * join_text - テキストの連結
 */
__declspec(dllexport) int CALLBACK join_text(const HWND hWnd, TOOL_EXEC_INFO *tei, TOOL_DATA_INFO *tdi)
{
	TOOL_DATA_INFO *wk_tdi;
	DATA_INFO *di;
	HANDLE data;
	BYTE *from_mem, *to_mem;
	TCHAR *p, *str_tr;
	DWORD size = 0;
	int text_add_return = join_text_add_return;
	int text_add_space = join_text_add_space;
	int text_reverse = join_text_reverse;

	if (tdi == NULL) {
		return TOOL_SUCCEED;
	}

	if (tei != NULL && tei->cmd_line != NULL && *tei->cmd_line != TEXT('\0')) {
		int lenCL = lstrlen(tei->cmd_line);
		if (lenCL > 0) {			
			if (*tei->cmd_line == TEXT('?') ) {		
				GlobalParr = TRUE;
				if (DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOG_JOIN_TEXT), hWnd, set_join_text_proc, (LPARAM)tei)) {
					text_add_return = join_text_add_return;
					text_add_space = join_text_add_space;
					text_reverse = join_text_reverse;
				}
				else
					return TOOL_CANCEL;
			}
			else {
				text_add_return = *tei->cmd_line == TEXT('1');
				if (lenCL > 2) {
					text_add_space = *(tei->cmd_line + 2) == TEXT('1');
					if (lenCL > 4) {
						text_reverse = *(tei->cmd_line + 4) == TEXT('1');
					}
				}
			}					
		}		
	}



	int cntTDI = 0;


	// get size - サイズ取得
	for (wk_tdi = tdi; wk_tdi != NULL; wk_tdi = wk_tdi->next) {
		cntTDI++;
#ifdef UNICODE
		di = (DATA_INFO *)SendMessage(hWnd, WM_ITEM_GET_FORMAT_TO_ITEM, (WPARAM)TEXT("UNICODE TEXT"), (LPARAM)wk_tdi->di);
#else
		di = (DATA_INFO *)SendMessage(hWnd, WM_ITEM_GET_FORMAT_TO_ITEM, (WPARAM)TEXT("TEXT"), (LPARAM)wk_tdi->di);
#endif
		if (di == NULL || di->data == NULL) {
			continue;
		}
		if ((from_mem = GlobalLock(di->data)) != NULL) {
			if (text_add_space == 1)
			{
				str_tr = mem_alloc(sizeof(TCHAR) * lstrlen((TCHAR*)from_mem) + 2);
				lstrcpy(str_tr, (TCHAR*)from_mem);
				Trim(str_tr);				
				size += lstrlen(str_tr);
				mem_free(&str_tr);
			}
			else
				size += lstrlen((TCHAR*)from_mem);

			GlobalUnlock(di->data);

			if (text_add_space == 1) {
				size++;
			}
			if (text_add_return == 1) {
				size += 2;
			}			
		}
	}
	if (size == 0) {
		return TOOL_SUCCEED;
	}
	size++;

	// Securing copy destinationм - コピー先確保
	if ((data = GlobalAlloc(GHND, sizeof(TCHAR) * size)) == NULL) {
		return TOOL_ERROR;
	}
	// Destination lock - コピー先ロック
	if ((to_mem = GlobalLock(data)) == NULL) {
		GlobalFree(data);
		return TOOL_ERROR;
	}



	// 2023-02-16 kaiu@mail.ru
	TOOL_DATA_INFO **masTDI = calloc(cntTDI, sizeof(TOOL_DATA_INFO*));
	wk_tdi = tdi;
	for(int i=0; wk_tdi != NULL && i < cntTDI; wk_tdi = wk_tdi->next, ++i) {
		if(text_reverse)
			masTDI[cntTDI-1-i] = wk_tdi;
		else
			masTDI[i] = wk_tdi;
	}
	

	

	// concatenation of text - テキストの連結
	p = (TCHAR *)to_mem;

	//for (wk_tdi = tdi; wk_tdi != NULL; wk_tdi = wk_tdi->next) {
	for (int i=0; i < cntTDI; ++i) { // 2023-02-16 kaiu@mail.ru
		wk_tdi = masTDI[i];

#ifdef UNICODE
		di = (DATA_INFO *)SendMessage(hWnd, WM_ITEM_GET_FORMAT_TO_ITEM, (WPARAM)TEXT("UNICODE TEXT"), (LPARAM)wk_tdi->di);
#else
		di = (DATA_INFO *)SendMessage(hWnd, WM_ITEM_GET_FORMAT_TO_ITEM, (WPARAM)TEXT("TEXT"), (LPARAM)wk_tdi->di);
#endif
		if (di == NULL || di->data == NULL) {
			continue;
		}
		if ((from_mem = GlobalLock(di->data)) != NULL) {
			if (text_add_space == 1)
			{
				str_tr = mem_alloc( sizeof(TCHAR) * lstrlen((TCHAR*)from_mem) + 2 );
				lstrcpy(str_tr, (TCHAR*)from_mem);
				Trim(str_tr);
				lstrcpy(p, str_tr);
				mem_free(&str_tr);
			}
			else
				lstrcpy(p, (TCHAR*)from_mem);

			p += lstrlen(p);

			GlobalUnlock(di->data);

			if (text_add_space == 1) {
				lstrcpy(p, TEXT(" "));
				p += 1;
			}
			if (text_add_return == 1) {
				lstrcpy(p, TEXT("\r\n"));
				p += 2;
			}
		}
	}
	GlobalUnlock(data);


	mem_free(&masTDI); // 2023-02-16 kaiu@mail.ru


	// send to clipboard - クリップボードに送る
	if (OpenClipboard(hWnd) == FALSE) {
		GlobalFree(data);
		return TOOL_ERROR;
	}
	if (EmptyClipboard() == FALSE) {
		CloseClipboard();
		GlobalFree(data);
		return TOOL_ERROR;
	}
#ifdef UNICODE
	if (SetClipboardData(CF_UNICODETEXT, data) == NULL) {
#else
	if (SetClipboardData(CF_TEXT, data) == NULL) {
#endif
		CloseClipboard();
		GlobalFree(data);
		return TOOL_ERROR;
	}
	CloseClipboard();
	return TOOL_SUCCEED;
}

/*
 * set_join_text_proc - テキストの連結設定
 */
BOOL CALLBACK set_join_text_proc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static TOOL_EXEC_INFO* tei;

	TCHAR buf[BUF_SIZE];

	switch (uMsg) {
	case WM_INITDIALOG:
		tei = (TOOL_EXEC_INFO*)lParam;
		if (tei == NULL || tei->cmd_line == NULL) GlobalParr = TRUE;

		if (GlobalParr) {
			CheckDlgButton(hDlg, IDC_CHECK_ADD_RETURN, join_text_add_return);
			CheckDlgButton(hDlg, IDC_CHECK_ADD_SPACE, join_text_add_space);
			CheckDlgButton(hDlg, IDC_CHECK_REVERSE_ORDER, join_text_reverse);
		}
		else {			
			int lenCL = lstrlen(tei->cmd_line);
			if (lenCL > 0) {
				CheckDlgButton(hDlg, IDC_CHECK_ADD_RETURN, *tei->cmd_line == TEXT('1') );
				if (lenCL > 2) {
					CheckDlgButton(hDlg, IDC_CHECK_ADD_SPACE, *(tei->cmd_line+2) == TEXT('1') );
					if(lenCL > 4) {
						CheckDlgButton(hDlg, IDC_CHECK_REVERSE_ORDER, *(tei->cmd_line + 4) == TEXT('1') );
					}
					else
						CheckDlgButton(hDlg, IDC_CHECK_REVERSE_ORDER, join_text_reverse);
				}
				else
					CheckDlgButton(hDlg, IDC_CHECK_ADD_SPACE, join_text_add_space);				
			}
			else
				CheckDlgButton(hDlg, IDC_CHECK_ADD_RETURN, join_text_add_return);			
		}
		

		if (!(rcPOS_JT.left == 0 && rcPOS_JT.bottom == 0 && rcPOS_JT.right == 0 && rcPOS_JT.top == 0)) {
			SetWindowPos(hDlg, HWND_TOP, rcPOS_JT.left, rcPOS_JT.top, 0, 0, SWP_NOSIZE);
		}

		break;

	case WM_CLOSE:
		GetWindowRect(hDlg, &rcPOS_JT);

		EndDialog(hDlg, FALSE);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			GetWindowRect(hDlg, &rcPOS_JT);

#ifdef UNICODE
			// For unicode to create a file if it does not exist
			if (!file_check_file(ini_path))
			{
				BYTE bt[2]; bt[0] = 0xFF; bt[1] = 0xFE; // UTF-16 LE BOM				
				file_write_buf(ini_path, bt, 2, NULL);
			}
#endif
			
			if (GlobalParr) {
				join_text_add_return = IsDlgButtonChecked(hDlg, IDC_CHECK_ADD_RETURN);
				join_text_add_space = IsDlgButtonChecked(hDlg, IDC_CHECK_ADD_SPACE);
				join_text_reverse = IsDlgButtonChecked(hDlg, IDC_CHECK_REVERSE_ORDER);
				profile_initialize(ini_path, TRUE);
				profile_write_int(TEXT("join_text"), TEXT("add_return"), join_text_add_return, ini_path);
				profile_write_int(TEXT("join_text"), TEXT("add_space"), join_text_add_space, ini_path);
				profile_write_int(TEXT("join_text"), TEXT("reverse"), join_text_reverse, ini_path);
				profile_flush(ini_path); profile_free();
			}
			else {
				BOOL text_add_return, text_add_space, text_reverse;
				text_add_return = IsDlgButtonChecked(hDlg, IDC_CHECK_ADD_RETURN);
				text_add_space = IsDlgButtonChecked(hDlg, IDC_CHECK_ADD_SPACE);
				text_reverse = IsDlgButtonChecked(hDlg, IDC_CHECK_REVERSE_ORDER);
				TCHAR cmdL[6] = TEXT("0,0,0");
				if (text_add_return) cmdL[0] = TEXT('1');
				if (text_add_space) cmdL[2] = TEXT('1');
				if (text_reverse) cmdL[4] = TEXT('1');
				tei->cmd_line = alloc_copy(cmdL);
			}

			
			EndDialog(hDlg, TRUE);
			break;

		case IDCANCEL:
			GetWindowRect(hDlg, &rcPOS_JT);
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
 * join_text_property GLOBAL - プロパティ表示
 */
__declspec(dllexport) BOOL CALLBACK join_text_property(const HWND hWnd, TOOL_EXEC_INFO *tei)
{
	GlobalParr = TRUE;
	DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOG_JOIN_TEXT), hWnd, set_join_text_proc, (LPARAM)tei);
	return TRUE;
}


/*
 * join_text_property - プロパティ表示
 */
__declspec(dllexport) BOOL CALLBACK join_text_property2(const HWND hWnd, TOOL_EXEC_INFO* tei)
{
	GlobalParr = FALSE;
	DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOG_JOIN_TEXT), hWnd, set_join_text_proc, (LPARAM)tei);
	return TRUE;
}

/* End of source */
