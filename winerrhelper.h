#ifndef _WINERR_HELP_H
#define _WINERR_HELP_H

/*       In loving memory of @pixo $oft       */
#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS
#define NOVIRTUALKEYCODES
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOKEYSTATES
#define NOSYSCOMMANDS
#define NORASTEROPS
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOKERNEL
#define NOMEMMGR
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWINOFFSETS
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NODRAWTEXT
#define NOWINSTYLES
#define NOSHOWWINDOW
#define NOWINMESSAGES
#define NOWH

#include <Windows.h>
#include "strhelper.h"

#pragma warning(disable : 4244)
#pragma warning(disable : 4996)

void errorMsgBox(const std::wstring errormsg);
void setLastError(const unsigned long code);
unsigned long getLastErrorCode();
std::wstring getErrorDetails(const unsigned long errcode,
	const std::wstring funcName = L"",
	const std::wstring additionalInfo = L"");

#endif