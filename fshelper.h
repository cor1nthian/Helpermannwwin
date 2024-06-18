#ifndef _FS_HELPER_H
#define _FS_HELPER_H

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

#pragma warning(disable : 4244)
#pragma warning(disable : 4996)
#pragma comment(lib, "Kernel32.lib")

#include <Windows.h>
#include <Wincrypt.h>
#include <winioctl.h>
#include <fileapi.h>
#include <regex>
#include <sstream>
#include <vector>
#include "strhelper.h"
#include "aclhelper.h"
#include "prochelper.h"
#include "winerrhelper.h"

// File system operation result
enum class PartsOpResult : unsigned char {
	Success,
	Fail
};

enum class HeapOpts : unsigned long {
	NoOpts = 0,
	EnableExecution = HEAP_CREATE_ENABLE_EXECUTE,
	GenerateExceptions = HEAP_GENERATE_EXCEPTIONS,
	NoSerialize = HEAP_NO_SERIALIZE
};

enum class BinPlatform : unsigned char {
	DOS,
	OSdiv2,
	Unix,
	Windows,
	PlatformUnknown
};

enum class BinBitDepth {
	X16,
	X32,
	X64,
	BitDepthUnknown
};

// Hash/control sum types
enum class HashType : unsigned long {
	SHA1 = CALG_SHA1,
	MD5 = CALG_MD5,
	SHA256 = CALG_SHA_256
};

struct BinData {
	BinData();
	BinData(const BinData &other);
	~BinData();
	BinData& operator=(const BinData &other) {
		BitDepth = other.BitDepth;
		Platform = other.Platform;
	}
	bool operator==(const BinData &other) const {
		return (BitDepth == other.BitDepth &&
			Platform == other.Platform);
	}
	bool operator!=(const BinData &other) const {
		return (BitDepth != other.BitDepth ||
			Platform != other.Platform);
	}
	BinBitDepth BitDepth;
	BinPlatform Platform;
};

/* File description struct
Operators are defined in the header file,
constructors and destructor are defined in the source code file */
struct FileRecord {
	// File description struct constructor
	FileRecord();
	// File description struct copy constructor
	FileRecord(const FileRecord &other);
	// File description struct destructor
	~FileRecord();
	FileRecord& operator=(const FileRecord& other) {
		fileName = other.fileName;
		filePath = other.filePath;
		hash = other.hash;
		size = other.size;
		return *this;
	}
	bool operator==(const FileRecord& other) const {
		return (hash == other.hash &&
			size == other.size &&
#if defined (_WIN32) || defined (_WIN64)
			lower_copy(filePath) == lower_copy(other.filePath));
#else
			filePath == other.filePath);
#endif
	}
	bool operator!=(const FileRecord& other) const {
		return (hash != other.hash ||
			size != other.size ||
#if defined (_WIN32) || defined (_WIN64)
			lower_copy(filePath) != lower_copy(other.filePath));
#else
			filePath != other.filePath);
#endif
	}
	// Name of a file
	std::wstring fileName;
	// Path to a file
	std::wstring filePath;
	// File hash string
	std::wstring hash;
	// File size
	unsigned long long size;
};

/* Folder description struct
Operators are defined in the header file,
constructors and destructor are defined in the source code file */
struct FolderRecord {
	// Folder description struct constructor
	FolderRecord();
	// Folder description struct copy constructor
	FolderRecord(const FolderRecord &other);
	// Folder description struct destructor
	~FolderRecord();
	FolderRecord& operator=(const FolderRecord &other) {
		folderName = other.folderName;
		folderPath = other.folderPath;
		files = other.files;
		folders = other.folders;
		return *this;
	}
	bool operator==(const FolderRecord &other) const {
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
	}
	bool operator!=(const FolderRecord &other) const {
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
	}
	std::wstring folderName;
	std::wstring folderPath;
	std::vector<FileRecord> files;
	std::vector<FolderRecord> folders;
};

/* Partition description struct
Operators are defined in the header file,
constructors and destructor are defined in the source code file */
struct PartitionDesc {
	// Partition description struct constructor
	PartitionDesc();
	// Partition description struct copy constructor
	PartitionDesc(const PartitionDesc &other);
	// Partition description struct destructor
	~PartitionDesc();
	PartitionDesc& operator=(const PartitionDesc& other) {
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
		return *this;
	}
	bool operator==(const PartitionDesc &other) const {
		return ((partLetter == other.partLetter &&
				lower_copy(partLetter) == lower_copy(other.partLetter)) &&
			(drivePath == other.drivePath &&
				lower_copy(drivePath) == lower_copy(other.drivePath)) &&
			(volumePath == other.volumePath &&
				lower_copy(volumePath) == lower_copy(other.volumePath)) &&
			volumeSerial == other.volumeSerial &&
			(volumeSerialStr == other.volumeSerialStr &&
				lower_copy(volumeSerialStr) == lower_copy(other.volumeSerialStr)) &&
			(volumeLabel == other.volumeLabel &&
				lower_copy(volumeLabel) == lower_copy(other.volumeLabel)) &&
			(fsName == other.fsName &&
				lower_copy(fsName) == lower_copy(other.fsName)) &&
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
	}
	bool operator!=(const PartitionDesc& other) const {
		return ((partLetter != other.partLetter ||
				lower_copy(partLetter) != lower_copy(other.partLetter)) ||
			(drivePath != other.drivePath ||
				lower_copy(drivePath) != lower_copy(other.drivePath)) ||
			(volumePath != other.volumePath ||
				lower_copy(volumePath) != lower_copy(other.volumePath)) ||
			volumeSerial != other.volumeSerial ||
			(volumeSerialStr != other.volumeSerialStr ||
				lower_copy(volumeSerialStr) != lower_copy(other.volumeSerialStr)) ||
			(volumeLabel != other.volumeLabel ||
				lower_copy(volumeLabel) != lower_copy(other.volumeLabel)) ||
			(fsName != other.fsName ||
				lower_copy(fsName) != lower_copy(other.fsName)) ||
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
	}
	// True if volume supports case sensitive search
	bool caseSensitiveSearch;
	// True if volume supports case preserved names
	bool casePreservedMames;
	// True if volume supports Unicode filenames
	bool unicodeFNames;
	// True if volume supports persistent ACLs
	bool persistentACLS;
	// True if volume supports compression
	bool supportFileCompress;
	// True if volume supports quotas
	bool supportQuota;
	// True if volume supports sparse files
	bool supportSparseFile;
	// True if volume supports reparse points
	bool supportReparsePoints;
	// True if volume supports remote storage
	bool supportRemoteStorage;
	// True if volume supports POSIX names/renames
	bool supportPosixUnlinkRename;
	// True if volume supports object IDs
	bool supportObjectIds;
	// True if volume supports enctyption
	bool supportEncryption;
	// True if volume supports named streams
	bool supportNamedStreams;
	// True if volume supports transactions
	bool supportTransactions;
	// True if volume supports hard links
	bool supportHardLink;
	// True if volume supports extended attributes
	bool suportExtendAttrib;
	// True if volume supports open file id
	bool supportFileIdOpen;
	// True if volume supports USN journal
	bool supportUSNJournal;
	// True if volume supports integrity streams
	bool supportIntegrityStream;
	// True if volume supports block reference count
	bool supportBlockRefCount;
	// True if volume supports sparse VDL
	bool supportSparseVdl;
	// True if volume supports ghosting
	bool supportGhosting;
	// True if volume supports cleanup result info
	bool returnCleanupResInfo;
	// True if volume is compressed
	bool volumeCompressed;
	// True if volume is read only
	bool volumeReadOnly;
	// True if volume is DAX
	bool volumeDax;
	bool seqWriteOnce;
	// Free space on volume, bytes
	unsigned long long spaceFree;
	// Total space on volume, bytes
	unsigned long long spaceTotal;
	// Volume serial number
	unsigned long volumeSerial;
	unsigned long maxComponentLen;
	// Volume SN string
	std::wstring volumeSerialStr;
	// Partition letter
	std::wstring partLetter;
	// DOS style path to a drive/partition
	std::wstring drivePath;
	// GUID string
	std::wstring volumePath;
	// Volume label
	std::wstring volumeLabel;
	// File system name
	std::wstring fsName;
};

// Handler class encapsulating file system functionality
class FSHandler {
	public:
		// File system handler constructor
		FSHandler();
		// File system handler copy constructor
		FSHandler(const FSHandler &other);
		// File system handler destructor
		~FSHandler();
		/* Lists all available partitions, providing description for each one
			[out] partition description list/vector
			[in] [default - true] cleat given list before collecting the data
			Returns result code of the operation (enum value) */
		PartsOpResult EnumPartitions(std::vector<PartitionDesc> &partList, const bool clearList = true);
		/* Gets the size for a given file
			Param:
			[in] path to a file to calc comtrol sum for
			Returns control sum string */
		unsigned long long GetFSize(const std::wstring filePath);
		PartsOpResult GetPhysDriveIndexByPartLetter(const std::wstring partLetter,
			unsigned long &driveIndex, std::vector<PartitionDesc> *parts = 0);
		PartsOpResult GetBinaryFileInfo(const std::wstring binaryPath, BinData &binaryData) const;
		/* Gets the control sum for a given file
			Param:
			[in] path to a file to calc control sum for
			[in] [default - SHA256] type of control sum to get for a file. Available options : SHA1, SHA256, MD5
			Returns control sum string */
		std::wstring GetFileControlSum(const std::wstring filePath,
			const HashType sumType = HashType::SHA256);
		/* Checks if a given path exists
			Param:
			[in] string path to check
			Returns true if path exists, false otherwise */
		bool PathExists(const std::wstring path);
		PartsOpResult GetObjectSecurity(SecDesc &secDesc, const std::wstring objectPath) const;
		PartsOpResult SetObjectSecurity(SecDesc &secDesc, const std::wstring objectPath) const;
		PartsOpResult EnumFolderContents(FolderRecord& folderInfo,
			const std::wstring folderPath, const bool getFileHashes = true,
			const HashType hashType = HashType::SHA256, const bool getFileSize = true);
		/* Does the file search baaed on a filename on all available partitions. Filename supports regex expressions.
			Param:
			[in] filename to search. Supports regex.
			[in] [default - true] get file sizes for result list.
			[in] [default - true] get file control sums for result list
			[in] [default - false] exclude empty files from result list
			[in] [default - SHA256] type of control sum to get for a found file. Available options : SHA1, SHA256, MD5
			[in] [default - empty list] exclusion list of directories not used in search
			[in] [default - 0] pointer to partition list on which data search is based
			Returns a list of file records matching search criteria */
		std::vector<FileRecord> SeekFile(const std::wstring filename,
			const bool getSize = true, const bool getControlSum = true,
			const bool excludeEmptyFiles = false, const HashType hash = HashType::SHA256,
			const std::vector<std::wstring> exclusions = std::vector<std::wstring>(),
			const std::vector<PartitionDesc>* parts = 0);
		/* Does the file search starting in given folder baaed on a filename. Filename supports regex expressions.
			Param:
			[in] search start path
			[in] filename to search. Supports regex.
			[in] [default - empty string] original path. DO NOT MODIFY TO KEEP ORIGINAL PATH WITH ORIGINAL CASES CORRECT
			[in] [default - true] get file sizes for result list.
			[in] [default - true] get file control sums for result list
			[in] [default - false] exclude empty files from result list
			[in] [default - SHA256] type of control sum to get for a found file. Available options : SHA1, SHA256, MD5
			[in] [default - empty list] exclusion list of directories not used in search
			Returns a list/vector of file records matching search criteria */
		std::vector<FileRecord> SeekFileInDir(const std::wstring startPath,
			const std::wstring filename, const std::wstring pathOrig = L"",
			const bool getSize = true, const bool getControlSum = true,
			const bool excludeEmptyFiles = false, const HashType hash = HashType::SHA256,
			const std::vector<std::wstring> &exclusions = std::vector<std::wstring>());
		/* Modifies given drive path to a matching partition letter
			Param:
			[in] drive path to modify
			[in] [default - true] do the modification ignoring letter case
			[in] [default - true] add backslash to partition letter
			[in] [default - 0] pointer to partition list on which data replacement is based
			Returns partition letter matching given drive path */
		std::wstring DrivePath2PartLetter(const std::wstring drivePath,
			const bool ignoreCase = true, const bool addSlash = true,
			std::vector<PartitionDesc> *parts = 0);
		/* Replaces drive path with partition letter in string
			Param:
			[in] drive path to replace
			[in] [default - true] do the replacement ignoring letter case
			[in] [default - 0] pointer to partition list on which data replacement is based
			Returns modified string with dtive path replaced with a partition letter */
		std::wstring ReplaceDrivePathWithPartLetter(const std::wstring path,
			const bool ignoreCase = true, std::vector<PartitionDesc> *parts = 0);
	protected:
	private:
		/* *** INTERNAL USE ONLY ***
			File search recursive function
			Param:
			[in] path to start/continue search from
			[in] filename to search. Supports regex.
			[in] regex to use in search
			[in] [default - true] get file sizes for search results
			[in] [default - true] calc control sums for records in search results
			[in] [default - true] exclude empty files from search results
			[in] [default - SHA256] type of control sum to get for a found file. Available options : SHA1, SHA256, MD5
			[in] [default - 0] pointer to exclusion list of directories not used in search */
		std::vector<FileRecord> SeekFileRecursive(const std::wstring startPath,
			const std::wstring pathOrig, const std::wstring filename,
			std::basic_regex<wchar_t> searchRegex,
			const bool getSize = true, const bool getControlSum = true,
			const bool excludeEmptyFiles = false, const HashType hash = HashType::SHA256,
			std::vector<std::wstring> *exclusions = 0);
		bool GetDriveSpace(const std::wstring partLetter,
			unsigned long long &freeSpace, unsigned long long &totalSpace);
		std::wstring calcHash(const std::wstring filePath,
			const HashType hashType, const bool hashUCase = true);
		unsigned char* File2Buf(const std::wstring filePath);
};

#endif // _FS_HELPER_H