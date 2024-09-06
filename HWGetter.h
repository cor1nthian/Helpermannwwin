#ifndef _HWGETTER_H
#define _HWGETTER_H

#include "config.h"
#include "strhelper.h"
#include "winerrhelper.h"

#pragma comment(lib, "setupapi.lib")

const std::wstring const gc_hwEmptyval		= L"(null)";
const std::wstring const gc_hwFriendlyName	= L"Friendly name:";
const std::wstring const gc_hwDevInstanceID	= L"DeviceInstanceId:";
const std::wstring const gc_hwClass			= L"Class:";
const std::wstring const gc_hwClassGUID		= L"Class GUID:";

std::vector<std::wstring> HW_GetHardDrives();
std::vector<std::wstring> HW_EnumAllDevices();

#endif