/*
 * tool_utl
 *
 * tool_utl.c
 *
 * Copyright (C) 1996-2023 by Nakashima Tomoaki. All rights reserved.
 *		http://www.nakka.com/
 *		nakka@nakka.com
 */



#pragma comment(lib,"Winmm.lib")

/* Include Files */
//#include "tool_utl.h"
#define _INC_OLE
#include <windows.h>
#undef  _INC_OLE

#include "resource.h"
#include "Memory.h"

#include "..\CLCLPlugin.h"
#include "File.h"

/* Define */
#define	INI_FILE_NAME				TEXT("tool_utl.ini")

/* Global Variables */
HINSTANCE hInst;
TCHAR ini_path[BUF_SIZE];
TCHAR app_path[MAX_PATH];


static TCHAR wave_file[BUF_SIZE];

TCHAR multi_save_format[BUF_SIZE];
TCHAR multi_save_file_name[BUF_SIZE];

/* Local Function Prototypes */
static BOOL dll_initialize(void);
static BOOL dll_uninitialize(void);



// 2022-02-05 kaiu@mail.ru
BOOL relative_path(TCHAR* retPath)
{
	if (lstrlen(retPath) > 0 && lstrlen(retPath) > lstrlen(app_path))
	{
		TCHAR retPathCOPY[BUF_SIZE];
		lstrcpy(retPathCOPY, retPath);
		TCHAR* new_retPath = retPathCOPY;
		int lwp = lstrlen(app_path);
		int n = 0;
		for (int i = 0; i < lwp; ++i)
		{
			if (tolower(retPath[i]) == tolower(app_path[i]))
			{
				new_retPath++;
				n++;
			}
			else
			{
				break;
			}
		}

		if (n == lwp)
		{
			if (*new_retPath == '\\') new_retPath++;
			lstrcpy(retPath, new_retPath);
			return TRUE;
		}
	}

	return FALSE;
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
	TCHAR DLL_path[MAX_PATH];
	TCHAR *p, *r;
	
	//SendMessage(hInst, WM_GET_WORKPATH, 0, (LPARAM)app_path); didn't work here		
	GetModuleFileName(NULL, app_path, BUF_SIZE - 1);
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


	GetModuleFileName(hInst, DLL_path, BUF_SIZE - 1);
	for (p = r = DLL_path; *p != TEXT('\0'); p++) {
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
	wsprintf(ini_path, TEXT("%s\\%s"), DLL_path, INI_FILE_NAME);

	GetPrivateProfileString(TEXT("play_sound"), TEXT("wave_file"), TEXT("tool_dll\\tool_utl_sound3.wav"), wave_file, BUF_SIZE - 1, ini_path);

	

	GetPrivateProfileString(TEXT("multi_save"), TEXT("format"), TEXT("TEXT"), multi_save_format, BUF_SIZE - 1, ini_path);
	GetPrivateProfileString(TEXT("multi_save"), TEXT("file_name"), TEXT("%04d.txt"), multi_save_file_name, BUF_SIZE - 1, ini_path);
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
		lstrcpy(tgi->title, message_get_res(IDS_STRING2001));
		lstrcpy(tgi->func_name, TEXT("clear_clipboard"));
		lstrcpy(tgi->cmd_line, TEXT(""));
		tgi->call_type = CALLTYPE_MENU | CALLTYPE_VIEWER;
		return TRUE;

	case 1:
		lstrcpy(tgi->title, message_get_res(IDS_STRING2000));
		lstrcpy(tgi->func_name, TEXT("clear_history"));
		lstrcpy(tgi->cmd_line, TEXT(""));
		tgi->call_type = CALLTYPE_MENU | CALLTYPE_VIEWER;
		return TRUE;	

	case 2:
		lstrcpy(tgi->title, message_get_res(IDS_STRING2002));
		lstrcpy(tgi->func_name, TEXT("play_sound"));
		lstrcpy(tgi->cmd_line, TEXT(""));
		tgi->call_type = CALLTYPE_ADD_HISTORY;
		return TRUE;

	/* 2023-02-16 kaiu@mail.ru
	* removed, since 2.1.3.2 itself can make its own window on top of all
	case 3:
		lstrcpy(tgi->title, message_get_res(IDS_STRING2003));
		lstrcpy(tgi->func_name, TEXT("ztop"));
		lstrcpy(tgi->cmd_line, TEXT(""));
		tgi->call_type = CALLTYPE_VIEWER | CALLTYPE_VIEWER_OPEN;
		return TRUE;

	case 4:
		lstrcpy(tgi->title, message_get_res(IDS_STRING2004));
		lstrcpy(tgi->func_name, TEXT("unztop"));
		lstrcpy(tgi->cmd_line, TEXT(""));
		tgi->call_type = CALLTYPE_VIEWER;
		return TRUE;
    */


	//case 5:
	case 3:
		lstrcpy(tgi->title, message_get_res(IDS_STRING2005));
		lstrcpy(tgi->func_name, TEXT("multi_save"));
		lstrcpy(tgi->cmd_line, TEXT(""));
		tgi->call_type = CALLTYPE_VIEWER;
		return TRUE;
	}
	return FALSE;
}

/*
 * clear_history - 履歴のクリア
 */
__declspec(dllexport) int CALLBACK clear_history(const HWND hWnd, TOOL_EXEC_INFO *tei, TOOL_DATA_INFO *tdi)
{
	
	// 2023-02-16 kaiu@mail.ru
	HWND active_wnd = (HWND)SendMessage(hWnd, WM_GET_FOCUS_WND_POPUP_MENU, 0, 0);

	DATA_INFO* di;
	if (MessageBox(hWnd, message_get_res(IDS_STRING2006), message_get_res(IDS_STRING2007),
		MB_ICONQUESTION | MB_YESNO | MB_TOPMOST) == IDNO) {
		return TOOL_SUCCEED;
	}
	// 履歴の取得
	di = (DATA_INFO*)SendMessage(hWnd, WM_HISTORY_GET_ROOT, 0, 0);
	if (di != NULL) {
		// 履歴の解放
		SendMessage(hWnd, WM_ITEM_FREE, 0, (LPARAM)di->child);
		di->child = NULL;
		// 履歴の変化を通知
		SendMessage(hWnd, WM_HISTORY_CHANGED, 0, 0);
	}

	DWORD ActiveThread, WndThread;
	WndThread = GetWindowThreadProcessId(active_wnd, NULL);
	ActiveThread = GetWindowThreadProcessId(hWnd, NULL);
	BOOL Attached = FALSE;
	if (ActiveThread != WndThread) {
		Attached = AttachThreadInput(WndThread, ActiveThread, TRUE);
	}

	if (Attached) {
		SetFocus(active_wnd);
		AttachThreadInput(WndThread, ActiveThread, FALSE);
	}

	
	return TOOL_SUCCEED;
}


// 2023-02-16 kaiu@mail.ru NOT DISPLAYED IN DLL (look at the file .def)
/*
 * clear_history_property - プロパティ表示
 */
__declspec(dllexport) BOOL CALLBACK clear_history_property(const HWND hWnd, TOOL_EXEC_INFO *tei)
{
	return FALSE;
}

/*
 * clear_clipboard - クリップボードのクリア
 */
__declspec(dllexport) int CALLBACK clear_clipboard(const HWND hWnd, TOOL_EXEC_INFO *tei, TOOL_DATA_INFO *tdi)
{

	
	// 2023-02-16 kaiu@mail.ru
	HWND active_wnd = (HWND)SendMessage(hWnd, WM_GET_FOCUS_WND_POPUP_MENU, 0, 0);
	
	
	if (OpenClipboard(hWnd) == TRUE) {
		int clw = SendMessage(hWnd, WM_GET_CLIPBOARD_WATCH, 0, 0);
		if (clw == 0)
			SendMessage(hWnd, WM_SET_CLIPBOARD_WATCH, TRUE, 0);
		EmptyClipboard();
		CloseClipboard();		
		if (clw == 0) {
			Sleep(50);
			SendMessage(hWnd, WM_SET_CLIPBOARD_WATCH, FALSE, 0);
		}
	}


	DWORD ActiveThread, WndThread;
	WndThread = GetWindowThreadProcessId(active_wnd, NULL);
	ActiveThread = GetWindowThreadProcessId(hWnd, NULL);
	BOOL Attached = FALSE;
	if (ActiveThread != WndThread) {
		Attached = AttachThreadInput(WndThread, ActiveThread, TRUE);
	}

	if (Attached) {
		SetFocus(active_wnd);
		AttachThreadInput(WndThread, ActiveThread, FALSE);
	}


	
	return TOOL_SUCCEED;
}

// 2023-02-16 kaiu@mail.ru NOT DISPLAYED IN DLL (look at the file .def)
/*
 * clear_clipboard_property - プロパティ表示
 */
__declspec(dllexport) BOOL CALLBACK clear_clipboard_property(const HWND hWnd, TOOL_EXEC_INFO *tei)
{
	return FALSE;
}

/*
 * play_sound - WAVEファイルを鳴らす
 */
__declspec(dllexport) int CALLBACK play_sound(const HWND hWnd, TOOL_EXEC_INFO *tei, TOOL_DATA_INFO *tdi)
{	
	if (tei == NULL || tei->cmd_line == NULL || *tei->cmd_line == TEXT('\0')) {
		if (*wave_file == TEXT('\0') || sndPlaySound(wave_file, SND_ASYNC | SND_NODEFAULT) == FALSE) {			
			MessageBeep(MB_ICONASTERISK);
		}
	}
	else {		
		if (*tei->cmd_line == TEXT('\0') || sndPlaySound(tei->cmd_line, SND_ASYNC | SND_NODEFAULT) == FALSE) {
			MessageBeep(MB_ICONASTERISK);
		}
	}

	return TOOL_SUCCEED;
}

/*
 * play_sound_property GLOBAL - プロパティ表示
 */
__declspec(dllexport) BOOL CALLBACK play_sound_property(const HWND hWnd, TOOL_EXEC_INFO *tei)
{
	OPENFILENAME of;
	TCHAR buf[MAX_PATH];

	// ファイルの選択
	ZeroMemory(&of, sizeof(OPENFILENAME));
	of.lStructSize = sizeof(OPENFILENAME);
	of.hInstance = hInst;
	of.hwndOwner = hWnd;
	of.lpstrTitle = message_get_res(IDS_STRING2008);
	of.lpstrFilter = TEXT("*.wav\0*.wav\0*.*\0*.*\0\0");
	of.nFilterIndex = 1;
	lstrcpy(buf, wave_file);
	of.lpstrFile = buf;
	of.nMaxFile = MAX_PATH - 1;
	of.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	if (GetOpenFileName(&of) == TRUE) {
		relative_path(buf);
		lstrcpy(wave_file, buf);
		WritePrivateProfileString(TEXT("play_sound"), TEXT("wave_file"), wave_file, ini_path);
		// WritePrivateProfileString(TEXT("app"), TEXT("app_path"), app_path, ini_path);  ONLY TEST !!!
		// play file
		sndPlaySound(wave_file, SND_ASYNC | SND_NODEFAULT);
	}
	return TRUE;
}


/*
 * play_sound_property - プロパティ表示
 */
__declspec(dllexport) BOOL CALLBACK play_sound_property2(const HWND hWnd, TOOL_EXEC_INFO* tei)
{
	OPENFILENAME of;
	TCHAR buf[MAX_PATH];

	// ファイルの選択
	ZeroMemory(&of, sizeof(OPENFILENAME));
	of.lStructSize = sizeof(OPENFILENAME);
	of.hInstance = hInst;
	of.hwndOwner = hWnd;
	of.lpstrTitle = message_get_res(IDS_STRING2008);
	of.lpstrFilter = TEXT("*.wav\0*.wav\0*.*\0*.*\0\0");
	of.nFilterIndex = 1;
	lstrcpy(buf, tei->cmd_line);
	of.lpstrFile = buf;
	of.nMaxFile = MAX_PATH - 1;
	of.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	if (GetOpenFileName(&of) == TRUE) {
		relative_path(buf);		
		tei->cmd_line = alloc_copy(buf);		
		// WritePrivateProfileString(TEXT("app"), TEXT("app_path"), app_path, ini_path);  ONLY TEST !!!
		// play file
		sndPlaySound(tei->cmd_line, SND_ASYNC | SND_NODEFAULT);
	}
	return TRUE;
}





/*
 * ztop - 最前面に表示
 */
 /* 2023-02-16 kaiu@mail.ru
	 * removed, since 2.1.3.2 itself can make its own window on top of all
__declspec(dllexport) int CALLBACK ztop(const HWND hWnd, TOOL_EXEC_INFO *tei, TOOL_DATA_INFO *tdi)
{
	HWND hViewerWnd;

	hViewerWnd = (HWND)SendMessage(hWnd, WM_VIEWER_GET_HWND, 0, 0);
	if (hViewerWnd != NULL) {
		// 2023-02-06 kaiu@mail.ru
		HWND active_wnd = (HWND)SendMessage(hWnd, WM_GET_FOCUS_WND_POPUP_MENU, 0, 0);
		// 最前面に表示
		SetWindowPos(hViewerWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);

		DWORD ActiveThread, WndThread;
		WndThread = GetWindowThreadProcessId(active_wnd, NULL);
		ActiveThread = GetWindowThreadProcessId(hWnd, NULL);
		BOOL Attached = FALSE;
		if (ActiveThread != WndThread) {
			Attached = AttachThreadInput(WndThread, ActiveThread, TRUE);
		}

		if (Attached) {
			SetFocus(active_wnd);
			AttachThreadInput(WndThread, ActiveThread, FALSE);
		}
					

	}
	return TOOL_SUCCEED;
}
*/

/*
 * ztop_property - プロパティ表示
 */
 /* 2023-02-16 kaiu@mail.ru
	 * removed, since 2.1.3.2 itself can make its own window on top of all
__declspec(dllexport) BOOL CALLBACK ztop_property(const HWND hWnd, TOOL_EXEC_INFO *tei)
{
	return FALSE;
}
*/




/*
 * unztop - 最前面に表示の解除
 */
 /* 2023-02-16 kaiu@mail.ru
	 * removed, since 2.1.3.2 itself can make its own window on top of all
__declspec(dllexport) int CALLBACK unztop(const HWND hWnd, TOOL_EXEC_INFO *tei, TOOL_DATA_INFO *tdi)
{	
	// 2023-02-06 kaiu@mail.ru
	HWND active_wnd = (HWND)SendMessage(hWnd, WM_GET_FOCUS_WND_POPUP_MENU, 0, 0);

	HWND hViewerWnd = (HWND)SendMessage(hWnd, WM_VIEWER_GET_HWND, 0, 0);
	SetWindowPos(hViewerWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);

	DWORD ActiveThread, WndThread;
	WndThread = GetWindowThreadProcessId(active_wnd, NULL);
	ActiveThread = GetWindowThreadProcessId(hWnd, NULL);
	BOOL Attached = FALSE;
	if (ActiveThread != WndThread)
	{
		Attached = AttachThreadInput(WndThread, ActiveThread, TRUE);
	}

	if (Attached)
	{
		SetFocus(active_wnd);
		AttachThreadInput(WndThread, ActiveThread, FALSE);
	}


	return TOOL_SUCCEED;
}
*/





/*
 * unztop_property - プロパティ表示
 */
 /* 2023-02-16 kaiu@mail.ru
	 * removed, since 2.1.3.2 itself can make its own window on top of all
__declspec(dllexport) BOOL CALLBACK unztop_property(const HWND hWnd, TOOL_EXEC_INFO *tei)
{
	return FALSE;
}
*/


/* End of source */
