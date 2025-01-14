/*
 * tool_text
 *
 * word_break.c
 *
 * Copyright (C) 1996-2003 by Nakashima Tomoaki. All rights reserved.
 *		http://www.nakka.com/
 *		nakka@nakka.com
 */


/* Include Files */
#define _INC_OLE
#include <windows.h>
#undef  _INC_OLE
#include <tchar.h>

#include "..\CLCLPlugin.h"
#include "File.h"
#include "resource.h"
#include "Profile.h"

/* Define */
#define IsBreakAlNum(c)		((c >= TEXT('a') && c <= TEXT('z')) || \
								(c >= TEXT('A') && c <= TEXT('Z')) || \
								c == TEXT('\'') || \
								(c >= TEXT('0') && c <= TEXT('9')))

/* Global Variables */
extern HINSTANCE hInst;
extern TCHAR ini_path[];

extern TCHAR quote_char[];

extern int word_break_break_cnt;
extern int word_break_tab_size;
extern TCHAR word_break_m_oida[BUF_SIZE];
extern TCHAR word_break_m_bura[BUF_SIZE];
extern TCHAR word_break_s_oida[BUF_SIZE];
extern TCHAR word_break_s_bura[BUF_SIZE];



extern void EscapingDel(TCHAR* s);
extern void cmd_line_to_array(TCHAR* cmdLine, TCHAR** arr, int lenArr, BOOL isEscapingDel);
extern void array_to_cmd_line(TCHAR** arr, int lenArr, BOOL isEscaping, TCHAR** cmdLine);


static RECT rcPOS_WB;
BOOL GlobalParr; // global parameter

static TCHAR str_break_tab_size[BUF_SIZE];
static TCHAR break_m_oida[BUF_SIZE];
static TCHAR break_m_bura[BUF_SIZE];
static TCHAR break_s_oida[BUF_SIZE];
static TCHAR break_s_bura[BUF_SIZE];


/* Local Function Prototypes */

/*
 * multibytes_rule_check - 全角文字の禁則チェック
 */
static void multibytes_rule_check(TCHAR *p, BOOL *top_flag, BOOL *end_flag)
{
	TCHAR *t;

	// 行頭禁則
	if (break_m_oida != NULL) {
		for (t = break_m_oida; *t != TEXT('\0'); t += 2) {
			if (*p == *t && *(p + 1) == *(t + 1)) {
				*top_flag = TRUE;
				return;
			}
		}
	}
	// 行末禁則
	if (break_m_bura != NULL) {
		for (t = break_m_bura; *t != TEXT('\0'); t += 2) {
			if (*p == *t && *(p + 1) == *(t + 1)) {
				*end_flag = TRUE;
				return;
			}
		}
	}
}

/*
 * rule_check - 半角文字の禁則チェック
 */
static void rule_check(TCHAR *p, BOOL *top_flag, BOOL *end_flag)
{
	TCHAR *t;

	// 行頭禁則
	if (break_s_oida != NULL) {
		for (t = break_s_oida; *t != TEXT('\0'); t++) {
			if (*p == *t) {
				*top_flag = TRUE;
				return;
			}
		}
	}
	// 行末禁則
	if (break_s_bura != NULL) {
		for (t = break_s_bura; *t != TEXT('\0'); t++) {
			if (*p == *t) {
				*end_flag = TRUE;
				return;
			}
		}
	}
}

/*
 * get_word_break_size - 文字列を指定の長さで折り返したときのサイズ
 */
static int get_word_break_size(TCHAR *buf, const int break_cnt)
{
	TCHAR *p, *s;
	int cnt = 0;
	int ret = 0;
	BOOL top_flag;
	BOOL end_flag;

	if (break_cnt <= 0) {
		return lstrlen(buf);
	}

	p = buf;

	while (*p != TEXT('\0')) {
#ifndef UNICODE
		if (IsDBCSLeadByte((BYTE)*p) == TRUE) {
			top_flag = FALSE;
			end_flag = FALSE;
			if ((cnt + 2) >= break_cnt) {
				multibytes_rule_check(p, &top_flag, &end_flag);
			}

			if ((((cnt + 2) > break_cnt && end_flag == FALSE) ||
				((cnt + 2) == break_cnt && top_flag == TRUE))) {
				cnt = 0;
				ret += 2;
			}
			cnt += 2;
			p += 2;
			ret += 2;
			continue;
		}
#endif
		if (*p == TEXT('\r')) {
			cnt = 0;
			p += 2;
			ret += 2;

		} else if (*p == TEXT('\t')) {
			cnt += (word_break_tab_size - (cnt % word_break_tab_size));
			if (cnt > break_cnt) {
				cnt = (word_break_tab_size - (cnt % word_break_tab_size));
				ret += 2;
			}
			p++;
			ret++;

		} else if (IsBreakAlNum(*p)) {
			for (s = p; IsBreakAlNum(*p); p++) {
				cnt++;
			}
			if (*p == ' ') {
				cnt++;
				p++;
			}
			if (cnt > break_cnt) {
				if (cnt != p - s) {
					ret += 2;
				}
				cnt = p - s;
			}
			ret += p - s;

		} else {
			top_flag = FALSE;
			end_flag = FALSE;
			if ((cnt + 1) >= break_cnt) {
				rule_check(p, &top_flag, &end_flag);
			}

			if ((((cnt + 1) > break_cnt && end_flag == FALSE) ||
				((cnt + 1) == break_cnt && top_flag == TRUE))) {
				cnt = 0;
				ret += 2;
			}
			cnt++;
			p++;
			ret++;
		}
	}
	return ret;
}

/*
 * set_word_break - 文字列を指定の長さで折り返す
 */
static void set_word_break(TCHAR *buf, TCHAR *ret, const int break_cnt)
{
	TCHAR *p, *r, *s;
	int cnt = 0;
	BOOL TopFlag;
	BOOL EndFlag;

	if (break_cnt <= 0) {
		lstrcpy(ret, buf);
		return;
	}

	p = buf;
	r = ret;
	while (*p != TEXT('\0')) {
#ifndef UNICODE
		if (IsDBCSLeadByte((BYTE)*p) == TRUE) {
			// 2バイトコード
			TopFlag = FALSE;
			EndFlag = FALSE;
			if ((cnt + 2) >= break_cnt) {
				multibytes_rule_check(p, &TopFlag, &EndFlag);
			}

			if ((((cnt + 2) > break_cnt && EndFlag == FALSE) ||
				((cnt + 2) == break_cnt && TopFlag == TRUE))) {
				cnt = 0;
				*(r++) = '\r';
				*(r++) = '\n';
			}
			cnt += 2;
			*(r++) = *(p++);
			*(r++) = *(p++);
			continue;
		}
#endif
		if (*p == TEXT('\r')) {
			// 改行
			cnt = 0;
			*(r++) = *(p++);
			*(r++) = *(p++);

		} else if (*p == TEXT('\t')) {
			// タブ
			cnt += (word_break_tab_size - (cnt % word_break_tab_size));
			if (cnt > break_cnt) {
				cnt = (word_break_tab_size - (cnt % word_break_tab_size));
				*(r++) = TEXT('\r');
				*(r++) = TEXT('\n');
			}
			*(r++) = *(p++);

		} else if (IsBreakAlNum(*p)) {
			// 英数字は途中改行しない
			for (s = p; IsBreakAlNum(*p); p++) {
				cnt++;
			}
			if (*p == TEXT(' ')) {
				cnt++;
				p++;
			}
			if (cnt > break_cnt) {
				if (cnt != p - s) {
					*(r++) = TEXT('\r');
					*(r++) = TEXT('\n');
				}
				cnt = p - s;
			}
			for (; s != p; s++) {
				*(r++) = *s;
			}

		} else {
			TopFlag = FALSE;
			EndFlag = FALSE;
			if ((cnt + 1) >= break_cnt) {
				rule_check(p, &TopFlag, &EndFlag);
			}

			if ((((cnt + 1) > break_cnt && EndFlag == FALSE) ||
				((cnt + 1) == break_cnt && TopFlag == TRUE))) {
				cnt = 0;
				*(r++) = TEXT('\r');
				*(r++) = TEXT('\n');
			}
			cnt++;
			*(r++) = *(p++);
		}
	}
	*r = TEXT('\0');
}

/*
 * item_quote - テキストの整形
 */
static int item_word_break(DATA_INFO *di, const int break_cnt)
{
	HANDLE ret;
	BYTE *from_mem, *to_mem;
	int size;

	// コピー元ロック
	if ((from_mem = GlobalLock(di->data)) == NULL) {
		return TOOL_ERROR;
	}

	// サイズを取得
	size = get_word_break_size((TCHAR *)from_mem, break_cnt);
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

	// 折り返し
	set_word_break((TCHAR *)from_mem, (TCHAR *)to_mem, break_cnt);

	GlobalUnlock(ret);
	GlobalUnlock(di->data);

	GlobalFree(di->data);
	di->data = ret;
	di->size = sizeof(TCHAR) * size;
	return TOOL_DATA_MODIFIED;
}

/*
 * set_break_cnt_proc - 折り返し文字数の設定
 */
static BOOL CALLBACK set_break_cnt_proc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TCHAR buf[BUF_SIZE];

	switch (uMsg) {
	case WM_INITDIALOG:
		SetDlgItemInt(hDlg, IDC_EDIT_BREAK_CNT, word_break_break_cnt, FALSE);

		if (!(rcPOS_WB.left == 0 && rcPOS_WB.bottom == 0 && rcPOS_WB.right == 0 && rcPOS_WB.top == 0)) {
			SetWindowPos(hDlg, HWND_TOP, rcPOS_WB.left, rcPOS_WB.top, 0, 0, SWP_NOSIZE);
		}
		break;

	case WM_CLOSE:
		GetWindowRect(hDlg, &rcPOS_WB);
		EndDialog(hDlg, FALSE);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			GetWindowRect(hDlg, &rcPOS_WB);
			word_break_break_cnt = GetDlgItemInt(hDlg, IDC_EDIT_BREAK_CNT, NULL, FALSE);

			profile_initialize(ini_path, TRUE);
			profile_write_int(TEXT("word_break"), TEXT("break_cnt"), word_break_break_cnt, ini_path);
			profile_flush(ini_path); profile_free();
			
			EndDialog(hDlg, TRUE);
			break;

		case IDCANCEL:
			GetWindowRect(hDlg, &rcPOS_WB);
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
 * word_break - テキストの整形
 */
__declspec(dllexport) int CALLBACK word_break(const HWND hWnd, TOOL_EXEC_INFO *tei, TOOL_DATA_INFO *tdi)
{
	DATA_INFO *di;
	int cnt;
	int ret = TOOL_SUCCEED;

	if (tei->cmd_line != NULL && *tei->cmd_line != TEXT('\0')) {
		if (wcsstr(tei->cmd_line, TEXT(",")) == NULL) {
			cnt = _ttol(tei->cmd_line);
			lstrcpy(break_m_oida, word_break_m_oida);
			lstrcpy(break_m_bura, word_break_m_bura);
			lstrcpy(break_s_oida, word_break_s_oida);
			lstrcpy(break_s_bura, word_break_s_bura);
		}
		else {
			CHAR* Arr[5];
			Arr[0] = str_break_tab_size;
			Arr[1] = break_m_oida;
			Arr[2] = break_m_bura;
			Arr[3] = break_s_oida;
			Arr[4] = break_s_bura;
			cmd_line_to_array(tei->cmd_line, Arr, 5, FALSE);
			cnt = _ttol(str_break_tab_size);
		}		
	} else {
		if ((tei->call_type & CALLTYPE_MENU) || (tei->call_type & CALLTYPE_VIEWER)) {
			if (DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOG_BREAK_CNT), GetForegroundWindow(), set_break_cnt_proc, 0) == FALSE) {
				return TOOL_CANCEL;
			}
		}
		cnt = word_break_break_cnt;
		lstrcpy(break_m_oida, word_break_m_oida);
		lstrcpy(break_m_bura, word_break_m_bura);
		lstrcpy(break_s_oida, word_break_s_oida);
		lstrcpy(break_s_bura, word_break_s_bura);
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
			ret |= item_word_break(di, cnt);
		}
	}
	return ret;
}

/*
 * set_word_break_proc - 引用符を設定
 */
static BOOL CALLBACK set_word_break_proc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TCHAR buf[BUF_SIZE];
	static TOOL_EXEC_INFO* tei;
	
	switch (uMsg) {
	case WM_INITDIALOG:
		tei = (TOOL_EXEC_INFO*)lParam;
		if (tei == NULL || tei->cmd_line == NULL) GlobalParr = TRUE;

		if (GlobalParr) {
			SetDlgItemInt(hDlg, IDC_EDIT_TAB_SIZE, word_break_tab_size, FALSE);
			SendDlgItemMessage(hDlg, IDC_EDIT_M_OIDA, WM_SETTEXT, 0, (LPARAM)word_break_m_oida);
			SendDlgItemMessage(hDlg, IDC_EDIT_M_BURA, WM_SETTEXT, 0, (LPARAM)word_break_m_bura);
			SendDlgItemMessage(hDlg, IDC_EDIT_S_OIDA, WM_SETTEXT, 0, (LPARAM)word_break_s_oida);
			SendDlgItemMessage(hDlg, IDC_EDIT_S_BURA, WM_SETTEXT, 0, (LPARAM)word_break_s_bura);
		}
		else {
			CHAR* Arr[5];
			Arr[0] = str_break_tab_size;
			Arr[1] = break_m_oida;
			Arr[2] = break_m_bura;
			Arr[3] = break_s_oida;
			Arr[4] = break_s_bura;
			cmd_line_to_array(tei->cmd_line, Arr, 5, FALSE);
			SendDlgItemMessage(hDlg, IDC_EDIT_TAB_SIZE, WM_SETTEXT, 0, (LPARAM)str_break_tab_size);
			SendDlgItemMessage(hDlg, IDC_EDIT_M_OIDA, WM_SETTEXT, 0, (LPARAM)break_m_oida);
			SendDlgItemMessage(hDlg, IDC_EDIT_M_BURA, WM_SETTEXT, 0, (LPARAM)break_m_bura);
			SendDlgItemMessage(hDlg, IDC_EDIT_S_OIDA, WM_SETTEXT, 0, (LPARAM)break_s_oida);
			SendDlgItemMessage(hDlg, IDC_EDIT_S_BURA, WM_SETTEXT, 0, (LPARAM)break_s_bura);
		}

		break;

	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:		
			{
			TCHAR* txt = mem_calloc(2050, sizeof(TCHAR));

#ifdef UNICODE
			// For unicode to create a file if it does not exist
			if (!file_check_file(ini_path))
			{
				BYTE bt[2]; bt[0] = 0xFF; bt[1] = 0xFE; // UTF-16 LE BOM				
				file_write_buf(ini_path, bt, 2, NULL);
			}
#endif

			if (GlobalParr) {
				word_break_tab_size = GetDlgItemInt(hDlg, IDC_EDIT_TAB_SIZE, NULL, FALSE);
				SendDlgItemMessage(hDlg, IDC_EDIT_M_OIDA, WM_GETTEXT, BUF_SIZE - 1, (LPARAM)word_break_m_oida);
				SendDlgItemMessage(hDlg, IDC_EDIT_M_BURA, WM_GETTEXT, BUF_SIZE - 1, (LPARAM)word_break_m_bura);
				SendDlgItemMessage(hDlg, IDC_EDIT_S_OIDA, WM_GETTEXT, BUF_SIZE - 1, (LPARAM)word_break_s_oida);
				SendDlgItemMessage(hDlg, IDC_EDIT_S_BURA, WM_GETTEXT, BUF_SIZE - 1, (LPARAM)word_break_s_bura);

				profile_initialize(ini_path, TRUE);
				wsprintf(buf, TEXT("%d"), word_break_tab_size);
				profile_write_string(TEXT("word_break"), TEXT("tab_size"), buf, ini_path);
				profile_write_string(TEXT("word_break"), TEXT("m_oida"), word_break_m_oida, ini_path);
				profile_write_string(TEXT("word_break"), TEXT("m_bura"), word_break_m_bura, ini_path);
				profile_write_string(TEXT("word_break"), TEXT("s_oida"), word_break_s_oida, ini_path);
				profile_write_string(TEXT("word_break"), TEXT("s_bura"), word_break_s_bura, ini_path);
				profile_flush(ini_path); profile_free();
			}
			else {
				SendDlgItemMessage(hDlg, IDC_EDIT_TAB_SIZE, WM_GETTEXT, BUF_SIZE - 1, (LPARAM)str_break_tab_size);
				SendDlgItemMessage(hDlg, IDC_EDIT_M_OIDA, WM_GETTEXT, BUF_SIZE - 1, (LPARAM)break_m_oida);
				SendDlgItemMessage(hDlg, IDC_EDIT_M_BURA, WM_GETTEXT, BUF_SIZE - 1, (LPARAM)break_m_bura);
				SendDlgItemMessage(hDlg, IDC_EDIT_S_OIDA, WM_GETTEXT, BUF_SIZE - 1, (LPARAM)break_s_oida);
				SendDlgItemMessage(hDlg, IDC_EDIT_S_BURA, WM_GETTEXT, BUF_SIZE - 1, (LPARAM)break_s_bura);

				CHAR* Arr[5];
				Arr[0] = str_break_tab_size;
				Arr[1] = break_m_oida;
				Arr[2] = break_m_bura;
				Arr[3] = break_s_oida;
				Arr[4] = break_s_bura;
				array_to_cmd_line(Arr, 5, FALSE, &txt);
				tei->cmd_line = alloc_copy(txt);
			}


			mem_free(&txt);
			EndDialog(hDlg, TRUE);
			}
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
 * word_break_property - プロパティ表示
 */
__declspec(dllexport) BOOL CALLBACK word_break_property(const HWND hWnd, TOOL_EXEC_INFO *tei)
{
	GlobalParr = TRUE;
	DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOG_WORD_BREAK), hWnd, set_word_break_proc, (LPARAM)tei);
	return TRUE;
}


/*
 * word_break_property - プロパティ表示
 */
__declspec(dllexport) BOOL CALLBACK word_break_property2(const HWND hWnd, TOOL_EXEC_INFO* tei)
{
	GlobalParr = FALSE;
	DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOG_WORD_BREAK), hWnd, set_word_break_proc, (LPARAM)tei);
	return TRUE;
}

/* End of source */
