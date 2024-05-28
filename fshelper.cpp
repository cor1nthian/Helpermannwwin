#include <iostream>
#include "fshelper.h"

const unsigned long REG_READBUFSZ = 32768;

const wchar_t* fs_pathseparator = L"\\";
const wchar_t* fs_pathnolim = L"\\\\?\\";
const wchar_t* fs_pathsall = L"\\*";

BinData::BinData() {
	Platform = BinPlatform::DOS;
	BitDepth = BinBitDepth::X16;
}

BinData::BinData(const BinData &other) {
	Platform = other.Platform;
	BitDepth = other.BitDepth;
}

BinData::~BinData() {}

FileRecord::FileRecord() {
	size = 0;
}

FileRecord::FileRecord(const FileRecord& other) {
	filePath = other.filePath;
	hash = other.hash;
	size = other.size;
}

FileRecord::~FileRecord() {}

FolderRecord::FolderRecord() {}

FolderRecord::FolderRecord(const FolderRecord &other) {
	folderPath = other.folderPath;
	files = other.files;
	folders = other.folders;
}

FolderRecord::~FolderRecord() {}

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

PartitionDesc::PartitionDesc(const PartitionDesc& other) {
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

PartitionDesc::~PartitionDesc() {}

FSHandler::FSHandler() {
	m_hHeap = 0;
}

FSHandler::FSHandler(const FSHandler& other) {
	m_hHeap = other.m_hHeap;
}

FSHandler::~FSHandler() {
	DestroyHeap();
}

PartsOpResult FSHandler::EnumPartitions(std::vector<PartitionDesc>& partList,
	const bool clearList) {
	unsigned long psz = 0, psn = 0;
	wchar_t plBuf[4] = { 0 };
	wchar_t drivePathBuf[MAX_PATH + 1] = { 0 };
	wchar_t vpnBuf[MAX_PATH + 1] = { 0 };
	std::vector<PartitionDesc> ret;
	PartitionDesc elem;
	// HANDLE hPart = FindFirstVolume(vol_name, ARRAYSIZE(vol_name))
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
		return PartsOpResult::Success;
	} else {
		return PartsOpResult::Fail;
	}
}

PartsOpResult FSHandler::CreateHeap(const unsigned long long initSz, const unsigned long long maxSz, HeapOpts heapOpts) {
	if (!m_hHeap) {
		m_hHeap = HeapCreate((unsigned long)heapOpts, initSz, maxSz);
		if (m_hHeap) {
			return PartsOpResult::Success;
		} else {
			return PartsOpResult::Fail;
		}
	}
	return PartsOpResult::Fail;
}

PartsOpResult FSHandler::DestroyHeap() {
	if (m_hHeap && INVALID_HANDLE_VALUE != m_hHeap) {
		if (HeapDestroy(m_hHeap)) {
			m_hHeap = 0;
			return PartsOpResult::Success;
		} else {
			return PartsOpResult::Fail;
		}
	}
	return PartsOpResult::Fail;
}

unsigned long long FSHandler::GetFSize(const std::wstring filePath) {
	LARGE_INTEGER li = { 0 };
	unsigned long fsizelow = 0, fsizehigh = 0;
	HANDLE hFile = ::CreateFile(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hFile && INVALID_HANDLE_VALUE != hFile) {
		fsizelow = ::GetFileSize(hFile, &fsizehigh);
		li.LowPart = fsizelow;
		li.HighPart = fsizehigh;
		::CloseHandle(hFile);
		return li.QuadPart;
	} else {
		return 0;
	}
}

PartsOpResult FSHandler::GetPhysDriveIndexByPartLetter(const std::wstring partLetter,
	unsigned long &driveIndex, std::vector<PartitionDesc> *parts) {
	std::vector<PartitionDesc> vec;
	if (parts) {
		vec = *parts;
	} else {
		if (PartsOpResult::Success != EnumPartitions(vec)) {
			return PartsOpResult::Fail;
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
			return PartsOpResult::Success;
		}
	}
	return PartsOpResult::Success;
}

PartsOpResult FSHandler::GetBinaryFileInfo(const std::wstring binaryPath, BinData &binaryData) const {
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
			return PartsOpResult::Success;
		}
	}
	return PartsOpResult::Fail;
}

std::wstring FSHandler::GetFileControlSum(const std::wstring filePath, const HashType sumType) {
	if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(filePath.c_str())) {
		return L"";
	} else {
		return calcHash(filePath, sumType);
	}
}

bool FSHandler::PathExists(const std::wstring path) {
	return (INVALID_FILE_ATTRIBUTES != GetFileAttributes(path.c_str()));
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
		if (PartsOpResult::Success != EnumPartitions(drives)) {
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
	WIN32_FIND_DATA fd;
	LARGE_INTEGER filesize;
	HANDLE hFind = INVALID_HANDLE_VALUE;
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
							std::vector<FileRecord> searchRes = SeekFileRecursive(seekpath + L"\\" + lower_copy(fd.cFileName) + fs_pathsall,
								path + L"\\" + std::wstring(fd.cFileName), fnamelow, fbr,
								getSize, getControlSum, excludeEmptyFiles, hash, &exclLow);
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
							searchres.filePath = path + L"\\" + fd.cFileName;
							if (getSize) {
								searchres.size = GetFSize(searchres.filePath);
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
	WIN32_FIND_DATA fd;
	LARGE_INTEGER filesize;
	HANDLE hFind = INVALID_HANDLE_VALUE;
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
						searchres.filePath = path + L"\\" + std::wstring(fd.cFileName);
						if (getSize) {
							searchres.size = GetFSize(searchres.filePath);
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
	WIN32_FIND_DATA fd;
	LARGE_INTEGER filesize;
	HANDLE hFind = INVALID_HANDLE_VALUE;
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
							searchres.size = GetFSize(searchres.filePath);
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
		if (PartsOpResult::Fail == EnumPartitions(vec)) {
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
		if (PartsOpResult::Fail == EnumPartitions(vec)) {
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

bool FSHandler::GetDriveSpace(const std::wstring partLetter,
	unsigned long long &freeSpace, unsigned long long &totalSpace) {
	ULARGE_INTEGER fSpace = { 0 },
		tSpace = { 0 }, fSpacePU = { 0 };
	if (GetDiskFreeSpaceEx(partLetter.c_str(), &fSpacePU,
		&tSpace, &fSpace)) {
		freeSpace = fSpace.QuadPart;
		totalSpace = tSpace.QuadPart;
	}
	std::wstring path = lower_copy(partLetter);
	if (endsWith(partLetter, L"\\")) {
		removeFromEnd(path, L"\\");
	}
	if (!startsWith(path, L"\\\\.\\")) {
		path = L"\\\\.\\" + path;
	}
	HANDLE hDevice = CreateFile(path.c_str(),	    // drive to open
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
		bool result = DeviceIoControl(hDevice,                  // device to be queried
								IOCTL_DISK_GET_DRIVE_GEOMETRY,	// operation to perform
								NULL, 0,						// no input buffer
								&pdg, sizeof(DISK_GEOMETRY),    // output buffer
								&junk,							// # bytes returned
								(LPOVERLAPPED)NULL);	
		CloseHandle(hDevice);
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
		if (CryptCreateHash(hProv, (ALG_ID)hashType, 0, 0, &hHash)) {
			unsigned char* fileContent = File2Buf(filePath);
			if (fileContent) {
				if (CryptHashData(hHash, static_cast<const unsigned char*>(fileContent), GetFSize(filePath), 0)) {
					unsigned long cbHashSize = 0, dwCount = sizeof(DWORD);
					if (CryptGetHashParam(hHash, HP_HASHSIZE, (unsigned char*)&cbHashSize, &dwCount, 0)) {
						std::vector<unsigned char> buffer(cbHashSize);
						if (CryptGetHashParam(hHash, HP_HASHVAL, reinterpret_cast<unsigned char*>(&buffer[0]), &cbHashSize, 0)) {
							std::ostringstream oss;
							for (std::vector<unsigned char>::const_iterator iter = buffer.begin(); iter != buffer.end(); ++iter) {
								oss.fill('0');
								oss.width(2);
								oss << std::hex << static_cast<const int>(*iter);
							}
							HeapFree(fileContent, 0, 0);
							CryptDestroyHash(hHash);
							CryptReleaseContext(hProv, 0);
							if (hashUCase) {
								return str2wstr(upper_copy(oss.str()));
							} else {
								return str2wstr(oss.str());
							}
						}
						HeapFree(fileContent, 0, 0);
						CryptDestroyHash(hHash);
						CryptReleaseContext(hProv, 0);
					}
					HeapFree(fileContent, 0, 0);
					CryptDestroyHash(hHash);
					CryptReleaseContext(hProv, 0);
				}
				CryptDestroyHash(hHash);
				CryptReleaseContext(hProv, 0);
			}
			CryptReleaseContext(hProv, 0);
		}
		CryptReleaseContext(hProv, 0);
	}
	return L"";
}

unsigned char* FSHandler::File2Buf(const std::wstring filePath) {
	unsigned long long fsize = GetFSize(filePath);
	if (fsize) {
		CreateHeap();
		if (m_hHeap) {
			unsigned char* readBuf = (unsigned char*)HeapAlloc(m_hHeap, 0, REG_READBUFSZ * sizeof(unsigned char));
			if (readBuf) {
				unsigned char* fileBuf = (unsigned char*)HeapAlloc(GetProcessHeap(), 0, fsize * sizeof(unsigned char));
				if (fileBuf) {
					HANDLE hFile = CreateFile(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ,
						NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
					if (INVALID_HANDLE_VALUE != hFile) {
						unsigned long bytesRead = 0;
						unsigned long readBufPos = 0;
						while ((ReadFile(hFile, readBuf, REG_READBUFSZ, &bytesRead, 0)) &&
							(readBufPos < fsize)) {
							if (!bytesRead) {
								break;
							}
							memcpy(&fileBuf[readBufPos], readBuf, bytesRead * sizeof(unsigned char));
							readBufPos += bytesRead;
						}
						HeapFree(readBuf, 0, 0);
						readBuf = 0;
						return fileBuf;
					}
				} else {
					return 0;
				}
			} else {
				return 0;
			}
		}
	}
}