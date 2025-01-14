/*
 * CLCLSet
 *
 * SetHistory.c
 *
 * Copyright (C) 1996-2019 by Ohno Tomoaki. All rights reserved.
 *		https://www.nakka.com/
 *		nakka@nakka.com
 */

/* Include Files */
#define _INC_OLE
#include <windows.h>
#undef  _INC_OLE
#include <commctrl.h>

#include "..\General.h"
#include "..\Ini.h"
#include "..\Window.h"

#include "CLCLSet.h"

#include "resource.h"

/* Define */

/* Global Variables */
extern HINSTANCE hInst;
extern int prop_ret;

// オプション
extern OPTION_INFO option;


extern BOOL centered_desktop;


/* Local Function Prototypes */






/*
 * set_histroy_proc - 履歴設定のプロシージャ
 */
BOOL CALLBACK set_histroy_proc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {

	case WM_INITDIALOG:	
		// 2023-02-12 kaiu@mail.ru display in the center of the desktop	
		if (centered_desktop) {
			HWND wndDl = FindWindow(TEXT("#32770"), message_get_res(IDS_OPTION_TITLE));
			window_centered_desktop(wndDl);
			SetWindowPos(wndDl, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
			centered_desktop = FALSE;
		}

		// スピンコントロールの設定
		SendDlgItemMessage(hDlg, IDC_SPIN_MAX, UDM_SETRANGE, 0, (LPARAM)MAKELONG(UD_MAXVAL, 1));
		SendDlgItemMessage(hDlg, IDC_SPIN_ADD_INTERVAL, UDM_SETRANGE, 0, (LPARAM)MAKELONG(UD_MAXVAL, 0));

		SetDlgItemInt(hDlg, IDC_EDIT_MAX, option.history_max, FALSE);
		SetDlgItemInt(hDlg, IDC_EDIT_ADD_INTERVAL, option.history_add_interval, FALSE);

		CheckDlgButton(hDlg, IDC_CHECK_SAVE, option.history_save);
		CheckDlgButton(hDlg, IDC_CHECK_ALWAYS_SAVE, option.history_always_save);
		CheckDlgButton(hDlg, IDC_CHECK_DELETE, option.history_delete);
		CheckDlgButton(hDlg, IDC_CHECK_IGNORE_REGIST_ITEM, option.history_ignore_regist_item);
		// 2023-02-06 kaiu@mail.ru
		CheckDlgButton(hDlg, IDC_CHECK_IGNORE_CLIPBOARD_ONACTIVATED, option.history_ignore_clipboard_onactivated);
		


		switch (option.history_overlap_check) {
		case 0:
		default:
			CheckDlgButton(hDlg, IDC_RADIO_CHECK_NO, 1);
			break;
		case 1:
			CheckDlgButton(hDlg, IDC_RADIO_CHECK_ONE, 1);
			break;
		case 2:
			CheckDlgButton(hDlg, IDC_RADIO_CHECK_ALL, 1);
			break;
		case 3:
			CheckDlgButton(hDlg, IDC_RADIO_CHECK_ALL_DELETE, 1);
			break;
		}

		EnableWindow(GetDlgItem(hDlg, IDC_CHECK_ALWAYS_SAVE),
			IsDlgButtonChecked(hDlg, IDC_CHECK_SAVE));
		break;

	case WM_NOTIFY:

		switch (((LPNMHDR)lParam)->code)
		{
		case PSN_SETACTIVE:
			option.main_start_page_option = 0;
			break;
		case PSN_RESET:
			break;
		case PSN_QUERYCANCEL:
			break;
		case PSN_KILLACTIVE:
			break;
		}

		return OptionNotifyProc(hDlg, uMsg, wParam, lParam);

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_CHECK_SAVE:
			EnableWindow(GetDlgItem(hDlg, IDC_CHECK_ALWAYS_SAVE),
				IsDlgButtonChecked(hDlg, IDC_CHECK_SAVE));
			break;

		case IDOK:			
			option.history_max = GetDlgItemInt(hDlg, IDC_EDIT_MAX, NULL, FALSE);
			option.history_add_interval = GetDlgItemInt(hDlg, IDC_EDIT_ADD_INTERVAL, NULL, FALSE);

			option.history_save = IsDlgButtonChecked(hDlg, IDC_CHECK_SAVE);
			option.history_always_save = IsDlgButtonChecked(hDlg, IDC_CHECK_ALWAYS_SAVE);
			option.history_delete = IsDlgButtonChecked(hDlg, IDC_CHECK_DELETE);
			option.history_ignore_regist_item = IsDlgButtonChecked(hDlg, IDC_CHECK_IGNORE_REGIST_ITEM);
			// 2023-02-06 kaiu@mail.ru
			option.history_ignore_clipboard_onactivated = IsDlgButtonChecked(hDlg, IDC_CHECK_IGNORE_CLIPBOARD_ONACTIVATED);

			if (IsDlgButtonChecked(hDlg, IDC_RADIO_CHECK_ONE) == 1) {
				option.history_overlap_check = 1;
			} else if (IsDlgButtonChecked(hDlg, IDC_RADIO_CHECK_ALL) == 1) {
				option.history_overlap_check = 2;
			} else if (IsDlgButtonChecked(hDlg, IDC_RADIO_CHECK_ALL_DELETE) == 1) {
				option.history_overlap_check = 3;
			} else {
				option.history_overlap_check = 0;
			}
			prop_ret = 1;
			break;

		case IDPCANCEL:			
			break;
		}
		break;

	default:
		return FALSE;
	}
	return TRUE;
}
/* End of source */
