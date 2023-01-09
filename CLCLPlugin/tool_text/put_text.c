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

#include "..\CLCLPlugin.h"
#include "File.h"
#include "resource.h"

/* Define */

/* Global Variables */
extern HINSTANCE hInst;
extern TCHAR ini_path[];

extern TCHAR put_text_open[];
extern TCHAR put_text_close[];
extern int put_text_escaping;

/* Local Function Prototypes */


/*
 * EscapingDel - Remove escape characters - エスケープ文字を削除する
 */
void EscapingDel(TCHAR *s)
{
	TCHAR *d = s; // destination = source 
  
	while( *s != TEXT('\0') )
	{
		if( d != s )
			*d = *s;
        
		if ( *s == TEXT('\\') )
		{
			switch( *(s+1) )
			{
			case TEXT('\\'):
				s++;
				//*d = TEXT('\\');
				break;
          
			case TEXT('t'):
				s++;
				*d = TEXT('\t');
				break;
          
			case TEXT('r'):
				s++;
				*d = TEXT('\r');
				break;  
          
			case TEXT('n'):
				s++;
				*d = TEXT('\n');
				break;  
			}          
		}
    
		s++;
		d++;
	}
  
	*d = TEXT('\0');
}
  



/*
 * set_put_text_proc - 挟み込む文字を設定
 */
static BOOL CALLBACK set_put_text_proc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  TCHAR buf[BUF_SIZE];
  
	switch (uMsg) {
	case WM_INITDIALOG:
		SendDlgItemMessage(hDlg, IDC_EDIT_OPEN, WM_SETTEXT, 0, (LPARAM)put_text_open);
		SendDlgItemMessage(hDlg, IDC_EDIT_CLOSE, WM_SETTEXT, 0, (LPARAM)put_text_close);
		CheckDlgButton(hDlg, IDC_EDIT_ESCAPING, put_text_escaping);
		break;

	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			SendDlgItemMessage(hDlg, IDC_EDIT_OPEN, WM_GETTEXT, BUF_SIZE - 1, (LPARAM)put_text_open);
			SendDlgItemMessage(hDlg, IDC_EDIT_CLOSE, WM_GETTEXT, BUF_SIZE - 1, (LPARAM)put_text_close);
			put_text_escaping = IsDlgButtonChecked(hDlg, IDC_EDIT_ESCAPING);

#ifdef UNICODE
			// For unicode to create a file if it does not exist
			if (!file_check_file(ini_path))
			{
				BYTE bt[2]; bt[0] = 0xFF; bt[1] = 0xFE; // UTF-16 LE BOM				
				file_write_buf(ini_path, bt, 2, NULL);
			}
#endif

			WritePrivateProfileString(TEXT("put_text"), TEXT("open"), put_text_open, ini_path);
			WritePrivateProfileString(TEXT("put_text"), TEXT("close"), put_text_close, ini_path);
      wsprintf(buf, TEXT("%d"), put_text_escaping);
			WritePrivateProfileString(TEXT("put_text"), TEXT("escaping"), buf, ini_path);
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

	if (tei->cmd_line != NULL && *tei->cmd_line != TEXT('\0')) {
		for (p = tei->cmd_line, r = str_open; *p != TEXT('\0') && *p != TEXT(','); p++) {
			if (*p == TEXT('\\') && *(p + 1) == TEXT(',')) {
				p++;
			}
			*(r++) = *p;
		}
		*r = TEXT('\0');
    
		if (*p == TEXT(',')) {
			p++;
		}
    
		for (r = str_close; *p != TEXT('\0') && *p != TEXT(','); p++) {
			if (*p == TEXT('\\') && *(p + 1) == TEXT(',')) {
				p++;
			}
			*(r++) = *p;
		}
		*r = TEXT('\0');

		if (*p == TEXT(',')) {
			p++;
		}
    
		for (r = str_escaping; *p != TEXT('\0'); p++) {
			if (*p == TEXT('\\') && *(p + 1) == TEXT(',')) {
				p++;
			}
			*(r++) = *p;
		}
		*r = TEXT('\0');
    
    
    if( str_escaping[0] == TEXT('1') ) {
		EscapingDel(str_open);
		EscapingDel(str_close);
    }      
    
    

	} else {
		if ((tei->cmd_line == NULL || *tei->cmd_line == TEXT('\0')) &&
			((tei->call_type & CALLTYPE_MENU) || (tei->call_type & CALLTYPE_VIEWER))) {
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
 * put_text_property - プロパティ表示
 */
__declspec(dllexport) BOOL CALLBACK put_text_property(const HWND hWnd, TOOL_EXEC_INFO *tei)
{
	DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOG_PUT_TEXT), hWnd, set_put_text_proc, 0);
	return TRUE;
}
/* End of source */
