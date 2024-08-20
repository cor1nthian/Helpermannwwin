#include <stdio.h>
#include "fshelper.h"

NTSTATUS(WINAPI* pRtlInitUnicodeString)(UnicodeString*, wchar_t*);
NTSTATUS(WINAPI* pNtCreateFile)(::HANDLE*, ACCESS_MASK, ObjectAttributes*, IOStatusBlock*, LARGE_INTEGER*, unsigned long,
	unsigned long, unsigned long, unsigned long, void*, unsigned long);
NTSTATUS(WINAPI* pNtCreateEvent)(::HANDLE*, ACCESS_MASK, ObjectAttributes*, EvtType, unsigned char);
NTSTATUS(WINAPI* pNtQueryDirectoryFile)(::HANDLE, ::HANDLE, PIO_APC_ROUTINE, void*, IOStatusBlock*, void*,
	unsigned long, FolderInfo, unsigned char, UnicodeString*, unsigned char);
NTSTATUS(WINAPI* pNtWaitForSingleobject)(::HANDLE, unsigned char, LARGE_INTEGER*);
NTSTATUS(WINAPI* pRtlUnicodeStringToAnsiString)(ANSIString*, UnicodeString*, unsigned char);
NTSTATUS(WINAPI* pNtClearEvent)(::HANDLE);
NTSTATUS(WINAPI* pNtClose)(::HANDLE);

UnicodeString::UnicodeString() {
	Length = 0;
	MaximumLength = 0;
	Buffer = 0;
}

UnicodeString::UnicodeString(const unsigned short length, const unsigned short maxlength) {
	Length = length;
	MaximumLength = maxlength;
	Buffer = (wchar_t*)malloc(Length * sizeof(wchar_t));
	// Buffer = (wchar_t*)::LocalAlloc(LPTR, Length);
	// NEW_ARR_NULLIFY_NO_REDEFINE(Buffer, wchar_t, Length);
}

UnicodeString::UnicodeString(const unsigned short length, const unsigned short maxlength, const wchar_t* buffer) {
	Length = length;
	MaximumLength = maxlength;
	Buffer = (wchar_t*)malloc(Length * sizeof(wchar_t));
	// Buffer = (wchar_t*)::LocalAlloc(LPTR, Length);
	// NEW_ARR_NULLIFY_NO_REDEFINE(Buffer, wchar_t, Length);
	if(Buffer) {
		memset(Buffer, 0, Length * sizeof(wchar_t));
		wsprintf(Buffer, L"%s", buffer);
	}
}

UnicodeString::UnicodeString(const UnicodeString &other) {
	if (this != &other) {
		Length = other.Length;
		MaximumLength = other.MaximumLength;
		Buffer = (wchar_t*)malloc(Length * sizeof(wchar_t));
		// Buffer = (wchar_t*)::LocalAlloc(LPTR, Length);
		// NEW_ARR_NULLIFY_NO_REDEFINE(Buffer, wchar_t, Length);
		if (Buffer && other.Buffer) {
			memset(Buffer, 0, Length * sizeof(wchar_t));
			wsprintf(Buffer, L"%s", other.Buffer);
		}
	}
}

UnicodeString::UnicodeString(UnicodeString &&other) noexcept {
	if (this != &other) {
		Length = other.Length;
		other.Length = 0;
		MaximumLength = other.MaximumLength;
		other.MaximumLength = 0;
		Buffer = other.Buffer;
		other.Buffer = 0;
	}
}

UnicodeString::~UnicodeString() {
	/*if (Buffer) {
		SAFE_FREE(Buffer);
	}*/
}

UnicodeString& UnicodeString::operator=(const UnicodeString &other) {
	if (this != &other) {
		Length = other.Length;
		MaximumLength = other.MaximumLength;
		Buffer = (wchar_t*)malloc(Length * sizeof(wchar_t));
		if (Buffer) {
			memset(Buffer, 0, Length * sizeof(wchar_t));
		}
		Buffer = other.Buffer;
	}
	return *this;
}

UnicodeString& UnicodeString::operator=(UnicodeString &&other) noexcept {
	if (this != &other) {
		Length = other.Length;
		other.Length = 0;
		MaximumLength = other.MaximumLength;
		other.MaximumLength = 0;
		Buffer = other.Buffer;
		other.Buffer = 0;
	}
	return *this;
}

bool UnicodeString::operator==(const UnicodeString& other) const {
	if (this != &other) {
		return (Length == other.Length &&
				MaximumLength == other.MaximumLength &&
				!memcmp(Buffer, other.Buffer, Length * sizeof(wchar_t)));
	} else {
		return true;
	}
}

bool UnicodeString::operator!=(const UnicodeString& other) const {
	if (this != &other) {
		return (Length != other.Length ||
				MaximumLength != other.MaximumLength ||
				memcmp(Buffer, other.Buffer, Length * sizeof(wchar_t)));
	} else {
		return false;
	}
}

ANSIString::ANSIString() {
	Length = 0;
	MaximumLength = 0;
	Buffer = 0;
}

ANSIString::ANSIString(const unsigned short length, const unsigned short maxlength) {
	Length = length;
	MaximumLength = maxlength;
	Buffer = (char*)malloc(Length * sizeof(char));
	if (Buffer) {
		memset(Buffer, 0, Length * sizeof(wchar_t));
	}
	// NEW_ARR_NULLIFY_NO_REDEFINE(Buffer, char, Length);
}

ANSIString::ANSIString(const unsigned short length, const unsigned short maxlength, const char* buffer) {
	Length = length;
	MaximumLength = maxlength;
	Buffer = (char*)malloc(Length * sizeof(char));
	// NEW_ARR_NULLIFY_NO_REDEFINE(Buffer, char, Length);
	if (Buffer) {
		memset(Buffer, 0, Length * sizeof(wchar_t));
		sprintf(Buffer, "%s", buffer);
	}
}

ANSIString::ANSIString(const ANSIString &other) {
	if (this != &other) {
		Length = other.Length;
		MaximumLength = other.MaximumLength;
		Buffer = (char*)malloc(Length * sizeof(char));
		if (Buffer) {
			memset(Buffer, 0, Length * sizeof(wchar_t));
		}
		if (Buffer && other.Buffer) {
			sprintf(Buffer, "%s", other.Buffer);
		}
	}
}

ANSIString::ANSIString(ANSIString &&other) noexcept {
	if (this != &other) {
		Length = other.Length;
		other.Length = 0;
		MaximumLength = other.MaximumLength;
		other.MaximumLength = 0;
		Buffer = other.Buffer;
		other.Buffer = 0;
	}
}

ANSIString::~ANSIString() {
	/*if (Buffer) {
		SAFE_FREE(Buffer);
	}*/
}

ANSIString& ANSIString::operator=(const ANSIString& other) {
	if (this != &other) {
		Length = other.Length;
		MaximumLength = other.MaximumLength;
		Buffer = (char*)malloc(Length * sizeof(char));
		if (Buffer) {
			memset(Buffer, 0, Length * sizeof(wchar_t));
		}
		if (Buffer && other.Buffer) {
			sprintf(Buffer, "%s", other.Buffer);
		}
	}
	return *this;
}

ANSIString& ANSIString::operator=(ANSIString&& other) noexcept {
	if (this != &other) {
		Length = other.Length;
		other.Length = 0;
		MaximumLength = other.MaximumLength;
		other.MaximumLength = 0;
		Buffer = other.Buffer;
		other.Buffer = 0;
	}
	return *this;
}

bool ANSIString::operator==(const ANSIString& other) const {
	if (this != &other) {
		return (Length == other.Length &&
				MaximumLength == other.MaximumLength &&
				!memcmp(Buffer, other.Buffer, Length * sizeof(char)));
	} else {
		return true;
	}
}

bool ANSIString::operator!=(const ANSIString& other) const {
	if (this != &other) {
		return (Length != other.Length ||
				MaximumLength != other.MaximumLength ||
				memcmp(Buffer, other.Buffer, Length * sizeof(char)));
	} else {
		return false;
	}
}

ObjectAttributes::ObjectAttributes() {
	uLength = 0;
	uAttributes = 0;
	pSecurityDescriptor = 0;
	pSecurityQualityOfService = 0;
	hRootDirectory = 0;
	pObjectName = 0;
}

ObjectAttributes::ObjectAttributes(const ObjectAttributes &other) {
	if (this != &other) {
		uLength = other.uLength;
		uAttributes = other.uAttributes;
		if (other.pSecurityDescriptor) {
			if (pSecurityDescriptor) {
				SAFE_FREE(pSecurityDescriptor);
			}
			pSecurityDescriptor = malloc(SECURITY_DESCRIPTOR_MIN_LENGTH);
			if (pSecurityDescriptor) {
				memcpy(pSecurityDescriptor, other.pSecurityDescriptor, sizeof(SECURITY_DESCRIPTOR));
			}
		} else {
			if (pSecurityDescriptor) {
				SAFE_FREE(pSecurityDescriptor);
			}
		}
		pSecurityQualityOfService = other.pSecurityQualityOfService;
		hRootDirectory = other.hRootDirectory;
		if (other.pObjectName) {
			if (pObjectName) {
				SAFE_DELETE(pObjectName);
			}
			pObjectName = new UnicodeString;
			if (pObjectName) {
				memcpy(pObjectName, other.pObjectName, sizeof(UnicodeString));
			}
		} else {
			if (pObjectName) {
				SAFE_DELETE(pObjectName);
			}
		}
	}
}

ObjectAttributes::ObjectAttributes(ObjectAttributes &&other) noexcept {
	if (this != &other) {
		uLength = other.uLength;
		other.uLength = 0;
		uAttributes = other.uAttributes;
		other.uLength = 0;
		pSecurityDescriptor = other.pSecurityDescriptor;
		other.pSecurityDescriptor = 0;
		pSecurityQualityOfService = other.pSecurityQualityOfService;
		other.pSecurityQualityOfService = 0;
		hRootDirectory = other.hRootDirectory;
		other.hRootDirectory = 0;
		pObjectName = other.pObjectName;
		other.pObjectName = 0;
	}
}

ObjectAttributes::~ObjectAttributes() {
	/*if (pObjectName) {
		SAFE_DELETE(pObjectName);
	}*/
}

ObjectAttributes& ObjectAttributes::operator=(const ObjectAttributes &other) {
	if (this != &other) {
		uLength = other.uLength;
		uAttributes = other.uAttributes;
		if (other.pSecurityDescriptor) {
			if (pSecurityDescriptor) {
				SAFE_FREE(pSecurityDescriptor);
			}
			pSecurityDescriptor = malloc(SECURITY_DESCRIPTOR_MIN_LENGTH);
			if (pSecurityDescriptor) {
				memcpy(pSecurityDescriptor, other.pSecurityDescriptor, sizeof(SECURITY_DESCRIPTOR));
			}
		} else {
			if (pSecurityDescriptor) {
				SAFE_FREE(pSecurityDescriptor);
			}
		}
		pSecurityQualityOfService = other.pSecurityQualityOfService;
		hRootDirectory = other.hRootDirectory;
		if (other.pObjectName) {
			if (pObjectName) {
				SAFE_DELETE(pObjectName);
			}
			pObjectName = new UnicodeString;
			if (pObjectName) {
				memcpy(pObjectName, other.pObjectName, sizeof(UnicodeString));
			}
		} else {
			if (pObjectName) {
				SAFE_DELETE(pObjectName);
			}
		}
	}
	return *this;
}

ObjectAttributes& ObjectAttributes::operator=(ObjectAttributes &&other) noexcept {
	if (this != &other) {
		uLength = other.uLength;
		other.uLength = 0;
		uAttributes = other.uAttributes;
		other.uLength = 0;
		pSecurityDescriptor = other.pSecurityDescriptor;
		other.pSecurityDescriptor = 0;
		pSecurityQualityOfService = other.pSecurityQualityOfService;
		other.pSecurityQualityOfService = 0;
		hRootDirectory = other.hRootDirectory;
		other.hRootDirectory = 0;
		pObjectName = other.pObjectName;
		other.pObjectName = 0;
	}
	return *this;
}

bool ObjectAttributes::operator==(const ObjectAttributes& other) const {
	if (this != &other) {
		return (uLength == other.uLength &&
				uAttributes == other.uAttributes &&
				hRootDirectory == other.hRootDirectory &&
				// pSecurityQualityOfService == other.pSecurityQualityOfService &&
				!memcmp(pSecurityDescriptor, other.pSecurityDescriptor, sizeof(SECURITY_DESCRIPTOR)) &&
				!memcmp(pObjectName, other.pObjectName, sizeof(UnicodeString)));
	} else {
		return true;
	}
}

bool ObjectAttributes::operator!=(const ObjectAttributes& other) const {
	if (this != &other) {
		return (uLength != other.uLength ||
				uAttributes != other.uAttributes ||
				hRootDirectory != other.hRootDirectory ||
				// pSecurityQualityOfService != other.pSecurityQualityOfService ||
				memcmp(pSecurityDescriptor, other.pSecurityDescriptor, sizeof(SECURITY_DESCRIPTOR)) ||
				memcmp(pObjectName, other.pObjectName, sizeof(UnicodeString)));
	} else {
		return false;
	}
}

IOStatusBlock::IOStatusBlock() {
	Status = 0;
	Pointer = 0;
	Information = 0;
}

IOStatusBlock::IOStatusBlock(const NTSTATUS statuus, const unsigned long long info) {
	Status = statuus;
	Information = info;
	Pointer = 0;
}

IOStatusBlock::IOStatusBlock(const IOStatusBlock &other) {
	if (this != &other) {
		Status = other.Status;
		Pointer = other.Pointer;
		Information = other.Information;
	}
}

IOStatusBlock::IOStatusBlock(IOStatusBlock &&other) noexcept {
	if (this != &other) {
		Status = other.Status;
		other.Status = 0;
		Pointer = other.Pointer;
		other.Pointer = 0;
		Information = other.Information;
		other.Information = 0;
	}
}

IOStatusBlock::~IOStatusBlock() {}

IOStatusBlock& IOStatusBlock::operator=(const IOStatusBlock& other) {
	if (this != &other) {
		Status = other.Status;
		Pointer = other.Pointer;
		Information = other.Information;
	}
	return *this;
}

IOStatusBlock& IOStatusBlock::operator=(IOStatusBlock&& other) {
	if (this != &other) {
		Status = other.Status;
		other.Status = 0;
		Pointer = other.Pointer;
		other.Pointer = 0;
		Information = other.Information;
		other.Information = 0;
	}
	return *this;
}

bool IOStatusBlock::operator==(const IOStatusBlock& other) const {
	if (this != &other) {
		return (Status == other.Status &&
				Information == other.Information);
	} else {
		return true;
	}
}

bool IOStatusBlock::operator!=(const IOStatusBlock& other) const {
	if (this != &other) {
		return (Status != other.Status ||
				Information != other.Information);
	} else {
		return false;
	}
}

FileDirBothInformation::FileDirBothInformation() {
	NextEntryOffset = 0;
	FileIndex = 0;
	FileAttributes = 0;
	FileNameLength = 0;
	EaSize = 0;
	ShortNameLength = 0;
	CreationTime = { 0 };
	LastAccessTime = { 0 };
	LastWriteTime = { 0 };
	ChangeTime = { 0 };
	EndOfFile = { 0 };
	AllocationSize = { 0 };
	memset(ShortName, 0, FSH_SHORTNAMELENGTH * sizeof(wchar_t));
	memset(FileName, 0, sizeof(wchar_t));
}

FileDirBothInformation::FileDirBothInformation(const FileDirBothInformation &other) {
	if (this != &other) {
		NextEntryOffset = other.NextEntryOffset;
		FileIndex = other.FileIndex;
		FileAttributes = other.FileAttributes;
		FileNameLength = other.FileNameLength;
		EaSize = other.EaSize;
		ShortNameLength = other.ShortNameLength;
		CreationTime.LowPart = other.CreationTime.LowPart;
		CreationTime.HighPart = other.CreationTime.HighPart;
		LastAccessTime.LowPart = other.LastAccessTime.LowPart;
		LastAccessTime.HighPart = other.LastAccessTime.HighPart;
		LastWriteTime.LowPart = other.LastWriteTime.LowPart;
		LastWriteTime.HighPart = other.LastWriteTime.HighPart;
		ChangeTime.LowPart = other.ChangeTime.LowPart;
		ChangeTime.HighPart = other.ChangeTime.HighPart;
		EndOfFile.LowPart = other.EndOfFile.LowPart;
		EndOfFile.HighPart = other.EndOfFile.HighPart;
		AllocationSize.LowPart = other.AllocationSize.LowPart;
		AllocationSize.HighPart = other.AllocationSize.HighPart;
		wsprintf(ShortName, L"%s", other.ShortName);
		wsprintf(FileName, L"%s", other.FileName);
	}
}

FileDirBothInformation::FileDirBothInformation(FileDirBothInformation &&other) noexcept {
	if (this != &other) {
		NextEntryOffset = other.NextEntryOffset;
		other.NextEntryOffset = 0;
		FileIndex = other.FileIndex;
		other.FileIndex = 0;
		FileAttributes = other.FileAttributes;
		other.FileAttributes = 0;
		FileNameLength = other.FileNameLength;
		other.FileNameLength = 0;
		EaSize = other.EaSize;
		other.EaSize = 0;
		ShortNameLength = other.ShortNameLength;
		other.ShortNameLength = 0;
		CreationTime.LowPart = other.CreationTime.LowPart;
		CreationTime.HighPart = other.CreationTime.HighPart;
		other.CreationTime.LowPart = 0;
		other.CreationTime.HighPart = 0;
		LastAccessTime.LowPart = other.LastAccessTime.LowPart;
		LastAccessTime.HighPart = other.LastAccessTime.HighPart;
		other.LastAccessTime.LowPart = 0;
		other.LastAccessTime.HighPart = 0;
		LastWriteTime.LowPart = other.LastWriteTime.LowPart;
		LastWriteTime.HighPart = other.LastWriteTime.HighPart;
		other.LastWriteTime.LowPart = 0;
		other.LastWriteTime.HighPart = 0;
		ChangeTime.LowPart = other.ChangeTime.LowPart;
		ChangeTime.HighPart = other.ChangeTime.HighPart;
		other.ChangeTime.LowPart = 0;
		other.ChangeTime.HighPart = 0;
		EndOfFile.LowPart = other.EndOfFile.LowPart;
		EndOfFile.HighPart = other.EndOfFile.HighPart;
		other.EndOfFile.LowPart = 0;
		other.EndOfFile.HighPart = 0;
		AllocationSize.LowPart = other.AllocationSize.LowPart;
		AllocationSize.HighPart = other.AllocationSize.HighPart;
		other.AllocationSize.LowPart = 0;
		other.AllocationSize.HighPart = 0;
		wsprintf(ShortName, L"%s", other.ShortName);
		memset(other.ShortName, 0, FSH_SHORTNAMELENGTH * sizeof(wchar_t));
		wsprintf(FileName, L"%s", other.FileName);
		memset(other.FileName, 0, sizeof(wchar_t));
	}
}

FileDirBothInformation::~FileDirBothInformation() {}

FileDirBothInformation& FileDirBothInformation::operator=(const FileDirBothInformation &other) {
	if (this != &other) {
		NextEntryOffset = other.NextEntryOffset;
		FileIndex = other.FileIndex;
		FileAttributes = other.FileAttributes;
		FileNameLength = other.FileNameLength;
		EaSize = other.EaSize;
		ShortNameLength = other.ShortNameLength;
		CreationTime.LowPart = other.CreationTime.LowPart;
		CreationTime.HighPart = other.CreationTime.HighPart;
		LastAccessTime.LowPart = other.LastAccessTime.LowPart;
		LastAccessTime.HighPart = other.LastAccessTime.HighPart;
		LastWriteTime.LowPart = other.LastWriteTime.LowPart;
		LastWriteTime.HighPart = other.LastWriteTime.HighPart;
		ChangeTime.LowPart = other.ChangeTime.LowPart;
		ChangeTime.HighPart = other.ChangeTime.HighPart;
		EndOfFile.LowPart = other.EndOfFile.LowPart;
		EndOfFile.HighPart = other.EndOfFile.HighPart;
		AllocationSize.LowPart = other.AllocationSize.LowPart;
		AllocationSize.HighPart = other.AllocationSize.HighPart;
		wsprintf(ShortName, L"%s", other.ShortName);
		wsprintf(FileName, L"%s", other.FileName);
	}
	return *this;
}

FileDirBothInformation& FileDirBothInformation::operator=(FileDirBothInformation &&other) noexcept {
	if (this != &other) {
		NextEntryOffset = other.NextEntryOffset;
		other.NextEntryOffset = 0;
		FileIndex = other.FileIndex;
		other.FileIndex = 0;
		FileAttributes = other.FileAttributes;
		other.FileAttributes = 0;
		FileNameLength = other.FileNameLength;
		other.FileNameLength = 0;
		EaSize = other.EaSize;
		other.EaSize = 0;
		ShortNameLength = other.ShortNameLength;
		other.ShortNameLength = 0;
		CreationTime.LowPart = other.CreationTime.LowPart;
		CreationTime.HighPart = other.CreationTime.HighPart;
		other.CreationTime.LowPart = 0;
		other.CreationTime.HighPart = 0;
		LastAccessTime.LowPart = other.LastAccessTime.LowPart;
		LastAccessTime.HighPart = other.LastAccessTime.HighPart;
		other.LastAccessTime.LowPart = 0;
		other.LastAccessTime.HighPart = 0;
		LastWriteTime.LowPart = other.LastWriteTime.LowPart;
		LastWriteTime.HighPart = other.LastWriteTime.HighPart;
		other.LastWriteTime.LowPart = 0;
		other.LastWriteTime.HighPart = 0;
		ChangeTime.LowPart = other.ChangeTime.LowPart;
		ChangeTime.HighPart = other.ChangeTime.HighPart;
		other.ChangeTime.LowPart = 0;
		other.ChangeTime.HighPart = 0;
		EndOfFile.LowPart = other.EndOfFile.LowPart;
		EndOfFile.HighPart = other.EndOfFile.HighPart;
		other.EndOfFile.LowPart = 0;
		other.EndOfFile.HighPart = 0;
		AllocationSize.LowPart = other.AllocationSize.LowPart;
		AllocationSize.HighPart = other.AllocationSize.HighPart;
		other.AllocationSize.LowPart = 0;
		other.AllocationSize.HighPart = 0;
		wsprintf(ShortName, L"%s", other.ShortName);
		memset(other.ShortName, 0, FSH_SHORTNAMELENGTH * sizeof(wchar_t));
		wsprintf(FileName, L"%s", other.FileName);
		memset(other.FileName, 0, sizeof(wchar_t));
	}
	return *this;
}

bool FileDirBothInformation::operator==(const FileDirBothInformation &other) const {
	if (this != &other) {
		return (NextEntryOffset == other.NextEntryOffset &&
				FileIndex == other.FileIndex &&
				FileAttributes == other.FileAttributes &&
				FileNameLength == other.FileNameLength &&
				EaSize == other.EaSize &&
				ShortNameLength == other.ShortNameLength &&
				CreationTime.QuadPart == other.CreationTime.QuadPart &&
				LastAccessTime.QuadPart == other.LastAccessTime.QuadPart &&
				LastWriteTime.QuadPart == other.LastWriteTime.QuadPart &&
				ChangeTime.QuadPart == other.ChangeTime.QuadPart &&
				EndOfFile.QuadPart == other.EndOfFile.QuadPart &&
				AllocationSize.QuadPart == other.AllocationSize.QuadPart &&
				!memcmp(ShortName, other.ShortName, FSH_SHORTNAMELENGTH * sizeof(wchar_t)) &&
				!memcmp(FileName, other.FileName, FileNameLength * sizeof(wchar_t)));
	} else {
		return true;
	}
}

bool FileDirBothInformation::operator!=(const FileDirBothInformation& other) const {
	if (this != &other) {
		return (NextEntryOffset != other.NextEntryOffset ||
				FileIndex != other.FileIndex ||
				FileAttributes != other.FileAttributes ||
				FileNameLength != other.FileNameLength ||
				EaSize != other.EaSize ||
				ShortNameLength != other.ShortNameLength ||
				CreationTime.QuadPart != other.CreationTime.QuadPart ||
				LastAccessTime.QuadPart != other.LastAccessTime.QuadPart ||
				LastWriteTime.QuadPart != other.LastWriteTime.QuadPart ||
				ChangeTime.QuadPart != other.ChangeTime.QuadPart ||
				EndOfFile.QuadPart != other.EndOfFile.QuadPart ||
				AllocationSize.QuadPart != other.AllocationSize.QuadPart ||
				!memcmp(ShortName, other.ShortName, FSH_SHORTNAMELENGTH * sizeof(wchar_t)) ||
				!memcmp(FileName, other.FileName, FileNameLength * sizeof(wchar_t)));
	} else {
		return false;
	}
}

BinData::BinData() {
	Platform = BinPlatform::PlatformUnknown;
	BitDepth = BinBitDepth::BitDepthUnknown;
}

BinData::BinData(const BinBitDepth bitDepth, const BinPlatform binPlatform) {
	BitDepth = bitDepth;
	Platform = binPlatform;
}

BinData::BinData(const BinData &other) {
	if (this != &other) {
		Platform = other.Platform;
		BitDepth = other.BitDepth;
	}
}

BinData::BinData(BinData&& other) noexcept {
	if (this != &other) {
		Platform = other.Platform;
		other.Platform = BinPlatform::PlatformUnknown;
		BitDepth = other.BitDepth;
		other.BitDepth = BinBitDepth::BitDepthUnknown;
	}
}

BinData::~BinData() {}

BinData& BinData::operator=(const BinData& other) {
	if (this != &other) {
		BitDepth = other.BitDepth;
		Platform = other.Platform;
	}
	return *this;
}

BinData& BinData::operator=(BinData &&other) noexcept {
	if (this != &other) {
		Platform = other.Platform;
		other.Platform = BinPlatform::PlatformUnknown;
		BitDepth = other.BitDepth;
		other.BitDepth = BinBitDepth::BitDepthUnknown;
	}
	return *this;
}

bool BinData::operator==(const BinData& other) const {
	if (this != &other) {
		return (BitDepth == other.BitDepth && Platform == other.Platform);
	} else {
		return true;
	}
}

bool BinData::operator!=(const BinData& other) const {
	if (this != &other) {
		return (BitDepth != other.BitDepth || Platform != other.Platform);
	} else {
		return false;
	}
}

FileRecord::FileRecord() {
	size = 0;
}

FileRecord::FileRecord(const FileRecord& other) {
	if (this != &other) {
		fileName = other.fileName;
		filePath = other.filePath;
		hash = other.hash;
		size = other.size;
	}
}

FileRecord::FileRecord(FileRecord &&other) noexcept {
	if (this != &other) {
		fileName = other.fileName;
		other.fileName.~basic_string();
		filePath = other.filePath;
		other.filePath.~basic_string();
		hash = other.hash;
		other.hash.~basic_string();
		size = other.size;
		other.size = 0;
	}
}

FileRecord::~FileRecord() {}

FileRecord& FileRecord::operator=(const FileRecord& other) {
	if (this != &other) {
		fileName = other.fileName;
		filePath = other.filePath;
		hash = other.hash;
		size = other.size;
	}
	return *this;
}

FileRecord& FileRecord::operator=(FileRecord &&other) noexcept {
	if (this != &other) {
		fileName = other.fileName;
		other.fileName.~basic_string();
		filePath = other.filePath;
		other.filePath.~basic_string();
		hash = other.hash;
		other.hash.~basic_string();
		size = other.size;
		other.size = 0;
	}
	return *this;
}

bool FileRecord::operator==(const FileRecord& other) const {
	if (this != &other) {
		return (lower_copy(hash) == lower_copy(other.hash) &&
				size == other.size &&
#if defined (_WIN32) || defined (_WIN64)
				lower_copy(filePath) == lower_copy(other.filePath));
#else
				filePath == other.filePath);
#endif
	} else {
		return true;
	}
}

bool FileRecord::operator!=(const FileRecord& other) const {
	if (this != &other) {
		return (lower_copy(hash) != lower_copy(other.hash) ||
				size != other.size ||
#if defined (_WIN32) || defined (_WIN64)
				lower_copy(filePath) != lower_copy(other.filePath));
#else
				filePath != other.filePath);
#endif
	} else {
		return false;
	}
}

FolderRecord::FolderRecord() {}

FolderRecord::FolderRecord(const FolderRecord &other) {
	if (this != &other) {
		folderName = other.folderName;
		folderPath = other.folderPath;
		files = other.files;
		folders = other.folders;
	}
}

FolderRecord::FolderRecord(FolderRecord&& other) noexcept {
	if (this != &other) {
		folderName = other.folderName;
		other.folderName.~basic_string();
		folderPath = other.folderPath;
		other.folderPath.~basic_string();
		files = other.files;
		other.files.~vector();
		folders = other.folders;
		other.folders.~vector();
	}
}

FolderRecord::~FolderRecord() {}

FolderRecord& FolderRecord::operator=(const FolderRecord& other) {
	if (this != &other) {
		folderName = other.folderName;
		folderPath = other.folderPath;
		files = other.files;
		folders = other.folders;
	}
	return *this;
}

FolderRecord& FolderRecord::operator=(const FolderRecord &&other) noexcept {
	if (this != &other) {
		folderName = other.folderName;
		other.folderName.~basic_string();
		folderPath = other.folderPath;
		other.folderPath.~basic_string();
		files = other.files;
		other.files.~vector();
		folders = other.folders;
		other.folders.~vector();
	}
	return *this;
}

bool FolderRecord::operator==(const FolderRecord &other) const {
	if (this != &other) {
		return (files == other.files &&
				folders == other.folders &&
#if defined (_WIN32) || defined (_WIN64)
				lower_copy(folderName) == lower_copy(other.folderName) &&
#else
				folderName == other.folderName) &&
#endif
#if defined (_WIN32) || defined (_WIN64)
				lower_copy(folderPath) == lower_copy(other.folderPath));
#else
				folderPath == other.folderPath);
#endif
	} else {
		return true;
	}
}

bool FolderRecord::operator!=(const FolderRecord &other) const {
	if (this != &other) {
		return (files != other.files ||
				folders != other.folders ||
#if defined (_WIN32) || defined (_WIN64)
				lower_copy(folderName) != lower_copy(other.folderName) ||
#else
				folderName == other.folderName) ||
#endif
#if defined (_WIN32) || defined (_WIN64)
				lower_copy(folderPath) != lower_copy(other.folderPath));
#else
				folderPath != other.folderPath);
#endif
	} else {
		return false;
	}
}

PartitionDesc::PartitionDesc() {
	volumeSerial = 0;
	spaceFree = 0;
	spaceTotal = 0;
	maxComponentLen = 0;
	caseSensitiveSearch = false;
	casePreservedMames = false;
	unicodeFNames = false;
	persistentACLS = false;
	supportFileCompress = false;
	supportQuota = false;
	supportSparseFile = false;
	supportReparsePoints = false;
	supportRemoteStorage = false;
	supportPosixUnlinkRename = false;
	supportObjectIds = false;
	supportEncryption = false;
	supportNamedStreams = false;
	supportTransactions = false;
	supportHardLink = false;
	suportExtendAttrib = false;
	supportFileIdOpen = false;
	supportUSNJournal = false;
	supportIntegrityStream = false;
	supportBlockRefCount = false;
	supportSparseVdl = false;
	supportGhosting = false;
	returnCleanupResInfo = false;
	volumeCompressed = false;
	volumeReadOnly = false;
	volumeDax = false;
	seqWriteOnce = false;
}

PartitionDesc::PartitionDesc(const PartitionDesc &other) {
	if (this != &other) {
		partLetter = other.partLetter;
		volumeLabel = other.volumeLabel;
		volumePath = other.volumePath;
		drivePath = other.drivePath;
		volumeSerial = other.volumeSerial;
		volumeSerialStr = other.volumeSerialStr;
		spaceFree = other.spaceFree;
		spaceTotal = other.spaceTotal;
		fsName = other.fsName;
		maxComponentLen = other.maxComponentLen;
		caseSensitiveSearch = other.caseSensitiveSearch;
		casePreservedMames = other.casePreservedMames;
		unicodeFNames = other.unicodeFNames;
		persistentACLS = other.persistentACLS;
		supportFileCompress = other.supportFileCompress;
		supportQuota = other.supportQuota;
		supportSparseFile = other.supportSparseFile;
		supportReparsePoints = other.supportReparsePoints;
		supportRemoteStorage = other.supportRemoteStorage;
		supportPosixUnlinkRename = other.supportPosixUnlinkRename;
		supportObjectIds = other.supportObjectIds;
		supportEncryption = other.supportEncryption;
		supportNamedStreams = other.supportNamedStreams;
		supportTransactions = other.supportTransactions;
		supportHardLink = other.supportHardLink;
		suportExtendAttrib = other.suportExtendAttrib;
		supportFileIdOpen = other.supportFileIdOpen;
		supportUSNJournal = other.supportUSNJournal;
		supportIntegrityStream = other.supportIntegrityStream;
		supportBlockRefCount = other.supportBlockRefCount;
		supportSparseVdl = other.supportSparseVdl;
		supportGhosting = other.supportGhosting;
		returnCleanupResInfo = other.returnCleanupResInfo;
		volumeCompressed = other.volumeCompressed;
		volumeReadOnly = other.volumeReadOnly;
		volumeDax = other.volumeDax;
		seqWriteOnce = other.seqWriteOnce;
	}
}

PartitionDesc::PartitionDesc(PartitionDesc &&other) noexcept {
	if (this != &other) {
		partLetter = other.partLetter;
		other.partLetter.~basic_string();
		volumeLabel = other.volumeLabel;
		other.volumeLabel.~basic_string();
		volumePath = other.volumePath;
		other.volumePath.~basic_string();
		drivePath = other.drivePath;
		other.drivePath.~basic_string();
		volumeSerial = other.volumeSerial;
		other.volumeSerial = 0;
		volumeSerialStr = other.volumeSerialStr;
		other.volumeSerialStr.~basic_string();
		spaceFree = other.spaceFree;
		other.spaceFree = 0;
		spaceTotal = other.spaceTotal;
		other.spaceTotal = 0;
		fsName = other.fsName;
		other.fsName.~basic_string();
		maxComponentLen = other.maxComponentLen;
		other.maxComponentLen = 0;
		caseSensitiveSearch = other.caseSensitiveSearch;
		other.caseSensitiveSearch = false;
		casePreservedMames = other.casePreservedMames;
		other.casePreservedMames = false;
		unicodeFNames = other.unicodeFNames;
		other.unicodeFNames = false;
		persistentACLS = other.persistentACLS;
		other.persistentACLS = false;
		supportFileCompress = other.supportFileCompress;
		other.supportFileCompress = false;
		supportQuota = other.supportQuota;
		other.supportQuota = false;
		supportSparseFile = other.supportSparseFile;
		other.supportSparseFile = false;
		supportReparsePoints = other.supportReparsePoints;
		other.supportReparsePoints = false;
		supportRemoteStorage = other.supportRemoteStorage;
		other.supportRemoteStorage = false;
		supportPosixUnlinkRename = other.supportPosixUnlinkRename;
		other.supportRemoteStorage = false;
		supportObjectIds = other.supportObjectIds;
		other.supportObjectIds = false;
		supportEncryption = other.supportEncryption;
		other.supportEncryption = false;
		supportNamedStreams = other.supportNamedStreams;
		other.supportNamedStreams = false;
		supportTransactions = other.supportTransactions;
		other.supportTransactions = false;
		supportHardLink = other.supportHardLink;
		other.supportHardLink = false;
		suportExtendAttrib = other.suportExtendAttrib;
		other.suportExtendAttrib = false;
		supportFileIdOpen = other.supportFileIdOpen;
		other.supportFileIdOpen = false;
		supportUSNJournal = other.supportUSNJournal;
		other.supportUSNJournal = false;
		supportIntegrityStream = other.supportIntegrityStream;
		other.supportIntegrityStream = false;
		supportBlockRefCount = other.supportBlockRefCount;
		other.supportBlockRefCount = false;
		supportSparseVdl = other.supportSparseVdl;
		other.supportSparseVdl = false;
		supportGhosting = other.supportGhosting;
		other.supportGhosting = false;
		returnCleanupResInfo = other.returnCleanupResInfo;
		other.returnCleanupResInfo = false;
		volumeCompressed = other.volumeCompressed;
		other.volumeCompressed = false;
		volumeReadOnly = other.volumeReadOnly;
		other.volumeReadOnly = false;
		volumeDax = other.volumeDax;
		other.volumeDax = false;
		seqWriteOnce = other.seqWriteOnce;
		other.seqWriteOnce = false;
	}
}

PartitionDesc& PartitionDesc::operator=(const PartitionDesc &other) {
	if (this != &other) {
		partLetter = other.partLetter;
		drivePath = other.drivePath;
		volumeSerial = other.volumeSerial;
		volumeSerialStr = other.volumeSerialStr;
		volumeLabel = other.volumeLabel;
		fsName = other.fsName;
		spaceFree = other.spaceFree;
		spaceTotal = other.spaceTotal;
		maxComponentLen = other.maxComponentLen;
		caseSensitiveSearch = other.caseSensitiveSearch;
		casePreservedMames = other.casePreservedMames;
		unicodeFNames = other.unicodeFNames;
		persistentACLS = other.persistentACLS;
		supportFileCompress = other.supportFileCompress;
		supportQuota = other.supportQuota;
		supportSparseFile = other.supportSparseFile;
		supportReparsePoints = other.supportReparsePoints;
		supportRemoteStorage = other.supportRemoteStorage;
		supportPosixUnlinkRename = other.supportPosixUnlinkRename;
		supportObjectIds = other.supportObjectIds;
		supportEncryption = other.supportEncryption;
		supportNamedStreams = other.supportNamedStreams;
		supportTransactions = other.supportTransactions;
		supportHardLink = other.supportHardLink;
		suportExtendAttrib = other.suportExtendAttrib;
		supportFileIdOpen = other.supportFileIdOpen;
		supportUSNJournal = other.supportUSNJournal;
		supportIntegrityStream = other.supportIntegrityStream;
		supportBlockRefCount = other.supportBlockRefCount;
		supportSparseVdl = other.supportSparseVdl;
		supportGhosting = other.supportGhosting;
		returnCleanupResInfo = other.returnCleanupResInfo;
		volumeCompressed = other.volumeCompressed;
		volumeReadOnly = other.volumeReadOnly;
		volumeDax = other.volumeDax;
		seqWriteOnce = other.seqWriteOnce;
	}
	return *this;
}

PartitionDesc& PartitionDesc::operator=(PartitionDesc&& other) noexcept {
	if (this != &other) {
		partLetter = other.partLetter;
		other.partLetter.~basic_string();
		volumeLabel = other.volumeLabel;
		other.volumeLabel.~basic_string();
		volumePath = other.volumePath;
		other.volumePath.~basic_string();
		drivePath = other.drivePath;
		other.drivePath.~basic_string();
		volumeSerial = other.volumeSerial;
		other.volumeSerial = 0;
		volumeSerialStr = other.volumeSerialStr;
		other.volumeSerialStr.~basic_string();
		spaceFree = other.spaceFree;
		other.spaceFree = 0;
		spaceTotal = other.spaceTotal;
		other.spaceTotal = 0;
		fsName = other.fsName;
		other.fsName.~basic_string();
		maxComponentLen = other.maxComponentLen;
		other.maxComponentLen = 0;
		caseSensitiveSearch = other.caseSensitiveSearch;
		other.caseSensitiveSearch = false;
		casePreservedMames = other.casePreservedMames;
		other.casePreservedMames = false;
		unicodeFNames = other.unicodeFNames;
		other.unicodeFNames = false;
		persistentACLS = other.persistentACLS;
		other.persistentACLS = false;
		supportFileCompress = other.supportFileCompress;
		other.supportFileCompress = false;
		supportQuota = other.supportQuota;
		other.supportQuota = false;
		supportSparseFile = other.supportSparseFile;
		other.supportSparseFile = false;
		supportReparsePoints = other.supportReparsePoints;
		other.supportReparsePoints = false;
		supportRemoteStorage = other.supportRemoteStorage;
		other.supportRemoteStorage = false;
		supportPosixUnlinkRename = other.supportPosixUnlinkRename;
		other.supportRemoteStorage = false;
		supportObjectIds = other.supportObjectIds;
		other.supportObjectIds = false;
		supportEncryption = other.supportEncryption;
		other.supportEncryption = false;
		supportNamedStreams = other.supportNamedStreams;
		other.supportNamedStreams = false;
		supportTransactions = other.supportTransactions;
		other.supportTransactions = false;
		supportHardLink = other.supportHardLink;
		other.supportHardLink = false;
		suportExtendAttrib = other.suportExtendAttrib;
		other.suportExtendAttrib = false;
		supportFileIdOpen = other.supportFileIdOpen;
		other.supportFileIdOpen = false;
		supportUSNJournal = other.supportUSNJournal;
		other.supportUSNJournal = false;
		supportIntegrityStream = other.supportIntegrityStream;
		other.supportIntegrityStream = false;
		supportBlockRefCount = other.supportBlockRefCount;
		other.supportBlockRefCount = false;
		supportSparseVdl = other.supportSparseVdl;
		other.supportSparseVdl = false;
		supportGhosting = other.supportGhosting;
		other.supportGhosting = false;
		returnCleanupResInfo = other.returnCleanupResInfo;
		other.returnCleanupResInfo = false;
		volumeCompressed = other.volumeCompressed;
		other.volumeCompressed = false;
		volumeReadOnly = other.volumeReadOnly;
		other.volumeReadOnly = false;
		volumeDax = other.volumeDax;
		other.volumeDax = false;
		seqWriteOnce = other.seqWriteOnce;
		other.seqWriteOnce = false;
	}
	return *this;
}

bool PartitionDesc::operator==(const PartitionDesc& other) const {
	if (this != &other) {
		return (lower_copy(partLetter) == lower_copy(other.partLetter) &&
				lower_copy(drivePath) == lower_copy(other.drivePath) &&
				lower_copy(volumePath) == lower_copy(other.volumePath) &&
				lower_copy(volumeSerialStr) == lower_copy(other.volumeSerialStr) &&
				lower_copy(volumeLabel) == lower_copy(other.volumeLabel) &&
				lower_copy(fsName) == lower_copy(other.fsName) &&
				volumeSerial == other.volumeSerial &&
				spaceFree == other.spaceFree &&
				spaceTotal == other.spaceTotal &&
				fsName == other.fsName &&
				maxComponentLen == other.maxComponentLen &&
				caseSensitiveSearch == other.caseSensitiveSearch &&
				casePreservedMames == other.casePreservedMames &&
				unicodeFNames == other.unicodeFNames &&
				persistentACLS == other.persistentACLS &&
				supportFileCompress == other.supportFileCompress &&
				supportQuota == other.supportQuota &&
				supportSparseFile == other.supportSparseFile &&
				supportReparsePoints == other.supportReparsePoints &&
				supportRemoteStorage == other.supportRemoteStorage &&
				supportPosixUnlinkRename == other.supportPosixUnlinkRename &&
				supportObjectIds == other.supportObjectIds &&
				supportEncryption == other.supportEncryption &&
				supportNamedStreams == other.supportNamedStreams &&
				supportTransactions == other.supportTransactions &&
				supportHardLink == other.supportHardLink &&
				suportExtendAttrib == other.suportExtendAttrib &&
				supportFileIdOpen == other.supportFileIdOpen &&
				supportUSNJournal == other.supportUSNJournal &&
				supportIntegrityStream == other.supportIntegrityStream &&
				supportBlockRefCount == other.supportBlockRefCount &&
				supportSparseVdl == other.supportSparseVdl &&
				supportGhosting == other.supportGhosting &&
				returnCleanupResInfo == other.returnCleanupResInfo &&
				volumeCompressed == other.volumeCompressed &&
				volumeReadOnly == other.volumeReadOnly &&
				volumeDax == other.volumeDax &&
				seqWriteOnce == other.seqWriteOnce);
	} else {
		return true;
	}
}

bool PartitionDesc::operator!=(const PartitionDesc& other) const {
	if (this != &other) {
		return (lower_copy(partLetter) != lower_copy(other.partLetter) ||
				lower_copy(drivePath) != lower_copy(other.drivePath) ||
				lower_copy(volumePath) != lower_copy(other.volumePath) ||
				lower_copy(volumeSerialStr) != lower_copy(other.volumeSerialStr) ||
				lower_copy(volumeLabel) != lower_copy(other.volumeLabel) ||
				lower_copy(fsName) != lower_copy(other.fsName) ||
				volumeSerial != other.volumeSerial ||
				spaceFree != other.spaceFree ||
				spaceTotal != other.spaceTotal ||
				fsName != other.fsName ||
				maxComponentLen != other.maxComponentLen ||
				caseSensitiveSearch != other.caseSensitiveSearch ||
				casePreservedMames != other.casePreservedMames ||
				unicodeFNames != other.unicodeFNames ||
				persistentACLS != other.persistentACLS ||
				supportFileCompress != other.supportFileCompress ||
				supportQuota != other.supportQuota ||
				supportSparseFile != other.supportSparseFile ||
				supportReparsePoints != other.supportReparsePoints ||
				supportRemoteStorage != other.supportRemoteStorage ||
				supportPosixUnlinkRename != other.supportPosixUnlinkRename ||
				supportObjectIds != other.supportObjectIds ||
				supportEncryption != other.supportEncryption ||
				supportNamedStreams != other.supportNamedStreams ||
				supportTransactions != other.supportTransactions ||
				supportHardLink != other.supportHardLink ||
				suportExtendAttrib != other.suportExtendAttrib ||
				supportFileIdOpen != other.supportFileIdOpen ||
				supportUSNJournal != other.supportUSNJournal ||
				supportIntegrityStream != other.supportIntegrityStream ||
				supportBlockRefCount != other.supportBlockRefCount ||
				supportSparseVdl != other.supportSparseVdl ||
				supportGhosting != other.supportGhosting ||
				returnCleanupResInfo != other.returnCleanupResInfo ||
				volumeCompressed != other.volumeCompressed ||
				volumeReadOnly != other.volumeReadOnly ||
				volumeDax != other.volumeDax ||
				seqWriteOnce != other.seqWriteOnce);
	} else {
		return false;
	}
}

PartitionDesc::~PartitionDesc() {}

FSHandler::FSHandler() {}

FSHandler::~FSHandler() {}

FSOpResult FSHandler::EnumPartitions(std::vector<PartitionDesc>& partList,
	const bool clearList) {
	unsigned long psz = 0, psn = 0;
	wchar_t plBuf[4] = { 0 };
	wchar_t drivePathBuf[MAX_PATH + 1] = { 0 };
	wchar_t vpnBuf[MAX_PATH + 1] = { 0 };
	std::vector<PartitionDesc> ret;
	PartitionDesc elem;
	// ::HANDLE hPart = FindFirstVolume(vol_name, ARRAYSIZE(vol_name))
	unsigned long drives = GetLogicalDrives();
	if (drives) {
		if (clearList) {
			partList.clear();
		}
		for (int i = 0; i < 26; ++i) {
			plBuf[0] = wchar_t(65 + i);
			plBuf[1] = L':';
			plBuf[2] = L'\\';
			plBuf[3] = L'\0';
			elem.partLetter = plBuf;
			if (GetVolumeNameForVolumeMountPoint(plBuf, drivePathBuf, MAX_PATH)) {
					elem.volumePath = drivePathBuf;
					unsigned long fileSystemFlags = 0;
					wchar_t fsname[MAX_PATH + 1] = { 0 };
				if (GetVolumeInformation(plBuf, drivePathBuf, MAX_PATH, &psn,
					&elem.maxComponentLen, &fileSystemFlags, fsname, MAX_PATH)) {
					wchar_t buf[16] = { 0 };
					elem.volumeSerial = psn;
					swprintf(buf, L"%X", psn);
					upper_chars(buf);
					elem.fsName = fsname;
					elem.volumeSerialStr = buf;
					elem.volumeLabel = drivePathBuf;
					elem.caseSensitiveSearch = fileSystemFlags & FILE_CASE_SENSITIVE_SEARCH;
					elem.casePreservedMames = fileSystemFlags & FILE_CASE_PRESERVED_NAMES;
					elem.unicodeFNames = fileSystemFlags & FILE_UNICODE_ON_DISK;
					elem.persistentACLS = fileSystemFlags & FILE_PERSISTENT_ACLS;
					elem.supportFileCompress = fileSystemFlags & FILE_FILE_COMPRESSION;
					elem.supportQuota = fileSystemFlags & FILE_VOLUME_QUOTAS;
					elem.supportSparseFile = fileSystemFlags & FILE_SUPPORTS_SPARSE_FILES;
					elem.supportReparsePoints = fileSystemFlags & FILE_SUPPORTS_REPARSE_POINTS;
					elem.supportRemoteStorage = fileSystemFlags & FILE_SUPPORTS_REMOTE_STORAGE;
					elem.supportPosixUnlinkRename = fileSystemFlags & FILE_SUPPORTS_POSIX_UNLINK_RENAME;
					elem.supportObjectIds = fileSystemFlags & FILE_SUPPORTS_OBJECT_IDS;
					elem.supportEncryption = fileSystemFlags & FILE_SUPPORTS_ENCRYPTION;
					elem.supportNamedStreams = fileSystemFlags & FILE_NAMED_STREAMS;
					elem.supportTransactions = fileSystemFlags & FILE_SUPPORTS_TRANSACTIONS;
					elem.supportHardLink = fileSystemFlags & FILE_SUPPORTS_HARD_LINKS;
					elem.suportExtendAttrib = fileSystemFlags & FILE_SUPPORTS_EXTENDED_ATTRIBUTES;
					elem.supportFileIdOpen = fileSystemFlags & FILE_SUPPORTS_OPEN_BY_FILE_ID;
					elem.supportUSNJournal = fileSystemFlags & FILE_SUPPORTS_USN_JOURNAL;
					elem.supportIntegrityStream = fileSystemFlags & FILE_SUPPORTS_INTEGRITY_STREAMS;
					elem.supportBlockRefCount = fileSystemFlags & FILE_SUPPORTS_BLOCK_REFCOUNTING;
					elem.supportSparseVdl = fileSystemFlags & FILE_SUPPORTS_SPARSE_VDL;
					elem.supportGhosting = fileSystemFlags & FILE_SUPPORTS_GHOSTING;
					elem.returnCleanupResInfo = fileSystemFlags & FILE_RETURNS_CLEANUP_RESULT_INFO;
					elem.volumeCompressed = fileSystemFlags & FILE_VOLUME_IS_COMPRESSED;
					elem.volumeReadOnly = fileSystemFlags & FILE_READ_ONLY_VOLUME;
					elem.volumeDax = fileSystemFlags & FILE_DAX_VOLUME;
					elem.seqWriteOnce = fileSystemFlags & FILE_SEQUENTIAL_WRITE_ONCE;
				}	
				plBuf[0] = wchar_t(65 + i);
				plBuf[1] = L':';
				plBuf[2] = L'\0';
				if (QueryDosDevice(plBuf, vpnBuf, elem.volumePath.length())) {
					elem.drivePath = vpnBuf;
				}
				GetDriveSpace(elem.partLetter, elem.spaceFree, elem.spaceTotal);
				partList.push_back(elem);
			}
			memset(plBuf, 0, 4 * sizeof(wchar_t));
			memset(drivePathBuf, 0, (MAX_PATH + 1) * sizeof(wchar_t));
			memset(vpnBuf, 0, (MAX_PATH + 1) * sizeof(wchar_t));
			psn = 0;
		}
	}
	if (partList.size()) {
		return FSOpResult::Success;
	} else {
		return FSOpResult::Fail;
	}
}

FSOpResult FSHandler::GetPhysDriveIndexByPartLetter(const std::wstring partLetter,
	unsigned long &driveIndex, std::vector<PartitionDesc> *parts) {
	std::vector<PartitionDesc> vec;
	if (parts) {
		vec = *parts;
	} else {
		if (FSOpResult::Success != EnumPartitions(vec)) {
			return FSOpResult::Fail;
		}
	}
	std::wstring partLetterMod = partLetter;
	if (endsWith(partLetterMod, L":")) {
		partLetterMod += L"\\";
	} else {
		if (!endsWith(partLetterMod, L":\\")) {
			partLetterMod += L":\\";
		}
	}
	size_t i = 0;
	wchar_t* digits = (wchar_t*)L"0123456789";
	for (i = 0; i < vec.size(); ++i) {
		if (lower_copy(vec[i].partLetter) == lower_copy(partLetterMod)) {
			driveIndex = wstr2ul(firstNumberFromString(vec[i].drivePath));
			return FSOpResult::Success;
		}
	}
	return FSOpResult::Success;
}

FSOpResult FSHandler::GetBinaryFileInfo(const std::wstring binaryPath, BinData &binaryData) const {
	if (INVALID_FILE_ATTRIBUTES != GetFileAttributes(binaryPath.c_str())) {
		unsigned long binType = 0;
		if (GetBinaryType(binaryPath.c_str(), &binType)) {
			if (SCS_32BIT_BINARY == binType) {
				binaryData.BitDepth = BinBitDepth::X32;
				binaryData.Platform = BinPlatform::Windows;
			} else if (SCS_64BIT_BINARY == binType) {
				binaryData.BitDepth = BinBitDepth::X64;
				binaryData.Platform = BinPlatform::Windows;
			} else if (SCS_DOS_BINARY == binType) {
				binaryData.BitDepth = BinBitDepth::X16;
				binaryData.Platform = BinPlatform::DOS;
			} else if (SCS_OS216_BINARY == binType) {
				binaryData.BitDepth = BinBitDepth::X16;
				binaryData.Platform = BinPlatform::OSdiv2;
			} else if (SCS_PIF_BINARY == binType) {
				binaryData.BitDepth = BinBitDepth::X16;
				binaryData.Platform = BinPlatform::Windows;
			} else if (SCS_POSIX_BINARY == binType) {
				binaryData.BitDepth = BinBitDepth::BitDepthUnknown;
				binaryData.Platform = BinPlatform::Unix;
			} else if (SCS_WOW_BINARY == binType) {
				binaryData.BitDepth = BinBitDepth::X16;
				binaryData.Platform = BinPlatform::Windows;
			} else {
				binaryData.BitDepth = BinBitDepth::BitDepthUnknown;
				binaryData.Platform = BinPlatform::PlatformUnknown;
			}
			return FSOpResult::Success;
		}
	}
	return FSOpResult::Fail;
}

std::wstring FSHandler::GetFileControlSum(const std::wstring filePath, const HashType sumType) {
	if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(filePath.c_str())) {
		return L"";
	} else {
		return calcHash(filePath, sumType);
	}
}

bool FSHandler::PathExists(const std::wstring path) const {
	return (INVALID_FILE_ATTRIBUTES != ::GetFileAttributes(path.c_str()));
}

FSOpResult FSHandler::IsFolder(bool &isFolder, const std::wstring path) const {
	return attrAnalyzer(isFolder, FILE_ATTRIBUTE_DIRECTORY, path);
}

FSOpResult FSHandler::IsTemporary(bool &isTemporary, const std::wstring path) const {
	return attrAnalyzer(isTemporary, FILE_ATTRIBUTE_TEMPORARY, path);
}

FSOpResult FSHandler::IsNormal(bool &isNormal, const std::wstring path) const {
	return attrAnalyzer(isNormal, FILE_ATTRIBUTE_NORMAL, path);
}

FSOpResult FSHandler::IsArchive(bool &isArchive, const std::wstring path) const {
	return attrAnalyzer(isArchive, FILE_ATTRIBUTE_ARCHIVE, path);
}

FSOpResult FSHandler::IsCompressed(bool &isCompressed, const std::wstring path) const {
	return attrAnalyzer(isCompressed, FILE_ATTRIBUTE_COMPRESSED, path);
}

FSOpResult FSHandler::IsHidden(bool &isHidden, const std::wstring path) const {
	return attrAnalyzer(isHidden, FILE_ATTRIBUTE_HIDDEN, path);
}

FSOpResult FSHandler::IsEncrypted(bool &isEncrtpted, const std::wstring path) const {
	return attrAnalyzer(isEncrtpted, FILE_ATTRIBUTE_ENCRYPTED, path);
}

FSOpResult FSHandler::IsVirtual(bool &isVirtual, const std::wstring path) const {
	return attrAnalyzer(isVirtual, FILE_ATTRIBUTE_VIRTUAL, path);
}

FSOpResult FSHandler::CreateFolder(const std::wstring folderPath) const {
	if (!PathExists(folderPath)) {
		if (::CreateDirectory(folderPath.c_str(), 0)) {
			return FSOpResult::Success;
		} else {
			return FSOpResult::Fail;
		}
	} else {
		return FSOpResult::Fail;
	}
}

FSOpResult FSHandler::CreateFolder(const std::wstring folderPath, const SECURITY_ATTRIBUTES *secAttr) const {
	if (!PathExists(folderPath)) {
		if (::CreateDirectory(folderPath.c_str(), (SECURITY_ATTRIBUTES*)secAttr)) {
			return FSOpResult::Success;
		} else {
			return FSOpResult::Fail;
		}
	} else {
		return FSOpResult::Fail;
	}
}

FSOpResult FSHandler::CreateFolder(const std::wstring folderPath, const SecDesc secDesc) const {
	if (!PathExists(folderPath)) {
		if (!::CreateDirectory(folderPath.c_str(), 0)) {
			return FSOpResult::Fail;
		}
	}
	if (FSOpResult::Success == SetObjectSecurity(secDesc, folderPath)) {
		return FSOpResult::Success;
	} else {
		return FSOpResult::Fail;
	}
}

FSOpResult FSHandler::RemoveFolder_SHFileOp(const std::wstring folderPath, std::wstring *infoBuf) const {
	if (infoBuf && IsBadWritePtr(infoBuf)) {
		return FSOpResult::Fail;
	}
	if (!PathExists(folderPath)) {
		if (infoBuf) {
			*infoBuf = L"Specified path does not exist";
		}
		return FSOpResult::Fail;
	}
	bool isfld = true;
	if (FSOpResult::Success == IsFolder(isfld, folderPath)) {
		if (!isfld) {
			if (infoBuf) {
				*infoBuf = L"Specified path is not a folder";
			}
			return FSOpResult::Fail;
		}
	}
	size_t folderPathLen = wcslen_c(folderPath.c_str()) + 2;
	wchar_t* folderPathBuf = (wchar_t*)malloc(folderPathLen * sizeof(wchar_t));
	if (!folderPathBuf) {
		return FSOpResult::Fail;
	}
	memset(folderPathBuf, 0, folderPathLen * sizeof(wchar_t));
	wsprintf(folderPathBuf, L"%s", folderPath.c_str());
	SHFILEOPSTRUCT fileOp = {
		0,
		FO_DELETE,
		folderPathBuf,
		L"",
		FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT,
		false,
		0,
		L"" };
	unsigned long opres = SHFileOperation(&fileOp);
	SAFE_FREE(folderPathBuf);
	if (opres) {
		if (infoBuf) {
			if (FSOpResult::Success != getSHFileOpDesc(opres, infoBuf)) {
				return FSOpResult::Fail;
			}
		}
		return FSOpResult::Fail;
	} else {
		return FSOpResult::Success;
	}
}

FSOpResult FSHandler::CustomFileOp_SHFileOp(const std::wstring folderPath, const std::wstring folderPathDest,
	const unsigned long operation, const unsigned long opCode, std::wstring *infoBuf) const {
	if (!PathExists(folderPath) && !PathExists(folderPathDest)) {
		if (infoBuf && !IsBadWritePtr(infoBuf)) {
			*infoBuf = L"Requested path does not exist";
		}
		return FSOpResult::Fail;
	}
	size_t folderPathLen = wcslen_c(folderPath.c_str()) + 2;
	wchar_t* folderPathBuf = 0;
	if (folderPathLen) {
		folderPathBuf = (wchar_t*)malloc(folderPathLen * sizeof(wchar_t));
		if (!folderPathBuf) {
			return FSOpResult::Fail;
		}
		memset(folderPathBuf, 0, folderPathLen * sizeof(wchar_t));
		wsprintf(folderPathBuf, L"%s", folderPath.c_str());
	}
	folderPathLen = wcslen_c(folderPathDest.c_str()) + 2;
	wchar_t* folderPathDestBuf = 0;
	if (folderPathLen) {
		folderPathDestBuf = (wchar_t*)malloc(folderPathLen * sizeof(wchar_t));
		if (!folderPathDestBuf) {
			return FSOpResult::Fail;
		}
		memset(folderPathDestBuf, 0, folderPathLen * sizeof(wchar_t));
		wsprintf(folderPathDestBuf, L"%s", folderPathDest.c_str());
	}
	SHFILEOPSTRUCT fileOp = {
		0,
		operation,
		folderPathBuf,
		folderPathDestBuf,
		opCode,
		false,
		0,
		L"" };
	unsigned long opres = SHFileOperation(&fileOp);
	SAFE_FREE(folderPathBuf);
	SAFE_FREE(folderPathDestBuf);
	if (!opres) {
		return FSOpResult::Success;
	} else {
		if (infoBuf) {
			if (FSOpResult::Success != getSHFileOpDesc(opres, infoBuf)) {
				return FSOpResult::Fail;
			}
		}
		return FSOpResult::Fail;
	}
}

FSOpResult FSHandler::RemoveFolder(const std::wstring folderPath, const bool includeFiles) {
	std::vector<FileRecord> folderFiles = SeekFileInDir(folderPath, L".*", L"", false, false);
	if (folderFiles.size()) {
		if (includeFiles) {
			for (size_t i = 0; i < folderFiles.size(); ++i) {
				if (FSOpResult::Success != RemoveFile(folderFiles[i].filePath)) {
					return FSOpResult::Fail;
				}
			}
		} else {
			return FSOpResult::Fail;
		}
	}
	if (::RemoveDirectory(folderPath.c_str())) {
		return FSOpResult::Success;
	} else {
		return FSOpResult::Fail;
	}
}

FSOpResult FSHandler::RenameFolder(const std::wstring folderPath, const std::wstring folderPathDest,
	const RenameBehaviour renameBehaviour) {
	if (lower_copy(folderPath) == lower_copy(folderPathDest)) {
		return FSOpResult::Fail;
	}
	if (RenameBehaviour::Rename == renameBehaviour) {
		if (!PathExists(folderPath)) {
			return FSOpResult::Fail;
		}
		if (PathExists(folderPathDest)) {
			return FSOpResult::Fail;
		}
		if (MAX_PATH <= folderPath.length()) {
			return FSOpResult::Fail;
		}
		if (MAX_PATH <= folderPathDest.length()) {
			return FSOpResult::Fail;
		}
		bool isfld = true;
		if (FSOpResult::Success == IsFolder(isfld, folderPath)) {
			if (!isfld) {
				return FSOpResult::Fail;
			}
		}
		char* bufsrc = wchar2char(folderPath.c_str());
		if (!bufsrc) {
			return FSOpResult::Fail;
		}
		char* buftgt = wchar2char(folderPathDest.c_str());
		if (!buftgt) {
			return FSOpResult::Fail;
		}
		int renres = rename(bufsrc, buftgt);
		SAFE_ARR_DELETE(bufsrc);
		SAFE_ARR_DELETE(buftgt);
		if (!renres) {
			return FSOpResult::Success;
		} else {
			return FSOpResult::Fail;
		}
	} else if (RenameBehaviour::CopyDelete == renameBehaviour) {
		if (FSOpResult::Success != MoveFolder(folderPath, folderPathDest, true)) {
			return FSOpResult::Fail;
		}
		return FSOpResult::Success;
	} else {
		return FSOpResult::Fail;
	}
	
}

FSOpResult FSHandler::MoveFolder(const std::wstring folderPath, const std::wstring folderPathDest,
	const bool checkDestSpace) {
	if (lower_copy(folderPath) == lower_copy(folderPathDest)) {
		return FSOpResult::Fail;
	}
	if (!PathExists(folderPath)) {
		return FSOpResult::Fail;
	}
	if (PathExists(folderPathDest)) {
		return FSOpResult::Fail;
	}
	bool isfld = true;
	if (FSOpResult::Success == IsFolder(isfld, folderPath)) {
		if (!isfld) {
			return FSOpResult::Fail;
		}
	}
	if (checkDestSpace) {
		std::vector<std::wstring> pathSpl = splitStr(folderPathDest, L"\\");
		if (2 > pathSpl.size()) {
			return FSOpResult::Fail;
		}
		unsigned long long freeSpace = 0, totalSpace = 0;
		std::wstring partLetter = pathSpl.front();
		if (endsWith(partLetter, L"\\")) {
			partLetter = removeFromEnd_copy(partLetter, L"\\");
		}
		if (!GetDriveSpace(partLetter, freeSpace, totalSpace)) {
			return FSOpResult::Fail;
		}
		unsigned long long foldersz = 0;
		if (FSOpResult::Success == GetFolderSizeOnDrive_NtQueryDir(foldersz, folderPath)) {
			return FSOpResult::Fail;
		}
		if (foldersz >= freeSpace) {
			return FSOpResult::Fail;
		}
	}
	if (!::CopyFile(folderPath.c_str(), folderPathDest.c_str(), true)) {
		return FSOpResult::Fail;
	}
	return RemoveFolder(folderPath);
}

FSOpResult FSHandler::CopyFolder(const std::wstring folderPath, const std::wstring folderPathDest,
	const bool checkDestSpace) {
	if (lower_copy(folderPath) == lower_copy(folderPathDest)) {
		return FSOpResult::Fail;
	}
	if (!PathExists(folderPath)) {
		return FSOpResult::Fail;
	}
	if (PathExists(folderPathDest)) {
		return FSOpResult::Fail;
	}
	bool isfld = true;
	if (FSOpResult::Success == IsFolder(isfld, folderPath)) {
		if (!isfld) {
			return FSOpResult::Fail;
		}
	}
	if (checkDestSpace) {
		std::vector<std::wstring> pathSpl = splitStr(folderPathDest, L"\\");
		if (2 > pathSpl.size()) {
			return FSOpResult::Fail;
		}
		unsigned long long freeSpace = 0, totalSpace = 0;
		std::wstring partLetter = pathSpl.front();
		if (endsWith(partLetter, L"\\")) {
			partLetter = removeFromEnd_copy(partLetter, L"\\");
		}
		if (!GetDriveSpace(partLetter, freeSpace, totalSpace)) {
			return FSOpResult::Fail;
		}
		unsigned long long foldersz = 0;
		if (FSOpResult::Success == GetFolderSizeOnDrive_NtQueryDir(foldersz, folderPath)) {
			return FSOpResult::Fail;
		}
		if (foldersz >= freeSpace) {
			return FSOpResult::Fail;
		}
	}
	std::wstring folderPathPrepped = fs_pathnolim + folderPath;
	std::wstring folderPathDestPrepped = fs_pathnolim + folderPathDest;
	if (!::CopyFile(folderPathPrepped.c_str(), folderPathDestPrepped.c_str(), true)) {
		return FSOpResult::Fail;
	}
	return FSOpResult::Success;
}

FSOpResult FSHandler::RenameFile(const std::wstring filePath, const std::wstring filePathDest,
	const RenameBehaviour renameBehaviour) {
	if (lower_copy(filePath) == lower_copy(filePathDest)) {
		return FSOpResult::Fail;
	}
	if (RenameBehaviour::Rename == renameBehaviour) {
		if (!PathExists(filePath)) {
			return FSOpResult::Fail;
		}
		if (PathExists(filePathDest)) {
			return FSOpResult::Fail;
		}
		if (MAX_PATH <= filePath.length()) {
			return FSOpResult::Fail;
		}
		if (MAX_PATH <= filePathDest.length()) {
			return FSOpResult::Fail;
		}
		bool isfld = true;
		if (FSOpResult::Success == IsFolder(isfld, filePath)) {
			if (isfld) {
				return FSOpResult::Fail;
			}
		}
		char* bufsrc = wchar2char(filePath.c_str());
		if (!bufsrc) {
			return FSOpResult::Fail;
		}
		char* buftgt = wchar2char(filePathDest.c_str());
		if (!buftgt) {
			return FSOpResult::Fail;
		}
		int renres = rename(bufsrc, buftgt);
		SAFE_ARR_DELETE(bufsrc);
		SAFE_ARR_DELETE(buftgt);
		if (!renres) {
			return FSOpResult::Success;
		} else {
			return FSOpResult::Fail;
		}
	} else if (RenameBehaviour::CopyDelete == renameBehaviour) {
		if (FSOpResult::Success != FileMove(filePath, filePathDest, true)) {
			return FSOpResult::Fail;
		}
		return FSOpResult::Success;
	} else {
		return FSOpResult::Fail;
	}
}

FSOpResult FSHandler::FileMove(const std::wstring filePath, const std::wstring filePathDest,
	const bool checkDestSpace) {
	if (FSOpResult::Success != FileCopy(filePath, filePathDest, checkDestSpace)) {
		return FSOpResult::Fail;
	}
	return RemoveFile(filePath);
}

FSOpResult FSHandler::FileCopy(const std::wstring filePath, const std::wstring filePathDest,
	const bool checkDestSpace) {
	if (lower_copy(filePath) == lower_copy(filePathDest)) {
		return FSOpResult::Fail;
	}
	if (!PathExists(filePath)) {
		return FSOpResult::Fail;
	}
	if (PathExists(filePathDest)) {
		return FSOpResult::Fail;
	}
	bool isfld = true;
	if (FSOpResult::Success == IsFolder(isfld, filePath)) {
		if (isfld) {
			return FSOpResult::Fail;
		}
	}
	if (checkDestSpace) {
		std::vector<std::wstring> pathSpl = splitStr(filePathDest, L"\\");
		if (2 > pathSpl.size()) {
			return FSOpResult::Fail;
		}
		unsigned long long freeSpace = 0, totalSpace = 0;
		std::wstring partLetter = pathSpl.front();
		if (endsWith(partLetter, L"\\")) {
			partLetter = removeFromEnd_copy(partLetter, L"\\");
		}
		if (!GetDriveSpace(partLetter, freeSpace, totalSpace)) {
			return FSOpResult::Fail;
		}
		unsigned long long filesz = 0;
		if (FSOpResult::Success != GetFSizeOnDrive_NtQueryDir(filesz, filePath)) {
			return FSOpResult::Fail;
		}
		if (filesz >= freeSpace) {
			return FSOpResult::Fail;
		}
	}
	std::wstring filePathPrepped = fs_pathnolim + filePath;
	std::wstring filePathDestPrepped = fs_pathnolim + filePathDest;
	if (::CopyFile(filePathPrepped.c_str(), filePathDestPrepped.c_str(), true)) {
		return FSOpResult::Success;
	} else {
		return FSOpResult::Fail;
	}
}

FSOpResult FSHandler::RemoveFile_SHFileOp(const std::wstring filePath, std::wstring *infoBuf) const {
	if (infoBuf && IsBadWritePtr(infoBuf)) {
		return FSOpResult::Fail;
	}
	if (!PathExists(filePath)) {
		if (infoBuf) {
			*infoBuf = L"Specified path does not exist";
		}
		return FSOpResult::Fail;
	}
	bool isfld = true;
	if (FSOpResult::Success == IsFolder(isfld, filePath)) {
		if (isfld) {
			if (infoBuf) {
				*infoBuf = L"Specified path is not a file";
			}
			return FSOpResult::Fail;
		}
	}
	size_t filePathLen = wcslen_c(filePath.c_str()) + 2;
	wchar_t* filePathBuf = (wchar_t*)malloc(filePathLen * sizeof(wchar_t));
	if (!filePathBuf) {
		if (infoBuf) {
			*infoBuf = L"Buffer allocation in RemoveFile_SHFileOp failed";
		}
		return FSOpResult::Fail;
	}
	memset(filePathBuf, 0, filePathLen * sizeof(wchar_t));
	wsprintf(filePathBuf, L"%s", filePath.c_str());
	SHFILEOPSTRUCT fileOp = {
		0,
		FO_DELETE,
		filePathBuf,
		L"",
		FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT,
		false,
		0,
		L"" };
	unsigned long opres = SHFileOperation(&fileOp);
	SAFE_FREE(filePathBuf);
	if (!opres) {
		return FSOpResult::Success;
	} else {
		if (infoBuf) {
			if (FSOpResult::Success != getSHFileOpDesc(opres, infoBuf)) {
				return FSOpResult::Fail;
			}
		}
		return FSOpResult::Fail;
	}
}

FSOpResult FSHandler::RemoveFile(const std::wstring filePath) const {
	if (::DeleteFile(filePath.c_str())) {
		return FSOpResult::Success;
	} else {
		return FSOpResult::Fail;
	}
}

FSOpResult FSHandler::GetFolderSizeOnDrive_NtQueryDir(unsigned long long &folderSize, const std::wstring folderPath) {
	if (!PathExists(folderPath)) {
		return FSOpResult::Fail;
	}
	bool isfld = false;
	if (FSOpResult::Success == IsFolder(isfld, folderPath)) {
		if (!isfld) {
			return FSOpResult::Fail;
		}
	} else {
		return FSOpResult::Fail;
	}
	::HMODULE hModule = ::LoadLibrary(L"ntdll.dll");
	if (!hModule) {
		return FSOpResult::Fail;
	}
	pRtlInitUnicodeString = (NTSTATUS(WINAPI*)(UnicodeString*, wchar_t*)) ::GetProcAddress(hModule, "RtlInitUnicodeString");
	pNtCreateFile = (NTSTATUS(WINAPI*)(::HANDLE*, ACCESS_MASK, ObjectAttributes*, IOStatusBlock*, LARGE_INTEGER*,
		unsigned long, unsigned long, unsigned long, unsigned long, void*, unsigned long))
		::GetProcAddress(hModule, "NtCreateFile");
	pNtCreateEvent = (NTSTATUS(WINAPI*)(::HANDLE*, ACCESS_MASK, ObjectAttributes*, EvtType, unsigned char))
		::GetProcAddress(hModule, "NtCreateEvent");
	pNtQueryDirectoryFile = (NTSTATUS(WINAPI*)(::HANDLE, ::HANDLE, PIO_APC_ROUTINE, void*, IOStatusBlock*, void*,
		unsigned long, FolderInfo, unsigned char, UnicodeString*, unsigned char))
		::GetProcAddress(hModule, "NtQueryDirectoryFile");
	pNtWaitForSingleobject = (NTSTATUS(WINAPI*)(::HANDLE, unsigned char, LARGE_INTEGER*))
		::GetProcAddress(hModule, "NtWaitForSingleObject");
	pRtlUnicodeStringToAnsiString = (NTSTATUS(WINAPI*)(ANSIString*, UnicodeString*, unsigned char))
		::GetProcAddress(hModule, "RtlUnicodeStringToAnsiString");
	pNtClearEvent = (NTSTATUS(WINAPI*)(::HANDLE)) ::GetProcAddress(hModule, "NtClearEvent");
	pNtClose = (NTSTATUS(WINAPI*)(::HANDLE)) ::GetProcAddress(hModule, "NtClose");
	if (0 == pRtlInitUnicodeString) {
		return FSOpResult::Fail;
	}
	if (0 == pRtlUnicodeStringToAnsiString) {
		return FSOpResult::Fail;
	}
	if (0 == pNtCreateFile) {
		return FSOpResult::Fail;
	}
	if (0 == pNtCreateEvent) {
		return FSOpResult::Fail;
	}
	if (0 == pNtQueryDirectoryFile) {
		return FSOpResult::Fail;
	}
	if (0 == pNtWaitForSingleobject) {
		return FSOpResult::Fail;
	}
	if (0 == pNtClearEvent) {
		return FSOpResult::Fail;
	}
	if (0 == pNtClose) {
		return FSOpResult::Fail;
	}
	::HANDLE RootDirectoryHandle = 0;
	::HANDLE Event = 0;
	NTSTATUS ntStatus = STATUS_SUCCESS;
	UnicodeString RootDirectoryName;
	ANSIString as;
	ObjectAttributes RootDirectoryAttributes;
	IOStatusBlock IOSB;
	FileDirBothInformation* DirInformation;
	wchar_t* wszBuffer = (wchar_t*)malloc(64 * sizeof(wchar_t));
	if (!wszBuffer) {
		return FSOpResult::Fail;
	}
	wchar_t* Buffer = (wchar_t*)malloc(65536 * sizeof(wchar_t));
	if (!Buffer) {
		return FSOpResult::Fail;
	}
	wsprintf(wszBuffer, L"\\??\\%s\\", folderPath.c_str());
	ntStatus = ((pRtlInitUnicodeString)(&RootDirectoryName, wszBuffer));
	if (!NT_SUCCESS(ntStatus)) {
		SAFE_FREE(wszBuffer);
		SAFE_FREE(Buffer);
		return FSOpResult::Fail;
	}
	InitializeObjectAttributes(&RootDirectoryAttributes, &RootDirectoryName, OBJ_CASE_INSENSITIVE, 0, 0);
	ntStatus = ((pNtCreateFile)(&RootDirectoryHandle,
		GENERIC_READ,
		&RootDirectoryAttributes,
		&IOSB,
		0,
		FILE_ATTRIBUTE_DIRECTORY,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		FILE_OPEN,
		FILE_DIRECTORY_FILE,
		0, 0));
	if (!NT_SUCCESS(ntStatus)) {
		SAFE_FREE(wszBuffer);
		SAFE_FREE(Buffer);
		return FSOpResult::Fail;
	}
	ntStatus = ((pNtCreateEvent)(&Event, GENERIC_ALL, 0, EvtType::NotificationEvent, 0));
	if (!NT_SUCCESS(ntStatus)) {
		SAFE_FREE(wszBuffer);
		SAFE_FREE(Buffer);
		return FSOpResult::Fail;
	}
	ntStatus = ((pNtQueryDirectoryFile)(RootDirectoryHandle,
		Event, 0, 0,
		&IOSB,
		Buffer,
		65536 * sizeof(unsigned char),
		FolderInfo::FileDirectoryBothInformation,
		0, 0, 0));
	if (STATUS_PENDING == ntStatus) {
		ntStatus = ((pNtWaitForSingleobject)(Event, 1, 0));
	}
	if (!NT_SUCCESS(ntStatus)) {
		SAFE_FREE(wszBuffer);
		SAFE_FREE(Buffer);
		return FSOpResult::Fail;
	}
	DirInformation = (FileDirBothInformation*)Buffer;
	UnicodeString EntryName;
	while (true) {
		EntryName.MaximumLength = EntryName.Length = (unsigned short)DirInformation->FileNameLength;
		EntryName.Buffer = &DirInformation->FileName[0];
		((pRtlUnicodeStringToAnsiString)(&as, &EntryName, 1));
		if (DirInformation->FileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (strcmp(as.Buffer, ".") && strcmp(as.Buffer, "..")) {
				wchar_t* tbuf = char2wchar(as.Buffer);
				std::wstring targpath = folderPath + L"\\" + tbuf;
				GetFolderSizeOnDriveRec(folderSize, targpath, hModule);
				SAFE_ARR_DELETE(tbuf);
			}
		} else {
			folderSize += DirInformation->AllocationSize.QuadPart;
		}
		if (!DirInformation->NextEntryOffset) {
			break;
		} else {
			DirInformation = (FileDirBothInformation*)(((unsigned char*)DirInformation) + DirInformation->NextEntryOffset);
		}
	}
	((pNtClose)(RootDirectoryHandle));
	ntStatus = ((pNtClearEvent)(Event));
	if (!NT_SUCCESS(ntStatus)) {
		SAFE_FREE(wszBuffer);
		SAFE_FREE(Buffer);
		return FSOpResult::Fail;
	}
	((pNtClose)(Event));
	SAFE_FREE(wszBuffer);
	SAFE_FREE(Buffer);
	if (!::FreeLibrary(hModule)) {
		return FSOpResult::Fail;
	}
	return FSOpResult::Success;
}

FSOpResult FSHandler::GetFolderSize(unsigned long long &folderSize, const std::wstring folderPath) {
	if (!PathExists(folderPath)) {
		return FSOpResult::Fail;
	}
	bool isfld = false;
	if (FSOpResult::Success == IsFolder(isfld, folderPath)) {
		if (!isfld) {
			return FSOpResult::Fail;
		}
	} else {
		return FSOpResult::Fail;
	}
	folderSize = 0;
	std::vector<FileRecord> filerecs = SeekFileInDir(folderPath, L".*", L"", true, false);
	for (size_t i = 0; i < filerecs.size(); ++i) {
		folderSize += filerecs[i].size;
	}
	return FSOpResult::Success;
}

FSOpResult FSHandler::GetFSizeOnDrive_NtQueryDir(unsigned long long &fileSize, const std::wstring filePath) const {
	if (!PathExists(filePath)) {
		return FSOpResult::Fail;
	}
	bool isfld = false;
	if (FSOpResult::Success == IsFolder(isfld, filePath)) {
		if (isfld) {
			return FSOpResult::Fail;
		}
	} else {
		return FSOpResult::Fail;
	}
	std::vector<std::wstring> filePathSpl = splitStr(filePath, L"\\");
	if (2 > filePathSpl.size()) {
		return FSOpResult::Fail;
	}
	std::string filename = wstr2str(filePathSpl.back());
	filePathSpl.pop_back();
	std::wstring ownerfolder = joinStrs(filePathSpl, L"\\");
	::HMODULE hModule = ::LoadLibrary(L"ntdll.dll");
	if (!hModule) {
		return FSOpResult::Fail;
	}
	pRtlInitUnicodeString = (NTSTATUS(WINAPI*)(UnicodeString*, wchar_t*))
		::GetProcAddress(hModule, "RtlInitUnicodeString");
	pNtCreateFile = (NTSTATUS(WINAPI*)(::HANDLE*, ACCESS_MASK, ObjectAttributes*, IOStatusBlock*, LARGE_INTEGER*,
		unsigned long, unsigned long, unsigned long, unsigned long, void*, unsigned long))
		::GetProcAddress(hModule, "NtCreateFile");
	pNtCreateEvent = (NTSTATUS(WINAPI*)(::HANDLE*, ACCESS_MASK, ObjectAttributes*, EvtType, unsigned char))
		::GetProcAddress(hModule, "NtCreateEvent");
	pNtQueryDirectoryFile = (NTSTATUS(WINAPI*)(::HANDLE, ::HANDLE, PIO_APC_ROUTINE, void*, IOStatusBlock*, void*,
		unsigned long, FolderInfo, unsigned char, UnicodeString*, unsigned char))
		::GetProcAddress(hModule, "NtQueryDirectoryFile");
	pNtWaitForSingleobject = (NTSTATUS(WINAPI*)(::HANDLE, unsigned char, LARGE_INTEGER*))
		::GetProcAddress(hModule, "NtWaitForSingleObject");
	pRtlUnicodeStringToAnsiString = (NTSTATUS(WINAPI*)(ANSIString*, UnicodeString*, unsigned char))
		::GetProcAddress(hModule, "RtlUnicodeStringToAnsiString");
	pNtClearEvent = (NTSTATUS(WINAPI*)(::HANDLE)) ::GetProcAddress(hModule, "NtClearEvent");
	pNtClose = (NTSTATUS(WINAPI*)(::HANDLE)) ::GetProcAddress(hModule, "NtClose");
	if (0 == pRtlInitUnicodeString) {
		return FSOpResult::Fail;
	}
	if (0 == pRtlUnicodeStringToAnsiString) {
		return FSOpResult::Fail;
	}
	if (0 == pNtCreateFile) {
		return FSOpResult::Fail;
	}
	if (0 == pNtCreateEvent) {
		return FSOpResult::Fail;
	}
	if (0 == pNtQueryDirectoryFile) {
		return FSOpResult::Fail;
	}
	if (0 == pNtWaitForSingleobject) {
		return FSOpResult::Fail;
	}
	if (0 == pNtClearEvent) {
		return FSOpResult::Fail;
	}
	if (0 == pNtClose) {
		return FSOpResult::Fail;
	}
	::HANDLE RootDirectoryHandle = 0;
	::HANDLE Event = 0;
	NTSTATUS ntStatus = STATUS_SUCCESS;
	UnicodeString RootDirectoryName;
	ANSIString as;
	ObjectAttributes RootDirectoryAttributes;
	IOStatusBlock IOSB;
	FileDirBothInformation* DirInformation;
	wchar_t* wszBuffer = (wchar_t*)malloc(64 * sizeof(wchar_t));
	if (!wszBuffer) {
		return FSOpResult::Fail;
	}
	wchar_t* Buffer = (wchar_t*)malloc(65536 * sizeof(wchar_t));
	if (!Buffer) {
		return FSOpResult::Fail;
	}
	wsprintf(wszBuffer, L"\\??\\%s\\", ownerfolder.c_str());
	ntStatus = ((pRtlInitUnicodeString)(&RootDirectoryName, wszBuffer));
	if (!NT_SUCCESS(ntStatus)) {
		SAFE_FREE(wszBuffer);
		SAFE_FREE(Buffer);
		return FSOpResult::Fail;
	}
	InitializeObjectAttributes(&RootDirectoryAttributes, &RootDirectoryName, OBJ_CASE_INSENSITIVE, 0, 0);
	ntStatus = ((pNtCreateFile)(&RootDirectoryHandle,
		GENERIC_READ,
		&RootDirectoryAttributes,
		&IOSB,
		0,
		FILE_ATTRIBUTE_DIRECTORY,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		FILE_OPEN,
		FILE_DIRECTORY_FILE,
		0, 0));
	if (!NT_SUCCESS(ntStatus)) {
		SAFE_FREE(wszBuffer);
		SAFE_FREE(Buffer);
		return FSOpResult::Fail;
	}
	ntStatus = ((pNtCreateEvent)(&Event, GENERIC_ALL, 0, EvtType::NotificationEvent, 0));
	if (!NT_SUCCESS(ntStatus)) {
		SAFE_FREE(wszBuffer);
		SAFE_FREE(Buffer);
		return FSOpResult::Fail;
	}
	ntStatus = ((pNtQueryDirectoryFile)(RootDirectoryHandle,
		Event, 0, 0,
		&IOSB,
		Buffer,
		65536 * sizeof(unsigned char),
		FolderInfo::FileDirectoryBothInformation,
		0, 0, 0));
	if (STATUS_PENDING == ntStatus) {
		ntStatus = ((pNtWaitForSingleobject)(Event, 1, 0));
	}
	if (!NT_SUCCESS(ntStatus)) {
		SAFE_FREE(wszBuffer);
		SAFE_FREE(Buffer);
		return FSOpResult::Fail;
	}
	DirInformation = (FileDirBothInformation*)Buffer;
	UnicodeString EntryName;
	while (true) {
		EntryName.MaximumLength = EntryName.Length = (unsigned short)DirInformation->FileNameLength;
		EntryName.Buffer = &DirInformation->FileName[0];
		((pRtlUnicodeStringToAnsiString)(&as, &EntryName, 1));
		if (DirInformation->FileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
		} else {
			if (!stricmp(as.Buffer, filename.c_str())) {
				fileSize = DirInformation->AllocationSize.QuadPart;
				break;
			}
		}
		if (!DirInformation->NextEntryOffset) {
			break;
		} else {
			DirInformation = (FileDirBothInformation*)(((unsigned char*)DirInformation) + DirInformation->NextEntryOffset);
		}
	}
	((pNtClose)(RootDirectoryHandle));
	ntStatus = ((pNtClearEvent)(Event));
	if (!NT_SUCCESS(ntStatus)) {
		SAFE_FREE(wszBuffer);
		SAFE_FREE(Buffer);
		return FSOpResult::Fail;
	}
	((pNtClose)(Event));
	SAFE_FREE(wszBuffer);
	SAFE_FREE(Buffer);
	if (!::FreeLibrary(hModule)) {
		return FSOpResult::Fail;
	}
	return FSOpResult::Success;
}

FSOpResult FSHandler::GetFSize(unsigned long long &fileSize, const std::wstring filePath) const {
	if (PathExists(filePath)) {
		bool isfld = false;
		if (FSOpResult::Success == IsFolder(isfld, filePath)) {
			if (isfld) {
				return FSOpResult::Fail;
			}
		} else {
			return FSOpResult::Fail;
		}
	} else {
		return FSOpResult::Fail;
	}
	HANDLE hFile = ::CreateFile(filePath.c_str(), READ_CONTROL | FILE_READ_ATTRIBUTES, FILE_SHARE_READ, 0, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile && INVALID_HANDLE_VALUE != hFile) {
		LARGE_INTEGER li = { 0 };
		unsigned long fszhigh = 0;
		li.LowPart = ::GetFileSize(hFile, &fszhigh);
		li.HighPart = fszhigh;
		fileSize = li.QuadPart;
		::CloseHandle(hFile);
		hFile = 0;
		return FSOpResult::Success;
	} else {
		return FSOpResult::Fail;
	}
}

FSOpResult FSHandler::GetObjectSecurity(SecDesc &secDesc, const std::wstring objectPath) const {
	if (!PathExists(objectPath)) {
		return FSOpResult::Fail;
	}
	ProcessHandler proc;
	unsigned long procid = proc.GetCurrentProcPid();
	std::vector<std::wstring> privs = proc.GetProcPrivileges(procid);
	if (!valInList(privs, L"SeSecurityPrivilege")) {
		if (!proc.EnableSecurityPrivilege(procid)) {
			return FSOpResult::Fail;
		}
	}
	ACLHandler aclh;
	SysHandler sys;
	unsigned long secinfolen = 0;
	if (!::GetFileSecurity(objectPath.c_str(), static_cast<unsigned long>(SecInfo::DACLSecInfo), secDesc.daclInfo,
		secinfolen, &secinfolen)) {
		if (ERROR_INSUFFICIENT_BUFFER == getLastErrorCode()) {
			::SECURITY_DESCRIPTOR* tsd = (::SECURITY_DESCRIPTOR*)::LocalAlloc(LPTR, secinfolen);
			if (!tsd) {
				return FSOpResult::Fail;
			}
			secDesc.daclInfo = ::LocalAlloc(LPTR, secinfolen);
			if (secDesc.daclInfo) {
				if (!::GetFileSecurity(objectPath.c_str(), static_cast<unsigned long>(SecInfo::DACLSecInfo),
					tsd, secinfolen, &secinfolen)) {
					SAFE_LOCALFREE(secDesc.daclInfo);
					return FSOpResult::Fail;
				}
				memcpy(secDesc.daclInfo, tsd, secinfolen);
				SAFE_LOCALFREE(tsd);
				secDesc.daclInfoSz = secinfolen;
			} else {
				return FSOpResult::Fail;
			}
		}
	}
	secinfolen = 0;
	if (!::GetFileSecurity(objectPath.c_str(), static_cast<unsigned long>(SecInfo::SACLSecInfo), secDesc.saclInfo,
		secinfolen, &secinfolen)) {
		if (ERROR_INSUFFICIENT_BUFFER == getLastErrorCode()) {
			secDesc.saclInfo = ::LocalAlloc(LPTR, secinfolen);
			if (secDesc.saclInfo) {
				if (!::GetFileSecurity(objectPath.c_str(), static_cast<unsigned long>(SecInfo::SACLSecInfo),
					secDesc.saclInfo, secinfolen, &secinfolen)) {
					SAFE_LOCALFREE(secDesc.daclInfo);
					return FSOpResult::Fail;
				}
				secDesc.saclInfoSz = secinfolen;
			} else {
				return FSOpResult::Fail;
			}
		}
	}
	secinfolen = 0;
	::SECURITY_DESCRIPTOR* tsd = (::SECURITY_DESCRIPTOR*)::LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
	if (!tsd) {
		return FSOpResult::Fail;
	}
	if (!::GetFileSecurity(objectPath.c_str(), static_cast<unsigned long>(SecInfo::OwnerSecInfo), tsd, secinfolen,
		&secinfolen)) {
		if (ERROR_INSUFFICIENT_BUFFER == getLastErrorCode()) {
			if (!::GetFileSecurity(objectPath.c_str(),static_cast<unsigned long>(SecInfo::OwnerSecInfo),
				tsd, secinfolen, &secinfolen)) {
				SAFE_LOCALFREE(tsd);
				return FSOpResult::Fail;
			}
			::PSID tsid = 0;
			if (ACLOpResult::Success != aclh.OwnerSIDFromSecurityDescriptor(tsd, tsid)) {
				return FSOpResult::Fail;
			}
			secDesc.ownerInfo = sys.StrSIDFromSID(tsid);
			// std::wcout << L"xxx " << sys.GetAccountNameFromStrSID(secDesc.ownerInfo) << std::endl;
			secDesc.ownerInfoSz = secDesc.ownerInfo.length() * sizeof(wchar_t);
		}
	}
	SAFE_LOCALFREE(tsd);
	secinfolen = 0;
	tsd = (::SECURITY_DESCRIPTOR*)::LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
	if (!tsd) {
		return FSOpResult::Fail;
	}
	if (!::GetFileSecurity(objectPath.c_str(), static_cast<unsigned long>(SecInfo::GroupSecInfo), tsd, secinfolen,
		&secinfolen)) {
		if (ERROR_INSUFFICIENT_BUFFER == getLastErrorCode()) {
			if (!::GetFileSecurity(objectPath.c_str(), static_cast<unsigned long>(SecInfo::GroupSecInfo),
				tsd, secinfolen, &secinfolen)) {
				SAFE_LOCALFREE(tsd);
				return FSOpResult::Fail;
			}
			::PSID tsid = 0;
			if (ACLOpResult::Success != aclh.PrimaryGroupSIDFromSecurityDescriptor(tsd, tsid)) {
				return FSOpResult::Fail;
			}
			std::wstring sidname = sys.GetAccountNameFromSID(tsid);
			if((L"отсутствует" != lower_copy(sidname)) && (L"empty" != lower_copy(sidname)) &&
				(L"missing" != lower_copy(sidname))) {
				secDesc.primaryGroupInfo = sys.StrSIDFromSID(tsid);
				secDesc.primaryGroupInfoSz = secDesc.primaryGroupInfo.length() * sizeof(wchar_t);
			} else {
				secDesc.primaryGroupInfo = L"";
				secDesc.primaryGroupInfoSz = 0;
			}
			SAFE_LOCALFREE(tsd);
		}
	}
	if (ACLOpResult::Success != aclh.CreateAbsoluteSecDesc(secDesc)) {
		return FSOpResult::Fail;
	}
	return FSOpResult::Success;
}

FSOpResult FSHandler::SetObjectSecurity(const SecDesc secDesc, const std::wstring objectPath) const {
	if (!PathExists(objectPath)) {
		return FSOpResult::Fail;
	}
	ProcessHandler proc;
	unsigned long procid = proc.GetCurrentProcPid();
	std::vector<std::wstring> privs = proc.GetProcPrivileges(procid);
	if (!valInList(privs, L"SeBackupPrivilege")) {
		if (!proc.EnableBackupPrivilege(procid)) {
			return FSOpResult::Fail;
		}
	}
	if (!valInList(privs, L"SeRestorePrivilege")) {
		if (!proc.EnableRestorePrivilege(procid)) {
			return FSOpResult::Fail;
		}
	}
	if (!valInList(privs, L"SeSecurityPrivilege")) {
		if (!proc.EnableSecurityPrivilege(procid)) {
			return FSOpResult::Fail;
		}
	}
	if (!valInList(privs, L"SeTakeOwnershipPrivilege")) {
		if (!proc.EnableTakeOwnershipPrivilege(procid)) {
			return FSOpResult::Fail;
		}
	}
	ACLHandler aclh;
	if (secDesc.daclAbsInfo) {
		::ACL* acllist = 0;
		if (ACLOpResult::Success != aclh.DACLFromSecurityDescriptor(
			(::SECURITY_DESCRIPTOR*)secDesc.absoluteSDInfo, acllist)) {
			return FSOpResult::Fail;
		}
		::SECURITY_DESCRIPTOR* sd = (::SECURITY_DESCRIPTOR*)::LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
		if (!sd) {
			SAFE_LOCALFREE(acllist);
			return FSOpResult::Fail;
		}
		if (::InitializeSecurityDescriptor(sd, SECURITY_DESCRIPTOR_REVISION)) {
			if (!::SetSecurityDescriptorDacl(sd, true, acllist, false)) {
				SAFE_LOCALFREE(sd);
				SAFE_LOCALFREE(acllist);
				return FSOpResult::Fail;
			}
		} else {
			SAFE_LOCALFREE(sd);
			SAFE_LOCALFREE(acllist);
			return FSOpResult::Fail;
		}
		if (!::SetFileSecurity(objectPath.c_str(), static_cast<unsigned long>(SecInfo::DACLSecInfo), sd)) {
			SAFE_LOCALFREE(sd);
			SAFE_LOCALFREE(acllist);
			return FSOpResult::Fail;
		}
		SAFE_LOCALFREE(sd);
		SAFE_LOCALFREE(acllist);
	}
	if (secDesc.saclAbsInfo) {
		::ACL* acllist = 0;
		if (ACLOpResult::Success != aclh.SACLFromSecurityDescriptor((::SECURITY_DESCRIPTOR*)secDesc.absoluteSDInfo,
			acllist)) {
			return FSOpResult::Fail;
		}
		if (acllist) {
			::SECURITY_DESCRIPTOR* sd = (::SECURITY_DESCRIPTOR*)::LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
			if (!sd) {
				SAFE_LOCALFREE(acllist);
				return FSOpResult::Fail;
			}
			if (::InitializeSecurityDescriptor(sd, SECURITY_DESCRIPTOR_REVISION)) {
				if (!::SetSecurityDescriptorSacl(sd, true, acllist, false)) {
					SAFE_LOCALFREE(sd);
					SAFE_LOCALFREE(acllist);
					return FSOpResult::Fail;
				}
			} else {
				SAFE_LOCALFREE(sd);
				SAFE_LOCALFREE(acllist);
				return FSOpResult::Fail;
			}
			if (!::SetFileSecurity(objectPath.c_str(), static_cast<unsigned long>(SecInfo::SACLSecInfo), sd)) {
				SAFE_LOCALFREE(sd);
				SAFE_LOCALFREE(acllist);
				return FSOpResult::Fail;
			}
			SAFE_LOCALFREE(sd);
			SAFE_LOCALFREE(acllist);
		}
	}
	if (secDesc.ownerInfo.length()) {
		::SECURITY_DESCRIPTOR* sd = (::SECURITY_DESCRIPTOR*)::LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
		if (!sd) {
			return FSOpResult::Fail;
		}
		::PSID tsid = 0;
		if (::InitializeSecurityDescriptor(sd, SECURITY_DESCRIPTOR_REVISION)) {
			int ownerDefaulted = 0;
			SysHandler sys;
			tsid = sys.SIDFromStrSid(secDesc.ownerInfo);
			if (!tsid) {
				return FSOpResult::Fail;
			}
			if (!::SetSecurityDescriptorOwner(sd, tsid, ownerDefaulted)) {
				SAFE_LOCALFREE(tsid);
				return FSOpResult::Fail;
			}
		} else {
			SAFE_LOCALFREE(sd);
			return FSOpResult::Fail;
		}
		if (!::SetFileSecurity(objectPath.c_str(), static_cast<unsigned long>(SecInfo::OwnerSecInfo), sd)) {
			return FSOpResult::Fail;
		}
		SAFE_LOCALFREE(sd);
		SAFE_LOCALFREE(tsid);
	}
	if (secDesc.primaryGroupInfo.length()) {
		::SECURITY_DESCRIPTOR* sd = (::SECURITY_DESCRIPTOR*)::LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
		if (!sd) {
			return FSOpResult::Fail;
		}
		::PSID tsid = 0;
		if (::InitializeSecurityDescriptor(sd, SECURITY_DESCRIPTOR_REVISION)) {
			int groupDefaulted = 0;
			SysHandler sys;
			tsid = sys.SIDFromStrSid(secDesc.primaryGroupInfo);
			if (!tsid) {
				return FSOpResult::Fail;
			}
			if (!::SetSecurityDescriptorOwner(sd, tsid, groupDefaulted)) {
				SAFE_LOCALFREE(tsid);
				return FSOpResult::Fail;
			}
		} else {
			SAFE_LOCALFREE(sd);
			return FSOpResult::Fail;
		}
		if (!::SetFileSecurity(objectPath.c_str(), static_cast<unsigned long>(SecInfo::GroupSecInfo), sd)) {
			return FSOpResult::Fail;
		}
		SAFE_LOCALFREE(sd);
		SAFE_LOCALFREE(tsid);
	}
	return FSOpResult::Success;
}

FSOpResult FSHandler::EnumFolderContents(FolderRecord &folderInfo, const std::wstring folderPath,
	const bool getFileHashes, const HashType hashType, const bool getFileSize) {
	if (PathExists(folderPath)) {
		std::wstring seekpath = lower_copy(folderPath);
		if (endsWith(folderPath, L"\\")) {
			seekpath = removeFromEnd_copy(lower_copy(folderPath), L"\\");
		} else {
			seekpath = lower_copy(folderPath);
		}
		if (!startsWith(seekpath, fs_pathnolim)) {
			seekpath = fs_pathnolim + seekpath;
		}
		if (!endsWith(seekpath, fs_pathsall)) {
			seekpath = seekpath + fs_pathsall;
		}
		::WIN32_FIND_DATA fd;
		memset(&fd, 0, sizeof(::WIN32_FIND_DATA));
		::HANDLE hFind = FindFirstFile(seekpath.c_str(), &fd);
		if (INVALID_HANDLE_VALUE != hFind) {
			folderInfo.folderName = splitStr(lower_copy(folderPath), L"\\").back();
			folderInfo.folderPath = lower_copy(folderPath);
			do {
				removeFromEnd(seekpath, fs_pathsall);
				if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					if (wcscmp(fd.cFileName, L".") && wcscmp(fd.cFileName, L"..") &&
						wcscmp(fd.cFileName, L"System Volume Information") &&
						wcscmp(fd.cFileName, L"$Recycle.Bin")) {
						FolderRecord folderrec;
						folderrec.folderName = lower_copy(fd.cFileName);
						folderrec.folderPath = removeFromStart_copy(seekpath, fs_pathnolim) + L"\\" +
							lower_copy(fd.cFileName);
						if (FSOpResult::Success != EnumFolderContents(folderrec, folderrec.folderPath, getFileHashes,
							hashType, getFileSize)) {
							return FSOpResult::Fail;
						}
						folderInfo.folders.push_back(folderrec);
					}
				} else {
					FileRecord filerec;
					filerec.fileName = lower_copy(fd.cFileName);
					filerec.filePath = removeFromStart_copy(seekpath, fs_pathnolim) + L"\\" +
						lower_copy(fd.cFileName);
					if (getFileHashes) {
						filerec.hash = GetFileControlSum(filerec.filePath, hashType);
					}
					if (getFileSize) {
						if (FSOpResult::Success != GetFSize(filerec.size, filerec.filePath)) {
							return FSOpResult::Fail;
						}
					}
					folderInfo.files.push_back(filerec);
				}
			} while (FindNextFile(hFind, &fd));
			FindClose(hFind);
		}
	} else {
		return FSOpResult::Fail;
	}
	return FSOpResult::Success;
}

std::vector<FileRecord> FSHandler::SeekFile(const std::wstring filename,
	const bool getSize, const bool getControlSum,
	const bool excludeEmptyFiles, const HashType hash, const std::vector<std::wstring> exclusions,
	const std::vector<PartitionDesc>* parts) {
	std::vector<FileRecord> ret;
	std::vector<PartitionDesc> drives;
	if (parts) {
		drives = *parts;
	} else {
		if (FSOpResult::Success != EnumPartitions(drives)) {
			return ret;
		}
	}
	std::vector<std::wstring> exclLow = exclusions;
	for (size_t i = 0; i < exclLow.size(); ++i) {
		exclLow[i] = lower_copy(exclLow[i]);
		if (endsWith(exclLow[i], L"\\")) {
			removeFromEnd(exclLow[i], L"\\");
		}
	}
	std::basic_regex<wchar_t> fbr(prepRegexString_copy(filename));
	std::wstring path;
	std::wstring seekpath;
	std::wstring fnamelow = lower_copy(filename);
	::WIN32_FIND_DATA fd;
	::LARGE_INTEGER filesize;
	::HANDLE hFind = INVALID_HANDLE_VALUE;
	for (size_t i = 0; i < drives.size(); ++i) {
		memset(&fd, 0, sizeof(fd));
		if (endsWith(drives[i].partLetter, L"\\")) {
			path = removeFromEnd_copy(drives[i].partLetter, L"\\", false);
			seekpath = removeFromEnd_copy(lower_copy(drives[i].partLetter), L"\\");
		} else {
			path = drives[i].partLetter;
			seekpath = lower_copy(drives[i].partLetter);
		}
		if (!startsWith(seekpath, fs_pathnolim)) {
			seekpath = fs_pathnolim + seekpath;
		}
		if (!endsWith(seekpath, fs_pathsall)) {
			seekpath = seekpath + fs_pathsall;
		}
		std::wstring seekpathraw = removeFromEnd_copy(removeFromStart_copy(seekpath, fs_pathnolim), fs_pathsall);
		if (endsWith(seekpathraw, L"\\")) {
			removeFromEnd(seekpathraw, L"\\");
		}
		hFind = FindFirstFile(seekpath.c_str(), &fd);
		if (INVALID_HANDLE_VALUE != hFind) {
			do {
				removeFromEnd(seekpath, fs_pathsall);
				if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					if (wcscmp(fd.cFileName, L".") && wcscmp(fd.cFileName, L"..") &&
						wcscmp(fd.cFileName, L"System Volume Information") &&
						wcscmp(fd.cFileName, L"$Recycle.Bin")) {
						if (!valInList(exclLow, seekpathraw + L"\\" + lower_copy(std::wstring(fd.cFileName))) ||
							!valInList(exclLow, seekpathraw)) {
							if (!startsWith(seekpath, fs_pathnolim)) {
								seekpath = fs_pathnolim + seekpath;
							}
							std::vector<FileRecord> searchRes = SeekFileRecursive(seekpath + L"\\" +
								lower_copy(fd.cFileName) + fs_pathsall, path + L"\\" + std::wstring(fd.cFileName),
								fnamelow, fbr, getSize, getControlSum, excludeEmptyFiles,
								hash, &exclLow);
							ret.insert(ret.end(), searchRes.begin(), searchRes.end());
						}
					}
				} else {
					if (std::regex_match(fd.cFileName, fbr) ||
						std::regex_match(lower_copy(fd.cFileName), fbr) ||
						(fnamelow == lower_copy(fd.cFileName))) {
						if (!valInList(exclLow, seekpathraw + L"\\" + lower_copy(std::wstring(fd.cFileName))) ||
							!valInList(exclLow, seekpathraw)) {
							FileRecord searchres;
							searchres.fileName = lower_copy(fd.cFileName);
							searchres.filePath = path + L"\\" + fd.cFileName;
							if (getSize) {
								GetFSize(searchres.size, searchres.filePath);
							}
							if (getControlSum) {
								searchres.hash = GetFileControlSum(searchres.filePath, hash);
							}
							if (excludeEmptyFiles) {
								filesize.LowPart = fd.nFileSizeLow;
								filesize.HighPart = fd.nFileSizeHigh;
								if (filesize.QuadPart > 0) {
									ret.push_back(searchres);
								}
							} else {
								ret.push_back(searchres);
							}
						}
					}
				}
			} while (FindNextFile(hFind, &fd));
			FindClose(hFind);
		}
	}
	return ret;
}

std::vector<FileRecord> FSHandler::SeekFileInDir(const std::wstring startPath,
	const std::wstring filename, const std::wstring pathOrig, const bool getSize, const bool getControlSum,
	const bool excludeEmptyFiles, const HashType hash, const std::vector<std::wstring> &exclusions) {
	std::vector<FileRecord> ret;
	std::basic_regex<wchar_t> fbr(prepRegexString_copy(filename));
	std::wstring path;
	if (pathOrig.length()) {
		path = pathOrig;
	} else {
		path = startPath;
	}
	std::wstring fnamelow = lower_copy(filename);
	std::wstring seekpath = lower_copy(startPath);
	::WIN32_FIND_DATA fd;
	::LARGE_INTEGER filesize;
	::HANDLE hFind = INVALID_HANDLE_VALUE;
	memset(&fd, 0, sizeof(fd));
	if (!startsWith(seekpath, fs_pathnolim)) {
		seekpath = fs_pathnolim + seekpath;
	}
	if (!endsWith(seekpath, fs_pathsall)) {
		seekpath = seekpath + fs_pathsall;
	}
	std::wstring seekpathraw = removeFromEnd_copy(removeFromStart_copy(seekpath, fs_pathnolim), fs_pathsall);
	if (endsWith(seekpathraw, L"\\")) {
		removeFromEnd(seekpathraw, L"\\");
	}
	hFind = FindFirstFile(seekpath.c_str(), &fd);
	if (INVALID_HANDLE_VALUE != hFind) {
		do {
			removeFromEnd(seekpath, fs_pathsall);
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				if (wcscmp(fd.cFileName, L".") && wcscmp(fd.cFileName, L"..") &&
					wcscmp(fd.cFileName, L"System Volume Information") &&
					wcscmp(fd.cFileName, L"$Recycle.Bin") && wcscmp(fd.cFileName, L"$RECYCLE.BIN")) {
					if (!valInList(exclusions, seekpathraw + L"\\" + lower_copy(std::wstring(fd.cFileName))) ||
						!valInList(exclusions, seekpathraw)) {
						if (!startsWith(seekpath, fs_pathnolim)) {
							seekpath = fs_pathnolim + seekpath;
						}
						std::vector<FileRecord> searchRes = SeekFileInDir(seekpath + L"\\" + lower_copy(fd.cFileName) + fs_pathsall,
							fnamelow, path + L"\\" + std::wstring(fd.cFileName), getSize, getControlSum, excludeEmptyFiles, hash, exclusions);
						ret.insert(ret.end(), searchRes.begin(), searchRes.end());
					}
				}
			} else {
				if (std::regex_match(fd.cFileName, fbr) ||
					std::regex_match(lower_copy(fd.cFileName), fbr) ||
					(fnamelow == lower_copy(fd.cFileName))) {
					if (!valInList(exclusions, seekpathraw + L"\\" + lower_copy(std::wstring(fd.cFileName))) ||
						!valInList(exclusions, seekpathraw)) {
						FileRecord searchres;
						searchres.fileName = lower_copy(fd.cFileName);
						searchres.filePath = path + L"\\" + std::wstring(fd.cFileName);
						if (getSize) {
							GetFSize(searchres.size, searchres.filePath);
						}
						if (getControlSum) {
							searchres.hash = GetFileControlSum(searchres.filePath, hash);
						}
						if (excludeEmptyFiles) {
							filesize.LowPart = fd.nFileSizeLow;
							filesize.HighPart = fd.nFileSizeHigh;
							if (filesize.QuadPart > 0) {
								ret.push_back(searchres);
							}
						} else {
							ret.push_back(searchres);
						}
					}
				}
			}
		} while (FindNextFile(hFind, &fd));
		FindClose(hFind);
	}
	return ret;
}

std::vector<FileRecord> FSHandler::SeekFileRecursive(const std::wstring startPath, const std::wstring pathOrig,
	const std::wstring filename, std::basic_regex<wchar_t> searchRegex,
	const bool getSize, const bool getControlSum,
	const bool excludeEmptyFiles, const HashType hash,
	std::vector<std::wstring>* exclusions) {
	std::vector<FileRecord> ret;
	std::wstring path = pathOrig;
	std::wstring fnamelow = lower_copy(filename);
	std::wstring seekpath = lower_copy(startPath);
	::WIN32_FIND_DATA fd;
	::LARGE_INTEGER filesize;
	::HANDLE hFind = INVALID_HANDLE_VALUE;
	memset(&fd, 0, sizeof(fd));
	if (!startsWith(seekpath, fs_pathnolim)) {
		seekpath = fs_pathnolim + seekpath;
	}
	if (!endsWith(seekpath, fs_pathsall)) {
		seekpath = seekpath + fs_pathsall;
	}
	std::wstring seekpathraw = removeFromEnd_copy(removeFromStart_copy(seekpath, fs_pathnolim), fs_pathsall);
	if (endsWith(seekpathraw, L"\\")) {
		removeFromEnd(seekpathraw, L"\\");
	}
	hFind = FindFirstFile(seekpath.c_str(), &fd);
	if (INVALID_HANDLE_VALUE != hFind) {
		do {
		removeFromEnd(seekpath, fs_pathsall);
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				if (wcscmp(fd.cFileName, L".") && wcscmp(fd.cFileName, L"..") &&
					wcscmp(fd.cFileName, L"System Volume Information") &&
					wcscmp(fd.cFileName, L"$Recycle.Bin")) {
					if (!valInList(*exclusions, seekpathraw + L"\\" + lower_copy(std::wstring(fd.cFileName))) ||
						!valInList(*exclusions, seekpathraw)) {
						if (!startsWith(seekpath, fs_pathnolim)) {
							seekpath = fs_pathnolim + seekpath;
						}
						std::vector<FileRecord> searchRes = SeekFileRecursive(seekpath + L"\\" + lower_copy(fd.cFileName) + fs_pathsall,
							path + L"\\" + std::wstring(fd.cFileName), fnamelow, searchRegex, getSize, getControlSum, excludeEmptyFiles, hash, exclusions);
						ret.insert(ret.end(), searchRes.begin(), searchRes.end());
					}
				}
			} else {
				if (std::regex_match(fd.cFileName, searchRegex) ||
					std::regex_match(lower_copy(fd.cFileName), searchRegex) ||
					(fnamelow == lower_copy(fd.cFileName))) {
					if (!valInList(*exclusions, seekpathraw + L"\\" + lower_copy(std::wstring(fd.cFileName))) ||
						!valInList(*exclusions, seekpathraw)) {
						FileRecord searchres;
						searchres.filePath = path + L"\\" + std::wstring(fd.cFileName);
						if (getSize) {
							GetFSize(searchres.size, searchres.filePath);
						}
						if (getControlSum) {
							searchres.hash = GetFileControlSum(searchres.filePath, hash);
						}
						if (excludeEmptyFiles) {
							filesize.LowPart = fd.nFileSizeLow;
							filesize.HighPart = fd.nFileSizeHigh;
							if (filesize.QuadPart > 0) {
								ret.push_back(searchres);
							}
						} else {
							ret.push_back(searchres);
						}
					}
				}
			}
		} while (FindNextFile(hFind, &fd));
		FindClose(hFind);
	}
	return ret;
}

std::wstring FSHandler::DrivePath2PartLetter(const std::wstring drivePath,
	const bool addSlash, const bool ignoreCase, std::vector<PartitionDesc> *parts) {
	std::vector<PartitionDesc> vec;
	if (parts) {
		vec = *parts;
	} else {
		if (FSOpResult::Fail == EnumPartitions(vec)) {
			return drivePath;
		}
	}
	for (size_t i = 0; i < vec.size(); ++i) {
		if (ignoreCase) {
			if ((lower_copy(vec[i].drivePath) == lower_copy(drivePath)) ||
				(lower_copy(vec[i].drivePath) == lower_copy(drivePath) + L"\\")) {
				return vec[i].partLetter;
			}
		} else {
			if ((vec[i].drivePath == drivePath) ||
				(vec[i].drivePath == drivePath + L"\\")) {
				return vec[i].partLetter;
			}
		}
	}
}

std::wstring FSHandler::ReplaceDrivePathWithPartLetter(const std::wstring path,
	const bool ignoreCase, std::vector<PartitionDesc> *parts) {
	std::vector<PartitionDesc> vec;
	if (parts) {
		vec = *parts;
	} else {
		if (FSOpResult::Fail == EnumPartitions(vec)) {
			return path;
		}
	}
	std::wstring ret = path;
	for (size_t i = 0; i < vec.size(); ++i) {
		if (ignoreCase) {
			if (startsWith(lower_copy(ret), lower_copy(vec[i].drivePath))) {
				if (endsWith(vec[i].partLetter, L"\\")) {
					std::wstring plr = removeSubstr_copy(vec[i].partLetter, L"\\");
					return replaceSubstr_copy(ret, vec[i].drivePath, plr);
				} else {
					return replaceSubstr_copy(ret, vec[i].drivePath, vec[i].partLetter);
				}
			}
		} else {
			if (startsWith(ret, vec[i].drivePath)) {
				if (endsWith(vec[i].partLetter, L"\\")) {
					std::wstring plr = removeSubstr_copy(vec[i].partLetter, L"\\", false);
					return replaceSubstr_copy(ret, vec[i].drivePath, plr, false);
				} else {
					return replaceSubstr_copy(ret, vec[i].drivePath, vec[i].partLetter, false);
				}
			}
		}
	}
	return ret;
}

FSOpResult FSHandler::GetFolderSizeOnDriveRec(unsigned long long &folderSize, const std::wstring folderPath,
	::HMODULE ntDLLModule) {
	if (!PathExists(folderPath)) {
		return FSOpResult::Fail;
	}
	bool isfld = false;
	if (FSOpResult::Success == IsFolder(isfld, folderPath)) {
		if (!isfld) {
			return FSOpResult::Fail;
		}
	} else {
		return FSOpResult::Fail;
	}
	pRtlInitUnicodeString = (NTSTATUS(WINAPI*)(UnicodeString*, wchar_t*))
		::GetProcAddress(ntDLLModule, "RtlInitUnicodeString");
	pNtCreateFile = (NTSTATUS(WINAPI*)(::HANDLE*, ACCESS_MASK, ObjectAttributes*, IOStatusBlock*, LARGE_INTEGER*,
		unsigned long, unsigned long, unsigned long, unsigned long, void*, unsigned long))
		::GetProcAddress(ntDLLModule, "NtCreateFile");
	pNtCreateEvent = (NTSTATUS(WINAPI*)(::HANDLE*, ACCESS_MASK, ObjectAttributes*, EvtType, unsigned char))
		::GetProcAddress(ntDLLModule, "NtCreateEvent");
	pNtQueryDirectoryFile = (NTSTATUS(WINAPI*)(::HANDLE, ::HANDLE, PIO_APC_ROUTINE, void*, IOStatusBlock*, void*,
		unsigned long, FolderInfo, unsigned char, UnicodeString*, unsigned char))
		::GetProcAddress(ntDLLModule, "NtQueryDirectoryFile");
	pNtWaitForSingleobject = (NTSTATUS(WINAPI*)(::HANDLE, unsigned char, LARGE_INTEGER*))
		::GetProcAddress(ntDLLModule, "NtWaitForSingleObject");
	pRtlUnicodeStringToAnsiString = (NTSTATUS(WINAPI*)(ANSIString*, UnicodeString*, unsigned char))
		::GetProcAddress(ntDLLModule, "RtlUnicodeStringToAnsiString");
	pNtClearEvent = (NTSTATUS(WINAPI*)(::HANDLE)) ::GetProcAddress(ntDLLModule, "NtClearEvent");
	pNtClose = (NTSTATUS(WINAPI*)(::HANDLE)) ::GetProcAddress(ntDLLModule, "NtClose");
	if (0 == pRtlInitUnicodeString) {
		return FSOpResult::Fail;
	}
	if (0 == pRtlUnicodeStringToAnsiString) {
		return FSOpResult::Fail;
	}
	if (0 == pNtCreateFile) {
		return FSOpResult::Fail;
	}
	if (0 == pNtCreateEvent) {
		return FSOpResult::Fail;
	}
	if (0 == pNtQueryDirectoryFile) {
		return FSOpResult::Fail;
	}
	if (0 == pNtWaitForSingleobject) {
		return FSOpResult::Fail;
	}
	if (0 == pNtClearEvent) {
		return FSOpResult::Fail;
	}
	if (0 == pNtClose) {
		return FSOpResult::Fail;
	}
	::HANDLE RootDirectoryHandle = 0;
	::HANDLE Event = 0;
	NTSTATUS ntStatus = STATUS_SUCCESS;
	UnicodeString RootDirectoryName;
	ANSIString as;
	ObjectAttributes RootDirectoryAttributes;
	IOStatusBlock IOSB;
	FileDirBothInformation* DirInformation;
	wchar_t* wszBuffer = (wchar_t*)malloc(64 * sizeof(wchar_t));
	if (!wszBuffer) {
		return FSOpResult::Fail;
	}
	wchar_t* Buffer = (wchar_t*)malloc(65536 * sizeof(wchar_t));
	if (!Buffer) {
		return FSOpResult::Fail;
	}
	wsprintf(wszBuffer, L"\\??\\%s\\", folderPath.c_str());
	ntStatus = ((pRtlInitUnicodeString)(&RootDirectoryName, wszBuffer));
	if (!NT_SUCCESS(ntStatus)) {
		SAFE_FREE(wszBuffer);
		SAFE_FREE(Buffer);
		return FSOpResult::Fail;
	}
	InitializeObjectAttributes(&RootDirectoryAttributes, &RootDirectoryName, OBJ_CASE_INSENSITIVE, 0, 0);
	ntStatus = ((pNtCreateFile)(&RootDirectoryHandle,
		GENERIC_READ,
		&RootDirectoryAttributes,
		&IOSB,
		0,
		FILE_ATTRIBUTE_DIRECTORY,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		FILE_OPEN,
		FILE_DIRECTORY_FILE,
		0, 0));
	if (!NT_SUCCESS(ntStatus)) {
		SAFE_FREE(wszBuffer);
		SAFE_FREE(Buffer);
		return FSOpResult::Fail;
	}
	ntStatus = ((pNtCreateEvent)(&Event, GENERIC_ALL, 0, EvtType::NotificationEvent, 0));
	if (!NT_SUCCESS(ntStatus)) {
		SAFE_FREE(wszBuffer);
		SAFE_FREE(Buffer);
		return FSOpResult::Fail;
	}
	ntStatus = ((pNtQueryDirectoryFile)(RootDirectoryHandle,
		Event, 0, 0,
		&IOSB,
		Buffer,
		65536 * sizeof(unsigned char),
		FolderInfo::FileDirectoryBothInformation,
		0, 0, 0));
	if (STATUS_PENDING == ntStatus) {
		ntStatus = ((pNtWaitForSingleobject)(Event, 1, 0));
	}
	if (!NT_SUCCESS(ntStatus)) {
		SAFE_FREE(wszBuffer);
		SAFE_FREE(Buffer);
		return FSOpResult::Fail;
	}
	DirInformation = (FileDirBothInformation*)Buffer;
	UnicodeString EntryName;
	while (true) {
		EntryName.MaximumLength = EntryName.Length = (unsigned short)DirInformation->FileNameLength;
		EntryName.Buffer = &DirInformation->FileName[0];
		((pRtlUnicodeStringToAnsiString)(&as, &EntryName, 1));
		if (DirInformation->FileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			// printf("Directory name: ");
			if (strcmp(as.Buffer, ".") && strcmp(as.Buffer, "..")) {
				wchar_t* tbuf = char2wchar(as.Buffer);
				std::wstring targpath = folderPath + L"\\" + tbuf;
				GetFolderSizeOnDriveRec(folderSize, targpath, ntDLLModule);
				SAFE_ARR_DELETE(tbuf);
			}
		} else {
			// printf("Filename: ");
			folderSize += DirInformation->AllocationSize.QuadPart;
		}
		// printf("%s\n", as.Buffer);
		// printf("Directory size: %lld\n", DirInformation->AllocationSize.QuadPart);
		if (!DirInformation->NextEntryOffset) {
			break;
		} else {
			DirInformation = (FileDirBothInformation*)(((unsigned char*)DirInformation) + DirInformation->NextEntryOffset);
		}
	}
	((pNtClose)(RootDirectoryHandle));
	ntStatus = ((pNtClearEvent)(Event));
	if (!NT_SUCCESS(ntStatus)) {
		SAFE_FREE(wszBuffer);
		SAFE_FREE(Buffer);
		return FSOpResult::Fail;
	}
	((pNtClose)(Event));
	SAFE_FREE(wszBuffer);
	SAFE_FREE(Buffer);
	return FSOpResult::Success;
}

bool FSHandler::GetDriveSpace(const std::wstring partLetter,
	unsigned long long &freeSpace, unsigned long long &totalSpace) {
	std::wstring partLetterMod = fs_pathnolim + partLetter;
	if (!endsWith(partLetterMod, L"\\")) {
		partLetterMod = partLetterMod + L"\\";
	}
	ULARGE_INTEGER fSpace = { 0 }, tSpace = { 0 }, fSpacePU = { 0 };
	if (::GetDiskFreeSpaceEx(partLetterMod.c_str(), &fSpacePU, &tSpace, &fSpace)) {
		freeSpace = fSpacePU.QuadPart;
		totalSpace = tSpace.QuadPart;
		return true;
	} else {
		return false;
	}
}

bool FSHandler::GetDriveSpace_DriveGeometry(const std::wstring partLetter, unsigned long long &totalSpace) const {
	std::wstring path = lower_copy(partLetter);
	if (endsWith(partLetter, L"\\")) {
		removeFromEnd(path, L"\\");
	}
	if (!startsWith(path, L"\\\\.\\")) {
		path = L"\\\\.\\" + path;
	}
	::HANDLE hDevice = ::CreateFile(path.c_str(),	// drive to open
		0,					// no access to the drive
		FILE_SHARE_READ |	// share mode
		FILE_SHARE_WRITE,
		0,					// default security attributes
		OPEN_ALWAYS,		// disposition
		0,					// file attributes
		NULL);				// do not copy file attributes
	if (INVALID_HANDLE_VALUE != hDevice) {
		unsigned long junk = 0;
		DISK_GEOMETRY pdg = { 0 };
		bool result = ::DeviceIoControl(hDevice,                // device to be queried
			IOCTL_DISK_GET_DRIVE_GEOMETRY,	// operation to perform
			0, 0,							// no input buffer
			&pdg, sizeof(DISK_GEOMETRY),    // output buffer
			&junk,							// # bytes returned
			(LPOVERLAPPED)0);
		::CloseHandle(hDevice);
		if (result) {
			totalSpace = pdg.Cylinders.QuadPart * (unsigned long)pdg.TracksPerCylinder *
				(unsigned long)pdg.SectorsPerTrack * (unsigned long)pdg.BytesPerSector;
		}
		return result;
	}
	return false;
}

std::wstring FSHandler::calcHash(const std::wstring filePath,
	const HashType hashType, const bool hashUCase) {
	std::wstring ret;
	HCRYPTPROV hProv = 0;
	if (CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
		HCRYPTPROV hHash = 0;
		if (::CryptCreateHash(hProv, (ALG_ID)hashType, 0, 0, &hHash)) {
			unsigned char* fileContent = File2Buf(filePath);
			if (fileContent) {
				unsigned long long fsize = 0;
				if (FSOpResult::Success != GetFSize(fsize, filePath)) {
					return L"";
				}
				if (::CryptHashData(hHash, static_cast<const unsigned char*>(fileContent), fsize, 0)) {
					unsigned long cbHashSize = 0, dwCount = sizeof(DWORD);
					if (::CryptGetHashParam(hHash, HP_HASHSIZE, (unsigned char*)&cbHashSize, &dwCount, 0)) {
						std::vector<unsigned char> buffer(cbHashSize);
						if (::CryptGetHashParam(hHash, HP_HASHVAL, reinterpret_cast<unsigned char*>(&buffer[0]), &cbHashSize, 0)) {
							std::ostringstream oss;
							for (std::vector<unsigned char>::const_iterator iter = buffer.begin(); iter != buffer.end(); ++iter) {
								oss.fill('0');
								oss.width(2);
								oss << std::hex << static_cast<const int>(*iter);
							}
							SAFE_ARR_DELETE(fileContent);
							::CryptDestroyHash(hHash);
							::CryptReleaseContext(hProv, 0);
							if (hashUCase) {
								return str2wstr(upper_copy(oss.str()));
							} else {
								return str2wstr(oss.str());
							}
						}
						SAFE_ARR_DELETE(fileContent);
						// HeapFree(fileContent, 0, 0);
						::CryptDestroyHash(hHash);
						::CryptReleaseContext(hProv, 0);
					}
					SAFE_ARR_DELETE(fileContent);
					// HeapFree(fileContent, 0, 0);
					::CryptDestroyHash(hHash);
					::CryptReleaseContext(hProv, 0);
				}
				::CryptDestroyHash(hHash);
				::CryptReleaseContext(hProv, 0);
			}
			::CryptReleaseContext(hProv, 0);
		}
		::CryptReleaseContext(hProv, 0);
	}
	return L"";
}

unsigned char* FSHandler::File2Buf(const std::wstring filePath) {
	unsigned long long fsize = 0;
	if (FSOpResult::Success != GetFSize(fsize, filePath)) {
		return 0;
	}
	if (fsize) {
		NEW_ARR_NULLIFY(readBuf, unsigned char, FS_READBUFSZ);
		if (readBuf) {
			NEW_ARR_NULLIFY(fileBuf, unsigned char, fsize);
			if (fileBuf) {
				::HANDLE hFile = ::CreateFile(filePath.c_str(), READ_CONTROL | FILE_READ_ATTRIBUTES, FILE_SHARE_READ,
					0, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 0);
				if (INVALID_HANDLE_VALUE != hFile) {
					unsigned long bytesRead = 0;
					unsigned long readBufPos = 0;
					while ((ReadFile(hFile, readBuf, FS_READBUFSZ, &bytesRead, 0)) &&
						(readBufPos < fsize)) {
						if (!bytesRead) {
							break;
						}
						memcpy(&fileBuf[readBufPos], readBuf, bytesRead * sizeof(unsigned char));
						readBufPos += bytesRead;
					}
					SAFE_ARR_DELETE(readBuf);
					return fileBuf;
				}
			} else {
				return 0;
			}
		} else {
			return 0;
		}
	}
	return 0;
}

FSOpResult FSHandler::getSHFileOpDesc(const unsigned long msgCode, std::wstring *msgStr) const {
	if (!msgStr || IsBadWritePtr(msgStr)) {
		return FSOpResult::Fail;
	}
	if (!msgCode) {
		*msgStr = L"Succesful operation";
	} else if (DE_SAMEFILE == msgCode) {
		*msgStr = L"The source and destination files are the same file";
	} else if (DE_MANYSRC1DEST == msgCode) {
		*msgStr = L"Multiple file paths were specified in the source buffer, but only one destination file path";
	} else if (DE_DIFFDIR == msgCode) {
		*msgStr = L"Rename operation was specified but the destination path is a different directory Use the move operation instead";
	} else if (DE_ROOTDIR == msgCode) {
		*msgStr = L"The source is a root directory, which cannot be moved or renamed";
	} else if (DE_OPCANCELLED == msgCode) {
		*msgStr = L"The operation was canceled by the user, or silently canceled if the appropriate flags were supplied to SHFileOperation";
	} else if (DE_DESTSUBTREE == msgCode) {
		*msgStr = L"The destination is a subtree of the source";
	} else if (DE_ACCESSDENIEDSRC == msgCode) {
		*msgStr = L"Security settings denied access to the source";
	} else if (DE_PATHTOODEEP == msgCode) {
		*msgStr = L"The source or destination path exceeded or would exceed MAX_PATH";
	} else if (DE_MANYDEST == msgCode) {
		*msgStr = L"The operation involved multiple destination paths, which can fail in the case of a move operation";
	} else if (DE_INVALIDFILES == msgCode) {
		*msgStr = L"The path in the source or destination or both was invalid";
	} else if (DE_DESTSAMETREE == msgCode) {
		*msgStr = L"The source and destination have the same parent folder";
	} else if (DE_FLDDESTISFILE == msgCode) {
		*msgStr = L"The destination path is an existing file";
	} else if (DE_FILEDESTISFLD == msgCode) {
		*msgStr = L"The destination path is an existing folder";
	} else if (DE_FILENAMETOOLONG == msgCode) {
		*msgStr = L"The name of the file exceeds MAX_PATH";
	} else if (DE_DEST_IS_CDROM == msgCode) {
		*msgStr = L"The destination is a read-only DVD, possibly unformatted";
	} else if (DE_DEST_IS_DVD == msgCode) {
		*msgStr = L"The destination is a writable CD-ROM, possibly unformatted";
	} else if (DE_DEST_IS_CDRECORD == msgCode) {
		*msgStr = L"The destination is a writable CD-ROM, possibly unformatted";
	} else if (DE_FILE_TOO_LARGE == msgCode) {
		*msgStr = L"The file involved in the operation is too large for the destination media or file system";
	} else if (DE_SRC_IS_DVD == msgCode) {
		*msgStr = L"The source is a read-only DVD, possibly unformatted";
	} else if (DE_SRC_IS_CDROM == msgCode) {
		*msgStr = L"The source is a read-only CD-ROM, possibly unformatted";
	} else if (DE_ERROR_MAX == msgCode) {
		*msgStr = L"MAX_PATH was exceeded during the operation";
	} else if (DE_UNKNOWN == msgCode) {
		// This error does not occur on Windows Vista and later
		*msgStr = L"An unknown error occurred. This is typically due to an invalid path in the source or destination";
	} else if (DE_ERRORONDEST == msgCode) {
		*msgStr = L"An unspecified error occurred on the destination";
	} else if (DE_CANTRENAME == msgCode) {
		*msgStr = L"Destination is a root directory and cannot be renamed";
	} else {
		*msgStr = L"Unknown code specifed";
	}
	return FSOpResult::Success;
}

FSOpResult FSHandler::attrAnalyzer(bool &isTrue, const unsigned long attr, const std::wstring path) const {
	if (PathExists(path)) {
		unsigned long fileattr = ::GetFileAttributes(path.c_str());
		isTrue = attr & fileattr;
		return FSOpResult::Success;
	} else {
		isTrue = false;
		return FSOpResult::Fail;
	}
}