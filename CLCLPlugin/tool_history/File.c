/*
 * CLCL
 *
 * file.c
 *
 * Copyright (C) 1996-2019 by Ohno Tomoaki. All rights reserved.
 *		https://www.nakka.com/
 *		nakka@nakka.com
 */

/* Include Files */

#define _INC_OLE
#include <windows.h>
#undef  _INC_OLE
#include <tchar.h>

//#include "General.h"
#include "Memory.h"
#include "String.h"
//#include "data.h"
#include "Message.h"
#include "File.h"
//#include "Format.h"
//#include "ClipBoard.h"

/* Define */
#define TYPE_DATA						0
#define TYPE_ITEM						1
#define TYPE_FOLDER						2
#define TYPE_ROOT						3

/* Global Variables */
extern HINSTANCE hInst;

/* ocal Function Prototypes */
/*
static void file_expand_option(DATA_INFO *di, char *option);
static BYTE *file_file_to_item(const BYTE *buf, BYTE *p, const DWORD size, DATA_INFO **root, const int level, TCHAR *err_str);
static BOOL file_item_to_file(const HANDLE hFile, DATA_INFO *di, TCHAR *err_str);
*/

/*
 * file_name_check - ファイル名にできない文字列が含まれていないかチェックを行う
 */
BOOL file_name_check(TCHAR *file_name)
{
	TCHAR *p;

	for (p = file_name; *p != TEXT('\0'); p++) {
#ifndef UNICODE
		if (IsDBCSLeadByte((BYTE)*p) == TRUE) {
			// ２バイトコードの場合
			p++;
			continue;

		}
#endif
		// ファイル名にできない文字のチェック
		if (*p == TEXT('\\') ||
			*p == TEXT('/') ||
			*p == TEXT(':') ||
			*p == TEXT(',') ||
			*p == TEXT(';') ||
			*p == TEXT('*') ||
			*p == TEXT('?') ||
			*p == TEXT('\"') ||
			*p == TEXT('<') ||
			*p == TEXT('>') ||
			*p == TEXT('|')) {
			return FALSE;
		}
	}
	return TRUE;
}

/*
 * file_name_conv - ファイル名に使えない文字を変換する
 */
void file_name_conv(TCHAR *file_name, TCHAR conv_char)
{
	TCHAR *p;

	for (p = file_name; *p != TEXT('\0'); p++) {
#ifndef UNICODE
		if (IsDBCSLeadByte((BYTE)*p) == TRUE) {
			// ２バイトコードの場合
			p++;
			continue;
		}
#endif
		// ファイル名にできない文字は指定の文字に変換
		if (*p == TEXT('\\') ||
			*p == TEXT('/') ||
			*p == TEXT(':') ||
			*p == TEXT(',') ||
			*p == TEXT(';') ||
			*p == TEXT('*') ||
			*p == TEXT('?') ||
			*p == TEXT('\"') ||
			*p == TEXT('<') ||
			*p == TEXT('>') ||
			*p == TEXT('|')) {
			*p = conv_char;
		}
	}
}

/*
 * file_check_directory - ディレクトリが存在するかチェックする
 */
BOOL file_check_directory(const TCHAR *path)
{
	WIN32_FIND_DATA FindData;
	HANDLE hFindFile;

	if ((hFindFile = FindFirstFile(path, &FindData)) == INVALID_HANDLE_VALUE) {
		return FALSE;
	}
	FindClose(hFindFile);

	if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
		// ディレクトリが存在した場合
		return TRUE;
	}
	return FALSE;
}

/*
 * file_check_file - ファイルが存在するかチェックする
 */
BOOL file_check_file(const TCHAR *path)
{
	WIN32_FIND_DATA FindData;
	HANDLE hFindFile;

	if ((hFindFile = FindFirstFile(path, &FindData)) == INVALID_HANDLE_VALUE) {
		return FALSE;
	}
	FindClose(hFindFile);

	if ((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
		// ファイルが存在した場合
		return TRUE;
	}
	return FALSE;
}

/*
 * file_read_buf - ファイルを読み込む
 */
BYTE *file_read_buf(const TCHAR *path, DWORD *ret_size, TCHAR *err_str)
{
	HANDLE hFile;
	DWORD size;
	DWORD ret;
	BYTE *buf;

	// ファイルを開く
	hFile = CreateFile(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == NULL || hFile == (HANDLE)-1) {
		message_get_error(GetLastError(), err_str);
		return NULL;
	}
	if ((size = GetFileSize(hFile, NULL)) == 0xFFFFFFFF) {
		message_get_error(GetLastError(), err_str);
		CloseHandle(hFile);
		return NULL;
	}

	if ((buf = (BYTE *)mem_alloc(size + 1)) == NULL) {
		message_get_error(GetLastError(), err_str);
		CloseHandle(hFile);
		return NULL;
	}
	// ファイルを読みこむ
	if (ReadFile(hFile, buf, size, &ret, NULL) == FALSE) {
		message_get_error(GetLastError(), err_str);
		mem_free(&buf);
		CloseHandle(hFile);
		return NULL;
	}
	CloseHandle(hFile);

	if (ret_size != NULL) {
		*ret_size = size;
	}
	return buf;
}

/*
 * file_write_buf - ファイルに書き込む
 */
BOOL file_write_buf(const TCHAR *path, const BYTE *data, const DWORD size, TCHAR *err_str)
{
	HANDLE hFile;
	DWORD ret;

	// ファイルを開く
	hFile = CreateFile(path, GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == NULL || hFile == (HANDLE)-1) {
		message_get_error(GetLastError(), err_str);
		return FALSE;
	}
	// ファイルの書き込み
	if (WriteFile(hFile, data, size, &ret, NULL) == FALSE) {
		message_get_error(GetLastError(), err_str);
		CloseHandle(hFile);
		return FALSE;
	}
	FlushFileBuffers(hFile);
	CloseHandle(hFile);
	return TRUE;
}




/* End of source */
