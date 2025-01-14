/*
 * CLCL
 *
 * fmt_metafile_view.h
 *
 * Copyright (C) 1996-2003 by Nakashima Tomoaki. All rights reserved.
 *		http://www.nakka.com/
 *		nakka@nakka.com
 */

#ifndef _INC_FMT_METAFILE_VIEW_H
#define _INC_FMT_METAFILE_VIEW_H

/* Include Files */

/* Define */
#define WM_SET_METADATA				(WM_APP + 1)

/* Struct */

/* Function Prototypes */
BOOL metaview_regist(const HINSTANCE hInstance);
HWND metaview_create(const HINSTANCE hInstance, const HWND pWnd, int id);

#endif
/* End of source */
