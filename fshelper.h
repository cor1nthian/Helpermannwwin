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

#include <Windows.h>
#include <Wincrypt.h>
#include <winioctl.h>
#include <fileapi.h>
#include <shellapi.h>
#include <WinIoCtl.h>
#include <Ntddscsi.h>
#include <regex>
#include <sstream>
#include <vector>
#include "config.h"
#include "strhelper.h"
#include "aclhelper.h"
#include "prochelper.h"
#include "winerrhelper.h"
#include "HWGetter.h"

// File system operation result
enum class FSOpResult : unsigned char {
	Success,
	Fail
};

enum class HeapOpts : unsigned long {
	NoOpts = 0,
	EnableExecution = HEAP_CREATE_ENABLE_EXECUTE,
	GenerateExceptions = HEAP_GENERATE_EXCEPTIONS,
	NoSerialize = HEAP_NO_SERIALIZE
};

enum class TextFileEnc : unsigned char {
	UTF8,
	UTF16LE
};

enum class DevType : unsigned char {
	Unknown = 0,
	NoRootDir,
	Removable,
	Fixed,
	Remote,
	CDROM,
	RAMDisk
};

enum class RenameBehaviour : unsigned char {
	Rename,
	CopyDelete
};

// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-ntqueryinformationfile
enum class FileInfo : unsigned char {
	FileDirectoryInformation = 1,
	FileFullDirectoryInformation,                   // 2
	FileDirectoryBothInformation,                   // 3
	FileBasicInformation,                           // 4
	FileStandardInformation,                        // 5
	FileInternalInformation,                        // 6
	FileEaInformation,                              // 7
	FileAccessInformation,                          // 8
	FileNameInformation,                            // 9
	FileRenameInformation,                          // 10
	FileLinkInformation,                            // 11
	FileNamesInformation,                           // 12
	FileDispositionInformation,                     // 13
	FilePositionInformation,                        // 14
	FileFullEaInformation,                          // 15
	FileModeInformation,                            // 16
	FileAlignmentInformation,                       // 17
	FileAllInformation,                             // 18
	FileAllocationInformation,                      // 19
	FileEndOfFileInformation,                       // 20
	FileAlternateNameInformation,                   // 21
	FileStreamInformation,                          // 22
	FilePipeInformation,                            // 23
	FilePipeLocalInformation,                       // 24
	FilePipeRemoteInformation,                      // 25
	FileMailslotQueryInformation,                   // 26
	FileMailslotSetInformation,                     // 27
	FileCompressionInformation,                     // 28
	FileObjectIdInformation,                        // 29
	FileCompletionInformation,                      // 30
	FileMoveClusterInformation,                     // 31
	FileQuotaInformation,                           // 32
	FileReparsePointInformation,                    // 33
	FileNetworkOpenInformation,                     // 34
	FileAttributeTagInformation,                    // 35
	FileTrackingInformation,                        // 36
	FileIdBothDirectoryInformation,                 // 37
	FileIdFullDirectoryInformation,                 // 38
	FileValidDataLengthInformation,                 // 39
	FileShortNameInformation,                       // 40
	FileIoCompletionNotificationInformation,        // 41
	FileIoStatusBlockRangeInformation,              // 42
	FileIoPriorityHintInformation,                  // 43
	FileSfioReserveInformation,                     // 44
	FileSfioVolumeInformation,                      // 45
	FileHardLinkInformation,                        // 46
	FileProcessIdsUsingFileInformation,             // 47
	FileNormalizedNameInformation,                  // 48
	FileNetworkPhysicalNameInformation,             // 49
	FileIdGlobalTxDirectoryInformation,             // 50
	FileIsRemoteDeviceInformation,                  // 51
	FileUnusedInformation,                          // 52
	FileNumaNodeInformation,                        // 53
	FileStandardLinkInformation,                    // 54
	FileRemoteProtocolInformation,                  // 55
	FileRenameInformationBypassAccessCheck,         // 56
	FileLinkInformationBypassAccessCheck,           // 57
	FileVolumeNameInformation,                      // 58
	FileIdInformation,                              // 59
	FileIdExtdDirectoryInformation,                 // 60
	FileReplaceCompletionInformation,               // 61
	FileHardLinkFullIdInformation,                  // 62
	FileIdExtdBothDirectoryInformation,             // 63
	FileDispositionInformationEx,                   // 64
	FileRenameInformationEx,                        // 65
	FileRenameInformationExBypassAccessCheck,       // 66
	FileDesiredStorageClassInformation,             // 67
	FileStatInformation,                            // 68
	FileMemoryPartitionInformation,                 // 69
	FileStatLxInformation,                          // 70
	FileCaseSensitiveInformation,                   // 71
	FileLinkInformationEx,                          // 72
	FileLinkInformationExBypassAccessCheck,         // 73
	FileStorageReserveIdInformation,                // 74
	FileCaseSensitiveInformationForceAccessCheck,   // 75
	FileKnownFolderInformation,                     // 76
	FileMaximumInformation							// 77
};

enum class FolderInfo : unsigned char {
	FileDirectoryInformation = 1,
	FileFullDirectoryInformation,					// 2
	FileDirectoryBothInformation,					// 3
	FileBasicInformation,							// 4
	FileStandardInformation,						// 5
	FileInternalInformation,						// 6
	FileEaInformation,								// 7
	FileAccessInformation,							// 8
	FileNameInformation,							// 9
	FileRenameInformation,							// 10
	FileLinkInformation,							// 11
	FileNamesInformation,							// 12
	FileDispositionInformation,						// 13
	FilePositionInformation,						// 14
	FileFullEaInformation,							// 15
	FileModeInformation,							// 16
	FileAlignmentInformation,						// 17
	FileAllInformation,								// 18
	FileAllocationInformation,						// 19
	FileEndOfFileInformation,						// 20
	FileAlternateNameInformation,					// 21
	FileStreamInformation,							// 22
	FilePipeInformation,							// 23
	FilePipeLocalInformation,						// 24
	FilePipeRemoteInformation,						// 25
	FileMailslotQueryInformation,					// 26
	FileMailslotSetInformation,						// 27
	FileCompressionInformation,						// 28
	FileObjectIdInformation,						// 29
	FileCompletionInformation,						// 30
	FileMoveClusterInformation,						// 31
	FileQuotaInformation,							// 32
	FileReparsePointInformation,					// 33
	FileNetworkOpenInformation,						// 34
	FileAttributeTagInformation,					// 35
	FileTrackingInformation,						// 36
	FileIdBothDirectoryInformation,					// 37
	FileIdFullDirectoryInformation,					// 38
	FileValidDataLengthInformation,					// 39
	FileShortNameInformation,						// 40
	FileMaximumInformation							// 41
};

enum class EvtType : unsigned char {
	NotificationEvent,
	SynchronizationEvent
};

enum class BinPlatform : unsigned char {
	DOS,
	OSdiv2,
	Unix,
	Windows,
	PlatformUnknown
};

enum class BinBitDepth : unsigned char {
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

const unsigned long const FS_READBUFSZ = 32768;

const wchar_t* const fs_pathseparator = L"\\";
const wchar_t* const fs_pathnolim = L"\\\\?\\";
const wchar_t* const fs_pathsall = L"\\*";

struct UnicodeString;
struct ANSIString;
struct ObjectAttributes;
struct IOStatusBlock;
struct FileDirBothInformation;
struct BinData;
struct FileRecord;
struct FolderRecord;
struct VolumeDiskExtents;
struct DiskExtents;
struct VolumeDesc;

class FSHandler;

typedef void(NTAPI* PIO_APC_ROUTINE) (IN void* ApcContext, IN IOStatusBlock* IoStatusBlock, IN unsigned long Reserved);

struct UnicodeString {
	UnicodeString();
	UnicodeString(const unsigned short length, const unsigned short maxlength);
	UnicodeString(const unsigned short length, const unsigned short maxlength, const wchar_t* buffer);
	UnicodeString(const UnicodeString& other);
#if (defined(STDVER) && STDVER >= 11 && STDVER != 98)
	UnicodeString(UnicodeString&& other) noexcept;
#endif
	~UnicodeString();
	UnicodeString& operator=(const UnicodeString& other);
	UnicodeString& operator=(UnicodeString&& other) noexcept;
	bool operator==(const UnicodeString& other) const;
	bool operator!=(const UnicodeString& other) const;
	unsigned short	Length;
	unsigned short	MaximumLength;
	wchar_t* Buffer;
};

struct ANSIString {
	ANSIString();
	ANSIString(const unsigned short length, const unsigned short maxlength);
	ANSIString(const unsigned short length, const unsigned short maxlength, const char* buffer);
	ANSIString(const ANSIString& other);
#if (defined(STDVER) && STDVER >= 11 && STDVER != 98)
	ANSIString(ANSIString&& other) noexcept;
#endif
	~ANSIString();
	ANSIString& operator=(const ANSIString& other);
#if (defined(STDVER) && STDVER >= 11 && STDVER != 98)
	ANSIString& operator=(ANSIString&& other) noexcept;
#endif
	bool operator==(const ANSIString& other) const;
	bool operator!=(const ANSIString& other) const;
	unsigned short	Length;
	unsigned short	MaximumLength;
	char* Buffer;
};

struct ObjectAttributes {
	ObjectAttributes();
	ObjectAttributes(const ObjectAttributes& other);
#if (defined(STDVER) && STDVER >= 11 && STDVER != 98)
	ObjectAttributes(ObjectAttributes&& other) noexcept;
#endif
	~ObjectAttributes();
	ObjectAttributes& operator=(const ObjectAttributes& other);
#if (defined(STDVER) && STDVER >= 11 && STDVER != 98)
	ObjectAttributes& operator=(ObjectAttributes&& other) noexcept;
#endif
	bool operator==(const ObjectAttributes& other) const;
	bool operator!=(const ObjectAttributes& other) const;
	unsigned long	uLength;
	::HANDLE		hRootDirectory;
	UnicodeString* pObjectName;
	unsigned long	uAttributes;
	void* pSecurityDescriptor;
	void* pSecurityQualityOfService;
};

struct IOStatusBlock {
	IOStatusBlock();
	IOStatusBlock(const NTSTATUS statuus, const unsigned long long info);
	IOStatusBlock(const IOStatusBlock& other);
#if (defined(STDVER) && STDVER >= 11 && STDVER != 98)
	IOStatusBlock(IOStatusBlock&& other) noexcept;
#endif
	~IOStatusBlock();
	IOStatusBlock& operator=(const IOStatusBlock& other);
#if (defined(STDVER) && STDVER >= 11 && STDVER != 98)
	IOStatusBlock& operator=(IOStatusBlock&& other);
#endif
	bool operator==(const IOStatusBlock& other) const;
	bool operator!=(const IOStatusBlock& other) const;
	union {
		NTSTATUS		Status;
		void* Pointer;
	};
	unsigned long long	Information;
};

struct FileDirBothInformation {
	FileDirBothInformation();
	FileDirBothInformation(const FileDirBothInformation& other);
#if (defined(STDVER) && STDVER >= 11 && STDVER != 98)
	FileDirBothInformation(FileDirBothInformation&& other) noexcept;
#endif
	~FileDirBothInformation();
	FileDirBothInformation& operator=(const FileDirBothInformation& other);
#if (defined(STDVER) && STDVER >= 11 && STDVER != 98)
	FileDirBothInformation& operator=(FileDirBothInformation&& other) noexcept;
#endif
	bool operator==(const FileDirBothInformation& other) const;
	bool operator!=(const FileDirBothInformation& other) const;
	unsigned long	NextEntryOffset;
	unsigned long	FileIndex;
	LARGE_INTEGER	CreationTime;
	LARGE_INTEGER	LastAccessTime;
	LARGE_INTEGER	LastWriteTime;
	LARGE_INTEGER	ChangeTime;
	LARGE_INTEGER	EndOfFile;
	LARGE_INTEGER	AllocationSize;
	unsigned long	FileAttributes;
	unsigned long	FileNameLength;
	unsigned long	EaSize;
	char			ShortNameLength;
	wchar_t			ShortName[FSH_SHORTNAMELENGTH];
	wchar_t			FileName[1];
};

struct BinData {
	BinData();
	BinData(const BinBitDepth bitDepth, const BinPlatform binPlatform);
	BinData(const BinData& other);
#if (defined(STDVER) && STDVER >= 11 && STDVER != 98)
	BinData(BinData&& other) noexcept;
#endif
	~BinData();
	BinData& operator=(const BinData& other);
#if (defined(STDVER) && STDVER >= 11 && STDVER != 98)
	BinData& operator=(BinData&& other) noexcept;
#endif
	bool operator==(const BinData& other) const;
	bool operator!=(const BinData& other) const;
	BinBitDepth BitDepth;
	BinPlatform Platform;
};

// File description struct
struct FileRecord {
	// File description struct constructor
	FileRecord();
	// File description struct copy constructor
	FileRecord(const FileRecord& other);
	// File description struct move constructor
#if (defined(STDVER) && STDVER >= 11 && STDVER != 98)
	FileRecord(FileRecord&& other) noexcept;
#endif
	// File description struct destructor
	~FileRecord();
	FileRecord& operator=(const FileRecord& other);
#if (defined(STDVER) && STDVER >= 11 && STDVER != 98)
	FileRecord& operator=(FileRecord&& other) noexcept;
#endif
	bool operator==(const FileRecord& other) const;
	bool operator!=(const FileRecord& other) const;
	// Name of a file
	std::wstring fileName;
	// Path to a file
	std::wstring filePath;
	// File hash string
	std::wstring hash;
	// File size
	unsigned long long size;
};

// Folder description struct
struct FolderRecord {
	// Folder description struct constructor
	FolderRecord();
	// Folder description struct copy constructor
	FolderRecord(const FolderRecord& other);
	// Folder description struct move constructor
#if (defined(STDVER) && STDVER >= 11 && STDVER != 98)
	FolderRecord(FolderRecord&& other) noexcept;
#endif
	// Folder description struct destructor
	~FolderRecord();
	// Folder description struct copy operator =
	FolderRecord& operator=(const FolderRecord& other);
	// Folder description struct move operator =
	FolderRecord& operator=(const FolderRecord&& other) noexcept;
	// Folder description struct operator ==
	bool operator==(const FolderRecord& other) const;
	// Folder description struct operator !=
	bool operator!=(const FolderRecord& other) const;
	// Variables / struct fields
	std::wstring folderName;
	std::wstring folderPath;
	std::vector<FileRecord> files;
	std::vector<FolderRecord> folders;
};

struct DriveDesc {
	DriveDesc();
	DriveDesc(const unsigned long long freespace, const unsigned long long totalspace, const std::wstring drivephyspath,
		const std::wstring drivepath);
	DriveDesc(const DriveDesc& other);
#if (defined(STDVER) && STDVER >= 11 && STDVER != 98)
	DriveDesc(DriveDesc&& other) noexcept;
#endif
	~DriveDesc();
	DriveDesc& operator=(const DriveDesc& other);
#if (defined(STDVER) && STDVER >= 11 && STDVER != 98)
	DriveDesc& operator=(DriveDesc&& other) noexcept;
#endif
	bool operator==(const DriveDesc& other) const;
	bool operator!=(const DriveDesc& other) const;
	unsigned long long spaceFree;
	unsigned long long spaceTotal;
	std::wstring drivePhysPath;
	std::wstring drivePath;
	std::vector<std::wstring> volumes;
	std::vector<std::wstring> partitions;
};

struct PartitionDesc {
	PartitionDesc();
	PartitionDesc(const PartitionDesc& other);
#if (defined(STDVER) && STDVER >= 11 && STDVER != 98)
	PartitionDesc(PartitionDesc&& other) noexcept;
#endif
	~PartitionDesc();
	PartitionDesc& operator=(const PartitionDesc& other);
#if (defined(STDVER) && STDVER >= 11 && STDVER != 98)
	PartitionDesc& operator=(PartitionDesc&& other) noexcept;
#endif
	bool operator==(const PartitionDesc& other) const;
	bool operator!=(const PartitionDesc& other) const;
	unsigned long long spaceFree;
	unsigned long long spaceTotal;
	// GUID string
	std::wstring partitionPath;
	std::wstring volumePath;
	std::vector<std::wstring> volumes;
	std::vector<std::wstring> drives;
};

// Volume description struct
struct VolumeDesc {
	// Volume description struct constructor
	VolumeDesc();
	// Volume description struct copy constructor
	VolumeDesc(const VolumeDesc& other);
	// Volume description struct move constructor
#if (defined(STDVER) && STDVER >= 11 && STDVER != 98)
	VolumeDesc(VolumeDesc&& other) noexcept;
#endif
	// Volume description struct destructor
	~VolumeDesc();
	VolumeDesc& operator=(const VolumeDesc& other);
#if (defined(STDVER) && STDVER >= 11 && STDVER != 98)
	VolumeDesc& operator=(VolumeDesc&& other) noexcept;
#endif
	bool operator==(const VolumeDesc& other) const;
	bool operator!=(const VolumeDesc& other) const;
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
	DevType deviceType;
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
	// DOS style path to a drive/partition/Volume
	std::wstring drivePath;
	// GUID string
	std::wstring volumePath;
	// Volume label
	std::wstring volumeLabel;
	// File system name
	std::wstring fsName;
	std::vector<std::wstring> physDrives;
};

// Handler class encapsulating file system functionality
class FSHandler {
public:
	// File system handler constructor
	FSHandler();
	// File system handler copy constructor disabled
#if (defined(STDVER) && STDVER >= 11 && STDVER != 98)
	FSHandler(const FSHandler& other) = delete;
#else
	FSHandler(const FSHandler& other) {}
#endif
	// File system handler move constructor disabled
#if (defined(STDVER) && STDVER >= 11 && STDVER != 98)
	FSHandler(FSHandler&& other) noexcept = delete;
#endif
	// File system handler operator = (copy) disabled
#if (defined(STDVER) && STDVER >= 11 && STDVER != 98)
	FSHandler& operator=(const FSHandler& other) = delete;
#else
	FSHandler& operator=(const FSHandler& other) {}
#endif
	// File system handler operator = (move) disabled
#if (defined(STDVER) && STDVER >= 11 && STDVER != 98)
	FSHandler& operator=(FSHandler&& other) noexcept = delete;
#endif
	// File system handler equality comparison disabled
#if (defined(STDVER) && STDVER >= 11 && STDVER != 98)
	bool operator==(const FSHandler& other) const = delete;
#else
	bool operator==(const FSHandler& other) {}
#endif
	// File system handler inequality comparison disabled
#if (defined(STDVER) && STDVER >= 11 && STDVER != 98)
	bool operator!=(const FSHandler& other) const = delete;
#else
	bool operator!=(const FSHandler& other) {}
#endif
	// File system handler destructor
	~FSHandler();
	/* Lists all available partitions, providing description for each one
		[out] partition description list/vector
		[in] [default - true] cleat given list before collecting the data
		Returns result code of the operation (enum value) */
	FSOpResult EnumVolumes(std::vector<VolumeDesc>& volumeList, const bool clearList = true);
	FSOpResult EnumPartitions(std::vector<PartitionDesc>& partList, const bool clearList = true);
	FSOpResult EnumDrives(std::vector<DriveDesc>& driveList, const bool clearList = true);
	FSOpResult GetBinaryFileInfo(const std::wstring binaryPath, BinData& binaryData) const;
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
	bool PathExists(const std::wstring path) const;
	FSOpResult IsFolder(bool& isFolder, const std::wstring path) const;
	FSOpResult IsTemporary(bool& isTemporary, const std::wstring path) const;
	FSOpResult IsNormal(bool& isNormal, const std::wstring path) const;
	FSOpResult IsArchive(bool& isArchive, const std::wstring path) const;
	FSOpResult IsCompressed(bool& isCompressed, const std::wstring path) const;
	FSOpResult IsHidden(bool& isHidden, const std::wstring path) const;
	FSOpResult IsEncrypted(bool& isEncrtpted, const std::wstring path) const;
	FSOpResult IsVirtual(bool& isVirtual, const std::wstring path) const;
	FSOpResult CreateFolder(const std::wstring folderPath) const;
	FSOpResult CreateFolder(const std::wstring folderPath, const SECURITY_ATTRIBUTES *secAttr = 0) const;
	FSOpResult CreateFolder(const std::wstring folderPath, const SecDesc secDesc) const;
	FSOpResult RemoveFolder_SHFileOp(const std::wstring folderPath, std::wstring *infoBuf = 0) const;
	FSOpResult CustomFileOp_SHFileOp(const std::wstring folderPath, const std::wstring folderPathDest,
		const unsigned long operation, const unsigned long opCode, std::wstring* infoBuf = 0) const;
	FSOpResult RemoveFolder(const std::wstring folderPath, const bool includeFiles = true);
	FSOpResult RenameFolder(const std::wstring folderPath, const std::wstring folderPathDest,
		const RenameBehaviour renameBehaviour = RenameBehaviour::CopyDelete);
	FSOpResult MoveFolder(const std::wstring folderPath, const std::wstring folderPathDest,
		const bool checkDestSpace = true);
	FSOpResult CopyFolder(const std::wstring folderPath, const std::wstring folderPathDest,
		const bool checkDestSpace = true);
	FSOpResult RenameFile(const std::wstring filePath, const std::wstring filePathDest,
		const RenameBehaviour renameBehaviour = RenameBehaviour::CopyDelete);
	FSOpResult FileMove(const std::wstring filePath, const std::wstring filePathDest,
		const bool checkDestSpace = true);
	FSOpResult FileCopy(const std::wstring filePath, const std::wstring filePathDest,
		const bool checkDestSpace = true);
	FSOpResult RemoveFile_SHFileOp(const std::wstring filePath, std::wstring* infoBuf = 0) const;
	FSOpResult RemoveFile(const std::wstring filePath) const;
	FSOpResult GetFolderSizeOnDrive_NtQueryDir(unsigned long long& folderSize, const std::wstring folderPath);
	FSOpResult GetFolderSize(unsigned long long& folderSize, const std::wstring folderPath);
	FSOpResult GetFSizeOnDrive_NtQueryDir(unsigned long long& fileSize, const std::wstring filePath) const;
	FSOpResult GetFSize(unsigned long long& fileSize, const std::wstring filePath) const;
	FSOpResult GetObjectSecurity(SecDesc &secDesc, const std::wstring objectPath) const;
	FSOpResult SetObjectSecurity(const SecDesc secDesc, const std::wstring objectPath) const;
	FSOpResult WriteToTextFile(const std::wstring textFilename, const std::wstring data,
		const TextFileEnc fileEncpding = TextFileEnc::UTF8);
	FSOpResult EnumFolderContents(FolderRecord& folderInfo, const std::wstring folderPath,
		const bool getFileHashes = true, const HashType hashType = HashType::SHA256, const bool getFileSize = true);
	FSOpResult GetDriveSpace(const std::wstring partLetter, unsigned long long& freeSpace,
		unsigned long long& totalSpace);
	FSOpResult GetDriveSpace_DriveGeometry(const std::wstring partLetter, unsigned long long& totalSpace) const;
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
		const std::vector<VolumeDesc>* parts = 0);
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
		const std::vector<std::wstring>& exclusions = std::vector<std::wstring>());
	/* Modifies given drive path to a matching partition letter
		Param:
		[in] drive path to modify
		[in] [default - true] do the modification ignoring letter case
		[in] [default - true] add backslash to partition letter
		[in] [default - 0] pointer to partition list on which data replacement is based
		Returns partition letter matching given drive path */
	std::wstring DrivePath2PartLetter(const std::wstring drivePath,
		const bool ignoreCase = true, const bool addSlash = true,
		std::vector<VolumeDesc>* parts = 0);
	/* Replaces drive path with partition letter in string
		Param:
		[in] drive path to replace
		[in] [default - true] do the replacement ignoring letter case
		[in] [default - 0] pointer to partition list on which data replacement is based
		Returns modified string with dtive path replaced with a partition letter */
	std::wstring ReplaceDrivePathWithPartLetter(const std::wstring path,
		const bool ignoreCase = true, std::vector<VolumeDesc>* parts = 0);
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
	std::vector<FileRecord> SeekFileRecursive(const std::wstring startPath, const std::wstring pathOrig,
		const std::wstring filename, std::basic_regex<wchar_t> searchRegex, const bool getSize = true,
		const bool getControlSum = true, const bool excludeEmptyFiles = false,
		const HashType hash = HashType::SHA256, std::vector<std::wstring>* exclusions = 0);
	FSOpResult GetFolderSizeOnDriveRec(unsigned long long& folderSize, const std::wstring folderPath,
		::HMODULE ntDLLModule);
	std::wstring calcHash(const std::wstring filePath,
		const HashType hashType, const bool hashUCase = true);
	unsigned char* File2Buf(const std::wstring filePath);
	FSOpResult getSHFileOpDesc(const unsigned long msgCode, std::wstring* msgStr) const;
	FSOpResult attrAnalyzer(bool& isTrue, const unsigned long attr, const std::wstring path) const;
};

#endif // _FS_HELPER_H