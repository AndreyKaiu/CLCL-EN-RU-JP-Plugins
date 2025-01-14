/*
 * tool_text
 *
 * put_text.c
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

#define ID_PUT_TEXT_TIMER	1	

#include "..\CLCLPlugin.h"
#include "File.h"
#include "Memory.h"
#include "Profile.h"
#include "resource.h"

/* Define */

/* Global Variables */
extern HINSTANCE hInst;
extern TCHAR ini_path[];

extern TCHAR put_text_open[];
extern TCHAR put_text_close[];
extern int put_text_escaping;


extern int op_count_cbPT;
extern TCHAR op_items_cbPT[COUNT_CB][1025];

static RECT rcPOS_PT;
static BOOL GlobalParr; // global parameter



extern void EscapingDel(TCHAR* s);
extern void cmd_line_to_array(TCHAR* cmdLine, TCHAR** arr, int lenArr, BOOL isEscapingDel);
extern void array_to_cmd_line(TCHAR** arr, int lenArr, BOOL isEscaping, TCHAR** cmdLine);


/* Local Function Prototypes */


/*
 * set_put_text_proc - 挟み込む文字を設定
 */
static BOOL CALLBACK set_put_text_proc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static TCHAR buf[BUF_SIZE];
	static TCHAR str_open[BUF_SIZE];
	static TCHAR str_close[BUF_SIZE];
	static TCHAR str_escaping[BUF_SIZE];
	static int int_escaping;
	static TOOL_EXEC_INFO* tei;
  
	switch (uMsg) {
	case WM_INITDIALOG:
		tei = (TOOL_EXEC_INFO*)lParam;
		if (tei == NULL || tei->cmd_line == NULL) GlobalParr = TRUE;

		SendDlgItemMessage(hDlg, IDC_COMBO_OPEN, (UINT)CB_RESETCONTENT, 0, 0);

		if (GlobalParr) {
			lstrcpy(str_open, put_text_open);
			lstrcpy(str_close, put_text_close);
			int_escaping = put_text_escaping;
			if (int_escaping) {
				*str_escaping = TEXT('1');
			}
			else {				
				*str_escaping = TEXT('0');
			}
			*(str_escaping + 1) = TEXT('\0');
		} 
		else {
			TCHAR* Arr[3];
			Arr[0] = str_open;
			Arr[1] = str_close;
			Arr[2] = str_escaping;
			cmd_line_to_array(tei->cmd_line, Arr, 3, FALSE);
			int_escaping = str_escaping[0] == TEXT('1');
		}

		SendDlgItemMessage(hDlg, IDC_COMBO_OPEN, WM_SETTEXT, 0, (LPARAM)str_open);
		SendDlgItemMessage(hDlg, IDC_EDIT_CLOSE, WM_SETTEXT, 0, (LPARAM)str_close);
		CheckDlgButton(hDlg, IDC_EDIT_ESCAPING, int_escaping);
				
		for (int i = 0; i < op_count_cbPT && i < COUNT_CB; ++i) {
			SendDlgItemMessage(hDlg, IDC_COMBO_OPEN, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)op_items_cbPT[i]);
		}
		
		if (!(rcPOS_PT.left == 0 && rcPOS_PT.bottom == 0 && rcPOS_PT.right == 0 && rcPOS_PT.top == 0)) {
			SetWindowPos(hDlg, HWND_TOP, rcPOS_PT.left, rcPOS_PT.top, 0, 0, SWP_NOSIZE);
		}
		break;


	case WM_CLOSE:
		GetWindowRect(hDlg, &rcPOS_PT);
		EndDialog(hDlg, FALSE);
		break;
	

	case WM_TIMER:
		switch (wParam) {
		case ID_PUT_TEXT_TIMER:
			KillTimer(hDlg, wParam);
			SendDlgItemMessage(hDlg, IDC_COMBO_OPEN, WM_SETTEXT, 0, (LPARAM)str_open);
			SendDlgItemMessage(hDlg, IDC_COMBO_OPEN, CB_SETEDITSEL, 0, 0xFFFF0000);
			SendDlgItemMessage(hDlg, IDC_EDIT_CLOSE, WM_SETTEXT, 0, (LPARAM)str_close);
			CheckDlgButton(hDlg, IDC_EDIT_ESCAPING, int_escaping);
			break;
		}

		break;


	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		

		case IDC_COMBO_OPEN:
			switch (HIWORD(wParam)) {
			case CBN_SELENDOK:
			{						
				TCHAR* txt = mem_calloc(2050, sizeof(TCHAR));
				int idxsel = SendDlgItemMessage(hDlg, IDC_COMBO_OPEN, CB_GETCURSEL, 0, 0);
				if (idxsel != CB_ERR) {							
					SendDlgItemMessage(hDlg, IDC_COMBO_OPEN, CB_GETLBTEXT, (WPARAM)idxsel, (LPARAM)txt);
					TCHAR* Arr[3];
					Arr[0] = str_open;
					Arr[1] = str_close;
					Arr[2] = str_escaping;
					cmd_line_to_array(txt, Arr, 3, FALSE);
					int_escaping = str_escaping[0] == TEXT('1');					
					SendDlgItemMessage(hDlg, IDC_COMBO_OPEN, (UINT)CB_SETCURSEL, (WPARAM)-1, (LPARAM)0);					
				}								
			}

			case CBN_CLOSEUP:		
				KillTimer(hDlg, ID_PUT_TEXT_TIMER);
				SetTimer(hDlg, ID_PUT_TEXT_TIMER, 100, NULL);							
				break;
			}
			break;	


		case IDOK:
			GetWindowRect(hDlg, &rcPOS_PT);
			{				
				TCHAR* txt = mem_calloc(2050, sizeof(TCHAR));
				int idxsel = SendDlgItemMessage(hDlg, IDC_COMBO_OPEN, CB_GETCURSEL, 0, 0);
				if (idxsel != CB_ERR) {
					SendDlgItemMessage(hDlg, IDC_COMBO_OPEN, WM_GETTEXT, (WPARAM)(BUF_SIZE - 1), (LPARAM)str_open);
				}
				else {
					GetWindowText(GetDlgItem(hDlg, IDC_COMBO_OPEN), str_open, BUF_SIZE);
				}

				SendDlgItemMessage(hDlg, IDC_EDIT_CLOSE, WM_GETTEXT, (WPARAM)(BUF_SIZE - 1), (LPARAM)str_close);
				int_escaping = IsDlgButtonChecked(hDlg, IDC_EDIT_ESCAPING);
				if (int_escaping) {
					*str_escaping = TEXT('1');
				}
				else {
					*str_escaping = TEXT('0');
				}
				*(str_escaping + 1) = TEXT('\0');


				TCHAR* Arr[3];
				Arr[0] = str_open;
				Arr[1] = str_close;
				Arr[2] = str_escaping;
				array_to_cmd_line(Arr, 3, FALSE, &txt);

				if (*txt != TEXT('\0'))
				{
					int cnt = SendDlgItemMessage(hDlg, IDC_COMBO_OPEN, (UINT)CB_GETCOUNT, (WPARAM)0, (LPARAM)0);
					int f = -1;
					TCHAR ftxt[1025];
					for (int i = 0; i < cnt; ++i) {
						int lstr = SendDlgItemMessage(hDlg, IDC_COMBO_OPEN, CB_GETLBTEXTLEN, (WPARAM)i, (LPARAM)0);
						if (lstr < 1025) {
							SendDlgItemMessage(hDlg, IDC_COMBO_OPEN, CB_GETLBTEXT, (WPARAM)i, (LPARAM)ftxt);
							if (lstrcmp(txt, ftxt) == 0) {
								f = i;
								if (i > 0) {
									SendDlgItemMessage(hDlg, IDC_COMBO_OPEN, (UINT)CB_DELETESTRING, (WPARAM)i, (LPARAM)0); // del
									SendDlgItemMessage(hDlg, IDC_COMBO_OPEN, (UINT)CB_INSERTSTRING, (WPARAM)0, (LPARAM)txt); // insert 0
									SendDlgItemMessage(hDlg, IDC_COMBO_OPEN, (UINT)CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
								}
								break;
							}
						}
					}

					if (f == -1) {
						SendDlgItemMessage(hDlg, IDC_COMBO_OPEN, (UINT)CB_INSERTSTRING, (WPARAM)0, (LPARAM)txt); // insert 0
						SendDlgItemMessage(hDlg, IDC_COMBO_OPEN, (UINT)CB_DELETESTRING, (WPARAM)COUNT_CB, (LPARAM)0); // only COUNT_CB str
						SendDlgItemMessage(hDlg, IDC_COMBO_OPEN, (UINT)CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
					}

					op_count_cbPT = SendDlgItemMessage(hDlg, IDC_COMBO_OPEN, (UINT)CB_GETCOUNT, (WPARAM)0, (LPARAM)0);
				}


#ifdef UNICODE
				// For unicode to create a file if it does not exist
				if (!file_check_file(ini_path))
				{
					BYTE bt[2]; bt[0] = 0xFF; bt[1] = 0xFE; // UTF-16 LE BOM				
					file_write_buf(ini_path, bt, 2, NULL);
				}
#endif

				TCHAR buf[1025];
				profile_initialize(ini_path, TRUE);
				profile_write_int(TEXT("put_text"), TEXT("count_cbPT"), op_count_cbPT, ini_path);
				TCHAR itemname[256];
				for (int i = 0; i < op_count_cbPT && i < COUNT_CB; ++i) {
					wsprintf(itemname, TEXT("item_cbPT_%d"), i);
					SendDlgItemMessage(hDlg, IDC_COMBO_OPEN, (UINT)CB_GETLBTEXT, (WPARAM)i, (LPARAM)op_items_cbPT[i]);
					profile_write_string(TEXT("put_text"), itemname, op_items_cbPT[i], ini_path);
				}

				if (GlobalParr) {					
					lstrcpy(put_text_open, str_open);
					lstrcpy(put_text_close, str_close);
					put_text_escaping = int_escaping;
					profile_write_string(TEXT("put_text"), TEXT("open"), put_text_open, ini_path);
					profile_write_string(TEXT("put_text"), TEXT("close"), put_text_close, ini_path);					
					profile_write_int(TEXT("put_text"), TEXT("escaping"), put_text_escaping, ini_path);
				}
				else {
					tei->cmd_line = alloc_copy(txt);
				}
				profile_flush(ini_path); profile_free();

				mem_free(&txt);
			}
			EndDialog(hDlg, TRUE);			
			break;

		case IDCANCEL:
			GetWindowRect(hDlg, &rcPOS_PT);
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
 * item_put_text - テキストの挟み込み
 */
static int item_put_text(DATA_INFO *di, const TCHAR *str_open, const TCHAR *str_close)
{
	HANDLE ret;
	BYTE *from_mem, *to_mem;
	TCHAR *r;
	int size;

	// コピー元ロック
	if ((from_mem = GlobalLock(di->data)) == NULL) {
		return TOOL_ERROR;
	}

	// サイズを取得
	size = lstrlen((TCHAR *)str_open) + lstrlen((TCHAR *)from_mem) + lstrlen(str_close) + 1;

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

	// 挟み込み
	r = lstrcpy((TCHAR *)to_mem, str_open) + lstrlen(str_open);
	r = lstrcpy(r, (TCHAR *)from_mem) + lstrlen((TCHAR *)from_mem);
	lstrcpy(r, str_close);

	GlobalUnlock(ret);
	GlobalUnlock(di->data);

	GlobalFree(di->data);
	di->data = ret;
	di->size = sizeof(TCHAR) * size;
	return TOOL_DATA_MODIFIED;
}






/*
 * put_text - テキストの挟み込み
 */
__declspec(dllexport) int CALLBACK put_text(const HWND hWnd, TOOL_EXEC_INFO *tei, TOOL_DATA_INFO *tdi)
{
	DATA_INFO *di;
	TCHAR str_open[BUF_SIZE];
	TCHAR str_close[BUF_SIZE];
	TCHAR str_escaping[BUF_SIZE];
	str_escaping[0] = TEXT('0');
	str_escaping[1] = TEXT('\0');
  
	TCHAR *p, *r, *r2;
	int ret = TOOL_SUCCEED;


	TCHAR* Arr[3];
	Arr[0] = str_open;
	Arr[1] = str_close;
	Arr[2] = str_escaping;


	if (tei->cmd_line != NULL && *tei->cmd_line != TEXT('\0')) {

		cmd_line_to_array(tei->cmd_line, Arr, 2, FALSE);
		if (str_escaping[0] == TEXT('1')) {
			EscapingDel(str_open);
			EscapingDel(str_close);
		}

	} else {
		if ((tei->cmd_line == NULL || *tei->cmd_line == TEXT('\0')) &&
			((tei->call_type & CALLTYPE_MENU) || (tei->call_type & CALLTYPE_VIEWER))) {
			GlobalParr = TRUE;
			if (DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOG_PUT_TEXT), GetForegroundWindow(), set_put_text_proc, 0) == FALSE) {
				return TOOL_CANCEL;
			}
		}
    
		lstrcpy(str_open, put_text_open);
		lstrcpy(str_close, put_text_close);
      
		if( put_text_escaping == 1 ) {     
			EscapingDel(str_open);
			EscapingDel(str_close);
		}   
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
			ret |= item_put_text(di, str_open, str_close);
		}
	}
	return ret;
}

/*
 * put_text_property GLOBAL - プロパティ表示
 */
__declspec(dllexport) BOOL CALLBACK put_text_property(const HWND hWnd, TOOL_EXEC_INFO *tei)
{
	GlobalParr = TRUE;
	DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOG_PUT_TEXT), hWnd, set_put_text_proc, (LPARAM)tei);
	return TRUE;
}


/*
 * put_text_property - プロパティ表示
 */
__declspec(dllexport) BOOL CALLBACK put_text_property2(const HWND hWnd, TOOL_EXEC_INFO* tei)
{
	GlobalParr = FALSE;
	DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOG_PUT_TEXT), hWnd, set_put_text_proc, (LPARAM)tei);
	return TRUE;
}

/* End of source */
