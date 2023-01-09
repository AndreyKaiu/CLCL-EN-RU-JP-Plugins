/*
 * CLCL
 *
 * Viewer.c
 *
 * Copyright (C) 1996-2022 by Ohno Tomoaki. All rights reserved.
 *		https://www.nakka.com/
 *		nakka@nakka.com
 */

/* Include Files */
#define _INC_OLE
#include <windows.h>
#undef  _INC_OLE
#include <commctrl.h>
#include <commdlg.h>
#include <tchar.h>

#pragma comment(lib, "Version.lib")

#include "General.h"
#include "Memory.h"
#include "String.h"
#include "Data.h"
#include "Ini.h"
#include "Message.h"
#include "Menu.h"
#include "File.h"
#include "ClipBoard.h"
#include "History.h"
#include "Regist.h"
#include "Format.h"
#include "Filter.h"
#include "Tool.h"
#include "Viewer.h"
#include "Frame.h"
#include "ImageList.h"
#include "TreeView.h"
#include "Container.h"
#include "ListView.h"
#include "BinView.h"
#include "ToolBar.h"
#include "StatusBar.h"
#include "SelectFolder.h"
#include "SelectFormat.h"
#include "SetHotkey.h"
#include "OleDragDrop.h"
#include "ViewerOLEDnD.h"
#include "ViewerDnD.h"

#include "resource.h"

/* Define */
#define WINDOW_CLASS					TEXT("CLCLViewer")
//#define WINDOW_TITLE					TEXT("CLCL")
#define WINDOW_TITLE					MAIN_WINDOW_TITLE
#define ERROR_TITLE						TEXT("CLCL - Error")

// ѓ^ѓCѓ}Ѓ[ID
#define TIMER_SEP						1
#define TIMER_DRAG						2
#define TIMER_SET_MENU					3
#define TIMER_REFLECT_DATA				4
#define TIMER_LV_REFRESH				5
#define TIMER_TV_ACTION					6

#define POPUPMENU_CLIPBOARD				0
#define POPUPMENU_HISTORY				1
#define POPUPMENU_HISTORY_LV			2
#define POPUPMENU_REGIST				3
#define POPUPMENU_REGIST_LV				4
#define POPUPMENU_DRAGDROP				5

#define WINDOW_MENU_FILE				0
#define WINDOW_MENU_EDIT				1
#define WINDOW_MENU_VIEW				2
#define WINDOW_MENU_TOOL				3

#define ID_MENUITEM_TOOL				(WM_APP + 1000)

/* Global Variables */
static HWND main_wnd;
static HWND current_wnd;
HMENU h_popup_menu;

HTREEITEM clip_treeitem;
HTREEITEM history_treeitem;
HTREEITEM regist_treeitem;
DATA_INFO clip_di;

static BOOL save_flag = TRUE;
static BOOL DnD_mode;

// extern
extern HINSTANCE hInst;
extern DATA_INFO history_data;
extern DATA_INFO regist_data;

// ѓIѓvѓVѓ‡ѓ“
extern OPTION_INFO option;

/* Local Function Prototypes */
static void set_cursor(const BOOL wiat_flag);
static void set_enable_window_menu(const HWND hWnd);
static void set_enadle_popup_menu(const HWND hWnd, const HMENU hMenu, const int index, const HTREEITEM hItem);
static void viewer_show_menu(const HWND hWnd);
static void viewer_show_item(const HWND hWnd);
static void viewer_item_activate(const HWND hWnd);
static void treeview_to_clipboard(const HWND hWnd, const HTREEITEM sel_item);
static void viewer_regist_add(const HWND hWnd, const HTREEITEM sel_item, const BOOL regist_move);
static BOOL viewer_move_up(const HWND hWnd, const HTREEITEM sel_item);
static BOOL viewer_move_down(const HWND hWnd, const HTREEITEM sel_item);
static void viewer_create_item(const HWND hWnd, const HTREEITEM sel_item);
static void viewer_create_folder(const HWND hWnd, const HTREEITEM sel_item);
static void viewer_data_save(const HWND hWnd, const HTREEITEM sel_item);
static BOOL viewer_import_item(const HWND hWnd);
static BOOL viewer_export_item(const HWND hWnd);
static BOOL viewer_rename(const HWND hWnd, const HTREEITEM sel_item, TCHAR *title);
static BOOL viewer_clear_name(const HWND hWnd, const HTREEITEM sel_item);
static BOOL viewer_set_hotkey(const HWND hWnd, const HTREEITEM sel_item);
static void viewer_delete_item(const HWND hWnd, const HTREEITEM sel_item);
static BOOL viewer_format_to_option(const HWND hWnd, const HTREEITEM sel_item, const TCHAR *mode);
static void viewer_tool_execute(const HWND hWnd, const HTREEITEM sel_item, const int index);
static void viewer_set_datetime(const HWND hWnd, const HTREEITEM hItem);
static DATA_INFO *clipboard_to_datainfo(const HWND hWnd);
static void viewer_save_data(const HWND hWnd, const HTREEITEM hItem);
static void viewer_set_list_column(const HWND hTreeView, const HWND hListView, const HTREEITEM hItem);
static BOOL viewer_sel_cheange(const HWND hWnd, const HTREEITEM old_item, const HTREEITEM new_item);
static BOOL viewer_initialize(const HWND hWnd);
static void viewer_set_controls(const HWND hWnd);
static BOOL viewer_close(const HWND hWnd);
static LRESULT CALLBACK viewer_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

/*
 * set_cursor - ѓJЃ[ѓ\ѓ‹‚рђЭ’и
 */
static void set_cursor(const BOOL wiat_flag)
{
	static HCURSOR old_cursor;

	if (wiat_flag == TRUE) {
		// Ќ»ЋћЊvѓJЃ[ѓ\ѓ‹‚Й‚·‚й
		old_cursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
	} else {
		if (old_cursor == NULL) {
			SetCursor(LoadCursor(NULL, IDC_ARROW));
		} else {
			// Њі‚МѓJЃ[ѓ\ѓ‹‚Й–Я‚·
			SetCursor(old_cursor);
			old_cursor = NULL;
		}
	}
}

/*
 * set_enable_window_menu - ѓЃѓjѓ…Ѓ[ЌЂ–Ъ‚МЋg—p‰В”\¤ Ћg—p•s”\‚рђЭ’и
 */
static void set_enable_window_menu(const HWND hWnd)
{
	HWND hTreeView = GetDlgItem(hWnd, ID_TREE);
	HWND hListView = GetDlgItem(GetDlgItem(hWnd, ID_CONTAINER), ID_LIST);
	HTREEITEM hItem;
	HTREEITEM wk_item;
	DATA_INFO *di;
	UINT enable;
	BOOL tb_enable;
	BOOL sel_flag;
	BOOL folder_flag;
	int i;

	if (GetFocus() == hListView) {
		sel_flag = (ListView_GetSelectedCount(hListView) != 0) ? TRUE : FALSE;
		folder_flag = FALSE;
		i = -1;
		while ((i = ListView_GetNextItem(hListView, i, LVNI_SELECTED)) != -1) {
			wk_item = (HTREEITEM)listview_get_lparam(hListView, i);
			if (wk_item != NULL &&
				(di = (DATA_INFO *)treeview_get_lparam(hTreeView, wk_item)) != NULL &&
				di->type == TYPE_FOLDER) {
				folder_flag = TRUE;
				break;
			}
		}
		// menu
		enable = (sel_flag == FALSE || folder_flag == TRUE) ? MF_GRAYED : MF_ENABLED;
		EnableMenuItem(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_FILE), ID_MENUITEM_DATA_SAVE, enable);
		EnableMenuItem(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_EDIT), ID_MENUITEM_CLIPBOARD, enable);

		enable = (regist_treeitem == NULL) ? MF_GRAYED : MF_ENABLED;
		EnableMenuItem(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_FILE), ID_MENUITEM_IMPORT, enable);
		EnableMenuItem(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_FILE), ID_MENUITEM_EXPORT, enable);

		enable = (sel_flag == FALSE || regist_treeitem == NULL) ? MF_GRAYED : MF_ENABLED;
		EnableMenuItem(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_EDIT), ID_MENUITEM_REGIST_ADD, enable);

		enable = (sel_flag == FALSE) ? MF_GRAYED : MF_ENABLED;
		EnableMenuItem(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_EDIT), ID_MENUITEM_UP, enable);
		EnableMenuItem(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_EDIT), ID_MENUITEM_DOWN, enable);
		EnableMenuItem(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_EDIT), ID_MENUITEM_DELETE, enable);

		EnableMenuItem(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_EDIT), ID_MENUITEM_PASTE, MF_ENABLED);
		EnableMenuItem(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_FILE), ID_MENUITEM_NEW_ITEM, MF_ENABLED);
		EnableMenuItem(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_EDIT), ID_MENUITEM_SELECT_ALL, MF_ENABLED);

		// tool bar
		tb_enable = (sel_flag == FALSE || folder_flag == TRUE) ? FALSE : TRUE;
		SendDlgItemMessage(hWnd, ID_TOOLBAR, TB_ENABLEBUTTON, ID_MENUITEM_CLIPBOARD_TB, (LPARAM)MAKELONG(tb_enable, 0));

		tb_enable = (sel_flag == FALSE || regist_treeitem == NULL) ? FALSE : TRUE;
		SendDlgItemMessage(hWnd, ID_TOOLBAR, TB_ENABLEBUTTON, ID_MENUITEM_REGIST_ADD_TB, (LPARAM)MAKELONG(tb_enable, 0));

		tb_enable = (sel_flag == FALSE) ? FALSE : TRUE;
		SendDlgItemMessage(hWnd, ID_TOOLBAR, TB_ENABLEBUTTON, ID_MENUITEM_DELETE_TB, (LPARAM)MAKELONG(tb_enable, 0));
		SendDlgItemMessage(hWnd, ID_TOOLBAR, TB_ENABLEBUTTON, ID_MENUITEM_UP_TB, (LPARAM)MAKELONG(tb_enable, 0));
		SendDlgItemMessage(hWnd, ID_TOOLBAR, TB_ENABLEBUTTON, ID_MENUITEM_DOWN_TB, (LPARAM)MAKELONG(tb_enable, 0));

		SendDlgItemMessage(hWnd, ID_TOOLBAR, TB_ENABLEBUTTON, ID_MENUITEM_PASTE_TB, (LPARAM)MAKELONG(TRUE, 0));
		SendDlgItemMessage(hWnd, ID_TOOLBAR, TB_ENABLEBUTTON, ID_MENUITEM_NEW_ITEM_TB, (LPARAM)MAKELONG(TRUE, 0));

	} else {
		hItem = TreeView_GetSelection(hTreeView);
		folder_flag = TRUE;
		if ((di = (DATA_INFO *)treeview_get_lparam(hTreeView, hItem)) != NULL) {
			folder_flag = (di->type == TYPE_FOLDER) ? TRUE : FALSE;
		}
		// menu
		enable = (treeview_get_rootitem(hTreeView, hItem) == clip_treeitem) ? MF_GRAYED : MF_ENABLED;
		EnableMenuItem(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_FILE), ID_MENUITEM_NEW_ITEM, enable);
		EnableMenuItem(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_EDIT), ID_MENUITEM_SELECT_ALL, enable);

		enable = (regist_treeitem == NULL) ? MF_GRAYED : MF_ENABLED;
		EnableMenuItem(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_FILE), ID_MENUITEM_IMPORT, enable);
		EnableMenuItem(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_FILE), ID_MENUITEM_EXPORT, enable);

		enable = (hItem == history_treeitem || hItem == regist_treeitem || folder_flag == TRUE) ? MF_GRAYED : MF_ENABLED;
		EnableMenuItem(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_FILE), ID_MENUITEM_DATA_SAVE, enable);

		enable = (treeview_get_rootitem(hTreeView, hItem) == clip_treeitem ||
			hItem == history_treeitem || hItem == regist_treeitem || folder_flag == TRUE) ? MF_GRAYED : MF_ENABLED;
		EnableMenuItem(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_EDIT), ID_MENUITEM_CLIPBOARD, enable);

		enable = (treeview_get_rootitem(hTreeView, hItem) == clip_treeitem || folder_flag == FALSE) ? MF_GRAYED : MF_ENABLED;
		EnableMenuItem(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_EDIT), ID_MENUITEM_PASTE, enable);

		enable = (hItem == history_treeitem || hItem == regist_treeitem || regist_treeitem == NULL) ? MF_GRAYED : MF_ENABLED;
		EnableMenuItem(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_EDIT), ID_MENUITEM_REGIST_ADD, enable);

		enable = (treeview_get_rootitem(hTreeView, hItem) == clip_treeitem ||
			hItem == history_treeitem || hItem == regist_treeitem) ? MF_GRAYED : MF_ENABLED;
		EnableMenuItem(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_EDIT), ID_MENUITEM_UP, enable);
		EnableMenuItem(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_EDIT), ID_MENUITEM_DOWN, enable);
		EnableMenuItem(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_EDIT), ID_MENUITEM_DELETE, enable);

		// tool bar
		tb_enable = (treeview_get_rootitem(hTreeView, hItem) == clip_treeitem ||
			hItem == history_treeitem || hItem == regist_treeitem || folder_flag == TRUE) ? FALSE : TRUE;
		SendDlgItemMessage(hWnd, ID_TOOLBAR, TB_ENABLEBUTTON, ID_MENUITEM_CLIPBOARD_TB, (LPARAM)MAKELONG(tb_enable, 0));

		tb_enable = (treeview_get_rootitem(hTreeView, hItem) == clip_treeitem || folder_flag == FALSE) ? FALSE : TRUE;
		SendDlgItemMessage(hWnd, ID_TOOLBAR, TB_ENABLEBUTTON, ID_MENUITEM_PASTE_TB, (LPARAM)MAKELONG(tb_enable, 0));

		tb_enable = (hItem == history_treeitem || hItem == regist_treeitem || regist_treeitem == NULL) ? FALSE : TRUE;
		SendDlgItemMessage(hWnd, ID_TOOLBAR, TB_ENABLEBUTTON, ID_MENUITEM_REGIST_ADD_TB, (LPARAM)MAKELONG(tb_enable, 0));

		tb_enable = (treeview_get_rootitem(hTreeView, hItem) == clip_treeitem) ? FALSE : TRUE;
		SendDlgItemMessage(hWnd, ID_TOOLBAR, TB_ENABLEBUTTON, ID_MENUITEM_NEW_ITEM_TB, (LPARAM)MAKELONG(tb_enable, 0));

		tb_enable = (treeview_get_rootitem(hTreeView, hItem) == clip_treeitem ||
			hItem == history_treeitem || hItem == regist_treeitem) ? FALSE : TRUE;
		SendDlgItemMessage(hWnd, ID_TOOLBAR, TB_ENABLEBUTTON, ID_MENUITEM_DELETE_TB, (LPARAM)MAKELONG(tb_enable, 0));
		SendDlgItemMessage(hWnd, ID_TOOLBAR, TB_ENABLEBUTTON, ID_MENUITEM_UP_TB, (LPARAM)MAKELONG(tb_enable, 0));
		SendDlgItemMessage(hWnd, ID_TOOLBAR, TB_ENABLEBUTTON, ID_MENUITEM_DOWN_TB, (LPARAM)MAKELONG(tb_enable, 0));
	}
}

/*
 * set_enadle_popup_menu - ѓЃѓjѓ…Ѓ[ЌЂ–Ъ‚МЋg—p‰В”\¤ Ћg—p•s”\‚рђЭ’и
 */
static void set_enadle_popup_menu(const HWND hWnd, const HMENU hMenu, const int index, const HTREEITEM hItem)
{
	HWND hTreeView = GetDlgItem(hWnd, ID_TREE);
	HWND hListView = GetDlgItem(GetDlgItem(hWnd, ID_CONTAINER), ID_LIST);
	HTREEITEM menu_item = NULL;
	HTREEITEM wk_item;
	DATA_INFO *di;
	UINT enable;
	BOOL sel_flag;
	BOOL folder_flag;
	int i;
	int def_menu = -1;

	switch (index) {
	case POPUPMENU_CLIPBOARD:
		// clipboard
		enable = (regist_treeitem != NULL) ? MF_ENABLED : MF_GRAYED;
		EnableMenuItem(GetSubMenu(hMenu, index), ID_MENUITEM_REGIST_ADD, enable);

		enable = (clip_treeitem != hItem) ? MF_ENABLED : MF_GRAYED;
		EnableMenuItem(GetSubMenu(hMenu, index), ID_MENUITEM_SET_FORMAT, enable);
		EnableMenuItem(GetSubMenu(hMenu, index), ID_MENUITEM_SET_FILTER, enable);
		return;

	case POPUPMENU_HISTORY_LV:
		def_menu = option.list_default_action;
	case POPUPMENU_HISTORY:
		// history
		menu_item = history_treeitem;
		break;

	case POPUPMENU_REGIST_LV:
		def_menu = option.list_default_action;
	case POPUPMENU_REGIST:
		// regist
		menu_item = regist_treeitem;
		break;
	}
	// ѓfѓtѓHѓ‹ѓgѓЃѓjѓ…Ѓ[ђЭ’и
	switch (def_menu) {
	case -1:
		break;
	case 0:
	default:
		SetMenuDefaultItem(GetSubMenu(hMenu, index), ID_MENUITEM_OPEN, 0);
		break;
	case 1:
		SetMenuDefaultItem(GetSubMenu(hMenu, index), ID_MENUITEM_CLIPBOARD, 0);
		break;
	case 2:
		SetMenuDefaultItem(GetSubMenu(hMenu, index), ID_MENUITEM_DATA_SAVE, 0);
		break;
	}
	if (GetFocus() == hTreeView) {
		sel_flag = (hItem != menu_item) ? TRUE : FALSE;
		enable = MF_GRAYED;
		if ((di = (DATA_INFO *)treeview_get_lparam(hTreeView, hItem)) != NULL) {
			folder_flag = (di->type == TYPE_FOLDER) ? TRUE : FALSE;
			enable = (di->type == TYPE_DATA) ? MF_ENABLED : MF_GRAYED;
		}
		EnableMenuItem(GetSubMenu(hMenu, index), ID_MENUITEM_SET_FORMAT, enable);
		EnableMenuItem(GetSubMenu(hMenu, index), ID_MENUITEM_SET_FILTER, enable);

		enable = MF_ENABLED;
		if (hItem != menu_item && (di = (DATA_INFO *)treeview_get_lparam(hTreeView, hItem)) != NULL) {
			enable = (di->type == TYPE_FOLDER) ? MF_ENABLED : MF_GRAYED;
		}
		EnableMenuItem(GetSubMenu(hMenu, index), ID_MENUITEM_PASTE, enable);

	} else if (GetFocus() == hListView) {
		sel_flag = (ListView_GetSelectedCount(hListView) != 0) ? TRUE : FALSE;
		folder_flag = FALSE;
		i = -1;
		while ((i = ListView_GetNextItem(hListView, i, LVNI_SELECTED)) != -1) {
			if ((wk_item = (HTREEITEM)listview_get_lparam(hListView, i)) != NULL &&
				(di = (DATA_INFO *)treeview_get_lparam(hTreeView, wk_item)) != NULL &&
				di->type == TYPE_FOLDER) {
				folder_flag = TRUE;
				break;
			}
		}
		EnableMenuItem(GetSubMenu(hMenu, index), ID_MENUITEM_PASTE, MF_ENABLED);
	}
	enable = (sel_flag == TRUE && folder_flag == FALSE) ? MF_ENABLED : MF_GRAYED;
	EnableMenuItem(GetSubMenu(hMenu, index), ID_MENUITEM_CLIPBOARD, enable);
	EnableMenuItem(GetSubMenu(hMenu, index), ID_MENUITEM_CLEAR_NAME, enable);
	EnableMenuItem(GetSubMenu(hMenu, index), ID_MENUITEM_SET_HOTKEY, enable);
	EnableMenuItem(GetSubMenu(hMenu, index), ID_MENUITEM_DATA_SAVE, enable);

	enable = (sel_flag == TRUE) ? MF_ENABLED : MF_GRAYED;
	EnableMenuItem(GetSubMenu(hMenu, index), ID_MENUITEM_OPEN, enable);
	EnableMenuItem(GetSubMenu(hMenu, index), ID_MENUITEM_RENAME, enable);
	EnableMenuItem(GetSubMenu(hMenu, index), ID_MENUITEM_DELETE, enable);
	EnableMenuItem(GetSubMenu(hMenu, index), ID_MENUITEM_UP, enable);
	EnableMenuItem(GetSubMenu(hMenu, index), ID_MENUITEM_DOWN, enable);
	EnableMenuItem(GetSubMenu(hMenu, index), ID_MENUITEM_REGIST_ADD, enable);
	EnableMenuItem(GetSubMenu(hMenu, index), ID_MENUITEM_REGIST_MOVE, enable);

	enable = (sel_flag == TRUE && regist_treeitem != NULL) ? MF_ENABLED : MF_GRAYED;
	EnableMenuItem(GetSubMenu(hMenu, index), ID_MENUITEM_REGIST_ADD, enable);
}

/*
 * viewer_show_menu - ѓЃѓjѓ…Ѓ[•\Ћ¦
 */
static void viewer_show_menu(const HWND hWnd)
{
	HWND hTreeView = GetDlgItem(hWnd, ID_TREE);
	HTREEITEM hItem;
	HTREEITEM sel_item;
	int offset = 0;
	int ret;

	if (GetFocus() == hTreeView) {
		// TreeView
		if ((sel_item = (HTREEITEM)SendMessage(hTreeView, TVM_GETNEXTITEM, TVGN_DROPHILITE, 0)) == NULL) {
			sel_item = TreeView_GetSelection(hTreeView);
		}
	} else if (GetFocus() == GetDlgItem(GetDlgItem(hWnd, ID_CONTAINER), ID_LIST)) {
		// ListView
		sel_item = TreeView_GetSelection(hTreeView);
		offset = 1;
	} else {
		return;
	}

	if ((hItem = treeview_get_rootitem(hTreeView, sel_item)) == clip_treeitem) {
		// clipboard
		set_enadle_popup_menu(hWnd, h_popup_menu, POPUPMENU_CLIPBOARD, sel_item);
		_SetForegroundWindow(hWnd);
		ret = menu_show(hWnd, GetSubMenu(h_popup_menu, POPUPMENU_CLIPBOARD), NULL);
	} else if (hItem == history_treeitem) {
		// history
		set_enadle_popup_menu(hWnd, h_popup_menu, POPUPMENU_HISTORY + offset, sel_item);
		_SetForegroundWindow(hWnd);
		ret = menu_show(hWnd, GetSubMenu(h_popup_menu, POPUPMENU_HISTORY + offset), NULL);
	} else if (hItem == regist_treeitem) {
		// regist
		set_enadle_popup_menu(hWnd, h_popup_menu, POPUPMENU_REGIST + offset, sel_item);
		_SetForegroundWindow(hWnd);
		ret = menu_show(hWnd, GetSubMenu(h_popup_menu, POPUPMENU_REGIST + offset), NULL);
	}
	if (ret <= 0 || ret == IDCANCEL) {
		return;
	}
	SendMessage(hWnd, WM_COMMAND, ret, (LPARAM)sel_item);
}

/*
 * viewer_show_item - ѓfЃ[ѓ^‚р•\Ћ¦
 */
static void viewer_show_item(const HWND hWnd)
{
	HWND hTreeView = GetDlgItem(hWnd, ID_TREE);
	HWND hListView = GetDlgItem(GetDlgItem(hWnd, ID_CONTAINER), ID_LIST);
	HTREEITEM hItem;

	if (GetFocus() == hListView) {
		if (ListView_GetNextItem(hListView, -1, LVNI_FOCUSED | LVNI_SELECTED) == -1) {
			return;
		}
		hItem = (HTREEITEM)listview_get_lparam(hListView,
			ListView_GetNextItem(hListView, -1, LVNI_FOCUSED | LVNI_SELECTED));
		if (hItem == NULL) {
			return;
		}
	} else {
		return;
	}
	TreeView_SelectItem(hTreeView, hItem);
	SetFocus(GetDlgItem(hWnd, ID_CONTAINER));
}

/*
 * viewer_item_activate - ѓЉѓXѓgѓrѓ…Ѓ[‚МѓAѓCѓeѓЂ‘I‘р‘ЂЌм
 */
static void viewer_item_activate(const HWND hWnd)
{
	HWND hTreeView = GetDlgItem(hWnd, ID_TREE);
	HWND hListView = GetDlgItem(GetDlgItem(hWnd, ID_CONTAINER), ID_LIST);
	HTREEITEM hItem;
	DATA_INFO *di;
	BOOL lv_focus = FALSE;

	if (GetFocus() == hListView) {
		if (ListView_GetNextItem(hListView, -1, LVNI_FOCUSED | LVNI_SELECTED) == -1) {
			return;
		}
		hItem = (HTREEITEM)listview_get_lparam(hListView,
			ListView_GetNextItem(hListView, -1, LVNI_FOCUSED | LVNI_SELECTED));
		if (hItem == NULL) {
			return;
		}
	} else {
		return;
	}

	if ((di = (DATA_INFO *)treeview_get_lparam(hTreeView, hItem)) == NULL) {
		return;
	}
	if (di->type == TYPE_FOLDER) {
		viewer_show_item(hWnd);
		return;
	}
	switch (option.list_default_action) {
	case 0:
	default:
		// •\Ћ¦
		viewer_show_item(hWnd);
		break;
	case 1:
		// ѓNѓЉѓbѓvѓ{Ѓ[ѓh‚Й‘—‚й
		treeview_to_clipboard(hWnd, NULL);
		break;
	case 2:
		// –ј‘O‚р•t‚Ї‚Д•Ы‘¶
		viewer_data_save(hWnd, hItem);
		break;
	}
}

/*
 * treeview_to_clipboard - ѓfЃ[ѓ^‚рѓNѓЉѓbѓvѓ{Ѓ[ѓh‚Й‘—ђM
 */
static void treeview_to_clipboard(const HWND hWnd, const HTREEITEM sel_item)
{
	HWND hTreeView = GetDlgItem(hWnd, ID_TREE);
	HWND hListView = GetDlgItem(GetDlgItem(hWnd, ID_CONTAINER), ID_LIST);
	HTREEITEM hItem;
	DATA_INFO *di;
	BOOL lv_focus = FALSE;

	if (GetFocus() == hListView) {
		lv_focus = TRUE;
		if (ListView_GetNextItem(hListView, -1, LVNI_FOCUSED | LVNI_SELECTED) == -1) {
			return;
		}
		hItem = (HTREEITEM)listview_get_lparam(hListView,
			ListView_GetNextItem(hListView, -1, LVNI_FOCUSED | LVNI_SELECTED));
		if (hItem == NULL) {
			return;
		}
	} else {
		SetFocus(hTreeView);

		if ((hItem = sel_item) == NULL) {
			hItem = TreeView_GetSelection(hTreeView);
		}
		if (treeview_get_rootitem(hTreeView, hItem) == clip_treeitem) {
			return;
		}
	}
	if ((di = (DATA_INFO *)treeview_get_lparam(hTreeView, hItem)) == NULL || di->type == TYPE_FOLDER) {
		return;
	}
	set_cursor(TRUE);
	SendMessage(hWnd, WM_ITEM_TO_CLIPBOARD, 0, (LPARAM)di);
	set_cursor(FALSE);
}

/*
 * viewer_item_copy - ѓAѓCѓeѓЂ‚рѓRѓsЃ[
 */
HTREEITEM viewer_item_copy(const HWND hWnd, HTREEITEM from_item, HTREEITEM to_item, const BOOL move_flag, TCHAR *err_str)
{
	HWND hTreeView = GetDlgItem(hWnd, ID_TREE);
	HTREEITEM hItem;
	DATA_INFO *di, *wk_di, *prev_di;
	DATA_INFO **pdi;
	TCHAR buf[BUF_SIZE];
	int i;

	if (from_item == to_item) {
		// “Ї€кѓtѓHѓ‹ѓ_
		return NULL;
	}
	hItem = to_item;
	while ((hItem = TreeView_GetParent(hTreeView, hItem)) != NULL) {
		if (hItem == from_item) {
			// ѓRѓsЃ[ђж‚ЄѓRѓsЃ[Њі‚МѓTѓuѓtѓHѓ‹ѓ_
			return NULL;
		}
	}

	// ѓRѓsЃ[‚МЌмђ¬
	if ((di = (DATA_INFO *)treeview_get_lparam(hTreeView, from_item)) == NULL) {
		return NULL;
	}
	if (di->type == TYPE_DATA) {
		// ѓRѓsЃ[ЊіѓfЃ[ѓ^‚рЉЬ‚ЮѓAѓCѓeѓЂ‚рЌмђ¬
		if ((wk_di = data_create_item(NULL, TRUE, err_str)) == NULL) {
			return NULL;
		}
		if ((wk_di->child = data_item_copy(di, FALSE, move_flag, err_str)) == NULL) {
			mem_free(&wk_di);
			return NULL;
		}
	} else {
		// ѓRѓsЃ[
		wk_di = data_item_copy(di, FALSE, move_flag, err_str);
	}
	if (wk_di == NULL) {
		return NULL;
	}

	pdi = NULL;
	if (to_item == clip_treeitem) {
		if (wk_di->type == TYPE_FOLDER) {
			data_free(wk_di);
			return NULL;
		}
		// ѓfЃ[ѓ^‚рѓNѓЉѓbѓvѓ{Ѓ[ѓh‚ЙђЭ’и
		SendMessage(hWnd, WM_ITEM_TO_CLIPBOARD, 1, (LPARAM)di);
		data_free(wk_di);
		return NULL;
	} else if (to_item == history_treeitem) {
		pdi = &history_data.child;
	} else if (to_item == regist_treeitem) {
		pdi = &regist_data.child;
	} else {
		di = (DATA_INFO *)treeview_get_lparam(hTreeView, to_item);
		pdi = &di->child;
	}
	if (pdi == NULL) {
		data_free(wk_di);
		return NULL;
	}

	if (treeview_get_rootitem(hTreeView, to_item) == history_treeitem) {
		if (wk_di->type == TYPE_FOLDER) {
			data_free(wk_di);
			return NULL;
		}
		// ѓtѓBѓ‹ѓ^‚Мѓ`ѓFѓbѓN
		di = wk_di->child;
		prev_di = NULL;
		while (di != NULL) {
			if (filter_format_check(di->format_name) == FALSE ||
				filter_size_check(di->format_name, di->size) == FALSE) {
				if (prev_di == NULL) {
					wk_di->child = di->next;
					di->next = NULL;
					data_free(di);
					di = wk_di->child;
				} else {
					prev_di->next = di->next;
					di->next = NULL;
					data_free(di);
					di = prev_di->next;
				}
				continue;
			}
			prev_di = di;
			di = di->next;
		}
		if (wk_di->child == NULL) {
			data_free(wk_di);
			return NULL;
		}
		// ѓ^ѓCѓgѓ‹‚МЏњ‹Ћ
		mem_free(&wk_di->title);
		// —љ—р‚Й’З‰Б
		if (history_add(pdi, wk_di, TRUE) == FALSE) {
			data_free(wk_di);
			return NULL;
		}
		// —љ—р‚Й’З‰Б‚і‚к‚ЅЋћ‚ЙЋАЌs‚·‚йѓcЃ[ѓ‹
		tool_execute_all(hWnd, CALLTYPE_ADD_HISTORY, wk_di);
		SendMessage(hWnd, WM_HISTORY_CHANGED, 0, 0);
		return TreeView_GetChild(hTreeView, to_item);

	} else {
		if (wk_di->type == TYPE_FOLDER) {
			if (TreeView_GetParent(hTreeView, from_item) == to_item) {
				// ѓtѓHѓ‹ѓ_–ј‚МЌмђ¬
				wsprintf(buf, message_get_res(IDS_VIEWER_COPYNAME), wk_di->title);
				i = 2;
				while (i < 100) {
					for (di = *pdi; di != NULL; di = di->next) {
						if (di->type == TYPE_FOLDER && lstrcmpi(di->title, buf) == 0) {
							break;
						}
					}
					if (di == NULL) {
						break;
					}
					wsprintf(buf, message_get_res(IDS_VIEWER_COPYNAME_NUM), i++, wk_di->title);
				}
				mem_free(&wk_di->title);
				wk_di->title = alloc_copy(buf);
			}
			// ѓtѓHѓ‹ѓ_‚Мѓ}Ѓ[ѓW
			di = regist_merge_item(pdi, wk_di, move_flag, err_str);
			data_free(wk_di);
			if (di == NULL) {
				return NULL;
			}
		} else {
			// ѓEѓBѓ“ѓhѓE–ј‚МЏњ‹Ћ
			mem_free(&wk_di->window_name);

			if (*pdi == NULL) {
				// ђж“Є‚Й’З‰Б
				*pdi = wk_di;
			} else {
				// ––”ц‚Й’З‰Б
				for (di = *pdi; di->next != NULL; di = di->next)
					;
				di->next = wk_di;
			}
			di = wk_di;
		}
	}
	return treeview_datainfo_to_treeitem(hTreeView, to_item, di);
}

/*
 * viewer_regist_add - “o^ѓAѓCѓeѓЂ‚Ц’З‰Б
 */
static void viewer_regist_add(const HWND hWnd, const HTREEITEM sel_item, const BOOL regist_move)
{
	HWND hTreeView = GetDlgItem(hWnd, ID_TREE);
	HWND hListView = GetDlgItem(GetDlgItem(hWnd, ID_CONTAINER), ID_LIST);
	HTREEITEM hItem;
	HTREEITEM to_item;
	HTREEITEM ret_item;
	HTREEITEM wk_Item;
	DATA_INFO *di;
	TCHAR *msg;
	TCHAR err_str[BUF_SIZE];
	int i;
	BOOL move = regist_move;

	if (GetFocus() == hListView) {
		if (ListView_GetSelectedCount(hListView) == 0) {
			return;
		}

		// ’З‰Б‚·‚йѓtѓHѓ‹ѓ_‚М‘I‘р
		if (treeview_get_rootitem(hTreeView, TreeView_GetSelection(hTreeView)) == regist_treeitem) {
			if (move == TRUE) {
				msg = message_get_res(IDS_DIALOG_MOVEPOS);
			} else {
				msg = message_get_res(IDS_DIALOG_COPYPOS);
			}
		} else {
			msg = message_get_res(IDS_DIALOG_ADDPOS);
		}
		if ((to_item = select_folder(hInst, hWnd, hTreeView, regist_treeitem, msg)) == NULL) {
			return;
		}
		if (move == TRUE && TreeView_GetSelection(hTreeView) == to_item) {
			return;
		}
		if (treeview_get_rootitem(hTreeView, TreeView_GetSelection(hTreeView)) != regist_treeitem) {
			move = FALSE;
		}

		// ѓtѓHѓ‹ѓ_‚ЙѓAѓCѓeѓЂ‚р’З‰Б
		set_cursor(TRUE);
		ret_item = NULL;
		i = -1;
		while ((i = ListView_GetNextItem(hListView, i, LVNI_SELECTED)) != -1) {
			if ((hItem = (HTREEITEM)listview_get_lparam(hListView, i)) != NULL) {
				*err_str = TEXT('\0');
				wk_Item = viewer_item_copy(hWnd, hItem, to_item, move, err_str);
				if (wk_Item == NULL && *err_str != TEXT('\0')) {
					set_cursor(FALSE);
					MessageBox(hWnd, err_str, ERROR_TITLE, MB_ICONERROR);
					return;
				}
				if (ret_item == NULL) {
					ret_item = wk_Item;
				}
				if (move == TRUE) {
					// €Ъ“®
					treeview_delete_item(hTreeView, hItem);
					ListView_DeleteItem(hListView, i);
					i = -1;
				}
			}
		}
		if (ret_item != NULL) {
			TreeView_SelectItem(hTreeView, ret_item);
		}
		// “o^ѓAѓCѓeѓЂ‚М•Ы‘¶
		SendMessage(hWnd, WM_REGIST_SAVE, 0, 0);
		SendMessage(hWnd, WM_VIEWER_REFRESH_STATUS, 0, 0);
		set_cursor(FALSE);

	} else {
		SetFocus(hTreeView);

		if ((hItem = sel_item) == NULL) {
			hItem = TreeView_GetSelection(hTreeView);
		}
		if (hItem == history_treeitem || hItem == regist_treeitem) {
			return;
		}

		// ѓNѓЉѓbѓvѓ{Ѓ[ѓh‚М“а—eЋж“ѕ
		viewer_get_clipboard_data(hWnd, hItem);

		// ’З‰Б‚·‚йѓtѓHѓ‹ѓ_‚М‘I‘р
		if (treeview_get_rootitem(hTreeView, hItem) == regist_treeitem) {
			if ((di = (DATA_INFO *)treeview_get_lparam(hTreeView, hItem)) != NULL && di->type == TYPE_DATA) {
				move = FALSE;
			}
			if (move == TRUE) {
				msg = message_get_res(IDS_DIALOG_MOVEPOS);
			} else {
				msg = message_get_res(IDS_DIALOG_COPYPOS);
			}
		} else {
			msg = message_get_res(IDS_DIALOG_ADDPOS);
		}
		if ((to_item = select_folder(hInst, hWnd, hTreeView, regist_treeitem, msg)) == NULL) {
			return;
		}
		if (move == TRUE && TreeView_GetParent(hTreeView, hItem) == to_item) {
			return;
		}

		// ѓtѓHѓ‹ѓ_‚ЙѓAѓCѓeѓЂ‚р’З‰Б
		set_cursor(TRUE);
		*err_str = TEXT('\0');
		if ((ret_item = viewer_item_copy(hWnd, hItem, to_item, move, err_str)) == NULL) {
			set_cursor(FALSE);
			if (*err_str != TEXT('\0')) {
				MessageBox(hWnd, err_str, ERROR_TITLE, MB_ICONERROR);
			}
			return;
		}
		if (move == TRUE && treeview_get_rootitem(hTreeView, hItem) == regist_treeitem) {
			// €Ъ“®
			treeview_delete_item(hTreeView, hItem);
		}
		TreeView_SelectItem(hTreeView, ret_item);
		// “o^ѓAѓCѓeѓЂ‚М•Ы‘¶
		SendMessage(hWnd, WM_REGIST_SAVE, 0, 0);
		SendMessage(hWnd, WM_VIEWER_REFRESH_STATUS, 0, 0);
		set_cursor(FALSE);
	}
}

/*
 * viewer_item_paste - ѓAѓCѓeѓЂ‚р“\‚и•t‚Ї
 */
static void viewer_item_paste(const HWND hWnd, const HTREEITEM sel_item)
{
	HWND hTreeView = GetDlgItem(hWnd, ID_TREE);
	HWND hListView = GetDlgItem(GetDlgItem(hWnd, ID_CONTAINER), ID_LIST);
	HTREEITEM to_item = sel_item;
	HTREEITEM ret_item;
	DATA_INFO *di;
	TCHAR err_str[BUF_SIZE];
	BOOL free_item = FALSE;

	if (to_item == NULL) {
		to_item = TreeView_GetSelection(hTreeView);
	}
	if (treeview_get_rootitem(hTreeView, to_item) == clip_treeitem) {
		return;
	} else if (to_item != history_treeitem && to_item != regist_treeitem) {
		if ((di = (DATA_INFO *)treeview_get_lparam(hTreeView, to_item)) == NULL || di->type != TYPE_FOLDER) {
			return;
		}
	}

	// ѓNѓЉѓbѓvѓ{Ѓ[ѓh‚М“а—eЋж“ѕ
	if (clip_treeitem == NULL) {
		clip_treeitem = treeview_set_item(hTreeView, message_get_res(IDS_TREEITEM_CLIPBOARD),
			(HTREEITEM)TVI_ROOT, (HTREEITEM)TVI_LAST, 0, 0, (LPARAM)&clip_di);
		free_item = TRUE;
	}
	treeview_delete_child(hTreeView, clip_treeitem);
	data_free(clip_di.child);
	clip_di.child = clipboard_to_datainfo(hWnd);
	treeview_datainfo_to_treeitem(hTreeView, clip_treeitem, clip_di.child);
	viewer_get_clipboard_data(hWnd, clip_treeitem);

	// ѓtѓHѓ‹ѓ_‚ЙѓAѓCѓeѓЂ‚р’З‰Б
	set_cursor(TRUE);
	*err_str = TEXT('\0');
	if ((ret_item = viewer_item_copy(hWnd, clip_treeitem, to_item, FALSE, err_str)) == NULL) {
		if (free_item == TRUE) {
			TreeView_DeleteItem(hTreeView, clip_treeitem);
			clip_treeitem = NULL;
		}
		set_cursor(FALSE);
		if (*err_str != TEXT('\0')) {
			MessageBox(hWnd, err_str, ERROR_TITLE, MB_ICONERROR);
		}
		return;
	}

	if (GetFocus() == hListView) {
		// ѓcѓЉЃ[ѓrѓ…Ѓ[‚ЖѓЉѓXѓgѓrѓ…Ѓ[‚М“ЇЉъ
		treeview_to_listview(hTreeView, TreeView_GetSelection(hTreeView), hListView);
		listview_lparam_select(hListView, (LPARAM)ret_item);
	} else {
		TreeView_SelectItem(hTreeView, ret_item);
	}
	if (treeview_get_rootitem(hTreeView, to_item) == regist_treeitem) {
		// “o^ѓAѓCѓeѓЂ‚М•Ы‘¶
		SendMessage(hWnd, WM_REGIST_SAVE, 0, 0);
	} else if (option.history_save == 1 && option.history_always_save == 1 &&
		treeview_get_rootitem(hTreeView, to_item) == history_treeitem) {
		// —љ—р‚М•Ы‘¶
		SendMessage(hWnd, WM_HISTORY_SAVE, 0, 0);
	}
	SendMessage(hWnd, WM_VIEWER_REFRESH_STATUS, 0, 0);
	if (free_item == TRUE) {
		TreeView_DeleteItem(hTreeView, clip_treeitem);
		clip_treeitem = NULL;
	}
	set_cursor(FALSE);
}

/*
 * viewer_move_up - ѓAѓCѓeѓЂ‚рЏг‚Й€Ъ“®
 */
static BOOL viewer_move_up(const HWND hWnd, const HTREEITEM sel_item)
{
	HWND hTreeView = GetDlgItem(hWnd, ID_TREE);
	HWND hListView = GetDlgItem(GetDlgItem(hWnd, ID_CONTAINER), ID_LIST);
	HTREEITEM hItem;
	int i;

	if (GetFocus() == hListView) {
		if (ListView_GetSelectedCount(hListView) == 0) {
			return FALSE;
		}

		SendMessage(hTreeView, WM_SETREDRAW, (WPARAM)FALSE, 0);
		SendMessage(hListView, WM_SETREDRAW, (WPARAM)FALSE, 0);

		i = -1;
		while ((i = ListView_GetNextItem(hListView, i, LVNI_SELECTED)) != -1) {
			if ((hItem = (HTREEITEM)listview_get_lparam(hListView, i)) == NULL) {
				continue;
			}
			// ѓAѓCѓeѓЂ‚М“ь‚к‘Ц‚¦
			if ((hItem = treeview_move_up(hTreeView, hItem)) == NULL) {
				break;
			}
			listview_set_lparam(hListView, i, (LPARAM)hItem);
			listview_move_item(hListView, i, -1);
		}
		SendMessage(hTreeView, WM_SETREDRAW, (WPARAM)TRUE, 0);
		UpdateWindow(hTreeView);
		SendMessage(hListView, WM_SETREDRAW, (WPARAM)TRUE, 0);
		UpdateWindow(hListView);

	} else {
		SetFocus(hTreeView);

		if ((hItem = sel_item) == NULL) {
			hItem = TreeView_GetSelection(hTreeView);
		}
		// ѓAѓCѓeѓЂ‚М“ь‚к‘Ц‚¦
		if (treeview_move_up(hTreeView, hItem) == NULL) {
			SendMessage(hTreeView, WM_SETREDRAW, (WPARAM)TRUE, 0);
			UpdateWindow(hTreeView);
			return FALSE;
		}
		if (current_wnd == hListView) {
			// ѓЉѓXѓgѓrѓ…Ѓ[‚МЌXђV
			treeview_to_listview(hTreeView, TreeView_GetSelection(hTreeView), hListView);
		}
	}
	return TRUE;
}

/*
 * viewer_move_down - ѓAѓCѓeѓЂ‚р‰є‚Й€Ъ“®
 */
static BOOL viewer_move_down(const HWND hWnd, const HTREEITEM sel_item)
{
	HWND hTreeView = GetDlgItem(hWnd, ID_TREE);
	HWND hListView = GetDlgItem(GetDlgItem(hWnd, ID_CONTAINER), ID_LIST);
	HTREEITEM hItem;
	int i;

	if (GetFocus() == hListView) {
		if (ListView_GetSelectedCount(hListView) == 0) {
			return FALSE;
		}

		SendMessage(hTreeView, WM_SETREDRAW, (WPARAM)FALSE, 0);
		SendMessage(hListView, WM_SETREDRAW, (WPARAM)FALSE, 0);

		for (i = ListView_GetItemCount(hListView) - 1; i >= 0; i--) {
			if (ListView_GetItemState(hListView, i, LVNI_SELECTED) == LVNI_SELECTED) {
				if ((hItem = (HTREEITEM)listview_get_lparam(hListView, i)) == NULL) {
					continue;
				}
				// ѓAѓCѓeѓЂ‚М“ь‚к‘Ц‚¦
				if ((hItem = treeview_move_down(hTreeView, hItem)) == NULL) {
					break;
				}
				listview_set_lparam(hListView, i, (LPARAM)hItem);
				listview_move_item(hListView, i, 1);
			}
		}
		SendMessage(hTreeView, WM_SETREDRAW, (WPARAM)TRUE, 0);
		UpdateWindow(hTreeView);
		SendMessage(hListView, WM_SETREDRAW, (WPARAM)TRUE, 0);
		UpdateWindow(hListView);

	} else {
		SetFocus(hTreeView);

		if ((hItem = sel_item) == NULL) {
			hItem = TreeView_GetSelection(hTreeView);
		}
		// ѓAѓCѓeѓЂ‚М“ь‚к‘Ц‚¦
		if (treeview_move_down(hTreeView, hItem) == NULL) {
			SendMessage(hTreeView, WM_SETREDRAW, (WPARAM)TRUE, 0);
			UpdateWindow(hTreeView);
			return FALSE;
		}
		if (current_wnd == hListView) {
			// ѓЉѓXѓgѓrѓ…Ѓ[‚МЌXђV
			treeview_to_listview(hTreeView, TreeView_GetSelection(hTreeView), hListView);
		}
	}
	return TRUE;
}

/*
 * viewer_create_item - ѓAѓCѓeѓЂ‚МЌмђ¬
 */
static void viewer_create_item(const HWND hWnd, const HTREEITEM sel_item)
{
	HWND hTreeView = GetDlgItem(hWnd, ID_TREE);
	HWND hListView = GetDlgItem(GetDlgItem(hWnd, ID_CONTAINER), ID_LIST);
	HTREEITEM hItem = sel_item;
	HTREEITEM cItem;
	HANDLE data = NULL;
	DATA_INFO *di;
	DATA_INFO *new_di;
	DATA_INFO *wk_di;
	TCHAR format_name[BUF_SIZE];
	TCHAR file_name[MAX_PATH];
	TCHAR err_str[BUF_SIZE];
	DWORD size = 0;

	if (GetFocus() == hListView) {
		// ѓЉѓXѓgѓrѓ…Ѓ[‚М‘I‘рѓAѓCѓeѓЂЋж“ѕ
		if (ListView_GetNextItem(hListView, -1, LVNI_FOCUSED | LVNI_SELECTED) != -1) {
			hItem = (HTREEITEM)listview_get_lparam(hListView,
				ListView_GetNextItem(hListView, -1, LVNI_FOCUSED | LVNI_SELECTED));
		}
	}
	if (hItem == NULL) {
		hItem = TreeView_GetSelection(hTreeView);
	}
	if (treeview_get_rootitem(hTreeView, hItem) == clip_treeitem) {
		return;
	}

	// Њ`Ћ®‚Жѓtѓ@ѓCѓ‹–ј‚М‘I‘р
	if (select_format(hInst, hWnd, format_name, file_name) == FALSE || *format_name == TEXT('\0')) {
		return;
	}
	if (*file_name != TEXT('\0')) {
		// ѓtѓ@ѓCѓ‹‚©‚зѓfЃ[ѓ^‚рЌмђ¬
		set_cursor(TRUE);
		*err_str = TEXT('\0');
		if ((data = format_file_to_data(file_name, format_name, &size, err_str)) == NULL) {
			if (*err_str != TEXT('\0')) {
				set_cursor(FALSE);
				MessageBox(hWnd, err_str, ERROR_TITLE, MB_ICONERROR);
				return;
			}
			if ((data = clipboard_file_to_data(file_name, format_name, &size, err_str)) == NULL && *err_str != TEXT('\0')) {
				set_cursor(FALSE);
				MessageBox(hWnd, err_str, ERROR_TITLE, MB_ICONERROR);
				return;
			}
		}
		set_cursor(FALSE);
	}

	if (hItem == history_treeitem) {
		di = &history_data;
	} else if (hItem == regist_treeitem) {
		di = &regist_data;
	} else {
		if ((di = (DATA_INFO *)treeview_get_lparam(hTreeView, hItem)) != NULL && di->type == TYPE_DATA) {
			hItem = TreeView_GetParent(hTreeView, hItem);
			di = (DATA_INFO *)treeview_get_lparam(hTreeView, hItem);
		}
	}
	if (di == NULL) {
		if (data != NULL && format_free_data(format_name, data) == FALSE) {
			clipboard_free_data(format_name, data);
		}
		return;
	}

	if (hItem == history_treeitem ||
		(di->type == TYPE_FOLDER && treeview_get_rootitem(hTreeView, hItem) == history_treeitem)) {
		// —љ—р‚ЙѓAѓCѓeѓЂ‚р’З‰Б
		if ((new_di = data_create_item(NULL, TRUE, err_str)) == NULL) {
			if (*err_str != TEXT('\0')) {
				MessageBox(hWnd, err_str, ERROR_TITLE, MB_ICONERROR);
			}
			if (data != NULL && format_free_data(format_name, data) == FALSE) {
				clipboard_free_data(format_name, data);
			}
			return;
		}
		if ((new_di->child = data_create_data(0, format_name, data, size, TRUE, err_str)) == NULL) {
			if (*err_str != TEXT('\0')) {
				MessageBox(hWnd, err_str, ERROR_TITLE, MB_ICONERROR);
			}
			if (data != NULL && format_free_data(format_name, data) == FALSE) {
				clipboard_free_data(format_name, data);
			}
			data_free(new_di);
			return;
		}

		// —љ—р‚Й’З‰Б
		if (history_add(&di->child, new_di, TRUE) == FALSE) {
			data_free(new_di);
			return;
		}
		// —љ—р‚Й’З‰Б‚і‚к‚ЅЋћ‚ЙЋАЌs‚·‚йѓcЃ[ѓ‹
		tool_execute_all(hWnd, CALLTYPE_ADD_HISTORY, new_di);

		if (option.history_save == 1 && option.history_always_save == 1) {
			// —љ—р‚М•Ы‘¶
			SendMessage(hWnd, WM_HISTORY_SAVE, 0, 0);
		}
		SendMessage(hWnd, WM_HISTORY_CHANGED, 0, 0);
		if ((cItem = TreeView_GetChild(hTreeView, hItem)) != NULL) {
			TreeView_SelectItem(hTreeView, cItem);
			SetFocus(GetDlgItem(hWnd, ID_CONTAINER));
		}
		return;
	}

	switch (di->type) {
	case TYPE_ROOT:
	case TYPE_FOLDER:
		// ѓAѓCѓeѓЂ‚МЌмђ¬
		if ((new_di = data_create_item(NULL, TRUE, err_str)) == NULL) {
			if (*err_str != TEXT('\0')) {
				MessageBox(hWnd, err_str, ERROR_TITLE, MB_ICONERROR);
			}
			if (data != NULL && format_free_data(format_name, data) == FALSE) {
				clipboard_free_data(format_name, data);
			}
			return;
		}
		if ((new_di->child = data_create_data(0, format_name, data, size, TRUE, err_str)) == NULL) {
			if (*err_str != TEXT('\0')) {
				MessageBox(hWnd, err_str, ERROR_TITLE, MB_ICONERROR);
			}
			if (data != NULL && format_free_data(format_name, data) == FALSE) {
				clipboard_free_data(format_name, data);
			}
			data_free(new_di);
			return;
		}
		break;

	case TYPE_ITEM:
		// ѓAѓCѓeѓЂ‚ЙЊ`Ћ®‚р’З‰Б
		// “Ї–ј‚МЊ`Ћ®‚Є‘¶ЌЭ‚·‚й‚©ѓ`ѓFѓbѓN
		for (wk_di = di->child; wk_di != NULL; wk_di = wk_di->next) {
			if (lstrcmpi(format_name, wk_di->format_name) == 0) {
				if (MessageBox(hWnd, message_get_res(IDS_QUESTION_REPLACE), WINDOW_TITLE, MB_ICONQUESTION | MB_YESNO) == IDNO ||
					SendMessage(hWnd, WM_ITEM_CHECK, 0, (LPARAM)wk_di) == -1) {
					if (data != NULL && format_free_data(format_name, data) == FALSE) {
						clipboard_free_data(format_name, data);
					}
					return;
				}
				// ѓcѓЉЃ[ѓrѓ…Ѓ[‚©‚зѓAѓCѓeѓЂ‚рЌнЏњ
				cItem = TreeView_GetChild(hTreeView, hItem);
				while (cItem != NULL) {
					if ((DATA_INFO *)treeview_get_lparam(hTreeView, cItem) == wk_di) {
						TreeView_DeleteItem(hTreeView, cItem);
						break;
					}
					cItem = TreeView_GetNextSibling(hTreeView, cItem);
				}
				data_delete(&di->child, wk_di, TRUE);
				break;
			}
		}
		// Њ`Ћ®‚МЌмђ¬
		if ((new_di = data_create_data(0, format_name, data, size, TRUE, err_str)) == NULL) {
			if (*err_str != TEXT('\0')) {
				MessageBox(hWnd, err_str, ERROR_TITLE, MB_ICONERROR);
			}
			if (data != NULL && format_free_data(format_name, data) == FALSE) {
				clipboard_free_data(format_name, data);
			}
			return;
		}
		break;

	default:
		if (data != NULL && format_free_data(format_name, data) == FALSE) {
			clipboard_free_data(format_name, data);
		}
		return;
	}
	// ѓAѓCѓeѓЂ‚М’З‰Б
	if (di->child == NULL) {
		di->child = new_di;
	} else {
		for (di = di->child; di->next != NULL; di = di->next)
			;
		di->next = new_di;
	}

	// ѓcѓЉЃ[ѓrѓ…Ѓ[‚ЙѓAѓCѓeѓЂ‚р’З‰Б
	cItem = (new_di->type != TYPE_DATA || option.tree_show_format == 1) ?
		treeview_datainfo_to_treeitem(hTreeView, hItem, new_di) : NULL;
	TreeView_SelectItem(hTreeView, (cItem != NULL) ? cItem : hItem);
	SetFocus(GetDlgItem(hWnd, ID_CONTAINER));

	if (new_di->type == TYPE_DATA) {
		// ѓ^ѓCѓgѓ‹‚МЌXђV
		treeview_title_refresh(hTreeView, hItem);
		viewer_set_datetime(hWnd, hItem);
	}

	set_cursor(TRUE);
	if (treeview_get_rootitem(hTreeView, cItem) == regist_treeitem) {
		// “o^ѓAѓCѓeѓЂ‚М•Ы‘¶
		SendMessage(hWnd, WM_REGIST_SAVE, 0, 0);
	} else if (option.history_save == 1 && option.history_always_save == 1 &&
		treeview_get_rootitem(hTreeView, cItem) == history_treeitem) {
		// —љ—р‚М•Ы‘¶
		SendMessage(hWnd, WM_HISTORY_SAVE, 0, 0);
	}
	set_cursor(FALSE);
}

/*
 * viewer_create_folder - ѓtѓHѓ‹ѓ_‚МЌмђ¬
 */
static void viewer_create_folder(const HWND hWnd, const HTREEITEM sel_item)
{
	HWND hTreeView = GetDlgItem(hWnd, ID_TREE);
	HWND hListView = GetDlgItem(GetDlgItem(hWnd, ID_CONTAINER), ID_LIST);
	HTREEITEM hItem = sel_item;
	HTREEITEM new_hItem;
	DATA_INFO *di;
	DATA_INFO **pdi;
	TCHAR title[BUF_SIZE];
	TCHAR err_str[BUF_SIZE];
	int i;

	if (hItem == NULL) {
		hItem = TreeView_GetSelection(hTreeView);
	}
	if (treeview_get_rootitem(hTreeView, hItem) != regist_treeitem) {
		return;
	}

	pdi = NULL;
	if (hItem == regist_treeitem) {
		pdi = &regist_data.child;
	} else {
		di = (DATA_INFO *)treeview_get_lparam(hTreeView, hItem);
		while (di != NULL && di->type != TYPE_FOLDER) {
			hItem = TreeView_GetParent(hTreeView, hItem);
			di = (DATA_INFO *)treeview_get_lparam(hTreeView, hItem);
		}
		if (di == NULL) {
			pdi = &regist_data.child;
		} else {
			pdi = &di->child;
		}
	}
	if (pdi == NULL) {
		return;
	}

	// ѓtѓHѓ‹ѓ_ѓpѓX‚МЌмђ¬
	lstrcpy(title, message_get_res(IDS_TREEITEM_NEWFOLDER));
	i = 1;
	*err_str = TEXT('\0');
	while ((di = regist_create_folder(pdi, title, err_str)) == NULL) {
		if (*err_str != TEXT('\0')) {
			MessageBox(hWnd, err_str, ERROR_TITLE, MB_ICONERROR);
			return;
		}
		wsprintf(title, TEXT("%s (%d)"), message_get_res(IDS_TREEITEM_NEWFOLDER), ++i);
	}

	// ѓcѓЉЃ[ѓrѓ…Ѓ[‚ЙѓAѓCѓeѓЂ‚р’З‰Б
	new_hItem = treeview_set_item(hTreeView, di->title, hItem, (HTREEITEM)TVI_LAST,
		ICON_FOLDER, ICON_FOLDER_OPEN, (LPARAM)di);
	if (new_hItem == NULL) {
		data_delete(pdi, di, TRUE);
		return;
	}

	if (current_wnd == hListView) {
		// ѓcѓЉЃ[ѓrѓ…Ѓ[‚ЖѓЉѓXѓgѓrѓ…Ѓ[‚М“ЇЉъ
		treeview_to_listview(hTreeView, TreeView_GetSelection(hTreeView), hListView);
	}
	// “o^ѓAѓCѓeѓЂ‚М•Ы‘¶
	set_cursor(TRUE);
	SendMessage(hWnd, WM_REGIST_SAVE, 0, 0);
	set_cursor(FALSE);

	if (GetFocus() == hListView) {
		SetFocus(hTreeView);
	}
	// ѓ‰ѓxѓ‹‚М•ТЏW
	TreeView_Expand(hTreeView, hItem, TVE_EXPAND);
	TreeView_EditLabel(hTreeView, new_hItem);
}

/*
 * viewer_data_save - ѓfЃ[ѓ^‚М•Ы‘¶
 */
static void viewer_data_save(const HWND hWnd, const HTREEITEM sel_item)
{
	HWND hTreeView = GetDlgItem(hWnd, ID_TREE);
	HWND hListView = GetDlgItem(GetDlgItem(hWnd, ID_CONTAINER), ID_LIST);
	HTREEITEM hItem = sel_item;
	OPENFILENAME of;
	DATA_INFO *di;
	TCHAR file_name[MAX_PATH];
	TCHAR err_str[BUF_SIZE];

	if (GetFocus() == hListView) {
		// ѓЉѓXѓgѓrѓ…Ѓ[‚М‘I‘рѓAѓCѓeѓЂЋж“ѕ
		if (ListView_GetNextItem(hListView, -1, LVNI_FOCUSED | LVNI_SELECTED) != -1) {
			hItem = (HTREEITEM)listview_get_lparam(hListView,
				ListView_GetNextItem(hListView, -1, LVNI_FOCUSED | LVNI_SELECTED));
		}
	}
	if (hItem == NULL) {
		hItem = TreeView_GetSelection(hTreeView);
	}

	if ((di = (DATA_INFO *)treeview_get_lparam(hTreeView, hItem)) == NULL) {
		return;
	}
	switch (di->type) {
	case TYPE_FOLDER:
		return;

	case TYPE_ITEM:
		if ((di = format_get_priority_highest(di)) == NULL) {
			return;
		}
		break;
	}

	// ѓtѓ@ѓCѓ‹‚М‘I‘р
	ZeroMemory(&of, sizeof(OPENFILENAME));
	of.lStructSize = sizeof(OPENFILENAME);
	of.hInstance = hInst;
	of.hwndOwner = hWnd;
	of.lpstrFilter = TEXT("*.*\0*.*\0\0");
	of.nFilterIndex = 1;
	*file_name = TEXT('\0');
	of.lpstrFile = file_name;
	of.nMaxFile = MAX_PATH - 1;
	of.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	if (format_get_file_info(di->format_name, di, &of, FALSE) >= 0 && GetSaveFileName(&of) == FALSE) {
		return;
	}

	set_cursor(TRUE);
	// ѓNѓЉѓbѓvѓ{Ѓ[ѓh‚М“а—eЋж“ѕ
	viewer_get_clipboard_data(hWnd, hItem);
	// ѓtѓ@ѓCѓ‹‚Й•Ы‘¶
	*err_str = TEXT('\0');
	if (format_data_to_file(di, file_name, of.nFilterIndex, err_str) == FALSE) {
		if (*err_str != TEXT('\0')) {
			set_cursor(FALSE);
			MessageBox(hWnd, err_str, ERROR_TITLE, MB_ICONERROR);
			return;
		}
		if (clipboard_data_to_file(di, file_name, of.nFilterIndex, err_str) == FALSE && *err_str != TEXT('\0')) {
			set_cursor(FALSE);
			MessageBox(hWnd, err_str, ERROR_TITLE, MB_ICONERROR);
			return;
		}
	}
	set_cursor(FALSE);
}

/*
 * viewer_import_item - ѓAѓCѓeѓЂ‚МѓCѓ“ѓ|Ѓ[ѓg
 */
static BOOL viewer_import_item(const HWND hWnd)
{
	HWND hTreeView = GetDlgItem(hWnd, ID_TREE);
	HWND hListView = GetDlgItem(GetDlgItem(hWnd, ID_CONTAINER), ID_LIST);
	OPENFILENAME of;
	HTREEITEM to_item;
	DATA_INFO *load_di = NULL;
	DATA_INFO *to_di;
	DATA_INFO *di;
	TCHAR file_name[MAX_PATH];
	TCHAR err_str[BUF_SIZE];

	// ѓCѓ“ѓ|Ѓ[ѓg‚·‚йѓtѓ@ѓCѓ‹‚М‘I‘р
	ZeroMemory(&of, sizeof(OPENFILENAME));
	of.lStructSize = sizeof(OPENFILENAME);
	of.hInstance = hInst;
	of.hwndOwner = hWnd;
	of.lpstrFilter = TEXT("*.dat\0*.dat\0*.*\0*.*\0\0");
	of.nFilterIndex = 1;
	*file_name = TEXT('\0');
	of.lpstrFile = file_name;
	of.nMaxFile = MAX_PATH - 1;
	of.lpstrDefExt = TEXT("dat");
	of.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	if (GetOpenFileName(&of) == FALSE) {
		return FALSE;
	}

	// ѓCѓ“ѓ|Ѓ[ѓg‚µ‚ЅѓAѓCѓeѓЂ‚р’З‰Б‚·‚йѓtѓHѓ‹ѓ_‚М‘I‘р
	to_item = select_folder(hInst, hWnd, hTreeView, regist_treeitem, message_get_res(IDS_DIALOG_IMPORT));
	if (to_item == NULL) {
		return FALSE;
	}
	if (to_item == regist_treeitem) {
		to_di = &regist_data;
	} else {
		to_di = (DATA_INFO *)treeview_get_lparam(hTreeView, to_item);
	}
	if (to_di == NULL) {
		return FALSE;
	}

	set_cursor(TRUE);
	// ѓtѓ@ѓCѓ‹‚©‚зѓAѓCѓeѓЂ‚рЌмђ¬
	*err_str = TEXT('\0');
	if (file_read_data(file_name, &load_di, err_str) == FALSE && *err_str != TEXT('\0')) {
		set_cursor(FALSE);
		MessageBox(hWnd, err_str, file_name, MB_ICONERROR);
		return FALSE;
	}

	// ѓAѓCѓeѓЂ‚МѓRѓsЃ[
	for (di = load_di; di != NULL; di = di->next) {
		if (regist_merge_item(&to_di->child, di, FALSE, err_str) == NULL) {
			break;
		}
	}
	data_free(load_di);

	treeview_sync_datainfo(hTreeView, regist_treeitem, regist_data.child);
	TreeView_Expand(hTreeView, to_item, TVE_EXPAND);
	// “o^ѓAѓCѓeѓЂ‚М•Ы‘¶
	SendMessage(hWnd, WM_REGIST_SAVE, 0, 0);
	if (current_wnd == hListView) {
		// ѓЉѓXѓgѓrѓ…Ѓ[‚МЌXђV
		treeview_to_listview(hTreeView, TreeView_GetSelection(hTreeView), hListView);
	}
	SendMessage(hWnd, WM_VIEWER_REFRESH_STATUS, 0, 0);
	set_cursor(FALSE);
	return TRUE;
}

/*
 * viewer_export_item - ѓAѓCѓeѓЂ‚МѓGѓNѓXѓ|Ѓ[ѓg
 */
static BOOL viewer_export_item(const HWND hWnd)
{
	HWND hTreeView = GetDlgItem(hWnd, ID_TREE);
	OPENFILENAME of;
	HTREEITEM from_item;
	DATA_INFO *from_di;
	TCHAR file_name[MAX_PATH];
	TCHAR err_str[BUF_SIZE];

	// ѓGѓNѓXѓ|Ѓ[ѓg‚·‚йѓtѓHѓ‹ѓ_‚М‘I‘р
	from_item = select_folder(hInst, hWnd, hTreeView, regist_treeitem, message_get_res(IDS_DIALOG_EXPORT));
	if (from_item == regist_treeitem) {
		from_di = &regist_data;
	} else {
		from_di = (DATA_INFO *)treeview_get_lparam(hTreeView, from_item);
	}
	if (from_di == NULL) {
		return FALSE;
	}

	// ѓCѓ“ѓ|Ѓ[ѓg‚·‚йѓtѓ@ѓCѓ‹‚М‘I‘р
	ZeroMemory(&of, sizeof(OPENFILENAME));
	of.lStructSize = sizeof(OPENFILENAME);
	of.hInstance = hInst;
	of.hwndOwner = hWnd;
	of.lpstrFilter = TEXT("*.dat\0*.dat\0*.*\0*.*\0\0");
	of.nFilterIndex = 1;
	*file_name = TEXT('\0');
	of.lpstrFile = file_name;
	of.nMaxFile = MAX_PATH - 1;
	of.lpstrDefExt = TEXT("dat");
	of.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	if (GetSaveFileName(&of) == FALSE) {
		return FALSE;
	}

	// ѓtѓ@ѓCѓ‹‚ЦЏo—Н
	*err_str = TEXT('\0');
	if (file_write_data(file_name, from_di->child, err_str) == FALSE) {
		if (*err_str != TEXT('\0')) {
			MessageBox(hWnd, err_str, file_name, MB_ICONERROR);
		}
		return FALSE;
	}
	return TRUE;
}

/*
 * viewer_rename - ѓAѓCѓeѓЂ‚Мѓ^ѓCѓgѓ‹•ПЌX
 */
static BOOL viewer_rename(const HWND hWnd, const HTREEITEM sel_item, TCHAR *title)
{
	HWND hTreeView = GetDlgItem(hWnd, ID_TREE);
	HWND hListView = GetDlgItem(GetDlgItem(hWnd, ID_CONTAINER), ID_LIST);
	HTREEITEM hItem = sel_item;
	HTREEITEM pItem;
	DATA_INFO *di;
	DATA_INFO *wk_di;
	TCHAR *tmp;
	TCHAR err_str[BUF_SIZE];

	if (title == NULL || *title == TEXT('\0')) {
		return FALSE;
	}

	if (sel_item == NULL && current_wnd == hListView) {
		if (ListView_GetNextItem(hListView, -1, LVNI_FOCUSED) == -1) {
			return FALSE;
		}
		hItem = (HTREEITEM)listview_get_lparam(hListView,
			ListView_GetNextItem(hListView, -1, LVNI_FOCUSED));
		if (hItem == NULL) {
			return FALSE;
		}
	}

	// ѓfЃ[ѓ^‚МЋж“ѕ
	if ((di = (DATA_INFO *)treeview_get_lparam(hTreeView, hItem)) == NULL) {
		return FALSE;
	}

	switch (di->type) {
	case TYPE_DATA:
		return FALSE;

	case TYPE_FOLDER:
		// ѓtѓ@ѓCѓ‹–ј‚Мѓ`ѓFѓbѓN
		if (file_name_check(title) == FALSE) {
			MessageBox(hWnd, message_get_res(IDS_ERROR_FILENAME), ERROR_TITLE, MB_ICONERROR);
			return FALSE;
		}

		// “Ї–ј‚МѓtѓHѓ‹ѓ_‚Є‘¶ЌЭ‚µ‚И‚ў‚©ѓ`ѓFѓbѓN
		if ((pItem = TreeView_GetParent(hTreeView, hItem)) == history_treeitem) {
			wk_di = history_data.child;
		} else if (pItem == regist_treeitem) {
			wk_di = regist_data.child;
		} else {
			if ((wk_di = (DATA_INFO *)treeview_get_lparam(hTreeView, pItem)) == NULL) {
				return FALSE;
			}
			wk_di = wk_di->child;
		}
		for (; wk_di != NULL; wk_di = wk_di->next) {
			if (wk_di->type == TYPE_FOLDER && wk_di != di && lstrcmpi(wk_di->title, title) == 0) {
				MessageBox(hWnd, message_get_res(IDS_ERROR_FOLDERNAME), ERROR_TITLE, MB_ICONERROR);
				return FALSE;
			}
		}
	case TYPE_ITEM:
		// ђV‚µ‚ўѓ^ѓCѓgѓ‹‚МђЭ’и
		if ((tmp = alloc_copy(title)) == NULL) {
			message_get_error(GetLastError(), err_str);
			if (*err_str != TEXT('\0')) {
				MessageBox(hWnd, err_str, ERROR_TITLE, MB_ICONERROR);
			}
			return FALSE;
		}
		mem_free(&di->title);
		di->title = tmp;
		break;
	}

	if (current_wnd == hListView) {
		if (sel_item != NULL) {
			// ѓЉѓXѓgѓrѓ…Ѓ[‚МЌXђV
			SetTimer(hWnd, TIMER_LV_REFRESH, 1, NULL);
		} else {
			// ѓcѓЉЃ[ѓrѓ…Ѓ[‚МЌXђV
			treeview_set_text(hTreeView, hItem, title);
		}
	}
	if (treeview_get_rootitem(hTreeView, hItem) == regist_treeitem) {
		// “o^ѓAѓCѓeѓЂ‚М•Ы‘¶
		set_cursor(TRUE);
		SendMessage(hWnd, WM_REGIST_SAVE, 0, 0);
		set_cursor(FALSE);
	}
	return TRUE;
}

/*
 * viewer_clear_name - ѓAѓCѓeѓЂ‚Мѓ^ѓCѓgѓ‹ѓNѓЉѓA
 */
static BOOL viewer_clear_name(const HWND hWnd, const HTREEITEM sel_item)
{
	HWND hTreeView = GetDlgItem(hWnd, ID_TREE);
	HWND hListView = GetDlgItem(GetDlgItem(hWnd, ID_CONTAINER), ID_LIST);
	HTREEITEM hItem;
	DATA_INFO *di;
	int i;

	if (GetFocus() == hTreeView) {
		if ((hItem = sel_item) == NULL) {
			hItem = TreeView_GetSelection(hTreeView);
		}
		if (treeview_get_rootitem(hTreeView, hItem) == clip_treeitem) {
			return FALSE;
		}
		if ((di = (DATA_INFO *)treeview_get_lparam(hTreeView, hItem)) == NULL || di->type != TYPE_ITEM) {
			return FALSE;
		}
		mem_free(&di->title);
		treeview_title_refresh(hTreeView, hItem);

		if (current_wnd == hListView) {
			// ѓЉѓXѓgѓrѓ…Ѓ[‚МЌXђV
			treeview_to_listview(hTreeView, TreeView_GetSelection(hTreeView), hListView);
		}
	} else if (GetFocus() == hListView) {
		if (ListView_GetSelectedCount(hListView) == 0) {
			return FALSE;
		}

		set_cursor(TRUE);
		i = -1;
		while ((i = ListView_GetNextItem(hListView, i, LVNI_SELECTED)) != -1) {
			if ((hItem = (HTREEITEM)listview_get_lparam(hListView, i)) == NULL) {
				continue;
			}
			if ((di = (DATA_INFO *)treeview_get_lparam(hTreeView, hItem)) == NULL || di->type != TYPE_ITEM) {
				continue;
			}
			mem_free(&di->title);
			treeview_title_refresh(hTreeView, hItem);
		}
		// ѓЉѓXѓgѓrѓ…Ѓ[‚МЌXђV
		treeview_to_listview(hTreeView, TreeView_GetSelection(hTreeView), hListView);
		set_cursor(FALSE);

	} else {
		return FALSE;
	}
	return TRUE;
}

/*
 * viewer_set_hotkey - ѓzѓbѓgѓLЃ[‚МђЭ’и
 */
static BOOL viewer_set_hotkey(const HWND hWnd, const HTREEITEM sel_item)
{
	HWND hTreeView = GetDlgItem(hWnd, ID_TREE);
	HWND hListView = GetDlgItem(GetDlgItem(hWnd, ID_CONTAINER), ID_LIST);
	HTREEITEM hItem = sel_item;
	DATA_INFO *di;
	BOOL ret;

	if (GetFocus() == hListView) {
		// ѓЉѓXѓgѓrѓ…Ѓ[‚М‘I‘рѓAѓCѓeѓЂЋж“ѕ
		if (ListView_GetNextItem(hListView, -1, LVNI_FOCUSED | LVNI_SELECTED) != -1) {
			hItem = (HTREEITEM)listview_get_lparam(hListView,
				ListView_GetNextItem(hListView, -1, LVNI_FOCUSED | LVNI_SELECTED));
		}
	}
	if (hItem == NULL) {
		hItem = TreeView_GetSelection(hTreeView);
	}
	if (treeview_get_rootitem(hTreeView, hItem) != regist_treeitem) {
		return FALSE;
	}
	if ((di = (DATA_INFO *)treeview_get_lparam(hTreeView, hItem)) == NULL || di->type != TYPE_ITEM) {
		return FALSE;
	}
	// ѓzѓbѓgѓLЃ[ђЭ’и
	ret = set_hotkey(hInst, hWnd, di);
	if (current_wnd == hListView) {
		// ѓЉѓXѓgѓrѓ…Ѓ[‚МЌXђV
		treeview_to_listview(hTreeView, TreeView_GetSelection(hTreeView), hListView);
	}
	if (ret == TRUE) {
		// “o^ѓAѓCѓeѓЂ‚М•Ы‘¶
		set_cursor(TRUE);
		SendMessage(hWnd, WM_REGIST_SAVE, 0, 0);
		set_cursor(FALSE);
	}
	return ret;
}

/*
 * viewer_delete_item - ѓAѓCѓeѓЂ‚МЌнЏњ
 */
static void viewer_delete_item(const HWND hWnd, const HTREEITEM sel_item)
{
	HWND hTreeView = GetDlgItem(hWnd, ID_TREE);
	HWND hListView = GetDlgItem(GetDlgItem(hWnd, ID_CONTAINER), ID_LIST);
	HTREEITEM hItem;
	DATA_INFO *di;
	int i;

	if (GetFocus() == hListView) {
		if (ListView_GetSelectedCount(hListView) == 0) {
			return;
		}
		// Љm”FѓЃѓbѓZЃ[ѓW
		if (option.viewer_delete_confirm == 1 &&
			MessageBox(hWnd, message_get_res(IDS_QUESTION_DELETE), WINDOW_TITLE, MB_ICONQUESTION | MB_YESNO) == IDNO) {
			return;
		}
		set_cursor(TRUE);
		SendMessage(hTreeView, WM_SETREDRAW, (WPARAM)FALSE, 0);
		SendMessage(hListView, WM_SETREDRAW, (WPARAM)FALSE, 0);
		while ((i = ListView_GetNextItem(hListView, -1, LVNI_SELECTED)) != -1) {
			if ((hItem = (HTREEITEM)listview_get_lparam(hListView, i)) != NULL) {
				// ѓzѓbѓgѓLЃ[‚М‰рЏњ
				if ((di = (DATA_INFO *)treeview_get_lparam(hTreeView, hItem)) != NULL) {
					regist_unregist_hotkey(main_wnd, di->child);
					if (di->hkey_id != 0) {
						UnregisterHotKey(main_wnd, di->hkey_id);
						di->hkey_id = 0;
					}
				}
				// ѓcѓЉЃ[ѓrѓ…Ѓ[‚©‚зѓAѓCѓeѓЂ‚рЌнЏњ
				treeview_delete_item(hTreeView, hItem);
			}
			// ѓЉѓXѓgѓrѓ…Ѓ[‚©‚зѓAѓCѓeѓЂ‚рЌнЏњ
			ListView_DeleteItem(hListView, i);
		}
		SendMessage(hTreeView, WM_SETREDRAW, (WPARAM)TRUE, 0);
		SendMessage(hListView, WM_SETREDRAW, (WPARAM)TRUE, 0);
		UpdateWindow(hTreeView);
		UpdateWindow(hListView);
		set_cursor(FALSE);
	} else {
		SetFocus(hTreeView);

		if ((hItem = sel_item) == NULL) {
			hItem = TreeView_GetSelection(hTreeView);
		}
		if (treeview_get_rootitem(hTreeView, hItem) == clip_treeitem) {
			return;
		}
		if (treeview_get_lparam(hTreeView, hItem) == 0) {
			return;
		}
		// Љm”FѓЃѓbѓZЃ[ѓW
		if (option.viewer_delete_confirm == 1 &&
			MessageBox(hWnd, message_get_res(IDS_QUESTION_DELETE), WINDOW_TITLE, MB_ICONQUESTION | MB_YESNO) == IDNO) {
			return;
		}
		set_cursor(TRUE);
		// ѓzѓbѓgѓLЃ[‚М‰рЏњ
		if ((di = (DATA_INFO *)treeview_get_lparam(hTreeView, hItem)) != NULL) {
			regist_unregist_hotkey(main_wnd, di->child);
			if (di->hkey_id != 0) {
				UnregisterHotKey(main_wnd, di->hkey_id);
				di->hkey_id = 0;
			}
		}
		// ѓcѓЉЃ[ѓrѓ…Ѓ[‚©‚зѓAѓCѓeѓЂ‚рЌнЏњ
		treeview_delete_item(hTreeView, hItem);
		if (current_wnd == hListView) {
			// ѓcѓЉЃ[ѓrѓ…Ѓ[‚ЖѓЉѓXѓgѓrѓ…Ѓ[‚М“ЇЉъ
			treeview_to_listview(hTreeView, TreeView_GetSelection(hTreeView), hListView);
		}
		set_cursor(FALSE);
	}
}

/*
 * viewer_format_to_option - Њ`Ћ®–ј‚рѓIѓvѓVѓ‡ѓ“‚Й‘—‚й
 */
static BOOL viewer_format_to_option(const HWND hWnd, const HTREEITEM sel_item, const TCHAR *mode)
{
	HWND hTreeView = GetDlgItem(hWnd, ID_TREE);
	HTREEITEM hItem = sel_item;
	DATA_INFO *di;
	TCHAR buf[BUF_SIZE];

	if (hItem == NULL) {
		hItem = TreeView_GetSelection(hTreeView);
	}
	if ((di = (DATA_INFO *)treeview_get_lparam(hTreeView, hItem)) == NULL || di->type != TYPE_DATA) {
		return FALSE;
	}
	wsprintf(buf, TEXT("%s%s"), mode, di->format_name);

	SendMessage(hWnd, WM_OPTION_SHOW, 0, (LPARAM)buf);
	return TRUE;
}

/*
 * viewer_tool_execute - ѓcЃ[ѓ‹‚МЋАЌs
 */
static void viewer_tool_execute(const HWND hWnd, const HTREEITEM sel_item, const int index)
{
	HWND hTreeView = GetDlgItem(hWnd, ID_TREE);
	HWND hListView = GetDlgItem(GetDlgItem(hWnd, ID_CONTAINER), ID_LIST);
	HWND focus_wnd;
	HTREEITEM hItem;
	DATA_INFO *di;
	TOOL_DATA_INFO *root_tdi = NULL;
	TOOL_DATA_INFO *new_tdi;
	TOOL_DATA_INFO *tdi;
	int ret;
	int i;

	if (GetFocus() == hListView && ListView_GetSelectedCount(hListView) > 0) {
		// ѓcЃ[ѓ‹—pѓfЃ[ѓ^‚МЌмђ¬
		i = -1;
		while ((i = ListView_GetNextItem(hListView, i, LVNI_SELECTED)) != -1) {
			if ((hItem = (HTREEITEM)listview_get_lparam(hListView, i)) == NULL) {
				continue;
			}
			if ((di = (DATA_INFO *)treeview_get_lparam(hTreeView, hItem)) == NULL) {
				continue;
			}
			new_tdi = tool_data_copy(di, FALSE);
			if (root_tdi == NULL) {
				root_tdi = new_tdi;
			} else {
				tdi->next = new_tdi;
			}
			tdi = new_tdi;
		}
		if (root_tdi != NULL) {
			// ѓcЃ[ѓ‹‚МЊД‚СЏo‚µ
			ret = tool_execute(hWnd, option.tool_info + index, CALLTYPE_VIEWER, NULL, root_tdi);
			tool_data_free(root_tdi);
			if (ret & TOOL_DATA_MODIFIED) {
				set_cursor(TRUE);
				// ѓAѓCѓeѓЂ‚М“ъЋћЌXђV
				i = -1;
				while ((i = ListView_GetNextItem(hListView, i, LVNI_SELECTED)) != -1) {
					if ((hItem = (HTREEITEM)listview_get_lparam(hListView, i)) != NULL) {
						viewer_set_datetime(hWnd, hItem);
					}
				}
				// ѓcѓЉЃ[‚М“ЇЉъ
				if (treeview_get_rootitem(hTreeView, TreeView_GetSelection(hTreeView)) == history_treeitem) {
					data_adjust(&history_data.child);
					treeview_sync_datainfo(hTreeView, history_treeitem, history_data.child);
					if (option.history_save == 1 && option.history_always_save == 1) {
						// —љ—р‚М•Ы‘¶
						SendMessage(hWnd, WM_HISTORY_SAVE, 0, 0);
					}
				} else {
					data_adjust(&regist_data.child);
					treeview_sync_datainfo(hTreeView, regist_treeitem, regist_data.child);
					// “o^ѓAѓCѓeѓЂ‚М•Ы‘¶
					SendMessage(hWnd, WM_REGIST_SAVE, 0, 0);
				}
				// ѓЉѓXѓgѓrѓ…Ѓ[‚МЌXђV
				treeview_to_listview(hTreeView, TreeView_GetSelection(hTreeView), hListView);
				SendMessage(hWnd, WM_VIEWER_REFRESH_STATUS, 0, 0);
				set_cursor(FALSE);
			}
		}
	} else {
		focus_wnd = (GetFocus() == hTreeView) ? hTreeView : GetDlgItem(hWnd, ID_CONTAINER);
		SetFocus(hTreeView);

		if ((hItem = sel_item) == NULL) {
			hItem = TreeView_GetSelection(hTreeView);
		}
		if (treeview_get_rootitem(hTreeView, hItem) == clip_treeitem) {
			di = NULL;
		} else if (hItem == history_treeitem) {
			di = &history_data;
		} else if (hItem == regist_treeitem) {
			di = &regist_data;
		} else {
			di = (DATA_INFO *)treeview_get_lparam(hTreeView, hItem);
		}

		// Ћ©“®•Ы‘¶‰рЏњ
		viewer_save_data(hWnd, TreeView_GetSelection(hTreeView));
		save_flag = FALSE;

		// ѓcЃ[ѓ‹‚МЊД‚СЏo‚µ
		ret = tool_execute(hWnd, option.tool_info + index, CALLTYPE_VIEWER, di, NULL);

		// Ћ©“®•Ы‘¶ђЭ’и
		if (ret & TOOL_DATA_MODIFIED) {
			viewer_sel_cheange(hWnd, TreeView_GetSelection(hTreeView), TreeView_GetSelection(hTreeView));
		}
		save_flag = TRUE;

		set_cursor(TRUE);
		if ((ret & TOOL_DATA_MODIFIED) && treeview_get_rootitem(hTreeView, hItem) != clip_treeitem) {
			viewer_set_datetime(hWnd, hItem);
			// ѓcѓЉЃ[‚М“ЇЉъ
			if (treeview_get_rootitem(hTreeView, hItem) == history_treeitem) {
				data_adjust(&history_data.child);
				treeview_sync_datainfo(hTreeView, history_treeitem, history_data.child);
				if (option.history_save == 1 && option.history_always_save == 1) {
					// —љ—р‚М•Ы‘¶
					SendMessage(hWnd, WM_HISTORY_SAVE, 0, 0);
				}
			} else {
				data_adjust(&regist_data.child);
				treeview_sync_datainfo(hTreeView, regist_treeitem, regist_data.child);
				// “o^ѓAѓCѓeѓЂ‚М•Ы‘¶
				SendMessage(hWnd, WM_REGIST_SAVE, 0, 0);
			}
			if (current_wnd == hListView) {
				// ѓЉѓXѓgѓrѓ…Ѓ[‚МЌXђV
				treeview_to_listview(hTreeView, TreeView_GetSelection(hTreeView), hListView);
			}
		}
		SendMessage(hWnd, WM_VIEWER_REFRESH_STATUS, 0, 0);
		set_cursor(FALSE);
		SetFocus(focus_wnd);
	}
}

/*
 * viewer_set_datetime - ѓAѓCѓeѓЂ‚МЌXђV“ъЋћ‚рђЭ’и
 */
static void viewer_set_datetime(const HWND hWnd, const HTREEITEM hItem)
{
	HWND hTreeView = GetDlgItem(hWnd, ID_TREE);
	HTREEITEM parent_item;
	DATA_INFO *di;

	if ((di = (DATA_INFO *)treeview_get_lparam(hTreeView, hItem)) == NULL) {
		return;
	}
	if (di->type == TYPE_DATA) {
		if ((parent_item = TreeView_GetParent(hTreeView, hItem)) == NULL) {
			return;
		}
		if ((di = (DATA_INFO *)treeview_get_lparam(hTreeView, parent_item)) == NULL) {
			return;
		}
	}
	data_set_modified(di);
}

/*
 * clipboard_to_datainfo - ѓNѓЉѓbѓvѓ{Ѓ[ѓh‚М“а—e‚©‚зѓfЃ[ѓ^ѓЉѓXѓg‚рЌмђ¬
 */
static DATA_INFO *clipboard_to_datainfo(const HWND hWnd)
{
	DATA_INFO *ret_di;
	TCHAR err_str[BUF_SIZE];

	// ѓNѓЉѓbѓvѓ{Ѓ[ѓh‚МЏ‰Љъ‰»
	if (OpenClipboard(hWnd) == FALSE) {
		return NULL;
	}
	// ѓfЃ[ѓ^‚рЋж“ѕ
	ret_di = clipboard_get_datainfo(FALSE, FALSE, err_str);
	CloseClipboard();
	return ret_di;
}

/*
 * viewer_get_clipboard_data - ѓNѓЉѓbѓvѓ{Ѓ[ѓh‚М“а—eЋж“ѕ
 */
void viewer_get_clipboard_data(const HWND hWnd, const HTREEITEM hItem)
{
	HWND hTreeView = GetDlgItem(hWnd, ID_TREE);
	DATA_INFO *di;
	HANDLE data;

	set_cursor(TRUE);
	if (hItem == clip_treeitem) {
		// ѓNѓЉѓbѓvѓ{Ѓ[ѓh‚М‘SѓfЃ[ѓ^Ћж“ѕ
		if (OpenClipboard(hWnd) == TRUE) {
			for (di = clip_di.child; di != NULL; di = di->next) {
				if (di->data == NULL && (data = GetClipboardData(di->format)) != NULL) {
					if ((di->data = format_copy_data(di->format_name, data, &di->size)) == NULL) {
						di->data = clipboard_copy_data(di->format, data, &di->size);
					}
				}
			}
			CloseClipboard();
		}
	} else if (treeview_get_rootitem(hTreeView, hItem) == clip_treeitem) {
		// ѓNѓЉѓbѓvѓ{Ѓ[ѓh‚МѓfЃ[ѓ^Ћж“ѕ
		di = (DATA_INFO *)treeview_get_lparam(hTreeView, hItem);
		if (di != NULL && di->data == NULL && OpenClipboard(hWnd) == TRUE) {
			if ((data = GetClipboardData(di->format)) != NULL) {
				if ((di->data = format_copy_data(di->format_name, data, &di->size)) == NULL) {
					di->data = clipboard_copy_data(di->format, data, &di->size);
				}
			}
			CloseClipboard();
		}
	}
	set_cursor(FALSE);
}

/*
 * treeview_to_listview - ѓcѓЉЃ[ѓrѓ…Ѓ[‚М“а—e‚рѓЉѓXѓgѓrѓ…Ѓ[‚ЙѓRѓsЃ[
 */
void treeview_to_listview(const HWND hTreeView, const HTREEITEM parent_item, const HWND hListView)
{
	HTREEITEM hItem;
	HTREEITEM start_item;
	HTREEITEM last_item = NULL;
	HTREEITEM find_item;
	LV_ITEM lvi;
	int i, j;

	SendMessage(hListView, WM_SETREDRAW, (WPARAM)FALSE, 0);

	// ѓcѓЉЃ[ѓrѓ…Ѓ[ѓAѓCѓeѓЂ‚М––”ц‚рЋж“ѕ
	hItem = TreeView_GetNextItem(hTreeView, parent_item, TVGN_CHILD);
	while (hItem != NULL) {
		last_item = hItem;
		hItem = TreeView_GetNextItem(hTreeView, hItem, TVGN_NEXT);
	}
	if (last_item == NULL) {
		SendMessage(hListView, WM_SETREDRAW, (WPARAM)TRUE, 0);
		UpdateWindow(hListView);
		return;
	}
	start_item = last_item;
	// ѓЉѓXѓgѓrѓ…Ѓ[ѓAѓCѓeѓЂ‚МЌнЏњ
	for (i = ListView_GetItemCount(hListView) - 1; i >= 0; i--) {
		hItem = (HTREEITEM)listview_get_lparam(hListView, i);
		if (hItem == NULL) {
			ListView_DeleteItem(hListView, i);
			continue;
		}
		// ѓAѓCѓeѓЂ‚МЊџЌх
		find_item = start_item;
		while (find_item != NULL && find_item != hItem) {
			find_item = TreeView_GetNextItem(hTreeView, find_item, TVGN_PREVIOUS);
		}
		if (find_item == NULL) {
			find_item = last_item;
			while (find_item != start_item && find_item != hItem) {
				find_item = TreeView_GetNextItem(hTreeView, find_item, TVGN_PREVIOUS);
			}
			if (find_item != hItem) {
				find_item = NULL;
			}
		}
		if (find_item == NULL) {
			// ѓcѓЉЃ[ѓrѓ…Ѓ[Њ©‚В‚©‚з‚И‚ўѓAѓCѓeѓЂ‚рЌнЏњ
			ListView_DeleteItem(hListView, i);
		} else {
			// Ћџ‚МЊџЌхЉJЋn€К’u
			start_item = find_item;
		}
	}

	// ѓЉѓXѓgѓrѓ…Ѓ[ѓAѓCѓeѓЂ‚М’З‰Б
	find_item = TreeView_GetNextItem(hTreeView, parent_item, TVGN_CHILD);
	i = 0;
	while (find_item != NULL) {
		// ѓЉѓXѓgѓrѓ…Ѓ[‚МѓAѓCѓeѓЂЏо•сђЭ’и
		lvi.mask = LVIF_TEXT | TVIF_IMAGE | LVIF_PARAM;
		lvi.iItem = i;
		lvi.iSubItem = 0;
		lvi.pszText = LPSTR_TEXTCALLBACK;
		lvi.cchTextMax = 0;
		lvi.iImage = I_IMAGECALLBACK;
		lvi.lParam = (LPARAM)find_item;

		if ((hItem = (HTREEITEM)listview_get_lparam(hListView, i)) == find_item) {
			ListView_SetItem(hListView, &lvi);
		} else {
			// ѓЉѓXѓgѓrѓ…Ѓ[ѓAѓCѓeѓЂ‚М’З‰Б
			ListView_InsertItem(hListView, &lvi);
		}
		find_item = TreeView_GetNextItem(hTreeView, find_item, TVGN_NEXT);
		i++;
	}
	if (i != ListView_GetItemCount(hListView)) {
		for (j = ListView_GetItemCount(hListView); j >= i; j--) {
			ListView_DeleteItem(hListView, j);
		}
	}
	ListView_RedrawItems(hListView, 0, ListView_GetItemCount(hListView));
	SendMessage(hListView, WM_SETREDRAW, (WPARAM)TRUE, 0);
	UpdateWindow(hListView);
}

/*
 * listview_get_disp_item - ѓЉѓXѓgѓrѓ…Ѓ[‚Й•\Ћ¦‚·‚йѓAѓCѓeѓЂЏо•с‚МђЭ’и
 */
static void listview_get_disp_item(const HWND hTreeView, LV_ITEM *lvi)
{
	HTREEITEM hItem = (HTREEITEM)lvi->lParam;
	DATA_INFO *di, *cdi;
	TCHAR *str_hkey;
	int size;

	if (hItem == NULL) {
		return;
	}
	if ((di = (DATA_INFO *)treeview_get_lparam(hTreeView, hItem)) == NULL) {
		return;
	}

	// ѓAѓCѓRѓ“
	if (lvi->mask & LVIF_IMAGE) {
		lvi->iImage = treeview_get_icon(hTreeView, hItem);
	}

	// ѓeѓLѓXѓg
	if (lvi->mask & LVIF_TEXT) {
		switch (lvi->iSubItem) {
		case 0:
			treeview_get_text(hTreeView, hItem, lvi->pszText);
			break;

		case 1:
			if (di->type != TYPE_FOLDER) {
				size = 0;
				for (cdi = di->child; cdi != NULL; cdi = cdi->next) {
					size += cdi->size;
				}
				_itot_s(size, lvi->pszText, lvi->cchTextMax, 10);
			}
			break;

		case 2:
			if (di->type != TYPE_FOLDER) {
				data_get_modified_string(di, lvi->pszText);
			}
			break;

		case 3:
			if (treeview_get_rootitem(hTreeView, hItem) == history_treeitem) {
				// ѓEѓBѓ“ѓhѓEѓ^ѓCѓgѓ‹
				if (di->window_name != NULL) {
					lstrcpy(lvi->pszText, di->window_name);
				}
			} else {
				// ѓzѓbѓgѓLЃ[
				if ((str_hkey = menu_get_keyname(di->op_modifiers, di->op_virtkey)) != NULL) {
					lstrcpy(lvi->pszText, str_hkey);
					mem_free(&str_hkey);
				}
			}
			break;
		}
	}
}

/*
 * viewer_save_data - •\Ћ¦ѓfЃ[ѓ^‚М•Ы‘¶
 */
static void viewer_save_data(const HWND hWnd, const HTREEITEM hItem)
{
	DATA_INFO *di;
	BOOL ret;

	if (save_flag == FALSE || current_wnd == NULL ||
		treeview_get_rootitem(GetDlgItem(hWnd, ID_TREE), hItem) == clip_treeitem) {
		return;
	}
	// ѓfЃ[ѓ^Ћж“ѕ
	if ((di = (DATA_INFO *)treeview_get_lparam(GetDlgItem(hWnd, ID_TREE), hItem)) == NULL) {
		return;
	}
	switch (di->type) {
	case TYPE_FOLDER:
		break;

	case TYPE_ITEM:
		if ((di = format_get_priority_highest(di)) == NULL) {
			return;
		}
	case TYPE_DATA:
		// ѓfЃ[ѓ^•Ы‘¶
		set_cursor(TRUE);
		if (current_wnd == GetDlgItem(GetDlgItem(hWnd, ID_CONTAINER), ID_BINVIEW)) {
			// ѓoѓCѓiѓЉ•\Ћ¦‚М•Ы‘¶
			ret = SendMessage(current_wnd, WM_SAVE_BINDATA, 0, (LPARAM)di);
		} else {
			// Њ`Ћ®–€‚МѓfЃ[ѓ^•Ы‘¶
			ret = format_window_save_data(current_wnd, di);
		}
		if (ret == TRUE) {
			// ѓcѓЉЃ[ѓrѓ…Ѓ[ѓAѓCѓeѓЂ‚Й”Ѕ‰f
			treeview_title_refresh(GetDlgItem(hWnd, ID_TREE), hItem);
			viewer_set_datetime(hWnd, hItem);

			if (treeview_get_rootitem(GetDlgItem(hWnd, ID_TREE), hItem) == regist_treeitem) {
				// “o^ѓAѓCѓeѓЂ‚М•Ы‘¶
				SendMessage(hWnd, WM_REGIST_SAVE, 0, 0);
			}
			SendMessage(hWnd, WM_VIEWER_REFRESH_STATUS, 0, 0);
		}
		set_cursor(FALSE);
		break;
	}
}

/*
 * viewer_set_list_column - ѓJѓ‰ѓЂ•\Ћ¦ђЭ’и
 */
static void viewer_set_list_column(const HWND hTreeView, const HWND hListView, const HTREEITEM hItem)
{
	LVCOLUMN lvcol;

	ZeroMemory(&lvcol, sizeof(LVCOLUMN));
	lvcol.mask = LVCF_TEXT;
	ListView_GetColumn(hListView, 3, &lvcol);
	if (treeview_get_rootitem(hTreeView, hItem) == history_treeitem) {
		lvcol.pszText = message_get_res(IDS_LISTCOLUMN_WINDOW);
	} else {
		lvcol.pszText = message_get_res(IDS_LISTCOLUMN_HOTKEY);
	}
	ListView_SetColumn(hListView, 3, &lvcol);
}

/*
 * viewer_sel_cheange - ‘I‘р•ПЌX
 */
static BOOL viewer_sel_cheange(const HWND hWnd, const HTREEITEM old_item, const HTREEITEM new_item)
{
	HWND hTreeView = GetDlgItem(hWnd, ID_TREE);
	HWND hContainer = GetDlgItem(hWnd, ID_CONTAINER);
	HTREEITEM cItem = new_item;
	DATA_INFO *di;
	int i;

	SendMessage(hContainer, WM_SETREDRAW, (WPARAM)FALSE, 0);

	if (current_wnd != NULL) {
		// ‹ЊѓEѓBѓ“ѓhѓE‚р”с•\Ћ¦
		ShowWindow(current_wnd, SW_HIDE);
		if (old_item != NULL) {
			if ((di = (DATA_INFO *)treeview_get_lparam(hTreeView, old_item)) != NULL) {
				switch (di->type) {
				case TYPE_FOLDER:
					break;

				case TYPE_ITEM:
					if ((di = format_get_priority_highest(di)) == NULL) {
						break;
					}
				case TYPE_DATA:
					// ѓfЃ[ѓ^•Ы‘¶
					viewer_save_data(hWnd, old_item);
					// ѓfЃ[ѓ^”с•\Ћ¦
					if (current_wnd == GetDlgItem(hContainer, ID_BINVIEW)) {
						SendMessage(current_wnd, WM_SET_BINDATA, 0, 0);
					} else {
						format_window_hide_data(current_wnd, di);
					}
					break;
				}
			}
		}
	}
	current_wnd = NULL;

	if (new_item != NULL) {
		if (new_item == history_treeitem || new_item == regist_treeitem) {
			current_wnd = GetDlgItem(hContainer, ID_LIST);
			ListView_DeleteAllItems(GetDlgItem(hContainer, ID_LIST));
			treeview_to_listview(hTreeView, new_item, GetDlgItem(hContainer, ID_LIST));
			ListView_SetItemState(GetDlgItem(hContainer, ID_LIST), 0, LVIS_FOCUSED, LVIS_FOCUSED);
			viewer_set_list_column(hTreeView, GetDlgItem(hContainer, ID_LIST), new_item);

		} else {
			if ((di = (DATA_INFO *)treeview_get_lparam(hTreeView, new_item)) == NULL) {
				SendMessage(hContainer, WM_SETREDRAW, (WPARAM)TRUE, 0);
				InvalidateRect(hContainer, NULL, FALSE);
				UpdateWindow(hContainer);
				return FALSE;
			}
			switch (di->type) {
			case TYPE_FOLDER:
				current_wnd = GetDlgItem(hContainer, ID_LIST);
				ListView_DeleteAllItems(GetDlgItem(hContainer, ID_LIST));
				treeview_to_listview(hTreeView, new_item, GetDlgItem(hContainer, ID_LIST));
				ListView_SetItemState(GetDlgItem(hContainer, ID_LIST), 0, LVIS_FOCUSED, LVIS_FOCUSED);
				viewer_set_list_column(hTreeView, GetDlgItem(hContainer, ID_LIST), new_item);
				break;

			case TYPE_ITEM:
				// —DђжЏ‡€К‚МЌ‚‚ўѓfЃ[ѓ^‚рЋж“ѕ
				if ((di = format_get_priority_highest(di)) == NULL) {
					SendMessage(hContainer, WM_SETREDRAW, (WPARAM)TRUE, 0);
					InvalidateRect(hContainer, NULL, FALSE);
					UpdateWindow(hContainer);
					return FALSE;
				}
				if (cItem == clip_treeitem) {
					cItem = TreeView_GetChild(hTreeView, cItem);
					while (cItem != NULL) {
						if ((DATA_INFO *)treeview_get_lparam(hTreeView, cItem) == di) {
							break;
						}
						cItem = TreeView_GetNextSibling(hTreeView, cItem);
					}
				}

			case TYPE_DATA:
				// ѓNѓЉѓbѓvѓ{Ѓ[ѓh‚М“а—eЋж“ѕ
				viewer_get_clipboard_data(hWnd, cItem);

				i = format_get_index(di->format_name, 0);
				if (option.viewer_show_bin == 0 && i != -1 && (option.format_info + i)->hWnd != NULL) {
					// Њ`Ћ®–€‚МѓEѓBѓ“ѓhѓE
					current_wnd = (option.format_info + i)->hWnd;
					// ѓfЃ[ѓ^•\Ћ¦
					format_window_show_data(current_wnd, di,
						(treeview_get_rootitem(hTreeView, new_item) == clip_treeitem) ? TRUE : FALSE);
				} else {
					// ѓoѓCѓiѓЉѓrѓ…Ѓ[
					current_wnd = GetDlgItem(hContainer, ID_BINVIEW);
					SendMessage(current_wnd, WM_SET_BINDATA,
						(treeview_get_rootitem(hTreeView, new_item) == clip_treeitem) ? TRUE : FALSE, (LPARAM)di);
				}
				break;
			}
		}
		if (current_wnd != NULL) {
			// ѓEѓBѓ“ѓhѓE•\Ћ¦
			ShowWindow(current_wnd, SW_SHOW);
			SendMessage(hContainer, WM_SIZE, 0, 0);
		}
	}
	SendMessage(hContainer, WM_SETREDRAW, (WPARAM)TRUE, 0);
	InvalidateRect(hContainer, NULL, FALSE);
	UpdateWindow(hContainer);

	SendMessage(hWnd, WM_VIEWER_REFRESH_STATUS, 0, 0);
	return TRUE;
}

/*
 * viewer_initialize - ѓEѓBѓ“ѓhѓE‚МЏ‰Љъ‰»
 */
static BOOL viewer_initialize(const HWND hWnd)
{
#define CF_CNT				1
	HIMAGELIST icon_list;
	HWND hTreeView;
	TCHAR buf[BUF_SIZE];
	TCHAR *str_menu;
	TCHAR *str_hkey;
	UINT cf[CF_CNT];
	int i;

	// ѓRѓ“ѓgѓЌЃ[ѓ‹‚МЌмђ¬
	// ѓcЃ[ѓ‹ѓoЃ[
	toolbar_create(hWnd, ID_TOOLBAR);
	// ѓCѓЃЃ[ѓWѓЉѓXѓg
	icon_list = create_imagelist(hInst);
	// ѓcѓЉЃ[ѓrѓ…Ѓ[
	hTreeView = treeview_create(hInst, hWnd, ID_TREE, icon_list);
	// Њ`Ћ®–€‚МЏо•с‚р•\Ћ¦‚·‚йѓRѓ“ѓeѓi
	container_create(hInst, hWnd, ID_CONTAINER);
	// ѓЉѓXѓgѓrѓ…Ѓ[
	listview_create(hInst, GetDlgItem(hWnd, ID_CONTAINER), ID_LIST, icon_list);
	// ѓoѓCѓiѓЉ•\Ћ¦ѓEѓBѓ“ѓhѓE
	binview_create(hInst, GetDlgItem(hWnd, ID_CONTAINER), ID_BINVIEW);
	// Њ`Ћ®–€‚МѓEѓBѓ“ѓhѓE
	format_window_create(GetDlgItem(hWnd, ID_CONTAINER));
	SendMessage(GetDlgItem(hWnd, ID_CONTAINER), WM_ALLHIDE, 0, 0);
	// ѓXѓeЃ[ѓ^ѓXѓoЃ[
	statusbar_create(hWnd, ID_STATUSBAR);

	// ѓNѓЉѓbѓvѓ{Ѓ[ѓh‚МѓAѓCѓeѓЂЋж“ѕ
	clip_di.type = TYPE_ITEM;
	clip_di.child = clipboard_to_datainfo(hWnd);
	// Џ‰ЉъѓAѓCѓeѓЂђЭ’и
	treeview_set_init_item(hTreeView);

	// ѓEѓBѓ“ѓhѓEѓЃѓjѓ…Ѓ[
	CheckMenuItem(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_FILE), ID_MENUITEM_WATCH,
		((option.main_clipboard_watch == 0) ? MF_UNCHECKED : MF_CHECKED));
	CheckMenuItem(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_VIEW), ID_MENUITEM_SHOW_TOOLBAR,
		((option.viewer_show_toolbar == 0) ? MF_UNCHECKED : MF_CHECKED));
	CheckMenuItem(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_VIEW), ID_MENUITEM_SHOW_STATUSBAR,
		((option.viewer_show_statusbar == 0) ? MF_UNCHECKED : MF_CHECKED));
	CheckMenuItem(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_VIEW), ID_MENUITEM_SHOW_FORMAT,
		((option.tree_show_format == 0) ? MF_UNCHECKED : MF_CHECKED));
	CheckMenuItem(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_VIEW), ID_MENUITEM_SHOW_BIN,
		((option.viewer_show_bin == 0) ? MF_UNCHECKED : MF_CHECKED));

	// ѓ|ѓbѓvѓAѓbѓvѓЃѓjѓ…Ѓ[
	h_popup_menu = LoadMenu(hInst, MAKEINTRESOURCE(IDR_MENU_POPUP));
	// ѓ|ѓbѓvѓAѓbѓvѓЃѓjѓ…Ѓ[‚ЙѓcЃ[ѓ‹ѓЃѓjѓ…Ѓ[‚рЉЦA•t‚Ї‚й
	GetMenuString(GetMenu(hWnd), WINDOW_MENU_TOOL, buf, BUF_SIZE - 1, MF_BYPOSITION);
	ModifyMenu(GetSubMenu(h_popup_menu, POPUPMENU_HISTORY), ID_MENUITEM_TOOL_MENU, MF_POPUP,
		(UINT)GetSubMenu(GetMenu(hWnd), WINDOW_MENU_TOOL), buf);
	ModifyMenu(GetSubMenu(h_popup_menu, POPUPMENU_HISTORY_LV), ID_MENUITEM_TOOL_MENU, MF_POPUP,
		(UINT)GetSubMenu(GetMenu(hWnd), WINDOW_MENU_TOOL), buf);
	ModifyMenu(GetSubMenu(h_popup_menu, POPUPMENU_REGIST), ID_MENUITEM_TOOL_MENU, MF_POPUP,
		(UINT)GetSubMenu(GetMenu(hWnd), WINDOW_MENU_TOOL), buf);
	ModifyMenu(GetSubMenu(h_popup_menu, POPUPMENU_REGIST_LV), ID_MENUITEM_TOOL_MENU, MF_POPUP,
		(UINT)GetSubMenu(GetMenu(hWnd), WINDOW_MENU_TOOL), buf);
	// ѓcЃ[ѓ‹ѓЃѓjѓ…Ѓ[‚ЙѓcЃ[ѓ‹‚рђЭ’и
	for (i = 0; i < option.tool_cnt; i++) {
		if (!((option.tool_info + i)->call_type & CALLTYPE_VIEWER)) {
			continue;
		}
		if (lstrcmp((option.tool_info + i)->title, TEXT("-")) == 0) {
			AppendMenu(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_TOOL), MF_SEPARATOR, 0, NULL);
		} else {
			str_hkey = menu_get_keyname((option.tool_info + i)->modifiers, (option.tool_info + i)->virtkey);
			if (str_hkey == NULL) {
				AppendMenu(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_TOOL), MF_STRING, ID_MENUITEM_TOOL + i,
					(option.tool_info + i)->title);
			} else {
				str_menu = mem_alloc(sizeof(TCHAR) * (lstrlen((option.tool_info + i)->title) + 1 + lstrlen(str_hkey) + 1));
				if (str_menu != NULL) {
					wsprintf(str_menu, TEXT("%s\t%s"), (option.tool_info + i)->title, str_hkey);
					AppendMenu(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_TOOL), MF_STRING, ID_MENUITEM_TOOL + i, str_menu);
					mem_free(&str_menu);
				}
				mem_free(&str_hkey);
			}
		}
	}
	if (GetMenuItemCount(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_TOOL)) == 2) {
		AppendMenu(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_TOOL), MF_STRING | MF_GRAYED,
			ID_MENUITEM_TOOL, message_get_res(IDS_VIEWER_MENU_TOOL_NOTHING));
	}

	// ѓhѓЌѓbѓvѓ^Ѓ[ѓQѓbѓg‚ЙђЭ’и
	cf[0] = CF_TEXT;
	OLE_IDropTarget_RegisterDragDrop(hWnd, WM_DRAGDROP, cf, CF_CNT);

	// ѓrѓ…Ѓ[ѓA‚рЉJ‚ў‚ЅЋћ‚ЙЋАЌs‚·‚йѓcЃ[ѓ‹
	tool_execute_all(hWnd, CALLTYPE_VIEWER_OPEN, NULL);
	return TRUE;
}

/*
 * viewer_set_controls - ѓRѓ“ѓgѓЌЃ[ѓ‹‚М€К’uЃAѓTѓCѓY‚рђЭ’и‚·‚й
 */
static void viewer_set_controls(const HWND hWnd)
{
	RECT window_rect;
	RECT toolbar_rect;
	DWORD toolbar_size = 0;
	RECT statusbar_rect;
	DWORD statusbar_size = 0;

	GetClientRect(hWnd, (LPRECT)&window_rect);

	// ToolBar‚МѓTѓCѓY‚МЋж“ѕ
	if (IsWindowVisible(GetDlgItem(hWnd, ID_TOOLBAR)) != 0) {
		GetWindowRect(GetDlgItem(hWnd, ID_TOOLBAR), (LPRECT)&toolbar_rect);
		toolbar_size = (toolbar_rect.bottom - toolbar_rect.top);
	}
	// StatusBar‚МѓTѓCѓY‚МЋж“ѕ
	if (IsWindowVisible(GetDlgItem(hWnd, ID_STATUSBAR)) != 0) {
		GetWindowRect(GetDlgItem(hWnd, ID_STATUSBAR), (LPRECT)&statusbar_rect);
		statusbar_size = (statusbar_rect.bottom - statusbar_rect.top);
	}

	// TreeView‚М€К’uЃAѓTѓCѓY‚МђЭ’и
	MoveWindow(GetDlgItem(hWnd, ID_TREE),
		0, toolbar_size, option.viewer_sep_size, window_rect.bottom - statusbar_size - toolbar_size, TRUE);
	UpdateWindow(GetDlgItem(hWnd, ID_TREE));

	// Container‚М€К’uЃAѓTѓCѓY‚МђЭ’и
	MoveWindow(GetDlgItem(hWnd, ID_CONTAINER), option.viewer_sep_size + (FRAME_CNT * 2), toolbar_size,
		window_rect.right - option.viewer_sep_size - (FRAME_CNT * 2), window_rect.bottom - statusbar_size - toolbar_size, TRUE);
	UpdateWindow(GetDlgItem(hWnd, ID_CONTAINER));
}

/*
 * viewer_close - ѓEѓBѓ“ѓhѓE‚р•В‚¶‚й
 */
static BOOL viewer_close(const HWND hWnd)
{
	HWND hListView;

	// ѓTѓCѓY‚М‹­ђ§•Ы‘¶
	SendMessage(hWnd, WM_EXITSIZEMOVE, 0, 0);

	// ѓrѓ…Ѓ[ѓA‚р•В‚¶‚йЋћ‚ЙЋАЌs‚·‚йѓcЃ[ѓ‹
	tool_execute_all(hWnd, CALLTYPE_VIEWER_CLOSE, NULL);

	set_cursor(TRUE);
	viewer_sel_cheange(hWnd, TreeView_GetSelection(GetDlgItem(hWnd, ID_TREE)), NULL);

	if (option.history_save == 1 && option.history_always_save == 1) {
		// —љ—р‚М•Ы‘¶
		SendMessage(hWnd, WM_HISTORY_SAVE, 0, 0);
	}
	// “o^ѓAѓCѓeѓЂ‚М•Ы‘¶
	SendMessage(hWnd, WM_REGIST_SAVE, 0, 0);

	// ѓtѓHЃ[ѓ}ѓbѓg–€‚МѓEѓBѓ“ѓhѓE‚М”jЉь
	format_window_destroy();

	// ѓfЃ[ѓ^‰р•ъ
	data_free(clip_di.child);
	clip_di.child = NULL;

	// ѓЉѓXѓgѓrѓ…Ѓ[‚МѓJѓ‰ѓЂ•ќЋж“ѕ
	hListView = GetDlgItem(GetDlgItem(hWnd, ID_CONTAINER), ID_LIST);
	option.list_column_data = ListView_GetColumnWidth(hListView, 0);
	option.list_column_size = ListView_GetColumnWidth(hListView, 1);
	option.list_column_date = ListView_GetColumnWidth(hListView, 2);
	option.list_column_window = ListView_GetColumnWidth(hListView, 3);

	// ѓcѓЉѓrѓ…Ѓ[Ѓ[‚М‰р•ъ
	treeview_close(GetDlgItem(hWnd, ID_TREE));
	// ѓЉѓXѓgѓrѓ…Ѓ[‚М‰р•ъ
	listview_close(hListView);
	// ѓCѓЃЃ[ѓWѓЉѓXѓg‚М‰р•ъ
	ImageList_Destroy((void *)TreeView_SetImageList(GetDlgItem(hWnd, ID_TREE), NULL, TVSIL_NORMAL));

	DestroyMenu(h_popup_menu);

	OLE_IDropTarget_RevokeDragDrop(hWnd);
	set_cursor(FALSE);
	return TRUE;
}

/*
 * viewer_proc - ѓEѓBѓ“ѓhѓE‚МѓvѓЌѓVЃ[ѓWѓѓ
 */
static LRESULT CALLBACK viewer_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LPIDROPTARGET_NOTIFY pdtn;
	static HWND FocusWnd;
	static BOOL sep_move;
	static BOOL ctrl_flag;
	static int DnD_keystate;
	int ret;

	switch (msg) {
	case WM_CREATE:
		// ѓEѓBѓ“ѓhѓEЌмђ¬
		viewer_initialize(hWnd);
		viewer_set_controls(hWnd);

		FocusWnd = GetDlgItem(hWnd, ID_TREE);
		SetFocus(FocusWnd);
		break;

	case WM_CLOSE:
		// ѓEѓBѓ“ѓhѓE‚р•В‚¶‚й
		viewer_close(hWnd);

		SendMessage(main_wnd, WM_VIEWER_NOTIFY_CLOSE, 0, 0);
		DestroyWindow(hWnd);
		break;

	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE) {
			set_cursor(TRUE);
			viewer_save_data(hWnd, TreeView_GetSelection(GetDlgItem(hWnd, ID_TREE)));
			set_cursor(FALSE);
			if (IsIconic(hWnd) == 0 && GetFocus() != NULL) {
				FocusWnd = GetFocus();
			}
		}
		break;

	case WM_SETFOCUS:
		SetFocus(FocusWnd);
		break;

	case WM_LBUTTONDOWN:
		// ‹«ЉE‚М€Ъ“®
		if (GetForegroundWindow() != hWnd) {
			break;
		}
		if (sep_move == FALSE) {
			if (frame_initialize(hWnd) == FALSE) {
				break;
			}
			sep_move = TRUE;
			frame_draw(hWnd, GetDlgItem(hWnd, ID_TREE));
			SetTimer(hWnd, TIMER_SEP, 1, NULL);
		}
		break;

	case WM_MOUSEMOVE:
		if (sep_move == TRUE) {
			frame_draw(hWnd, GetDlgItem(hWnd, ID_TREE));
		}
		if (DnD_mode == TRUE) {
			// ѓhѓ‰ѓbѓO’†
			ret = dragdrop_set_drag_item(hWnd);
			if (ret == DRAG_MODE_NONE) {
				SetCursor(LoadCursor(hInst, MAKEINTRESOURCE(IDC_CURSOR_BAN)));
			} else if (ret & DRAG_MODE_MOVE) {
				SetCursor(LoadCursor(hInst, MAKEINTRESOURCE(IDC_CURSOR_MOVE)));
			} else {
				SetCursor(LoadCursor(hInst, MAKEINTRESOURCE(IDC_CURSOR_COPY)));
			}
		}
		break;

	case WM_LBUTTONUP:
		if (sep_move == TRUE) {
			KillTimer(hWnd, TIMER_SEP);
			sep_move = FALSE;
			if ((ret = frame_draw_end(hWnd)) == -1) {
				break;
			}
			option.viewer_sep_size = ret;
			set_cursor(TRUE);
			viewer_set_controls(hWnd);
			set_cursor(FALSE);
		}
	case WM_RBUTTONUP:
		if (DnD_mode == TRUE) {
			// ѓhѓЌѓbѓv
			KillTimer(hWnd, TIMER_DRAG);
			ReleaseCapture();
			DnD_mode = FALSE;
			TreeView_Select(GetDlgItem(hWnd, ID_TREE), NULL, TVGN_DROPHILITE);
			ListView_SetItemState(GetDlgItem(GetDlgItem(hWnd, ID_CONTAINER), ID_LIST), -1, 0, LVIS_DROPHILITED);

			// ѓzѓbѓgѓLЃ[‚М‰рЏњ
			SendMessage(hWnd, WM_UNREGIST_HOTKEY, 0, 0);
			// ѓhѓЌѓbѓvЏ€—ќ
			dragdrop_drop_item(hWnd, msg);
			// ѓzѓbѓgѓLЃ[‚МђЭ’и
			SendMessage(hWnd, WM_REGIST_HOTKEY, 0, 0);
		}
		break;

	case WM_SIZE:
		// ѓTѓCѓY•ПЌX
		SendMessage(GetDlgItem(hWnd, ID_TOOLBAR), WM_SIZE, wParam, lParam);
		SendMessage(GetDlgItem(hWnd, ID_STATUSBAR), WM_SIZE, wParam, lParam);
		viewer_set_controls(hWnd);
		break;

	case WM_EXITSIZEMOVE:
		// ѓTѓCѓY•ПЌXЉ®—№
		if (IsWindowVisible(hWnd) != 0 && IsIconic(hWnd) == 0 && IsZoomed(hWnd) == 0) {
			GetWindowRect(hWnd, (LPRECT)&option.viewer_rect);
			option.viewer_rect.right -= option.viewer_rect.left;
			option.viewer_rect.bottom -= option.viewer_rect.top;
		}
		break;

	case WM_TIMER:
		switch (wParam) {
		// ‹«ЉE‚М€Ъ“®
		case TIMER_SEP:
			if (hWnd != GetForegroundWindow() || GetAsyncKeyState(VK_ESCAPE) < 0 ||
				GetAsyncKeyState(VK_RBUTTON) < 0) {
				KillTimer(hWnd, wParam);
				frame_free();
				sep_move = FALSE;
				InvalidateRect(hWnd, NULL, FALSE);
				UpdateWindow(hWnd);
			}
			break;

		// D&D
		case TIMER_DRAG:
			if (hWnd != GetForegroundWindow() || GetKeyState(VK_ESCAPE) < 0) {
				KillTimer(hWnd, wParam);
				ReleaseCapture();
				DnD_mode = FALSE;
				TreeView_Select(GetDlgItem(hWnd, ID_TREE), NULL, TVGN_DROPHILITE);
				ListView_SetItemState(GetDlgItem(GetDlgItem(hWnd, ID_CONTAINER), ID_LIST), -1, 0, LVIS_DROPHILITED);
			} else if (ctrl_flag == FALSE && GetKeyState(VK_CONTROL) < 0) {
				SendMessage(hWnd, WM_MOUSEMOVE, 0, 0);
				ctrl_flag = TRUE;
			} else if (ctrl_flag == TRUE && GetKeyState(VK_CONTROL) >= 0) {
				SendMessage(hWnd, WM_MOUSEMOVE, 0, 0);
				ctrl_flag = FALSE;
			}
			break;

		case TIMER_SET_MENU:
			KillTimer(hWnd, wParam);
			set_enable_window_menu(hWnd);
			break;

		case TIMER_REFLECT_DATA:
			KillTimer(hWnd, wParam);
			set_cursor(TRUE);
			viewer_save_data(hWnd, TreeView_GetSelection(GetDlgItem(hWnd, ID_TREE)));
			set_cursor(FALSE);
			break;

		case TIMER_LV_REFRESH:
			KillTimer(hWnd, wParam);
			set_cursor(TRUE);
			treeview_to_listview(GetDlgItem(hWnd, ID_TREE),
				TreeView_GetSelection(GetDlgItem(hWnd, ID_TREE)),
				GetDlgItem(GetDlgItem(hWnd, ID_CONTAINER), ID_LIST));
			set_cursor(FALSE);
			break;

		case TIMER_TV_ACTION:
			KillTimer(hWnd, wParam);
			if (option.list_default_action == 2) {
				// –ј‘O‚р•t‚Ї‚Д•Ы‘¶
				viewer_data_save(hWnd, TreeView_GetSelection(GetDlgItem(hWnd, ID_TREE)));
			} else if (option.list_default_action == 1) { // 2022-11-30 kaiu@mail.ru
				// ѓNѓЉѓbѓvѓ{Ѓ[ѓh‚Й‘—‚й
				SendMessage(hWnd, WM_COMMAND, ID_MENUITEM_CLIPBOARD, 0);
			} else {
				HWND hTreeView = GetDlgItem(hWnd, ID_TREE);				
				TreeView_Expand(hTreeView, TreeView_GetSelection(GetDlgItem(hWnd, ID_TREE)), TVE_TOGGLE);
			}
			break;
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_ACCEL_CTRL_TAB:
			// ѓtѓHЃ[ѓJѓX‚М€Ъ“®
			if (GetFocus() != GetDlgItem(hWnd, ID_TREE)) {
				SetFocus(GetDlgItem(hWnd, ID_TREE));
			} else {
				SetFocus(GetDlgItem(hWnd, ID_CONTAINER));
			}
			break;

		case ID_MENUITEM_SAVE:
			// Њ»ЌЭ‚МЏу‘Ф‚р•Ы‘¶
			set_cursor(TRUE);
			viewer_save_data(hWnd, TreeView_GetSelection(GetDlgItem(hWnd, ID_TREE)));
			SendMessage(hWnd, WM_REGIST_SAVE, 0, 0);
			SendMessage(hWnd, WM_HISTORY_SAVE, 0, 0);
			SendMessage(hWnd, WM_OPTION_SAVE, 0, 0);
			set_cursor(FALSE);
			break;

		case ID_MENUITEM_IMPORT:
			// ѓCѓ“ѓ|Ѓ[ѓg
			if (regist_treeitem == NULL) {
				break;
			}
			viewer_import_item(hWnd);
			break;

		case ID_MENUITEM_EXPORT:
			// ѓGѓNѓXѓ|Ѓ[ѓg
			if (regist_treeitem == NULL) {
				break;
			}
			viewer_export_item(hWnd);
			break;

		case ID_MENUITEM_WATCH:
			// ѓNѓЉѓbѓvѓ{Ѓ[ѓh‚МЉДЋ‹ђШ‚и‘Ц‚¦
			SendMessage(hWnd, WM_SET_CLIPBOARD_WATCH, !option.main_clipboard_watch, 0);
			break;

		case ID_MENUITEM_CLOSE:
			SendMessage(hWnd, WM_CLOSE, 0, 0);
			break;

		case ID_MENUITEM_EXIT:
			SendMessage(main_wnd, WM_CLOSE, 0, 0);
			break;

		case ID_MENUITEM_SELECT_ALL:
			// ‚·‚Ч‚Д‘I‘р
			if (current_wnd != GetDlgItem(GetDlgItem(hWnd, ID_CONTAINER), ID_LIST)) {
				break;
			}
			SetFocus(current_wnd);
			ListView_SetItemState(current_wnd, -1, LVIS_SELECTED, LVIS_SELECTED);
			break;

		case ID_MENUITEM_SHOW_TOOLBAR:
			// ѓcЃ[ѓ‹ѓoЃ[•\Ћ¦ђШ‘Ц
			if (option.viewer_show_toolbar == 0) {
				option.viewer_show_toolbar = 1;
				CheckMenuItem(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_VIEW), LOWORD(wParam), MF_CHECKED);
				ShowWindow(GetDlgItem(hWnd, ID_TOOLBAR), SW_SHOW);
			} else {
				option.viewer_show_toolbar = 0;
				CheckMenuItem(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_VIEW), LOWORD(wParam), MF_UNCHECKED);
				ShowWindow(GetDlgItem(hWnd, ID_TOOLBAR), SW_HIDE);
			}
			viewer_set_controls(hWnd);
			break;

		case ID_MENUITEM_SHOW_STATUSBAR:
			// ѓXѓeЃ[ѓ^ѓXѓoЃ[•\Ћ¦ђШ‘Ц
			if (option.viewer_show_statusbar == 0) {
				option.viewer_show_statusbar = 1;
				CheckMenuItem(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_VIEW), LOWORD(wParam), MF_CHECKED);
				ShowWindow(GetDlgItem(hWnd, ID_STATUSBAR), SW_SHOW);
			} else {
				option.viewer_show_statusbar = 0;
				CheckMenuItem(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_VIEW), LOWORD(wParam), MF_UNCHECKED);
				ShowWindow(GetDlgItem(hWnd, ID_STATUSBAR), SW_HIDE);
			}
			viewer_set_controls(hWnd);
			break;

		case ID_MENUITEM_SHOW_FORMAT:
			// Њ`Ћ®•\Ћ¦
			if (option.tree_show_format == 0) {
				option.tree_show_format = 1;
				CheckMenuItem(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_VIEW), LOWORD(wParam), MF_CHECKED);
			} else {
				option.tree_show_format = 0;
				CheckMenuItem(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_VIEW), LOWORD(wParam), MF_UNCHECKED);
			}
			set_cursor(TRUE);
			SendMessage(hWnd, WM_SETREDRAW, (WPARAM)FALSE, 0);

			viewer_sel_cheange(hWnd, TreeView_GetSelection(GetDlgItem(hWnd, ID_TREE)), NULL);
			treeview_set_init_item(GetDlgItem(hWnd, ID_TREE));

			SendMessage(hWnd, WM_SETREDRAW, (WPARAM)TRUE, 0);
			InvalidateRect(hWnd, NULL, FALSE);
			UpdateWindow(hWnd);
			set_cursor(FALSE);
			break;

		case ID_MENUITEM_SHOW_BIN:
			// ѓoѓCѓiѓЉ•\Ћ¦
			if (option.viewer_show_bin == 0) {
				option.viewer_show_bin = 1;
				CheckMenuItem(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_VIEW), LOWORD(wParam), MF_CHECKED);
			} else {
				option.viewer_show_bin = 0;
				CheckMenuItem(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_VIEW), LOWORD(wParam), MF_UNCHECKED);
			}
			set_cursor(TRUE);
			viewer_sel_cheange(hWnd, TreeView_GetSelection(GetDlgItem(hWnd, ID_TREE)),
				TreeView_GetSelection(GetDlgItem(hWnd, ID_TREE)));
			set_cursor(FALSE);
			break;

		case ID_MENUITEM_OPTION:
			// ѓIѓvѓVѓ‡ѓ“
			SendMessage(hWnd, WM_OPTION_SHOW, 0, 0);
			break;

		case ID_MENUITEM_TOOL_SET:
			// ѓcЃ[ѓ‹ђЭ’и
			SendMessage(hWnd, WM_OPTION_SHOW, 0, (LPARAM)OPTION_SHOW_TOOL);
			break;

		case ID_MENUITEM_ABOUT:
			// ѓoЃ[ѓWѓ‡ѓ“Џо•с
		{
			TCHAR var_msg[2048];
			TCHAR path[MAX_PATH];
			DWORD size;
			lstrcpy(var_msg, TEXT("CLCL(クルクル)は、クリップボードの履歴を取るソフトです。\nCLCL is clipboard caching utility.\nCLCL — это утилита кэширования буфера обмена\n\n"));
			lstrcat(var_msg, APP_NAME);
						
			GetModuleFileName(NULL, path, sizeof(path));
			size = GetFileVersionInfoSize(path, NULL);
			if (size) {
				VS_FIXEDFILEINFO* FileInfo;
				UINT len;
				BYTE* buf = mem_alloc(size);
				if (buf != NULL) {
					GetFileVersionInfo(path, 0, size, buf);
					VerQueryValue(buf, TEXT("\\"), &FileInfo, &len);
					wsprintf(var_msg + lstrlen(var_msg), TEXT(" Ver %d.%d.%d"),
						HIWORD(FileInfo->dwFileVersionMS),
						LOWORD(FileInfo->dwFileVersionMS),
						HIWORD(FileInfo->dwFileVersionLS));
					mem_free(&buf);
				}
			}
			
			lstrcat(var_msg, TEXT("\nCopyright (C) 1996-2022 by Ohno Tomoaki.\nAll rights reserved.\n\n")
				TEXT("WEB SITE:  https://www.nakka.com/\nE-MAIL:  nakka@nakka.com\n\nRussian version editor:  Krutov A\nweb site:  kaiu.narod.ru\ne-mail:  kaiu@mail.ru"));
			MessageBox(hWnd, var_msg, TEXT("説明する \\ About \\ О программе"), MB_OK | MB_ICONINFORMATION);
		}
			break;

		case ID_MENUITEM_OPEN:
			// •\Ћ¦
			viewer_show_item(hWnd);
			break;

		case ID_MENUITEM_CLIPBOARD_TB:
			lParam = 0;
		case ID_MENUITEM_CLIPBOARD:
			// ѓNѓЉѓbѓvѓ{Ѓ[ѓh‚Й‘—‚й
			treeview_to_clipboard(hWnd, (HTREEITEM)lParam);
			break;

		case ID_MENUITEM_PASTE_TB:
			lParam = 0;
		case ID_MENUITEM_PASTE:
			// ѓNѓЉѓbѓvѓ{Ѓ[ѓh‚©‚з“\‚и•t‚Ї
			viewer_item_paste(hWnd, (HTREEITEM)lParam);
			break;

		case ID_MENUITEM_REGIST_ADD_TB:
			lParam = 0;
		case ID_MENUITEM_REGIST_ADD:
			// “o^ѓAѓCѓeѓЂ‚Й’З‰Б
			if (regist_treeitem == NULL) {
				break;
			}
			viewer_regist_add(hWnd, (HTREEITEM)lParam, FALSE);
			break;

		case ID_MENUITEM_REGIST_MOVE:
			// “o^ѓAѓCѓeѓЂ‚М€Ъ“®
			if (regist_treeitem == NULL) {
				break;
			}
			viewer_regist_add(hWnd, (HTREEITEM)lParam, TRUE);
			break;

		case ID_MENUITEM_UP_TB:
			lParam = 0;
		case ID_MENUITEM_UP:
			// Џг‚Ц
			viewer_move_up(hWnd, (HTREEITEM)lParam);
			break;

		case ID_MENUITEM_DOWN_TB:
			lParam = 0;
		case ID_MENUITEM_DOWN:
			// ‰є‚Ц
			viewer_move_down(hWnd, (HTREEITEM)lParam);
			break;

		case ID_MENUITEM_NEW_ITEM_TB:
			lParam = 0;
		case ID_MENUITEM_NEW_ITEM:
			// ђV‹KЌмђ¬
			viewer_create_item(hWnd, (HTREEITEM)lParam);
			break;

		case ID_MENUITEM_CREATE_FOLDER:
			// ѓtѓHѓ‹ѓ_‚МЌмђ¬
			viewer_create_folder(hWnd, (HTREEITEM)lParam);
			break;

		case ID_MENUITEM_DATA_SAVE:
			// –ј‘O‚р•t‚Ї‚Д•Ы‘¶
			viewer_data_save(hWnd, (HTREEITEM)lParam);
			break;

		case ID_MENUITEM_RENAME:
			// –ј‘O‚М•ПЌX
			if (GetFocus() == GetDlgItem(hWnd, ID_TREE)) {
				TreeView_EditLabel(GetDlgItem(hWnd, ID_TREE),
					((lParam != 0) ? (HTREEITEM)lParam : TreeView_GetSelection(GetDlgItem(hWnd, ID_TREE))));
			} else {
				ListView_SetItemState(GetDlgItem(GetDlgItem(hWnd, ID_CONTAINER), ID_LIST), -1, 0, LVIS_SELECTED);
				ListView_SetItemState(GetDlgItem(GetDlgItem(hWnd, ID_CONTAINER), ID_LIST),
					ListView_GetNextItem(GetDlgItem(GetDlgItem(hWnd, ID_CONTAINER), ID_LIST), -1, LVNI_FOCUSED),
					LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
				ListView_EditLabel(GetDlgItem(GetDlgItem(hWnd, ID_CONTAINER), ID_LIST),
					ListView_GetNextItem(GetDlgItem(GetDlgItem(hWnd, ID_CONTAINER), ID_LIST), -1, LVNI_FOCUSED));
			}
			break;

		case ID_MENUITEM_CLEAR_NAME:
			// –ј‘O‚МѓNѓЉѓA
			viewer_clear_name(hWnd, (HTREEITEM)lParam);
			break;

		case ID_MENUITEM_SET_HOTKEY:
			// ѓzѓbѓgѓLЃ[‚МђЭ’и
			SendMessage(hWnd, WM_UNREGIST_HOTKEY, 0, 0);
			if (viewer_set_hotkey(hWnd, (HTREEITEM)lParam) == TRUE) {
				SendMessage(hWnd, WM_VIEWER_REFRESH_STATUS, 0, 0);
			}
			SendMessage(hWnd, WM_REGIST_HOTKEY, 0, 0);
			break;

		case ID_MENUITEM_DELETE_TB:
			lParam = 0;
		case ID_MENUITEM_DELETE:
			// ЌнЏњ
			viewer_delete_item(hWnd, (HTREEITEM)lParam);
			SendMessage(hWnd, WM_VIEWER_REFRESH_STATUS, 0, 0);
			break;

		case ID_MENUITEM_SET_FORMAT:
			viewer_format_to_option(hWnd, (HTREEITEM)lParam, OPTION_SHOW_FORMAT);
			break;

		case ID_MENUITEM_SET_FILTER:
			viewer_format_to_option(hWnd, (HTREEITEM)lParam, OPTION_SHOW_FILTER);
			break;

		default:
			if (LOWORD(wParam) >= ID_MENUITEM_TOOL && LOWORD(wParam) < ID_MENUITEM_TOOL + 1000) {
				viewer_tool_execute(hWnd, (HTREEITEM)lParam, LOWORD(wParam) - ID_MENUITEM_TOOL);
			}
			break;
		}
		break;

	case WM_NOTIFY:
		// ѓRѓ“ѓgѓЌЃ[ѓ‹’К’mѓЃѓbѓZЃ[ѓW
		// ѓcѓЉЃ[ѓrѓ…Ѓ[
		if (((NMHDR *)lParam)->hwndFrom == GetDlgItem(hWnd, ID_TREE)) {
			return treeview_notify_proc(hWnd, lParam);
		}
		// ѓЉѓXѓgѓrѓ…Ѓ[
		if (((NMHDR *)lParam)->hwndFrom == GetDlgItem(GetDlgItem(hWnd, ID_CONTAINER), ID_LIST)) {
			return listview_notify_proc(hWnd, lParam);
		}
		if (((NMHDR *)lParam)->hwndFrom == GetWindow(GetDlgItem(GetDlgItem(hWnd, ID_CONTAINER), ID_LIST), GW_CHILD)) {
			return listview_header_notify_proc(GetDlgItem(GetDlgItem(hWnd, ID_CONTAINER), ID_LIST),
				GetDlgItem(hWnd, ID_TREE), lParam);
		}
		// ѓXѓeЃ[ѓ^ѓXѓoЃ[
		if (((NMHDR *)lParam)->code == TTN_NEEDTEXT && ((NMHDR *)lParam)->idFrom < 3) {
			return statusbar_notify_proc(GetDlgItem(hWnd, ID_STATUSBAR), lParam);
		}
		// ѓcЃ[ѓ‹ѓoЃ[
		if (((NMHDR *)lParam)->code == TTN_NEEDTEXT) {
			((TOOLTIPTEXT*)lParam)->hinst = hInst;
			((TOOLTIPTEXT*)lParam)->lpszText = MAKEINTRESOURCE(((NMHDR *)lParam)->idFrom);
		}
		break;

	case WM_TV_EVENT:
		// ѓcѓЉЃ[ѓrѓ…Ѓ[ѓCѓxѓ“ѓg
		switch (wParam) {
		case TVN_BEGINLABELEDIT:
			if (treeview_get_rootitem(GetDlgItem(hWnd, ID_TREE), ((TV_DISPINFO *)lParam)->item.hItem) == regist_treeitem &&
				treeview_get_lparam(GetDlgItem(hWnd, ID_TREE), ((TV_DISPINFO *)lParam)->item.hItem) != 0 &&
				((DATA_INFO *)treeview_get_lparam(GetDlgItem(hWnd, ID_TREE), ((TV_DISPINFO *)lParam)->item.hItem))->type != TYPE_DATA) {
				SendMessage(hWnd, WM_ENABLE_ACCELERATOR, FALSE, 0);
				return FALSE;
			}
			return TRUE;

		case TVN_ENDLABELEDIT:
			SendMessage(hWnd, WM_ENABLE_ACCELERATOR, TRUE, 0);
			return viewer_rename(hWnd, ((TV_DISPINFO *)lParam)->item.hItem, ((TV_DISPINFO *)lParam)->item.pszText);

		case TVN_SELCHANGED:
			set_cursor(TRUE);
			viewer_sel_cheange(hWnd,
				((NM_TREEVIEW *)lParam)->itemOld.hItem, ((NM_TREEVIEW *)lParam)->itemNew.hItem);
			set_enable_window_menu(hWnd);
			set_cursor(FALSE);
			return TRUE;

		case TVN_KEYDOWN:
			switch (((TV_KEYDOWN *)lParam)->wVKey) {
			case 'A':
				if (GetKeyState(VK_CONTROL) < 0) {
					// ‚·‚Ч‚Д‘I‘р
					SendMessage(hWnd, WM_COMMAND, ID_MENUITEM_SELECT_ALL, 0);
					return TRUE;
				}
				break;

			case 'C':
				if (GetKeyState(VK_CONTROL) < 0) {
					// ѓRѓsЃ[
					SendMessage(hWnd, WM_COMMAND, ID_MENUITEM_CLIPBOARD, 0);
					return TRUE;
				}
				break;

			case 'V':
				if (GetKeyState(VK_CONTROL) < 0) {
					// “\‚и•t‚Ї
					SendMessage(hWnd, WM_COMMAND, ID_MENUITEM_PASTE, 0);
					return TRUE;
				}
				break;

			case VK_TAB:
				SetFocus(GetDlgItem(hWnd, ID_CONTAINER));
				return TRUE;

			case VK_RETURN:												
				SetTimer(hWnd, TIMER_TV_ACTION, 1, NULL);
				return TRUE;			
				

			case VK_DELETE:
				SendMessage(hWnd, WM_COMMAND, ID_MENUITEM_DELETE, 0);
				return TRUE;

			case VK_APPS:
				viewer_show_menu(hWnd);
				return TRUE;

			case VK_F2:
				SendMessage(hWnd, WM_COMMAND, ID_MENUITEM_RENAME, 0);
				return TRUE;
			}
			break;

		case TVN_BEGINDRAG:
		case TVN_BEGINRDRAG:
			SetFocus(GetDlgItem(hWnd, ID_TREE));
			if (viewer_ole_start_drag(hWnd, ((NM_TREEVIEW *)lParam)->itemNew.hItem) == FALSE) {
				if ((DnD_mode = dragdrop_start_drag(hWnd, ((NM_TREEVIEW *)lParam)->itemNew.hItem)) == TRUE) {
					SetTimer(hWnd, TIMER_DRAG, 1, NULL);
				}
			}
			break;

		case NM_CUSTOMDRAW:
			// ѓJѓXѓ^ѓЂѓhѓЌЃ[
			switch (((LPNMTVCUSTOMDRAW)lParam)->nmcd.dwDrawStage) {
			case CDDS_PREPAINT:
				return CDRF_NOTIFYITEMDRAW;

			case CDDS_ITEMPREPAINT:
				{
					DATA_INFO *di;

					di = (DATA_INFO *)((LPNMTVCUSTOMDRAW)lParam)->nmcd.lItemlParam;
					if (di == NULL || di == &clip_di || di->type != TYPE_ITEM || di->title != NULL) {
						return CDRF_DODEFAULT;
					}
					di = format_get_priority_highest(di);
					if (di->menu_title == NULL && di->format_name != NULL) {
						if (((LPNMTVCUSTOMDRAW)lParam)->nmcd.uItemState == (CDIS_FOCUS | CDIS_SELECTED)) {
							((LPNMTVCUSTOMDRAW)lParam)->clrText = GetSysColor(COLOR_HIGHLIGHTTEXT);
						} else {
							((LPNMTVCUSTOMDRAW)lParam)->clrText = GetSysColor(COLOR_HIGHLIGHT);
						}
					}
				}
				return CDRF_DODEFAULT;
			}
			break;

		case NM_SETFOCUS:
			SetTimer(hWnd, TIMER_REFLECT_DATA, 1, NULL);
			set_enable_window_menu(hWnd);
			break;

		case NM_RCLICK:
			viewer_show_menu(hWnd);
			break;

		case NM_DBLCLK:
			if (TreeView_GetChild(GetDlgItem(hWnd, ID_TREE), TreeView_GetSelection(GetDlgItem(hWnd, ID_TREE))) == NULL &&
				treeview_get_hitest(GetDlgItem(hWnd, ID_TREE)) == TreeView_GetSelection(GetDlgItem(hWnd, ID_TREE))) {
				SetTimer(hWnd, TIMER_TV_ACTION, 1, NULL);
			}
			break;
		}
		break;

	case WM_LV_EVENT:
		// ѓЉѓXѓgѓrѓ…Ѓ[ѓCѓxѓ“ѓg
		switch (wParam) {
		case LVN_ITEMCHANGED:
			SetTimer(hWnd, TIMER_SET_MENU, 100, NULL);
			break;

		case LVN_GETDISPINFO:
			listview_get_disp_item(GetDlgItem(hWnd, ID_TREE), &(((LV_DISPINFO *)lParam)->item));
			return TRUE;

		case LVN_BEGINLABELEDIT:
			if (treeview_get_rootitem(GetDlgItem(hWnd, ID_TREE),
				TreeView_GetSelection(GetDlgItem(hWnd, ID_TREE))) == regist_treeitem) {
				SendMessage(hWnd, WM_ENABLE_ACCELERATOR, FALSE, 0);
				return FALSE;
			}
			return TRUE;

		case LVN_ENDLABELEDIT:
			SendMessage(hWnd, WM_ENABLE_ACCELERATOR, TRUE, 0);
			return viewer_rename(hWnd, NULL, ((LV_DISPINFO *)lParam)->item.pszText);

		case LVN_ITEMACTIVATE:
			viewer_item_activate(hWnd);
			break;

		case LVN_KEYDOWN:
			switch (((LV_KEYDOWN *)lParam)->wVKey) {
			case 'A':
				if (GetKeyState(VK_CONTROL) < 0) {
					// ‚·‚Ч‚Д‘I‘р
					SendMessage(hWnd, WM_COMMAND, ID_MENUITEM_SELECT_ALL, 0);
					return TRUE;
				}
				break;

			case 'C':
				if (GetKeyState(VK_CONTROL) < 0) {
					// ѓRѓsЃ[
					SendMessage(hWnd, WM_COMMAND, ID_MENUITEM_CLIPBOARD, 0);
					return TRUE;
				}
				break;

			case 'V':
				if (GetKeyState(VK_CONTROL) < 0) {
					// “\‚и•t‚Ї
					SendMessage(hWnd, WM_COMMAND, ID_MENUITEM_PASTE, 0);
					return TRUE;
				}
				break;

			case VK_TAB:
				SetFocus(GetDlgItem(hWnd, ID_TREE));
				return TRUE;

			case VK_DELETE:
				SendMessage(hWnd, WM_COMMAND, ID_MENUITEM_DELETE, 0);
				return TRUE;

			case VK_APPS:
				viewer_show_menu(hWnd);
				return TRUE;

			case VK_F2:
				SendMessage(hWnd, WM_COMMAND, ID_MENUITEM_RENAME, 0);
				return TRUE;

			case VK_BACK:
				if (TreeView_GetParent(GetDlgItem(hWnd, ID_TREE),
					TreeView_GetSelection(GetDlgItem(hWnd, ID_TREE))) != NULL) {
					TreeView_SelectItem(GetDlgItem(hWnd, ID_TREE),
						TreeView_GetParent(GetDlgItem(hWnd, ID_TREE),
						TreeView_GetSelection(GetDlgItem(hWnd, ID_TREE))));
					SetFocus(GetDlgItem(hWnd, ID_CONTAINER));
				}
				return TRUE;
			}
			break;

		case LVN_BEGINDRAG:
		case LVN_BEGINRDRAG:
			SetFocus(GetDlgItem(GetDlgItem(hWnd, ID_CONTAINER), ID_LIST));
			if (viewer_ole_start_drag(hWnd, NULL) == FALSE) {
				if ((DnD_mode = dragdrop_start_drag(hWnd, NULL)) == TRUE) {
					SetTimer(hWnd, TIMER_DRAG, 1, NULL);
				}
			}
			break;

		case NM_CUSTOMDRAW:
			// ѓJѓXѓ^ѓЂѓhѓЌЃ[
			switch (((LPNMLVCUSTOMDRAW)lParam)->nmcd.dwDrawStage) {
			case CDDS_PREPAINT:
				return CDRF_NOTIFYITEMDRAW;

			case CDDS_ITEMPREPAINT:
				{
					HTREEITEM hItem;
					DATA_INFO *di;

					hItem = (HTREEITEM)((LPNMLVCUSTOMDRAW)lParam)->nmcd.lItemlParam;
					if (hItem == NULL) {
						return CDRF_DODEFAULT;
					}
					di = (DATA_INFO *)treeview_get_lparam(GetDlgItem(hWnd, ID_TREE), hItem);
					if (di == NULL || di == &clip_di || di->type != TYPE_ITEM || di->title != NULL) {
						return CDRF_DODEFAULT;
					}
					di = format_get_priority_highest(di);
					if (di->menu_title == NULL && di->format_name != NULL) {
						if (((LPNMLVCUSTOMDRAW)lParam)->nmcd.uItemState == (CDIS_FOCUS | CDIS_SELECTED)) {
							((LPNMLVCUSTOMDRAW)lParam)->clrText = GetSysColor(COLOR_HIGHLIGHTTEXT);
						} else {
							((LPNMLVCUSTOMDRAW)lParam)->clrText = GetSysColor(COLOR_HIGHLIGHT);
						}
					}
				}
				return CDRF_DODEFAULT;
			}
			break;

		case NM_SETFOCUS:
			set_enable_window_menu(hWnd);
			break;

		case NM_RCLICK:
			viewer_show_menu(hWnd);
			break;
		}
		break;

	case WM_VIEWER_CHANGE_CLIPBOARD:
		// ѓNѓЉѓbѓvѓ{Ѓ[ѓh‚М“а—e•П‰»
		if (clip_treeitem != NULL) {
			BOOL clip_flag = FALSE;

			set_cursor(TRUE);
			SendMessage(GetDlgItem(hWnd, ID_TREE), WM_SETREDRAW, (WPARAM)FALSE, 0);

			// ѓAѓCѓeѓЂ‚МЌнЏњ
			if (treeview_get_rootitem(GetDlgItem(hWnd, ID_TREE),
				TreeView_GetSelection(GetDlgItem(hWnd, ID_TREE))) == clip_treeitem) {
				if (GetForegroundWindow() == hWnd) {
					SetFocus(GetDlgItem(hWnd, ID_TREE));
				}
				TreeView_SelectItem(GetDlgItem(hWnd, ID_TREE), NULL);
				clip_flag = TRUE;
			}
			treeview_delete_child(GetDlgItem(hWnd, ID_TREE), clip_treeitem);

			// ѓNѓЉѓbѓvѓ{Ѓ[ѓh‚МЊ`Ћ®Ћж“ѕ
			data_free(clip_di.child);
			clip_di.child = clipboard_to_datainfo(hWnd);

			// ѓNѓЉѓbѓvѓ{Ѓ[ѓh‚МЊ`Ћ®‚р•\Ћ¦
			treeview_datainfo_to_treeitem(GetDlgItem(hWnd, ID_TREE), clip_treeitem, clip_di.child);
			if (clip_flag == TRUE) {
				// •\Ћ¦ѓfЃ[ѓ^ЌXђV
				TreeView_SelectItem(GetDlgItem(hWnd, ID_TREE), clip_treeitem);
			}
			SendMessage(GetDlgItem(hWnd, ID_TREE), WM_SETREDRAW, (WPARAM)TRUE, 0);
			UpdateWindow(GetDlgItem(hWnd, ID_TREE));
			set_cursor(FALSE);
		}
		break;

	case WM_VIEWER_CHANGE_WATCH:
		// ѓNѓЉѓbѓvѓ{Ѓ[ѓhЉДЋ‹ђШ‚и‘Ц‚¦
		CheckMenuItem(GetSubMenu(GetMenu(hWnd), WINDOW_MENU_FILE), ID_MENUITEM_WATCH,
			((option.main_clipboard_watch == 0) ? MF_UNCHECKED : MF_CHECKED));
		break;

	case WM_VIEWER_REFRESH_STATUS:
		// ѓXѓeЃ[ѓ^ѓXѓoЃ[‚МЌXђV
		statusbar_set_text(hWnd, GetDlgItem(hWnd, ID_STATUSBAR));
		break;

	case WM_DRAGDROP:
		// ѓhѓ‰ѓbѓOЃ•ѓhѓЌѓbѓv
		pdtn = (LPIDROPTARGET_NOTIFY)lParam;
		switch (wParam) {
		case IDROPTARGET_NOTIFY_DRAGENTER:
			DnD_keystate = pdtn->grfKeyState;
		case IDROPTARGET_NOTIFY_DRAGOVER:
			viewer_ole_get_drag_effect(hWnd, pdtn);
			break;

		case IDROPTARGET_NOTIFY_DRAGLEAVE:
			TreeView_Select(GetDlgItem(hWnd, ID_TREE), NULL, TVGN_DROPHILITE);
			ListView_SetItemState(GetDlgItem(GetDlgItem(hWnd, ID_CONTAINER), ID_LIST), -1, 0, LVIS_DROPHILITED);
			break;

		case IDROPTARGET_NOTIFY_DROP:
			TreeView_Select(GetDlgItem(hWnd, ID_TREE), NULL, TVGN_DROPHILITE);
			ListView_SetItemState(GetDlgItem(GetDlgItem(hWnd, ID_CONTAINER), ID_LIST), -1, 0, LVIS_DROPHILITED);

			if (viewer_ole_create_drop_item(hWnd, pdtn, DnD_keystate) == FALSE) {
				pdtn->dwEffect = DROPEFFECT_NONE;
			}
			break;
		}
		break;

	case WM_GETDATA:
		// ѓhѓ‰ѓbѓOЃ•ѓhѓЌѓbѓv‚µ‚Д‚ў‚йѓfЃ[ѓ^‚рЋж“ѕ
		*((HGLOBAL *)lParam) = viewer_ole_get_drag_data(hWnd, wParam);
		break;

	case WM_GET_VERSION:
	case WM_GET_WORKPATH:
	case WM_GET_CLIPBOARD_WATCH:
	case WM_SET_CLIPBOARD_WATCH:
	case WM_GET_FORMAT_ICON:
	case WM_ENABLE_ACCELERATOR:
	case WM_REGIST_HOTKEY:
	case WM_UNREGIST_HOTKEY:
	case WM_OPTION_SHOW:
	case WM_OPTION_GET:
	case WM_OPTION_LOAD:
	case WM_OPTION_SAVE:
	case WM_HISTORY_GET_ROOT:
	case WM_HISTORY_LOAD:
	case WM_HISTORY_SAVE:
	case WM_REGIST_GET_ROOT:
	case WM_REGIST_LOAD:
	case WM_REGIST_SAVE:
	case WM_ITEM_TO_CLIPBOARD:
	case WM_ITEM_CREATE:
	case WM_ITEM_COPY:
	case WM_ITEM_FREE:
	case WM_ITEM_FREE_DATA:
	case WM_ITEM_CHECK:
	case WM_ITEM_TO_BYTES:
	case WM_ITEM_FROM_BYTES:
	case WM_ITEM_TO_FILE:
	case WM_ITEM_FROM_FILE:
	case WM_ITEM_GET_PARENT:
	case WM_ITEM_GET_FORMAT_TO_ITEM:
	case WM_ITEM_GET_PRIORITY_HIGHEST:
	case WM_ITEM_GET_TITLE:
	case WM_ITEM_GET_OPEN_INFO:
	case WM_ITEM_GET_SAVE_INFO:
	case WM_VIEWER_SHOW:
		return SendMessage(main_wnd, msg, wParam, lParam);

	case WM_HISTORY_CHANGED:
		// —љ—р‚М“а—e•П‰»
		data_adjust(&history_data.child);

		if (history_treeitem == NULL) {
			break;
		}
		set_cursor(TRUE);
		SendMessage(GetDlgItem(hWnd, ID_TREE), WM_SETREDRAW, (WPARAM)FALSE, 0);

		// —љ—р‚М“ЇЉъ
		treeview_sync_datainfo(GetDlgItem(hWnd, ID_TREE), history_treeitem, history_data.child);

		SendMessage(GetDlgItem(hWnd, ID_TREE), WM_SETREDRAW, (WPARAM)TRUE, 0);
		UpdateWindow(GetDlgItem(hWnd, ID_TREE));

		if (treeview_get_rootitem(GetDlgItem(hWnd, ID_TREE),
			TreeView_GetSelection(GetDlgItem(hWnd, ID_TREE))) == history_treeitem) {
			// ѓЉѓXѓgѓrѓ…Ѓ[ЌXђV
			treeview_to_listview(GetDlgItem(hWnd, ID_TREE),
				TreeView_GetSelection(GetDlgItem(hWnd, ID_TREE)),
				GetDlgItem(GetDlgItem(hWnd, ID_CONTAINER), ID_LIST));
		}
		SendMessage(hWnd, WM_VIEWER_REFRESH_STATUS, 0, 0);
		set_cursor(FALSE);
		break;

	case WM_REGIST_CHANGED:
		// “o^ѓAѓCѓeѓЂ‚М“а—e•П‰»
		data_adjust(&regist_data.child);

		if (regist_treeitem == NULL) {
			break;
		}
		set_cursor(TRUE);
		SendMessage(GetDlgItem(hWnd, ID_TREE), WM_SETREDRAW, (WPARAM)FALSE, 0);

		// “o^ѓAѓCѓeѓЂ‚М“ЇЉъ
		treeview_sync_datainfo(GetDlgItem(hWnd, ID_TREE), regist_treeitem, regist_data.child);

		SendMessage(GetDlgItem(hWnd, ID_TREE), WM_SETREDRAW, (WPARAM)TRUE, 0);
		UpdateWindow(GetDlgItem(hWnd, ID_TREE));

		if (treeview_get_rootitem(GetDlgItem(hWnd, ID_TREE),
			TreeView_GetSelection(GetDlgItem(hWnd, ID_TREE))) == regist_treeitem) {
			// ѓЉѓXѓgѓrѓ…Ѓ[ЌXђV
			treeview_to_listview(GetDlgItem(hWnd, ID_TREE),
				TreeView_GetSelection(GetDlgItem(hWnd, ID_TREE)),
				GetDlgItem(GetDlgItem(hWnd, ID_CONTAINER), ID_LIST));
		}
		SendMessage(hWnd, WM_VIEWER_REFRESH_STATUS, 0, 0);
		set_cursor(FALSE);
		break;

	case WM_VIEWER_GET_HWND:
		// ѓrѓ…Ѓ[ѓA‚МѓEѓBѓ“ѓhѓEѓnѓ“ѓhѓ‹‚рЋж“ѕ
		return (LRESULT)hWnd;

	case WM_VIEWER_GET_MAIN_HWND:
		// –{‘М‚МѓEѓBѓ“ѓhѓEѓnѓ“ѓhѓ‹‚рЋж“ѕ
		return (LRESULT)main_wnd;

	case WM_VIEWER_GET_SELECTION:
		// ‘I‘рѓAѓCѓeѓЂ‚рЋж“ѕ
		if (treeview_get_rootitem(GetDlgItem(hWnd, ID_TREE),
			TreeView_GetSelection(GetDlgItem(hWnd, ID_TREE))) == clip_treeitem) {
			return (LRESULT)NULL;
		}
		return (LRESULT)treeview_get_lparam(GetDlgItem(hWnd, ID_TREE),
			TreeView_GetSelection(GetDlgItem(hWnd, ID_TREE)));

	case WM_VIEWER_SELECT_ITEM:
		// ѓcѓЉЃ[ѓAѓCѓeѓЂ‚р‘I‘р
		{
			HTREEITEM hItem;

			if (history_treeitem != NULL &&
				(hItem = treeview_lparam_to_item(GetDlgItem(hWnd, ID_TREE), history_treeitem, lParam)) != NULL) {
				TreeView_SelectItem(GetDlgItem(hWnd, ID_TREE), hItem);
				return TRUE;
			}
			if (regist_treeitem != NULL &&
				(hItem = treeview_lparam_to_item(GetDlgItem(hWnd, ID_TREE), regist_treeitem, lParam)) != NULL) {
				TreeView_SelectItem(GetDlgItem(hWnd, ID_TREE), hItem);
				return TRUE;
			}
		}
		return FALSE;

	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

/*
 * viewer_regist - ѓEѓBѓ“ѓhѓEѓNѓ‰ѓX‚М“o^
 */
BOOL viewer_regist(const HINSTANCE hInstance)
{
	WNDCLASS wc;

	wc.style = 0;
	wc.lpfnWndProc = (WNDPROC)viewer_proc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_MAIN));
	wc.hCursor = LoadCursor(NULL, IDC_SIZEWE);
	wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU_VIEWER);
	wc.lpszClassName = WINDOW_CLASS;
	// ѓEѓBѓ“ѓhѓEѓNѓ‰ѓX‚М“o^
	return RegisterClass(&wc);
}

/*
 * viewer_create - ѓrѓ…Ѓ[ѓA‚МЌмђ¬
 */
HWND viewer_create(const HWND pWnd, const int CmdShow)
{
	HWND hWnd;
	main_wnd = pWnd;
	
	// ѓEѓBѓ“ѓhѓE‚МЌмђ¬
	hWnd = CreateWindow(WINDOW_CLASS,
		WINDOW_TITLE,
		WS_OVERLAPPEDWINDOW,
		option.viewer_rect.left,
		option.viewer_rect.top,
		option.viewer_rect.right,
		option.viewer_rect.bottom,
		NULL, NULL, hInst, NULL);

	if (hWnd == NULL) {
		MessageBox(NULL, message_get_res(IDS_ERROR_WINDOW_INIT), ERROR_TITLE, MB_ICONERROR);
		return NULL;
	}
	ShowWindow(hWnd, CmdShow);
	UpdateWindow(hWnd);
	return hWnd;
}
/* End of source */
