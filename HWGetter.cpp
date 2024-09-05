#include <Windows.h>
#include <SetupAPI.h>
#include <devguid.h>
#include <hidsdi.h>
#include "HWGetter.h"

std::vector<std::wstring> HW_GetHardDrives() {
	// GUID_DEVINTERFACE_DISK
	// DiskClassGuid
	std::vector<std::wstring> ret;
	HDEVINFO devHandle = ::SetupDiGetClassDevs(&GUID_DEVINTERFACE_DISK, 0, 0, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (INVALID_HANDLE_VALUE != devHandle) {
		SP_DEVICE_INTERFACE_DATA deviceInterfaceData = { 0 };
		PSP_DEVICE_INTERFACE_DETAIL_DATA deviceInterfaceDetailData = { 0 };
		memset(&deviceInterfaceData, 0, sizeof(SP_DEVICE_INTERFACE_DATA));
		deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
		unsigned long requiredSize = 0, deviceIndex = 0;
		while (::SetupDiEnumDeviceInterfaces(devHandle, 0, &GUID_DEVINTERFACE_DISK, deviceIndex,
			&deviceInterfaceData)) {
			++deviceIndex;
			if (!::SetupDiGetDeviceInterfaceDetail(devHandle, &deviceInterfaceData, 0, 0, &requiredSize, 0)) {
				if (ERROR_INSUFFICIENT_BUFFER == getLastErrorCode()) {
					deviceInterfaceDetailData = (SP_DEVICE_INTERFACE_DETAIL_DATA*)malloc(requiredSize);
					if (!deviceInterfaceDetailData) {
						return ret;
					}
					memset(deviceInterfaceDetailData, 0, requiredSize);
					deviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
					if (::SetupDiGetDeviceInterfaceDetail(devHandle, &deviceInterfaceData, deviceInterfaceDetailData,
						requiredSize, 0, 0)) {
						HANDLE hFile = CreateFile(deviceInterfaceDetailData->DevicePath, GENERIC_READ,
							FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
						if (INVALID_HANDLE_VALUE != hFile) {
							unsigned long bytesReturned = 0;
							STORAGE_DEVICE_NUMBER diskNumber = { 0 };
							if(::DeviceIoControl(hFile, IOCTL_STORAGE_GET_DEVICE_NUMBER, 0, 0, &diskNumber,
								sizeof(STORAGE_DEVICE_NUMBER), &bytesReturned, 0)) {
								if (INVALID_HANDLE_VALUE != hFile) {
									::CloseHandle(hFile);
								}
								wchar_t* physDiskBuf = (wchar_t*)malloc(FSH_PHYSDRIVEBUFLEN * sizeof(wchar_t));
								if (!physDiskBuf) {
									return ret;
								}
								wsprintf(physDiskBuf, L"%s", deviceInterfaceDetailData->DevicePath);
								ret.push_back(physDiskBuf);
#ifdef FSH_FULLPHYSDRIVESTRING
								ret.push_back(L"\\\\.\\PhysicalDrive" + std::to_wstring(diskNumber.DeviceNumber));
#else
								ret.push_back(L"PhysicalDrive" + std::to_wstring(diskNumber.DeviceNumber));
#endif
								SAFE_FREE(physDiskBuf);

							} else {
								if (INVALID_HANDLE_VALUE != devHandle) {
									::SetupDiDestroyDeviceInfoList(devHandle);
								}
								if (INVALID_HANDLE_VALUE != hFile) {
									::CloseHandle(hFile);
								}
								return ret;
							}
						} else {
							if (INVALID_HANDLE_VALUE != devHandle) {
								::SetupDiDestroyDeviceInfoList(devHandle);
							}
							return ret;
						}
					} else {
						if (INVALID_HANDLE_VALUE != devHandle) {
							::SetupDiDestroyDeviceInfoList(devHandle);
						}
						return ret;
					}
				} else {
					if (INVALID_HANDLE_VALUE != devHandle) {
						::SetupDiDestroyDeviceInfoList(devHandle);
					}
					return ret;
				}
			} else {
				if (INVALID_HANDLE_VALUE != devHandle) {
					::SetupDiDestroyDeviceInfoList(devHandle);
				}
				return ret;
			}
		}
	}
	if (INVALID_HANDLE_VALUE != devHandle) {
		::SetupDiDestroyDeviceInfoList(devHandle);
	}
	return ret;
}