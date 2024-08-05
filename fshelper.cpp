#include <iostream>
#include "fshelper.h"

const unsigned long REG_READBUFSZ = 32768;

const wchar_t* fs_pathseparator = L"\\";
const wchar_t* fs_pathnolim = L"\\\\?\\";
const wchar_t* fs_pathsall = L"\\*";

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

PartsOpResult FSHandler::EnumPartitions(std::vector<PartitionDesc>& partList,
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
		return PartsOpResult::Success;
	} else {
		return PartsOpResult::Fail;
	}
}

unsigned long long FSHandler::GetFSize(const std::wstring filePath) {
	LARGE_INTEGER li = { 0 };
	unsigned long fsizelow = 0, fsizehigh = 0;
	::HANDLE hFile = ::CreateFile(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
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

bool FSHandler::PathExists(const std::wstring path) const {
	return (INVALID_FILE_ATTRIBUTES != ::GetFileAttributes(path.c_str()));
}

PartsOpResult FSHandler::CreateFolder(const std::wstring folderPath) const {
	if (!PathExists(folderPath)) {
		if (CreateDirectory(folderPath.c_str(), 0)) {
			return PartsOpResult::Success;
		} else {
			return PartsOpResult::Fail;
		}
	} else {
		return PartsOpResult::Fail;
	}
}

PartsOpResult FSHandler::CreateFolder(const std::wstring folderPath, const SECURITY_ATTRIBUTES *secAttr) const {
	if (!PathExists(folderPath)) {
		if (CreateDirectory(folderPath.c_str(), (SECURITY_ATTRIBUTES*)secAttr)) {
			return PartsOpResult::Success;
		} else {
			return PartsOpResult::Fail;
		}
	} else {
		return PartsOpResult::Fail;
	}
}

PartsOpResult FSHandler::CreateFolder(const std::wstring folderPath, const SecDesc secDesc) const {
	if (!PathExists(folderPath)) {
		if (!CreateDirectory(folderPath.c_str(), 0)) {
			return PartsOpResult::Fail;
		}
	}
	if (PartsOpResult::Success == SetObjectSecurity(secDesc, folderPath)) {
		return PartsOpResult::Success;
	} else {
		return PartsOpResult::Fail;
	}
}

PartsOpResult FSHandler::RemoveFolder_SHFileOp(const std::wstring folderPath) const {
	size_t folderPathLen = wcslen_c(folderPath.c_str()) + 2;
	wchar_t* folderPathBuf = (wchar_t*)malloc(folderPathLen * sizeof(wchar_t));
	if (!folderPathBuf) {
		return PartsOpResult::Fail;
	}
	memset(folderPathBuf, 0, folderPathLen * sizeof(wchar_t));
	wsprintf(folderPathBuf, L"%s", folderPath.c_str());
	SHFILEOPSTRUCT fileOp = {
		0,
		FO_DELETE,
		folderPathBuf,
		L"",
		FOF_NOCONFIRMATION |
		FOF_NOERRORUI |
		FOF_SILENT,
		false,
		0,
		L"" };
	unsigned long opres = SHFileOperation(&fileOp);
	SAFE_FREE(folderPathBuf);
	if (!opres) {
		return PartsOpResult::Success;
	} else {
		return PartsOpResult::Fail;
	}
}

PartsOpResult FSHandler::RemoveFolder(const std::wstring folderPath, const bool includeFiles) {
	std::vector<FileRecord> folderFiles = SeekFileInDir(folderPath, L".*", L"", false, false);
	if (folderFiles.size()) {
		if (includeFiles) {
			for (size_t i = 0; i < folderFiles.size(); ++i) {
				if (PartsOpResult::Success != RemoveFile(folderFiles[i].filePath)) {
					return PartsOpResult::Fail;
				}
			}
		} else {
			return PartsOpResult::Fail;
		}
	}
	if (::RemoveDirectory(folderPath.c_str())) {
		return PartsOpResult::Success;
	} else {
		return PartsOpResult::Fail;
	}
}

PartsOpResult FSHandler::RemoveFile(const std::wstring filePath) const {
	if (::DeleteFile(filePath.c_str())) {
		return PartsOpResult::Success;
	} else {
		return PartsOpResult::Fail;
	}
}

PartsOpResult FSHandler::GetObjectSecurity(SecDesc &secDesc, const std::wstring objectPath) const {
	if (!PathExists(objectPath)) {
		return PartsOpResult::Fail;
	}
	ProcessHandler proc;
	unsigned long procid = proc.GetCurrentProcPid();
	std::vector<std::wstring> privs = proc.GetProcPrivileges(procid);
	if (!valInList(privs, L"SeSecurityPrivilege")) {
		if (!proc.EnableSecurityPrivilege(procid)) {
			return PartsOpResult::Fail;
		}
	}
	ACLHandler aclh;
	SysHandler sys;
	unsigned long secinfolen = 0;
	if (!::GetFileSecurity(objectPath.c_str(), static_cast<unsigned long>(SecInfo::DACLSecInfo), secDesc.daclInfo,
		secinfolen, &secinfolen)) {
		if (ERROR_INSUFFICIENT_BUFFER == getLastErrorCode()) {
			SECURITY_DESCRIPTOR* tsd = (SECURITY_DESCRIPTOR*)::LocalAlloc(LPTR, secinfolen);
			if (!tsd) {
				return PartsOpResult::Fail;
			}
			secDesc.daclInfo = ::LocalAlloc(LPTR, secinfolen);
			if (secDesc.daclInfo) {
				if (!::GetFileSecurity(objectPath.c_str(), static_cast<unsigned long>(SecInfo::DACLSecInfo),
					tsd, secinfolen, &secinfolen)) {
					SAFE_LOCALFREE(secDesc.daclInfo);
					return PartsOpResult::Fail;
				}
				memcpy(secDesc.daclInfo, tsd, secinfolen);
				SAFE_LOCALFREE(tsd);
				secDesc.daclInfoSz = secinfolen;
			} else {
				return PartsOpResult::Fail;
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
					return PartsOpResult::Fail;
				}
				secDesc.saclInfoSz = secinfolen;
			} else {
				return PartsOpResult::Fail;
			}
		}
	}
	secinfolen = 0;
	SECURITY_DESCRIPTOR* tsd = (SECURITY_DESCRIPTOR*)::LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
	if (!tsd) {
		return PartsOpResult::Fail;
	}
	if (!::GetFileSecurity(objectPath.c_str(), static_cast<unsigned long>(SecInfo::OwnerSecInfo), tsd, secinfolen,
		&secinfolen)) {
		if (ERROR_INSUFFICIENT_BUFFER == getLastErrorCode()) {
			if (!::GetFileSecurity(objectPath.c_str(),static_cast<unsigned long>(SecInfo::OwnerSecInfo),
				tsd, secinfolen, &secinfolen)) {
				SAFE_LOCALFREE(tsd);
				return PartsOpResult::Fail;
			}
			PSID tsid = 0;
			if (ACLOpResult::Success != aclh.OwnerSIDFromSecurityDescriptor(tsd, tsid)) {
				return PartsOpResult::Fail;
			}
			secDesc.ownerInfo = sys.StrSIDFromSID(tsid);
			// std::wcout << L"xxx " << sys.GetAccountNameFromStrSID(secDesc.ownerInfo) << std::endl;
			secDesc.ownerInfoSz = secDesc.ownerInfo.length() * sizeof(wchar_t);
		}
	}
	SAFE_LOCALFREE(tsd);
	secinfolen = 0;
	tsd = (SECURITY_DESCRIPTOR*)::LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
	if (!tsd) {
		return PartsOpResult::Fail;
	}
	if (!::GetFileSecurity(objectPath.c_str(), static_cast<unsigned long>(SecInfo::GroupSecInfo), tsd, secinfolen,
		&secinfolen)) {
		if (ERROR_INSUFFICIENT_BUFFER == getLastErrorCode()) {
			if (!::GetFileSecurity(objectPath.c_str(), static_cast<unsigned long>(SecInfo::GroupSecInfo),
				tsd, secinfolen, &secinfolen)) {
				SAFE_LOCALFREE(tsd);
				return PartsOpResult::Fail;
			}
			PSID tsid = 0;
			if (ACLOpResult::Success != aclh.PrimaryGroupSIDFromSecurityDescriptor(tsd, tsid)) {
				return PartsOpResult::Fail;
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
		return PartsOpResult::Fail;
	}
	return PartsOpResult::Success;
}

PartsOpResult FSHandler::SetObjectSecurity(const SecDesc secDesc, const std::wstring objectPath) const {
	if (!PathExists(objectPath)) {
		return PartsOpResult::Fail;
	}
	ProcessHandler proc;
	unsigned long procid = proc.GetCurrentProcPid();
	std::vector<std::wstring> privs = proc.GetProcPrivileges(procid);
	if (!valInList(privs, L"SeBackupPrivilege")) {
		if (!proc.EnableBackupPrivilege(procid)) {
			return PartsOpResult::Fail;
		}
	}
	if (!valInList(privs, L"SeRestorePrivilege")) {
		if (!proc.EnableRestorePrivilege(procid)) {
			return PartsOpResult::Fail;
		}
	}
	if (!valInList(privs, L"SeSecurityPrivilege")) {
		if (!proc.EnableSecurityPrivilege(procid)) {
			return PartsOpResult::Fail;
		}
	}
	if (!valInList(privs, L"SeTakeOwnershipPrivilege")) {
		if (!proc.EnableTakeOwnershipPrivilege(procid)) {
			return PartsOpResult::Fail;
		}
	}
	ACLHandler aclh;
	if (secDesc.daclAbsInfo) {
		ACL* acllist = 0;
		if (ACLOpResult::Success != aclh.DACLFromSecurityDescriptor(
			(SECURITY_DESCRIPTOR*)secDesc.absoluteSDInfo, acllist)) {
			return PartsOpResult::Fail;
		}
		SECURITY_DESCRIPTOR* sd = (SECURITY_DESCRIPTOR*)::LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
		if (!sd) {
			SAFE_LOCALFREE(acllist);
			return PartsOpResult::Fail;
		}
		if (::InitializeSecurityDescriptor(sd, SECURITY_DESCRIPTOR_REVISION)) {
			if (!::SetSecurityDescriptorDacl(sd, true, acllist, false)) {
				SAFE_LOCALFREE(sd);
				SAFE_LOCALFREE(acllist);
				return PartsOpResult::Fail;
			}
		} else {
			SAFE_LOCALFREE(sd);
			SAFE_LOCALFREE(acllist);
			return PartsOpResult::Fail;
		}
		if (!::SetFileSecurity(objectPath.c_str(), static_cast<unsigned long>(SecInfo::DACLSecInfo), sd)) {
			SAFE_LOCALFREE(sd);
			SAFE_LOCALFREE(acllist);
			return PartsOpResult::Fail;
		}
		SAFE_LOCALFREE(sd);
		SAFE_LOCALFREE(acllist);
	}
	if (secDesc.saclAbsInfo) {
		ACL* acllist = 0;
		if (ACLOpResult::Success != aclh.SACLFromSecurityDescriptor((SECURITY_DESCRIPTOR*)secDesc.absoluteSDInfo,
			acllist)) {
			return PartsOpResult::Fail;
		}
		if (acllist) {
			SECURITY_DESCRIPTOR* sd = (SECURITY_DESCRIPTOR*)::LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
			if (!sd) {
				SAFE_LOCALFREE(acllist);
				return PartsOpResult::Fail;
			}
			if (::InitializeSecurityDescriptor(sd, SECURITY_DESCRIPTOR_REVISION)) {
				if (!::SetSecurityDescriptorSacl(sd, true, acllist, false)) {
					SAFE_LOCALFREE(sd);
					SAFE_LOCALFREE(acllist);
					return PartsOpResult::Fail;
				}
			} else {
				SAFE_LOCALFREE(sd);
				SAFE_LOCALFREE(acllist);
				return PartsOpResult::Fail;
			}
			if (!::SetFileSecurity(objectPath.c_str(), static_cast<unsigned long>(SecInfo::SACLSecInfo), sd)) {
				SAFE_LOCALFREE(sd);
				SAFE_LOCALFREE(acllist);
				return PartsOpResult::Fail;
			}
			SAFE_LOCALFREE(sd);
			SAFE_LOCALFREE(acllist);
		}
	}
	if (secDesc.ownerInfo.length()) {
		SECURITY_DESCRIPTOR* sd = (SECURITY_DESCRIPTOR*)::LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
		if (!sd) {
			return PartsOpResult::Fail;
		}
		PSID tsid = 0;
		if (::InitializeSecurityDescriptor(sd, SECURITY_DESCRIPTOR_REVISION)) {
			int ownerDefaulted = 0;
			SysHandler sys;
			tsid = sys.SIDFromStrSid(secDesc.ownerInfo);
			if (!tsid) {
				return PartsOpResult::Fail;
			}
			if (!::SetSecurityDescriptorOwner(sd, tsid, ownerDefaulted)) {
				SAFE_LOCALFREE(tsid);
				return PartsOpResult::Fail;
			}
		} else {
			SAFE_LOCALFREE(sd);
			return PartsOpResult::Fail;
		}
		if (!::SetFileSecurity(objectPath.c_str(), static_cast<unsigned long>(SecInfo::OwnerSecInfo), sd)) {
			return PartsOpResult::Fail;
		}
		SAFE_LOCALFREE(sd);
		SAFE_LOCALFREE(tsid);
	}
	if (secDesc.primaryGroupInfo.length()) {
		SECURITY_DESCRIPTOR* sd = (SECURITY_DESCRIPTOR*)::LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
		if (!sd) {
			return PartsOpResult::Fail;
		}
		PSID tsid = 0;
		if (::InitializeSecurityDescriptor(sd, SECURITY_DESCRIPTOR_REVISION)) {
			int groupDefaulted = 0;
			SysHandler sys;
			tsid = sys.SIDFromStrSid(secDesc.primaryGroupInfo);
			if (!tsid) {
				return PartsOpResult::Fail;
			}
			if (!::SetSecurityDescriptorOwner(sd, tsid, groupDefaulted)) {
				SAFE_LOCALFREE(tsid);
				return PartsOpResult::Fail;
			}
		} else {
			SAFE_LOCALFREE(sd);
			return PartsOpResult::Fail;
		}
		if (!::SetFileSecurity(objectPath.c_str(), static_cast<unsigned long>(SecInfo::GroupSecInfo), sd)) {
			return PartsOpResult::Fail;
		}
		SAFE_LOCALFREE(sd);
		SAFE_LOCALFREE(tsid);
	}
	return PartsOpResult::Success;
}

PartsOpResult FSHandler::EnumFolderContents(FolderRecord &folderInfo, const std::wstring folderPath,
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
		WIN32_FIND_DATA fd;
		memset(&fd, 0, sizeof(WIN32_FIND_DATA));
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
						if (PartsOpResult::Success != EnumFolderContents(folderrec, folderrec.folderPath)) {
							return PartsOpResult::Fail;
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
						filerec.size = GetFSize(filerec.filePath);
					}
					folderInfo.files.push_back(filerec);
				}
			} while (FindNextFile(hFind, &fd));
			FindClose(hFind);
		}
	} else {
		return PartsOpResult::Fail;
	}
	return PartsOpResult::Success;
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
	::HANDLE hDevice = CreateFile(path.c_str(),	    // drive to open
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
							SAFE_ARR_DELETE(fileContent);
							// HeapFree(fileContent, 0, 0);
							CryptDestroyHash(hHash);
							CryptReleaseContext(hProv, 0);
							if (hashUCase) {
								return str2wstr(upper_copy(oss.str()));
							} else {
								return str2wstr(oss.str());
							}
						}
						SAFE_ARR_DELETE(fileContent);
						// HeapFree(fileContent, 0, 0);
						CryptDestroyHash(hHash);
						CryptReleaseContext(hProv, 0);
					}
					SAFE_ARR_DELETE(fileContent);
					// HeapFree(fileContent, 0, 0);
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
		NEW_ARR_NULLIFY(readBuf, unsigned char, REG_READBUFSZ);
		if (readBuf) {
			NEW_ARR_NULLIFY(fileBuf, unsigned char, fsize);
			if (fileBuf) {
				::HANDLE hFile = CreateFile(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ,
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
					SAFE_ARR_DELETE(readBuf);
					/*HeapFree(readBuf, 0, 0);
					readBuf = 0;*/
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