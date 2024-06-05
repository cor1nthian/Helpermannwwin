#ifndef _REGHELPER_H
#define _REGHELPER_H

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

#ifdef _WIN64
	#define RIGHTMOD 0
	// #define RIGHTMOD KEY_WOW64_32KEY
#else
	// #define RIGHTMOD 0
	#define RIGHTMOD KEY_WOW64_64KEY
#endif

#define CLOSEKEY_NULLIFY(x) ::RegCloseKey(x); x = 0

#define SOFTWAREVERSION std::pair<std::wstring, std::wstring>

#define REG_MAX_KEY_LENGTH 255
#define REG_MAX_VALUE_NAME 4096
#define REG_READBUFSZ 1024

#define REGHELPERVALBEGIN HKEY keyHandle = { 0 }, rootKey = { 0 }; \
				std::wstring keyPath, valueName = valName; \
				std::vector<std::wstring> strSpl; \
				if (root) { \
					rootKey = *root; \
				} else { \
					rootKey = { 0 }; \
				} \
				prepHKEYKeyPathValueName(rootKey, rootKey, valName, keyPath, valueName);

#define REGHELPERKEYBEGIN HKEY keyHandle = { 0 }, rootKey = { 0 }; \
				std::wstring keyPath, valueName = valName; \
				std::vector<std::wstring> strSpl; \
				if (root) { \
					rootKey = *root; \
				} else { \
					rootKey = { 0 }; \
				} \
				prepHKEYKeyPath(rootKey, path, rootKey, keyPath);

#pragma warning(disable : 4244)
#pragma warning(disable : 4996)

#include <Windows.h>
#include <algorithm>
#include <iterator>
#include <regex>
#include "syshelper.h"
#include "strhelper.h"
#include "winerrhelper.h"
#include "prochelper.h"
#include "aclhelper.h"

const void* const gc_wc_emptyVal = (void*)L'Z';
const void* const gc_wc_incorrectVal = (void*)L'Y';

const void* const gc_ul_emptyVal = (void*)0xFFFFFF11;
const void* const gc_ul_incorrectVal = (void*)11111111;

const void* const gc_ull_emptyVal = (void*)0xFFFFFF22;
const void* const gc_ull_incorrectVal = (void*)22222222;

const wchar_t* const gc_loadKeyNamePrefix = L"Loaded Hive ";

// maximum menu items added
const unsigned long gc_maxMenuItems = 512;

enum class RegLoadTarget : unsigned char {
	None,
	LocalMachine,
	Users
};

enum class RegValType : unsigned long {
	None = REG_NONE,
	Str = REG_SZ,
	MultiStr = REG_MULTI_SZ,
	ExpandStr = REG_EXPAND_SZ,
	DWord = REG_DWORD,
	DWordLE = REG_DWORD_LITTLE_ENDIAN,
	DWordBE = REG_DWORD_BIG_ENDIAN,
	QWord = REG_QWORD,
	QWordLE = REG_QWORD_LITTLE_ENDIAN,
	Binary = REG_BINARY,
	ResourceList = REG_RESOURCE_LIST,
	FullResourceDesc = REG_FULL_RESOURCE_DESCRIPTOR,
	ResourceRequirementsList = REG_RESOURCE_REQUIREMENTS_LIST,
	Link = REG_LINK
};

enum class RegOpResult : unsigned char {
	Success,
	Fail
};

struct RegValDesc {
	RegValDesc();
	RegValDesc(const RegValDesc &other);
	~RegValDesc();
	void FreeData();
	RegValDesc& operator=(const RegValDesc& other) {
		size_t sz = 0;
		valType = other.valType;
		if (valType == RegValType::Str || valType == RegValType::MultiStr ||
			valType == RegValType::ExpandStr || valType == RegValType::Link) {
			sz = sizeof(wchar_t);
		} else if (valType == RegValType::DWord || valType == RegValType::DWordLE ||
			valType == RegValType::DWordBE) {
			sz = sizeof(unsigned long);
		} else if (valType == RegValType::QWord || valType == RegValType::QWordLE) {
			sz = sizeof(unsigned long long);
		} else if (other.valType == RegValType::Binary ||
			other.valType == RegValType::None) {
			sz = sizeof(unsigned char);
		}
		valDataSz = other.valDataSz;
		valData = { 0 };
		if (other.valDataSz && other.valData && !IsBadReadPtr(other.valData)) {
			if (other.valType == RegValType::Str || other.valType == RegValType::MultiStr ||
				other.valType == RegValType::ExpandStr || other.valType == RegValType::Link) {
				NEW_ARR_NULLIFY(valData, wchar_t, (valDataSz / sizeof(wchar_t)) * sizeof(char));
			} else if (other.valType == RegValType::DWord ||
				other.valType == RegValType::DWordLE ||
				other.valType == RegValType::DWordBE) {
				NEW_NULLIFY(valData, unsigned long, valDataSz / sizeof(unsigned long));
			} else if (other.valType == RegValType::QWord ||
				other.valType == RegValType::QWordLE) {
				NEW_NULLIFY(valData, unsigned long long, valDataSz / sizeof(unsigned long long));
			} else if (other.valType == RegValType::Binary) {
				NEW_ARR_NULLIFY(valData, unsigned char, valDataSz / sizeof(unsigned char));
			} else if (other.valType == RegValType::None) {
			}
			if (valData && other.valType != RegValType::None) {
				memcpy(valData, other.valData, valDataSz); // *sz);
			}
		}
		valPath = other.valPath;
		valName = other.valName;
		valDataHex = other.valDataHex;
		return *this;
	}
	bool operator==(const RegValDesc& other) const {
		bool bufeq = false, otheq = false;
		size_t sz = 0;
		if (valType == RegValType::Str || valType == RegValType::MultiStr ||
			valType == RegValType::ExpandStr || valType == RegValType::Link) {
			sz = sizeof(wchar_t);
		} else if (valType == RegValType::DWord || valType == RegValType::DWordLE ||
			valType == RegValType::DWordBE) {
			sz = sizeof(unsigned long);
		} else if (valType == RegValType::QWord || valType == RegValType::QWordLE) {
			sz = sizeof(unsigned long long);
		} else if (valType == RegValType::Binary || valType == RegValType::None) {
			sz = sizeof(unsigned char);
		}
		if (valData && other.valData) {
			if (memcmp(valData, other.valData, valDataSz)) {
				bufeq = false;
			} else {
				bufeq = true;
			}
		} else if((!valData && other.valData) || (valData && !other.valData)) {
			bufeq = false;
		} else if (!valData && !other.valData) {
			bufeq = true;
		}
		otheq = ((valPath == other.valPath &&
				lower_copy(valPath) == lower_copy(other.valPath)) &&
			(valName == other.valName &&
				lower_copy(valName) == lower_copy(other.valName)) &&
			(valDataHex == other.valDataHex &&
				lower_copy(valDataHex) == lower_copy(other.valDataHex)) &&
			valDataSz == other.valDataSz &&
			valType == other.valType);
		return (bufeq && otheq);
	}
	bool operator!=(const RegValDesc &other) const {
		bool bufneq = false, othneq = false;
		size_t sz = 0;
		if (valType == RegValType::Str || valType == RegValType::MultiStr ||
			valType == RegValType::ExpandStr || valType == RegValType::Link) {
			sz = sizeof(wchar_t);
		} else if (valType == RegValType::DWord || valType == RegValType::DWordLE ||
			valType == RegValType::DWordBE) {
			sz = sizeof(unsigned long);
		} else if (valType == RegValType::QWord || valType == RegValType::QWordLE) {
			sz = sizeof(unsigned long long);
		} else if (valType == RegValType::Binary || valType == RegValType::None) {
			sz = sizeof(unsigned char);
		}
		if (valData && other.valData) {
			if (memcmp(valData, other.valData, valDataSz)) {
				bufneq = true;
			} else {
				bufneq = false;
			}
		} else if ((!valData && other.valData) || (valData && !other.valData)) {
			bufneq = true;
		} else if (!valData && !other.valData) {
			bufneq = false;
		}
		othneq = ((valPath != other.valPath ||
				lower_copy(valPath) != lower_copy(other.valPath)) ||
			(valName != other.valName ||
				lower_copy(valName) != lower_copy(other.valName)) ||
			(valDataHex != other.valDataHex ||
				lower_copy(valDataHex) != lower_copy(other.valDataHex)) ||
			valDataSz != other.valDataSz ||
			valType != other.valType);
		return (bufneq || othneq);
	}
	RegValType valType;
	void* valData;
	unsigned long valDataSz;
	std::wstring valPath;
	std::wstring valName;
	std::wstring valDataHex;
};

struct RegKeyDesc {
	RegKeyDesc();
	RegKeyDesc(const RegKeyDesc &other);
	~RegKeyDesc();
	void FreeValues();
	RegKeyDesc& operator=(const RegKeyDesc &other) {
		keyPath = other.keyPath;
		keyName = other.keyName;
		keys = other.keys;
		values = other.values;
		return *this;
	}
	bool operator==(const RegKeyDesc &other) const {
		return ((keyPath == other.keyPath &&
				lower_copy(keyPath) == lower_copy(other.keyPath)) &&
			(keyName == other.keyName &&
				lower_copy(keyName) == lower_copy(other.keyName)) &&
			keys != other.keys &&
			values == other.values);
	}
	bool operator!=(const RegKeyDesc &other) const {
		return ((keyPath != other.keyPath ||
				lower_copy(keyPath) != lower_copy(other.keyPath)) ||
			(keyName != other.keyName ||
				lower_copy(keyName) != lower_copy(other.keyName)) ||
			keys != other.keys ||
			values != other.values);
	}
	std::wstring keyPath;
	std::wstring keyName;
	std::vector<RegKeyDesc> keys;
	std::vector<RegValDesc> values;
};

class RegHandler {
	public:
		RegHandler();
		RegHandler(const RegHandler &other);
		~RegHandler();
		RegOpResult GetStrVal(const std::wstring valName, std::wstring &val,
			const bool cleanupString = true, const HKEY *root = 0) const;
		RegOpResult GetDWordVal(const std::wstring valName, unsigned long &val,
			const HKEY *root = 0) const;
		RegOpResult GetQWordVal(const std::wstring valName, unsigned long long &val,
			const HKEY *root = 0) const;
		RegOpResult GetBinaryVal(const std::wstring valName, unsigned char* &buf,
			unsigned long &bufSz, const HKEY *root = 0) const;
		RegOpResult GetMultiStrVal(const std::wstring valName, wchar_t* &buf,
			unsigned long &bufSz, const HKEY *root = 0) const;
		RegOpResult GetMultiStrVal(const std::wstring valName,
			std::vector<std::wstring> &val, const bool split = true,
			const wchar_t* splitter = L"\n", const HKEY *root = 0) const;
		RegOpResult GetResourceList(const std::wstring valName,
			unsigned long long &val, const HKEY *root = 0) const;
		RegOpResult GetVal(const std::wstring valName, unsigned char* &buf,
			unsigned long &bufSz, RegValType &valType, const HKEY *root = 0) const;
		RegOpResult GetValueType(const std::wstring valName, RegValType &valtype,
			const HKEY *root = 0) const;
		RegOpResult CreateStrVal(const std::wstring valName, const std::wstring& val,
			const HKEY *root = 0) const;
		RegOpResult CreateDWordVal(const std::wstring valName, const unsigned long& val,
			const HKEY *root = 0) const;
		RegOpResult CreateQWordVal(const std::wstring valName,
			const unsigned long long &val, const HKEY *root = 0) const;
		RegOpResult CreateBinaryVal(const std::wstring valName, unsigned char* &buf,
			unsigned long bufSz, const HKEY *root = 0) const;
		RegOpResult CreateMultiStrVal(const std::wstring valName,
			const std::vector<std::wstring> &val, const HKEY *root = 0) const;
		RegOpResult AddItem2ExplorerContextMenu(const std::wstring itemName,
			const std::wstring objectPath, const bool add2DirMenu = true, const bool add2DriveMenu = true,
			const HKEY *root = 0) const;
		RegOpResult RemoveItemFromExplorerContextMenu(const std::wstring itemName,
			const bool removeFromDirMenu = true, const bool removeFromDriveMenu = true,
			const HKEY *root = 0) const;
		RegOpResult AddItem2DesktopContextMenu(const std::wstring itemName, const std::wstring objectPath,
			const std::wstring subMenuName = L"My Menu", const std::wstring subCommandMenuName = L"MyMenu",
			const std::wstring subCommandMenuNameAdd = L".mycmd", const std::wstring menuIconPath = L"shell32.dll,43",
			const HKEY *root = 0) const;
		RegOpResult RemoveItemFromDesktopContextMenu(const std::wstring itemName,
			const std::wstring subMenuName = L"My Menu", const std::wstring subCommandMenuName = L"MyMenu",
			const std::wstring subCommandMenuNameAdd = L".mycmd", const HKEY *root = 0) const;
		RegOpResult GetSoftwareVersion(const std::wstring softwareName, std::vector<SOFTWAREVERSION> &versions,
			const bool uniqueValues = true, const bool anySoftware = true, const HKEY *root = 0) const;
		RegOpResult DeleteVal(const std::wstring valName, const HKEY *root = 0) const;
		RegOpResult CopyVal(const std::wstring source,
			const std::wstring destKeyName, const std::wstring destValName, const HKEY *root = 0) const;
		RegOpResult MoveVal(const std::wstring source,
			const std::wstring destKeyName, const std::wstring destValName,
			const HKEY *root = 0) const;
		RegOpResult SeekKey(const std::wstring keyName, std::vector<RegKeyDesc> &results,
			const std::wstring startPath = L"", const bool acquireVals = false,
			const unsigned long limitResults = 700, const bool cleanupStrings = true,
			const HKEY *root = 0) const;
		RegOpResult SeekVal2(const std::wstring valName, std::vector<RegValDesc> &results,
			const std::wstring startPath = L"", const bool acquireVals = false,
			const unsigned long limitResults = 700, const bool cleanupStrings = true,
			const HKEY *root = 0) const;
		RegOpResult SeekVal(const std::wstring valName, std::vector<RegValDesc> &results,
			const std::wstring startPath = L"", const bool acquireVals = false,
			const unsigned long limitResults = 700, const bool cleanupStrings = true,
			const HKEY *root = 0) const;
		RegOpResult SeekVal(const std::wstring valName, std::vector<RegValDesc>& results,
			const std::wstring startPath = L"", const RegValType valType = RegValType::Str,
			const bool acquireVals = false, const unsigned long limitResults = 700,
			const bool cleanupStrings = true, const HKEY *root = 0) const;
		RegOpResult AcquireValues(std::vector<RegKeyDesc> &keyList,
			const unsigned long limitVals = 700, const bool cleanupStrings = true,
			const HKEY *root = 0) const;
		RegOpResult AcquireValues(std::vector<RegValDesc> &valList,
			const unsigned long limitVals = 700, const bool cleanupStrings = true,
			const HKEY *root = 0) const;
		RegOpResult CreateValues(const std::vector<RegKeyDesc> &keyList, const HKEY *root = 0) const;
		RegOpResult CreateValues(const std::vector<RegValDesc> &valList, const HKEY *root = 0) const;
		RegOpResult DeleteValues(const std::vector<RegKeyDesc> &keyList, const HKEY *root = 0) const;
		RegOpResult DeleteValues(const std::vector<RegValDesc> &valList, const HKEY *root = 0) const;
		RegOpResult FreeValues(const std::vector<RegKeyDesc> &keyList, const HKEY *root = 0) const;
		RegOpResult FreeValues(const std::vector<RegValDesc> &valList, const HKEY *root = 0) const;
		RegOpResult GetKeySecurity(const std::wstring keyName, SecDesc &secDesc, const HKEY *root = 0) const;
		RegOpResult SetKeySecurity(const std::wstring keyName, SecDesc &secDesc, const HKEY* root = 0) const;
		RegOpResult CreateKey(const std::wstring keyName, const bool createMissingKeys = true,
			const HKEY *root = 0) const;
		RegOpResult DeleteKey(const std::wstring keyName, const bool deleteSubKeys = true,
			const HKEY *root = 0) const;
		RegOpResult CopyKey(const std::wstring source,
			const std::wstring destination, const bool copyVals = true,
			const HKEY *root = 0) const;
		RegOpResult MoveKey(const std::wstring source, const std::wstring destination,
			const bool moveValues = true, const HKEY *root = 0) const;
		RegOpResult EnumUserProfileHives(std::vector<std::wstring> &hiveList,
			const bool mountHives = true);
		RegOpResult MountHive_LoadKey(const std::wstring hivePath,
			const RegLoadTarget loadTarget = RegLoadTarget::Users);
		RegOpResult UnmountHive_UnloadKey(const std::wstring unloadKetName,
			const RegLoadTarget unloadTarget = RegLoadTarget::Users);
		RegOpResult ConnectRegistry(const std::wstring remoteComputerName);
		RegOpResult DisconnectRegistry(const HKEY connectedReg);
		unsigned long long ReadResourceList(const unsigned char* &resListBuf,
			const size_t bufSz) const;
		std::vector<std::wstring> MultiStrBuf2Vector(const wchar_t* multiStrBuf,
			const size_t bufSz) const;
	protected:
		
	private:
		/*       FUNCTIONS       */
		RegOpResult SeekKeyRec(const std::wstring keyName, std::vector<RegKeyDesc> &results,
			const std::wstring startPath = L"",	const HKEY *root = 0) const;
		RegOpResult SeekValRec(const std::wstring valName,
			std::vector<RegValDesc> &results, const std::wstring startPath = L"",
			const HKEY *root = 0) const;
		RegOpResult SeekValRecType(const std::wstring valName,
			std::vector<RegValDesc> &results, const RegValType valType = RegValType::Str,
			const std::wstring startPath = L"", const HKEY *root = 0) const;
		inline std::wstring hkey2wstr(const HKEY key) const;
		RegOpResult prepHKEYKeyPath(const HKEY &kayHandleSet,
			const std::wstring& keyPathSet, HKEY &kayHandle, std::wstring &keyPath) const;
		RegOpResult prepHKEYKeyPathValueName(HKEY& kayHandle, const HKEY &kayHandleSet,
			const std::wstring &valPath, std::wstring &keyPath,
			std::wstring &valName) const;
		RegOpResult assignHKEYKeyPath(const std::wstring keyPath, const HKEY &hkeySet,
			HKEY &tgtHKEY, std::wstring &tgtKeyPath) const;
		std::wstring rebuildSearchKeyPath(const HKEY &root, const std::wstring &origPath,
			const std::wstring keyName) const;
		unsigned long getRigtMod(const std::wstring keyPath) const;
		inline std::wstring pickLoadKey(const std::wstring hivePath) const;
		inline HKEY pickLoadTarget(const RegLoadTarget loadTarget = RegLoadTarget::None) const;
		HKEY pickUnloadKey(const std::wstring loadTarget) const;
		RegLoadTarget pickUnloadKey(const HKEY tgtHKEY) const;
		std::wstring pickMenuModNum(const std::wstring subMenuKey, const HKEY *root = 0) const;
		/*       VARIABLES       */
		size_t m_mountedHiveCount;
		std::map<std::wstring, HKEY> m_mountedHives;
		std::map<std::wstring, HKEY> m_connectedRegs;
};

#endif // _REGHELPER_H