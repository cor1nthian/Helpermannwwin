#ifndef _HWGETTER_H
#define _HWGETTER_H

#include "strhelper.h"

#pragma comment(lib, "setupapi.lib")

std::vector<std::wstring> HW_GetHardDrives();

#endif
