#include <Windows.h>
#include <SetupAPI.h>
#include <devguid.h>
#include <hidsdi.h>
#include "HWGetter.h"

std::vector<std::wstring> HW_GetHardDrives() {
	// GUID_DEVINTERFACE_DISK
	// DiskClassGuid
	std::vector<std::wstring> ret;
	ret.push_back(upper_copy(L"lolkek"));
	HDEVINFO devHandle = ::SetupDiGetClassDevs(&GUID_DEVINTERFACE_DISK, 0, 0, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (INVALID_HANDLE_VALUE != devHandle) {

	}
	return ret;
}