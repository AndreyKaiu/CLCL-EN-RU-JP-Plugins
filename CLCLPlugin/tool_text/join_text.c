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
#include "String.h"

/* Define */

/* Global Variables */
extern HINSTANCE hInst;
extern TCHAR ini_path[];

extern int join_text_add_return;
extern int join_text_add_space; // IDC_CHECK_ADD_SPACE

/* Local Function Prototypes */

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

	if (tdi == NULL) {
		return TOOL_SUCCEED;
	}

	// get size - サイズ取得
	for (wk_tdi = tdi; wk_tdi != NULL; wk_tdi = wk_tdi->next) {
#ifdef UNICODE
		di = (DATA_INFO *)SendMessage(hWnd, WM_ITEM_GET_FORMAT_TO_ITEM, (WPARAM)TEXT("UNICODE TEXT"), (LPARAM)wk_tdi->di);
#else
		di = (DATA_INFO *)SendMessage(hWnd, WM_ITEM_GET_FORMAT_TO_ITEM, (WPARAM)TEXT("TEXT"), (LPARAM)wk_tdi->di);
#endif
		if (di == NULL || di->data == NULL) {
			continue;
		}
		if ((from_mem = GlobalLock(di->data)) != NULL) {
			if (join_text_add_space == 1)
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

			if (join_text_add_space == 1) {
				size++;
			}
			if (join_text_add_return == 1) {
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
	// concatenation of text - テキストの連結
	p = (TCHAR *)to_mem;
	for (wk_tdi = tdi; wk_tdi != NULL; wk_tdi = wk_tdi->next) {
#ifdef UNICODE
		di = (DATA_INFO *)SendMessage(hWnd, WM_ITEM_GET_FORMAT_TO_ITEM, (WPARAM)TEXT("UNICODE TEXT"), (LPARAM)wk_tdi->di);
#else
		di = (DATA_INFO *)SendMessage(hWnd, WM_ITEM_GET_FORMAT_TO_ITEM, (WPARAM)TEXT("TEXT"), (LPARAM)wk_tdi->di);
#endif
		if (di == NULL || di->data == NULL) {
			continue;
		}
		if ((from_mem = GlobalLock(di->data)) != NULL) {
			if (join_text_add_space == 1)
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

			if (join_text_add_space == 1) {
				lstrcpy(p, TEXT(" "));
				p += 1;
			}
			if (join_text_add_return == 1) {
				lstrcpy(p, TEXT("\r\n"));
				p += 2;
			}
		}
	}
	GlobalUnlock(data);

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
	TCHAR buf[BUF_SIZE];

	switch (uMsg) {
	case WM_INITDIALOG:
		CheckDlgButton(hDlg, IDC_CHECK_ADD_RETURN, join_text_add_return);
		CheckDlgButton(hDlg, IDC_CHECK_ADD_SPACE, join_text_add_space);
		break;

	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			join_text_add_return = IsDlgButtonChecked(hDlg, IDC_CHECK_ADD_RETURN);
			join_text_add_space = IsDlgButtonChecked(hDlg, IDC_CHECK_ADD_SPACE);

#ifdef UNICODE
			// For unicode to create a file if it does not exist
			if (!file_check_file(ini_path))
			{
				BYTE bt[2]; bt[0] = 0xFF; bt[1] = 0xFE; // UTF-16 LE BOM				
				file_write_buf(ini_path, bt, 2, NULL);
			}
#endif

			wsprintf(buf, TEXT("%d"), join_text_add_return);
			WritePrivateProfileString(TEXT("join_text"), TEXT("add_return"), buf, ini_path);
			wsprintf(buf, TEXT("%d"), join_text_add_space);
			WritePrivateProfileString(TEXT("join_text"), TEXT("add_space"), buf, ini_path);
			EndDialog(hDlg, TRUE);
			break;

		case IDCANCEL:
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
 * join_text_property - プロパティ表示
 */
__declspec(dllexport) BOOL CALLBACK join_text_property(const HWND hWnd, TOOL_EXEC_INFO *tei)
{
	DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOG_JOIN_TEXT), hWnd, set_join_text_proc, 0);
	return TRUE;
}
/* End of source */
