/*
 * tool_text
 *
 * tool_text.c
 *
 * Copyright (C) 1996-2003 by Nakashima Tomoaki. All rights reserved.
 *		http://www.nakka.com/
 *		nakka@nakka.com
 */

/* Include Files */


#define _INC_OLE
#include <windows.h>
#undef  _INC_OLE

// Input history length
#ifndef COUNT_CB 
#define COUNT_CB		25
#endif

#ifndef BUF_SIZE 
#define BUF_SIZE 256
#endif

#include "..\CLCLPlugin.h"
#include "File.h"
#include "Font.h"
#include "Profile.h"
#include "Message.h"
#include "resource.h"



/* Define */
#define	INI_FILE_NAME TEXT("tool_text.ini")

/* Global Variables */
HINSTANCE hInst;
TCHAR ini_path[BUF_SIZE];

TCHAR date_format[BUF_SIZE];
TCHAR time_format[BUF_SIZE];

TCHAR quote_char[BUF_SIZE];

int op_count_cbQ = 0;
TCHAR op_items_cbQ[COUNT_CB][1025];


int word_break_break_cnt;
int word_break_tab_size;
TCHAR word_break_m_oida[BUF_SIZE];
TCHAR word_break_m_bura[BUF_SIZE];
TCHAR word_break_s_oida[BUF_SIZE];
TCHAR word_break_s_bura[BUF_SIZE];

TCHAR put_text_open[BUF_SIZE];
TCHAR put_text_close[BUF_SIZE];
int put_text_escaping;
int op_count_cbPT = 0;
TCHAR op_items_cbPT[COUNT_CB][1025];

int delete_crlf_delete_space;

int join_text_add_return;
int join_text_add_space;
int join_text_reverse;


TCHAR text_edit_font_name[BUF_SIZE];
int text_edit_font_size;
int text_edit_font_weight;
int text_edit_font_italic;
int text_edit_font_charset;


void EscapingDel(TCHAR* s);
void cmd_line_to_array(TCHAR* cmdLine, TCHAR** arr, int lenArr, BOOL isEscapingDel);
void array_to_cmd_line(TCHAR** arr, int lenArr, BOOL isEscaping, TCHAR** cmdLine);


/* Local Function Prototypes */
static BOOL dll_initialize(void);
static BOOL dll_uninitialize(void);




/*
 * EscapingDel - Remove escape characters - エスケープ文字を削除する
 */
void EscapingDel(TCHAR* s)
{
	TCHAR* d = s; // destination = source 

	while (*s != TEXT('\0'))
	{
		if (d != s)
			*d = *s;

		if (*s == TEXT('\\'))
		{
			switch (*(s + 1))
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
* cmd_line_to_array - split the command line into parts (comma separator)
* arr - array TCHAR [BUF_SIZE]
*/
void cmd_line_to_array(TCHAR* cmdLine, TCHAR** arr, int lenArr, BOOL isEscapingDel) {
	// null all
	for (int i = 0; i < lenArr; ++i) {
		for (int j = 0; j < BUF_SIZE; ++j)
			arr[i][j] = TEXT('\0');
	}

	int n, mx;
	TCHAR* p, * r, * r2;
	if (cmdLine != NULL && *cmdLine != TEXT('\0')) {
		n = 0;
		mx = 0;
		for (p = cmdLine, r = arr[0]; *p != TEXT('\0'); ++p) {
			if (*p == TEXT(',')) {
				n++; if (n >= lenArr) break;
				mx = 0;
				r = arr[n];
				continue;
			}

			if (*p == TEXT('\\') && *(p + 1) == TEXT(',')) p++;

			if (mx < BUF_SIZE) {
				*(r++) = *p;
				mx++;
			}
		}

		if (isEscapingDel) {
			for (int i = 0; i < lenArr; ++i) {
				EscapingDel(arr[i]);
			}
		}
	}
}



/*
* array_to_cmd_line - concatenate an array of strings on the command line (comma separator)
* arr - array TCHAR [BUF_SIZE]
*/
void array_to_cmd_line(TCHAR** arr, int lenArr, BOOL isEscaping, TCHAR** cmdLine) {
	int lenL = 0;
	for (int i = 0; i < lenArr; ++i) {
		lenL += lstrlen(arr[i]);
	}

	if (*cmdLine != NULL) {
		TCHAR* buf = mem_calloc(2 * lenL + sizeof(TCHAR) + 2 * (lenArr) * sizeof(TCHAR), sizeof(TCHAR));
		TCHAR* p = buf, * r;
		if (p != NULL) {
			for (int i = 0; i < lenArr; ++i) {
				r = arr[i];
				for (int j = 0; *r != TEXT('\0'); ++j) {
					if (*r == TEXT(',')) {
						*p++ = TEXT('\\');
						*p++ = *r++;
					}
					else if (isEscaping) {
						if (*r == TEXT('\\')) {
							*p++ = TEXT('\\');
							*p++ = TEXT('\\');
							r++;
						}
						else if (*r == TEXT('\t')) {
							*p++ = TEXT('\\');
							*p++ = TEXT('t');
							r++;
						}
						else if (*r == TEXT('\r')) {
							*p++ = TEXT('\\');
							*p++ = TEXT('r');
							r++;
						}
						else if (*r == TEXT('\n')) {
							*p++ = TEXT('\\');
							*p++ = TEXT('n');
							r++;
						}
						else
							*p++ = *r++;
					}
					else
						*p++ = *r++;
				}
				if (i < (lenArr - 1))
					*p++ = TEXT(',');
			}
			*p = TEXT('\0');

			mem_free(cmdLine);
			*cmdLine = alloc_copy(buf);
			mem_free(&buf);
		}
		else {
			mem_free(cmdLine);
			*cmdLine = mem_calloc(1, sizeof(TCHAR));
			**cmdLine = TEXT('\0');
		}
	}
}


/*
 * DllMain - メイン
 */
int WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, PVOID pvReserved)
{
	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
		hInst = hInstance;
		dll_initialize();
		break;

	case DLL_PROCESS_DETACH:
		dll_uninitialize();
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

/*
 * dll_initialize - 初期化
 */
static BOOL dll_initialize(void)
{
	TCHAR app_path[BUF_SIZE];
	TCHAR *p, *r;

	GetModuleFileName(hInst, app_path, BUF_SIZE - 1);
	for (p = r = app_path; *p != TEXT('\0'); p++) {
#ifndef UNICODE
		if (IsDBCSLeadByte((BYTE)*p) == TRUE) {
			p++;
			continue;
		}
#endif	// UNICODE
		if (*p == TEXT('\\') || *p == TEXT('/')) {
			r = p;
		}
	}
	*r = TEXT('\0');
	wsprintf(ini_path, TEXT("%s\\%s"), app_path, INI_FILE_NAME);

	profile_initialize(ini_path, TRUE);
	profile_get_string(TEXT("convert_date"), TEXT("date_format"), TEXT(""), date_format, BUF_SIZE - 1, ini_path);
	profile_get_string(TEXT("convert_date"), TEXT("time_format"), TEXT(""), time_format, BUF_SIZE - 1, ini_path);
		
	word_break_break_cnt = profile_get_int(TEXT("word_break"), TEXT("break_cnt"), 80, ini_path);
	word_break_tab_size = profile_get_int(TEXT("word_break"), TEXT("tab_size"), 8, ini_path);
	profile_get_string(TEXT("word_break"), TEXT("m_oida"),
		TEXT("‘“￥＄（［｛「〔《『【〈≪"), word_break_m_oida, BUF_SIZE - 1, ini_path);
	profile_get_string(TEXT("word_break"), TEXT("m_bura"),
		TEXT("、。，．？！゛゜…‥・’”：；）〕］｝〉》」』】≫々ゝゞぁぃぅぇぉっゃゅょァィゥェォッャュョー"),
		word_break_m_bura, BUF_SIZE - 1, ini_path);
	profile_get_string(TEXT("word_break"), TEXT("s_oida"), TEXT("\\$([{｢"), word_break_s_oida, BUF_SIZE - 1, ini_path);
	profile_get_string(TEXT("word_break"), TEXT("s_bura"), TEXT(",.?!%:;)]}｣｡､ﾞﾟ"), word_break_s_bura, BUF_SIZE - 1, ini_path);

	profile_get_string(TEXT("put_text"), TEXT("open"), TEXT("<TAG>"), put_text_open, BUF_SIZE - 1, ini_path);
	profile_get_string(TEXT("put_text"), TEXT("close"), TEXT("</TAG>"), put_text_close, BUF_SIZE - 1, ini_path);
	put_text_escaping = profile_get_int(TEXT("put_text"), TEXT("escaping"), 0, ini_path);
	op_count_cbPT = profile_get_int(TEXT("put_text"), TEXT("count_cbPT"), 0, ini_path);
	TCHAR itemname[256];
	for (int i = 0; i < op_count_cbPT && i < COUNT_CB; ++i) {
		wsprintf(itemname, TEXT("item_cbPT_%d"), i);
		profile_get_string(TEXT("put_text"), itemname, TEXT(""), op_items_cbPT[i], 1024, ini_path);
	}


	delete_crlf_delete_space = profile_get_int(TEXT("delete_crlf"), TEXT("delete_space"), 1, ini_path);

	join_text_add_return = profile_get_int(TEXT("join_text"), TEXT("add_return"), 1, ini_path);
	join_text_add_space = profile_get_int(TEXT("join_text"), TEXT("add_space"), 1, ini_path);
	join_text_reverse = profile_get_int(TEXT("join_text"), TEXT("reverse"), 0, ini_path);


	profile_get_string(TEXT("quote"), TEXT("char"), TEXT("> "), quote_char, BUF_SIZE - 1, ini_path);
	op_count_cbQ = profile_get_int(TEXT("quote"), TEXT("count_cbQ"), 0, ini_path);
	for (int i = 0; i < op_count_cbQ && i < COUNT_CB; ++i) {
		wsprintf(itemname, TEXT("item_cbQ_%d"), i);
		profile_get_string(TEXT("quote"), itemname, TEXT(""), op_items_cbQ[i], 1024, ini_path);
	}

	int char_set = GetTextCharset(GetDC(NULL));	
	profile_get_string( TEXT("text_edit"), TEXT("font_name"), TEXT(""), text_edit_font_name, BUF_SIZE - 1, ini_path);
	text_edit_font_size = profile_get_int(TEXT("text_edit"), TEXT("font_size"), 9, ini_path);
	text_edit_font_weight = profile_get_int(TEXT("text_edit"), TEXT("font_weight"), 0, ini_path);
	text_edit_font_italic = profile_get_int(TEXT("text_edit"), TEXT("font_italic"), 0, ini_path);
	text_edit_font_charset = profile_get_int(TEXT("text_edit"), TEXT("font_charset"), char_set, ini_path);


#ifdef UNICODE
	{
		// For unicode to create a file if it does not exist
		BOOL fex = file_check_file(ini_path);
		if (!fex)
		{
			BYTE bt[2]; bt[0] = 0xFF; bt[1] = 0xFE; // UTF-16 LE BOM				
			file_write_buf(ini_path, bt, 2, NULL);
		}
		if (!fex || profile_get_int(TEXT("text_edit"), TEXT("font_size"), -999, ini_path) == -999) {
			profile_write_string(TEXT("text_edit"), TEXT("font_name"), text_edit_font_name, ini_path);
			TCHAR buf[BUF_SIZE];
			profile_write_int(TEXT("text_edit"), TEXT("font_size"), text_edit_font_size, ini_path);
			profile_write_int(TEXT("text_edit"), TEXT("font_weight"), text_edit_font_weight, ini_path);
			profile_write_int(TEXT("text_edit"), TEXT("font_italic"), text_edit_font_italic, ini_path);
			profile_write_int(TEXT("text_edit"), TEXT("font_charset"), text_edit_font_charset, ini_path);
		}
	}
#endif

	profile_flush(ini_path); profile_free();
	return TRUE;
}

/*
 * dll_uninitialize - 終了処理
 */
static BOOL dll_uninitialize(void)
{
	return TRUE;
}

/*
 * get_tool_info_w - ツール情報取得  
 */
__declspec(dllexport) BOOL CALLBACK get_tool_info_w(const HWND hWnd, const int index, TOOL_GET_INFO *tgi)
{
	switch (index) {
	case 0:
		lstrcpy(tgi->title, message_get_res(IDS_TITLE_TOOL_0));
		lstrcpy(tgi->func_name, TEXT("convert_date"));
		lstrcpy(tgi->cmd_line, TEXT(""));
		tgi->call_type = CALLTYPE_ITEM_TO_CLIPBOARD;
		return TRUE;

	case 1:
		lstrcpy(tgi->title, message_get_res(IDS_TITLE_TOOL_1));
		lstrcpy(tgi->func_name, TEXT("to_lower"));
		lstrcpy(tgi->cmd_line, TEXT(""));
		tgi->call_type = CALLTYPE_MENU | CALLTYPE_MENU_COPY_PASTE | CALLTYPE_VIEWER;
		return TRUE;

	case 2:
		lstrcpy(tgi->title, message_get_res(IDS_TITLE_TOOL_2));
		lstrcpy(tgi->func_name, TEXT("to_upper"));
		lstrcpy(tgi->cmd_line, TEXT(""));
		tgi->call_type = CALLTYPE_MENU | CALLTYPE_MENU_COPY_PASTE | CALLTYPE_VIEWER;
		return TRUE;

	/*case 3:
		lstrcpy(tgi->title, TEXT("SJISからJISに変換(&J)"));
		lstrcpy(tgi->func_name, TEXT("sjis_to_jis"));
		lstrcpy(tgi->cmd_line, TEXT(""));
		tgi->call_type = CALLTYPE_MENU | CALLTYPE_MENU_COPY_PASTE | CALLTYPE_VIEWER;
		return TRUE;

	case 4:
		lstrcpy(tgi->title, TEXT("JISからSJISに変換(&S)"));
		lstrcpy(tgi->func_name, TEXT("jis_to_sjis"));
		lstrcpy(tgi->cmd_line, TEXT(""));
		tgi->call_type = CALLTYPE_MENU | CALLTYPE_MENU_COPY_PASTE | CALLTYPE_VIEWER;
		return TRUE;

	case 5:
		lstrcpy(tgi->title, TEXT("SJISからEUCに変換(&C)"));
		lstrcpy(tgi->func_name, TEXT("sjis_to_euc"));
		lstrcpy(tgi->cmd_line, TEXT(""));
		tgi->call_type = CALLTYPE_MENU | CALLTYPE_MENU_COPY_PASTE | CALLTYPE_VIEWER;
		return TRUE;

	case 6:
		lstrcpy(tgi->title, TEXT("EUCからSJISに変換(&I)"));
		lstrcpy(tgi->func_name, TEXT("euc_to_sjis"));
		lstrcpy(tgi->cmd_line, TEXT(""));
		tgi->call_type = CALLTYPE_MENU | CALLTYPE_MENU_COPY_PASTE | CALLTYPE_VIEWER;
		return TRUE;*/

	case 3:
		lstrcpy(tgi->title, message_get_res(IDS_TITLE_TOOL_3));
		lstrcpy(tgi->func_name, TEXT("quote"));
		lstrcpy(tgi->cmd_line, TEXT(""));
		tgi->call_type = CALLTYPE_MENU | CALLTYPE_MENU_COPY_PASTE | CALLTYPE_VIEWER;
		return TRUE;

	case 4:
		lstrcpy(tgi->title, message_get_res(IDS_TITLE_TOOL_4));
		lstrcpy(tgi->func_name, TEXT("unquote"));
		lstrcpy(tgi->cmd_line, TEXT(""));
		tgi->call_type = CALLTYPE_MENU | CALLTYPE_MENU_COPY_PASTE | CALLTYPE_VIEWER;
		return TRUE;

	case 5:
		lstrcpy(tgi->title, message_get_res(IDS_TITLE_TOOL_5));
		lstrcpy(tgi->func_name, TEXT("word_break"));
		lstrcpy(tgi->cmd_line, TEXT(""));
		tgi->call_type = CALLTYPE_MENU | CALLTYPE_MENU_COPY_PASTE | CALLTYPE_VIEWER;
		return TRUE;

	case 6:
		lstrcpy(tgi->title, message_get_res(IDS_TITLE_TOOL_6));
		lstrcpy(tgi->func_name, TEXT("put_text"));
		lstrcpy(tgi->cmd_line, TEXT(""));
		tgi->call_type = CALLTYPE_MENU | CALLTYPE_MENU_COPY_PASTE | CALLTYPE_VIEWER;
		return TRUE;

	case 7:
		lstrcpy(tgi->title, message_get_res(IDS_TITLE_TOOL_7));
		lstrcpy(tgi->func_name, TEXT("delete_crlf"));
		lstrcpy(tgi->cmd_line, TEXT(""));
		tgi->call_type = CALLTYPE_MENU | CALLTYPE_MENU_COPY_PASTE | CALLTYPE_VIEWER;
		return TRUE;

	case 8:
		lstrcpy(tgi->title, message_get_res(IDS_TITLE_TOOL_8));
		lstrcpy(tgi->func_name, TEXT("join_text"));
		lstrcpy(tgi->cmd_line, TEXT(""));
		tgi->call_type = CALLTYPE_VIEWER;
		return TRUE;

	case 9:
		lstrcpy(tgi->title, message_get_res(IDS_TITLE_TOOL_9));
		lstrcpy(tgi->func_name, TEXT("text_edit"));
		lstrcpy(tgi->cmd_line, TEXT(""));
		tgi->call_type = CALLTYPE_MENU | CALLTYPE_MENU_COPY_PASTE;
		return TRUE;
	}
	return FALSE;
}
/* End of source */
