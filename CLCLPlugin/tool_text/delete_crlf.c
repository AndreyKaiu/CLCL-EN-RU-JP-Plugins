/*
 * tool_text
 *
 * delete_crlf.c
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
#include "File.h"
#include "Memory.h"
#include "Profile.h"
#include "resource.h"

/* Define */

/* Global Variables */
extern HINSTANCE hInst;
extern TCHAR ini_path[];

extern int delete_crlf_delete_space;

static RECT rcPOS_D;
static BOOL GlobalParr; // global parameter

/* Local Function Prototypes */

/*
 * item_delete_crlf - 改行の除去
 */
static int item_delete_crlf(DATA_INFO *di, BOOL delSpace)
{
	HANDLE ret;
	BYTE *from_mem, *to_mem;
	TCHAR *p, *r;
	int size;

	// コピー元ロック
	if ((from_mem = GlobalLock(di->data)) == NULL) {
		return TOOL_ERROR;
	}

	// 改行を除去したサイズを取得
	size = 0;
	p = (TCHAR *)from_mem;
	if (delSpace == 1) {
		for (; *p == TEXT(' ') || *p == TEXT('\t'); p++)
			;
	}
	while (*p != TEXT('\0')) {
#ifndef UNICODE
		if (IsDBCSLeadByte((BYTE)*p) == TRUE) {
			p += 2;
			size += 2;
			continue;
		}
#endif
		if (*p == TEXT('\r') || *p == TEXT('\n')) {
			for (; *p == TEXT('\r') || *p == TEXT('\n'); p++)
				;
			if (delSpace == 1) {
				for (; *p == TEXT(' ') || *p == TEXT('\t'); p++)
					;
			}
		} else {
			p++;
			size++;
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

	// 改行の除去
	p = (TCHAR *)from_mem;
	r = (TCHAR *)to_mem;
	if (delSpace == 1) {
		for (; *p == TEXT(' ') || *p == TEXT('\t'); p++)
			;
	}
	while (*p != TEXT('\0')) {
#ifndef UNICODE
		if (IsDBCSLeadByte((BYTE)*p) == TRUE) {
			*(r++) = *(p++);
			*(r++) = *(p++);
			continue;
		}
#endif
		if (*p == TEXT('\r') || *p == TEXT('\n')) {
			for (; *p == TEXT('\r') || *p == TEXT('\n'); p++)
				;
			if (delSpace == 1) {
				for (; *p == TEXT(' ') || *p == TEXT('\t'); p++)
					;
			}
		} else {
			*(r++) = *(p++);
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
 * set_delete_crlf_proc - 設定
 */
BOOL CALLBACK set_delete_crlf_proc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TCHAR buf[BUF_SIZE];
	static TOOL_EXEC_INFO* tei;

	switch (uMsg) {
	case WM_INITDIALOG:
		tei = (TOOL_EXEC_INFO*)lParam;
		if (tei == NULL || tei->cmd_line == NULL) GlobalParr = TRUE;

		if (GlobalParr)
			CheckDlgButton(hDlg, IDC_CHECK_DELETE_SPACE, delete_crlf_delete_space);
		else
			CheckDlgButton(hDlg, IDC_CHECK_DELETE_SPACE, lstrcmp(tei->cmd_line, TEXT("1")) == 0 );

		if (!(rcPOS_D.left == 0 && rcPOS_D.bottom == 0 && rcPOS_D.right == 0 && rcPOS_D.top == 0)) {
			SetWindowPos(hDlg, HWND_TOP, rcPOS_D.left, rcPOS_D.top, 0, 0, SWP_NOSIZE);
		}
		break;

	case WM_CLOSE:
		GetWindowRect(hDlg, &rcPOS_D);
		EndDialog(hDlg, FALSE);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			GetWindowRect(hDlg, &rcPOS_D);
			if (GlobalParr) {
				delete_crlf_delete_space = IsDlgButtonChecked(hDlg, IDC_CHECK_DELETE_SPACE);

#ifdef UNICODE
				// For unicode to create a file if it does not exist
				if (!file_check_file(ini_path))
				{
					BYTE bt[2]; bt[0] = 0xFF; bt[1] = 0xFE; // UTF-16 LE BOM				
					file_write_buf(ini_path, bt, 2, NULL);
				}
#endif
				profile_initialize(ini_path, TRUE);
				profile_write_int(TEXT("delete_crlf"), TEXT("delete_space"), delete_crlf_delete_space, ini_path);
				profile_flush(ini_path); profile_free();

			}
			else {
				if(IsDlgButtonChecked(hDlg, IDC_CHECK_DELETE_SPACE))
					tei->cmd_line = alloc_copy(TEXT("1"));
				else
					tei->cmd_line = alloc_copy(TEXT("0"));
			}
			

			EndDialog(hDlg, TRUE);
			break;

		case IDCANCEL:
			GetWindowRect(hDlg, &rcPOS_D);
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
 * delete_crlf - 改行の除去
 */
__declspec(dllexport) int CALLBACK delete_crlf(const HWND hWnd, TOOL_EXEC_INFO* tei, TOOL_DATA_INFO* tdi)
{
	DATA_INFO* di;
	int ret = TOOL_SUCCEED;
	BOOL delSpace = delete_crlf_delete_space;

	if (tei != NULL && tei->cmd_line != NULL && *tei->cmd_line != TEXT('\0')) {
		if (lstrcmp(tei->cmd_line, TEXT("1")) == 0) {
			delSpace = TRUE;
		}
		else {
			if (lstrcmp(tei->cmd_line, TEXT("0")) == 0) {
				delSpace = FALSE;
			}
			else { // !=0 and !=1
				GlobalParr = TRUE;
				if (DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOG_DELETE_CRLF), GetForegroundWindow(), set_delete_crlf_proc, 0) == FALSE) {
					return TOOL_CANCEL;
				}				

				delSpace = delete_crlf_delete_space;
			}
		}
	}

	for (; tdi != NULL; tdi = tdi->next) {
#ifdef UNICODE
		di = (DATA_INFO*)SendMessage(hWnd, WM_ITEM_GET_FORMAT_TO_ITEM, (WPARAM)TEXT("UNICODE TEXT"), (LPARAM)tdi->di);
#else
		di = (DATA_INFO*)SendMessage(hWnd, WM_ITEM_GET_FORMAT_TO_ITEM, (WPARAM)TEXT("TEXT"), (LPARAM)tdi->di);
#endif
		if (di != NULL && di->data != NULL) {
			ret |= item_delete_crlf(di, delSpace);
		}
	}
	return ret;
}






/*
 * delete_crlf_property GLOBAL - プロパティ表示
 */
__declspec(dllexport) BOOL CALLBACK delete_crlf_property(const HWND hWnd, TOOL_EXEC_INFO *tei)
{
	GlobalParr = TRUE;
	DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOG_DELETE_CRLF), GetForegroundWindow(), set_delete_crlf_proc, 0);
	return TRUE;
}

/*
 * delete_crlf_property - プロパティ表示
 */
__declspec(dllexport) BOOL CALLBACK delete_crlf_property2(const HWND hWnd, TOOL_EXEC_INFO* tei)
{
	GlobalParr = FALSE;
	DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOG_DELETE_CRLF), GetForegroundWindow(), set_delete_crlf_proc, tei);
	return TRUE;
}
/* End of source */
