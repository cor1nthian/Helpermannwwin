#ifndef _SYSHELPER_H
#define _SYSHELPER_H

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

#include <Windows.h>
#include <accctrl.h>
#include <Sddl.h>
#include <lm.h>
#include <time.h>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <thread>
#include <unordered_map>
#include "strhelper.h"
#include "winerrhelper.h"

#pragma warning(disable : 4244)
#pragma warning(disable : 4996)
#pragma comment(lib, "netapi32.lib")

// malloc with ease
#define MALLOC_NULLIFY(x, type, sz) type* x = (type*)malloc(sz); if(x) { *x = { 0 }; }

// malloc with ease
#define MALLOC_NULLIFY_NO_REDEFINE(x, type, sz) x = (type*)malloc(sz); if(x) { *x = { 0 }; }

// malloc with ease
#define MALLOC_NULLIFY_NO_REDEFINE_NO_CONV(x, type, sz) x = malloc(sz); if(x) { *x = { 0 }; }

// new with ease
#define NEW_NULLIFY(x, type) type* x = 0; x = new type; if(x) { *x = { 0 }; }

// new with ease
#define NEW_ARR_NULLIFY_NO_REDEFINE(x, type, sz) x = new type[sz]; if(x) { *x = { 0 }; }

// new with ease
#define NEW_ARR_NULLIFY(x, type, sz) type* x = 0; x = new type[sz]; if(x) { *x = { 0 }; }

// new with ease
#define NEW_ARR_NULLIFY_ET(x, type, sz) type x = 0; x = new type[sz]; if(x) { *x = { 0 }; }

// safety first! no errors trying to at least delete this pointer further
#define SAFE_DELETE(x) delete x; x = 0

// safety first! no errors trying to at least delete this pointer further
#define SAFE_ARR_DELETE(x) delete[] x; x = 0

// safety first! no errors trying to at least free this pointer further
#define SAFE_FREE(x) free(x); x = 0

// safety first! no errors trying to at least free this pointer further
#define SAFE_NetApiBufferFree(x) NetApiBufferFree(x); x = 0

// safety first! no errors trying to at least free this pointer further
#define SAFE_LOCALFREE(x) LocalFree((HLOCAL)x); x =0

// can be used for short, unsigned short, word, unsigned word (2-byte types)
#define BYTESWAP16(n) (((n&0xFF00)>>8)|((n&0x00FF)<<8))

// can be used for int or unsigned int or float (4-byte types)
#define BYTESWAP32(n) ((BYTESWAP16((n&0xFFFF0000)>>16))|((BYTESWAP16(n&0x0000FFFF))<<16))

// can be used for unsigned long long or double (8-byte types)
#define BYTESWAP64(n) ((BYTESWAP32((n&0xFFFFFFFF00000000)>>32))|((BYTESWAP32(n&0x00000000FFFFFFFF))<<32))

// Check memory address access
const unsigned long const dwForbiddenArea = PAGE_GUARD | PAGE_NOACCESS;
const unsigned long const dwReadRights = PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;
const unsigned long const dwWriteRights = PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;

template<unsigned long dwAccessRights>
bool CheckAccess(void* pAddress, size_t nSize) {
	if (!pAddress || !nSize) {
		return false;
	}
	MEMORY_BASIC_INFORMATION sMBI;
	bool bRet = false;
	unsigned long long pCurrentAddress = (unsigned long long)pAddress;
	unsigned long long pEndAdress = pCurrentAddress + (nSize - 1);
	do {
		ZeroMemory(&sMBI, sizeof(sMBI));
		::VirtualQuery((const void*)pCurrentAddress, &sMBI, sizeof(sMBI));
		bRet = (sMBI.State & MEM_COMMIT)			// memory allocated and
			&& !(sMBI.Protect & dwForbiddenArea)	// access to page allowed and
			&& (sMBI.Protect & dwAccessRights);		// the required rights
		pCurrentAddress = ((unsigned long long)sMBI.BaseAddress + sMBI.RegionSize);
	} while (bRet && pCurrentAddress <= pEndAdress);
	return bRet;
}

#define IsBadWritePtrSz(p, n) (!CheckAccess<dwWriteRights>(p, n))
#define IsBadReadPtrSz(p, n) (!CheckAccess<dwReadRights>(p, n))
#define IsBadStringPtrWSz(p, n) (!CheckAccess<dwReadRights>(p, 2 * n))

typedef bool(__stdcall* LPFN_ISWOW64PROCESS) (HANDLE procHandle, int &result);

// constexpr to get TZ offset
static constexpr time_t const NULL_TIME = -1;

struct AccountDesc;
struct GroupDesc;
struct WKSid;

enum class SysOpResult : unsigned char {
	Success,
	Fail
};

enum class SysArch : unsigned char {
	X32,
	X64
};

enum class SidType : unsigned char {
	User = SID_NAME_USE::SidTypeUser,
	Group = SID_NAME_USE::SidTypeGroup,
	Domain = SID_NAME_USE::SidTypeDomain,
	Alias = SID_NAME_USE::SidTypeAlias,
	WellKnownGroup = SID_NAME_USE::SidTypeWellKnownGroup,
	DeletedAcc = SID_NAME_USE::SidTypeDeletedAccount,
	Invalid = SID_NAME_USE::SidTypeInvalid,
	Unknown = SID_NAME_USE::SidTypeUnknown,
	Computer = SID_NAME_USE::SidTypeComputer,
	Label = SID_NAME_USE::SidTypeLabel,
	LogonSeesion = SID_NAME_USE::SidTypeLogonSession,
	SidUnknown = 255
};

bool IsBadReadPtr(void* p);
bool IsBadWritePtr(void* p);
bool IsSIDWellKnown(const std::wstring strsid);
bool IsSIDWellKnown(const PSID sid);
std::vector<WKSid> GetWellKnownStrSIDs(PSID domainSID = 0);

const std::vector<WKSid> const gc_WellKnownStrSIDs = GetWellKnownStrSIDs();

struct WKSid {
	WKSid() {}
	WKSid(std::wstring strsid,
		std::wstring sidname, std::wstring sidnameorig) {
		StrSID = strsid;
		SIDName = sidname;
		SIDNameOrig = sidnameorig;
	}
	WKSid(const WKSid &other) {
		StrSID = other.StrSID;
		SIDName = other.SIDName;
		SIDNameOrig = other.SIDNameOrig;
	}
	WKSid& operator=(const WKSid &other) {
		StrSID = other.StrSID;
		SIDName = other.SIDName;
		SIDNameOrig = other.SIDNameOrig;
		return *this;
	}
	bool operator==(const WKSid &other) const {
		return (lower_copy(StrSID) == lower_copy(other.StrSID) &&
			lower_copy(SIDName) == lower_copy(other.SIDName) &&
			lower_copy(SIDNameOrig) == lower_copy(other.SIDNameOrig));
	}
	bool operator!=(const WKSid &other) const {
		return (lower_copy(StrSID) != lower_copy(other.StrSID) ||
			lower_copy(SIDName) != lower_copy(other.SIDName) ||
			lower_copy(SIDNameOrig) != lower_copy(other.SIDNameOrig));
	}
	~WKSid() {}
	std::wstring StrSID;
	std::wstring SIDName;
	std::wstring SIDNameOrig;
};

/* User account description struct
Operators are defined in the header file,
constructors and destructor are defined in the source code file */
struct AccountDesc {
	AccountDesc();
	AccountDesc(const AccountDesc &other);
	~AccountDesc();
	AccountDesc& operator=(const AccountDesc &other) {
		ncNormalAcc = other.ncNormalAcc;
		ncTempDupAcc = other.ncTempDupAcc;
		ncWorkstationTrustAcc = other.ncWorkstationTrustAcc;
		ncServerTrustAcc = other.ncServerTrustAcc;
		ncInterdomainTrustAcc = other.ncInterdomainTrustAcc;
		accountLocked = other.accountLocked;
		accountDisabled = other.accountDisabled;
		accountNotDelegated = other.accountNotDelegated;
		accountDESKeysOnly = other.accountDESKeysOnly;
		accountNoKerberosPreauth = other.accountNoKerberosPreauth;
		accountTrustedToAuthForDelegation = other.accountTrustedToAuthForDelegation;
		accountTrustedForDelegation = other.accountTrustedForDelegation;
		logonScriptExecuted = other.logonScriptExecuted;
		passwordNotNeeded = other.passwordNotNeeded;
		passwordCantChange = other.passwordCantChange;
		passwordNoExpire = other.passwordNoExpire;
		passwordReverseEncryption = other.passwordReverseEncryption;
		passwordSmartCardNeeded = other.passwordSmartCardNeeded;
		passwordExpired = other.passwordExpired;
		passwordEncrypted = other.passwordEncrypted;
		authPrintOp = other.authPrintOp;
		authCommOp = other.authCommOp;
		authSrvOp = other.authSrvOp;
		authAccOp = other.authAccOp;
		passwordAge = other.passwordAge;
		passwordBadCount = other.passwordBadCount;
		lastLogOn = other.lastLogOn;
		lastLogOff = other.lastLogOff;
		privileges = other.privileges;
		expireDate = other.expireDate;
		maxStorage = other.maxStorage;
		logonCount = other.logonCount;
		countryCode = other.countryCode;
		codePage = other.codePage;
		primaryGroupId = other.primaryGroupId;
		sid = other.sid;
		comment = other.comment;
		accountName = other.accountName;
		accountFullname = other.accountFullname;
		logonScriptPath = other.logonScriptPath;
		homeDirPath = other.homeDirPath;
		homeDirDrive = other.homeDirDrive;
		profileDirPath = other.profileDirPath;
		params = other.params;
		allowedWorkstations = other.allowedWorkstations;
		logonServer = other.logonServer;
		passwordSetStr = other.passwordSetStr;
		lastLogOnStr = other.lastLogOnStr;
		lastLogOffStr = other.lastLogOffStr;
		privilegesStr = other.privilegesStr;
		expireDateStr = other.expireDateStr;
		maxStorageStr = other.maxStorageStr;
		groups = other.groups;
		return *this;
	}
	bool operator==(const AccountDesc &other) const {
		return (ncNormalAcc == other.ncNormalAcc &&
			ncTempDupAcc == other.ncTempDupAcc &&
			ncWorkstationTrustAcc == other.ncWorkstationTrustAcc &&
			ncServerTrustAcc == other.ncServerTrustAcc &&
			ncInterdomainTrustAcc == other.ncInterdomainTrustAcc &&
			accountLocked == other.accountLocked &&
			accountDisabled == other.accountDisabled &&
			accountNotDelegated == other.accountNotDelegated &&
			accountDESKeysOnly == other.accountDESKeysOnly &&
			accountNoKerberosPreauth == other.accountNoKerberosPreauth &&
			accountTrustedToAuthForDelegation == other.accountTrustedToAuthForDelegation &&
			accountTrustedForDelegation == other.accountTrustedForDelegation &&
			logonScriptExecuted == other.logonScriptExecuted &&
			passwordNotNeeded == other.passwordNotNeeded &&
			passwordCantChange == other.passwordCantChange &&
			passwordNoExpire == other.passwordNoExpire &&
			passwordReverseEncryption == other.passwordReverseEncryption &&
			passwordSmartCardNeeded == other.passwordSmartCardNeeded &&
			passwordExpired == other.passwordExpired &&
			authPrintOp == other.authPrintOp &&
			authCommOp == other.authCommOp &&
			authSrvOp == other.authSrvOp &&
			authAccOp == other.authAccOp &&
			passwordEncrypted == other.passwordEncrypted &&
			passwordAge == other.passwordAge &&
			passwordBadCount == other.passwordBadCount &&
			lastLogOn == other.lastLogOn &&
			lastLogOff == other.lastLogOff &&
			privileges == other.privileges &&
			expireDate == other.expireDate &&
			maxStorage == other.maxStorage &&
			logonCount == other.logonCount &&
			countryCode == other.countryCode &&
			codePage == other.codePage &&
			primaryGroupId == other.primaryGroupId &&
			groups == other.groups &&
			(sid == other.sid &&
				lower_copy(sid) == lower_copy(other.sid)) &&
			(comment == other.comment &&
				lower_copy(comment) == lower_copy(other.comment)) &&
			(accountName == other.accountName &&
				lower_copy(accountName) == lower_copy(other.accountName)) &&
			(accountFullname == other.accountFullname &&
				lower_copy(accountFullname) == lower_copy(other.accountFullname)) &&
			(logonScriptPath == other.logonScriptPath &&
				lower_copy(logonScriptPath) == lower_copy(other.logonScriptPath)) &&
			(homeDirPath == other.homeDirPath &&
				lower_copy(homeDirPath) == lower_copy(other.homeDirPath)) &&
			(homeDirDrive == other.homeDirDrive &&
				lower_copy(homeDirDrive) == lower_copy(other.homeDirDrive)) &&
			(profileDirPath == other.profileDirPath &&
				lower_copy(profileDirPath) == lower_copy(other.profileDirPath)) &&
			(params == other.params &&
				lower_copy(params) == lower_copy(other.params)) &&
			(allowedWorkstations == other.allowedWorkstations &&
				lower_copy(allowedWorkstations) == lower_copy(other.allowedWorkstations)) &&
			(logonServer == other.logonServer &&
				lower_copy(logonServer) == lower_copy(other.logonServer)) &&
			(passwordSetStr == other.passwordSetStr &&
				lower_copy(passwordSetStr) == lower_copy(other.passwordSetStr)) &&
			(lastLogOnStr == other.lastLogOnStr &&
				lower_copy(lastLogOnStr) == lower_copy(other.lastLogOnStr)) &&
			(lastLogOffStr == other.lastLogOffStr &&
				lower_copy(lastLogOffStr) == lower_copy(other.lastLogOffStr)) &&
			(privilegesStr == other.privilegesStr &&
				lower_copy(privilegesStr) == lower_copy(other.privilegesStr)) &&
			(expireDateStr == other.expireDateStr &&
				lower_copy(expireDateStr) == lower_copy(other.expireDateStr)) &&
			(maxStorageStr == other.maxStorageStr &&
				lower_copy(maxStorageStr) == lower_copy(other.maxStorageStr)));
	}
	bool operator!=(const AccountDesc &other) const {
		return (ncNormalAcc != other.ncNormalAcc ||
			ncTempDupAcc != other.ncTempDupAcc ||
			ncWorkstationTrustAcc != other.ncWorkstationTrustAcc ||
			ncServerTrustAcc != other.ncServerTrustAcc ||
			ncInterdomainTrustAcc != other.ncInterdomainTrustAcc ||
			accountLocked != other.accountLocked ||
			accountDisabled != other.accountDisabled ||
			accountNotDelegated != other.accountNotDelegated ||
			accountDESKeysOnly != other.accountDESKeysOnly ||
			accountNoKerberosPreauth != other.accountNoKerberosPreauth ||
			accountTrustedToAuthForDelegation != other.accountTrustedToAuthForDelegation ||
			accountTrustedForDelegation != other.accountTrustedForDelegation ||
			logonScriptExecuted != other.logonScriptExecuted ||
			passwordNotNeeded != other.passwordNotNeeded ||
			passwordCantChange != other.passwordCantChange ||
			passwordNoExpire != other.passwordNoExpire ||
			passwordReverseEncryption != other.passwordReverseEncryption ||
			passwordSmartCardNeeded != other.passwordSmartCardNeeded ||
			passwordExpired != other.passwordExpired ||
			passwordEncrypted != other.passwordEncrypted ||
			authPrintOp != other.authPrintOp ||
			authCommOp != other.authCommOp ||
			authSrvOp != other.authSrvOp ||
			authAccOp != other.authAccOp ||
			passwordAge != other.passwordAge ||
			passwordBadCount != other.passwordBadCount ||
			lastLogOn != other.lastLogOn ||
			lastLogOff != other.lastLogOff ||
			privileges != other.privileges ||
			expireDate != other.expireDate ||
			maxStorage != other.maxStorage ||
			logonCount != other.logonCount ||
			countryCode != other.countryCode ||
			codePage != other.codePage ||
			primaryGroupId != other.primaryGroupId ||
			groups != other.groups ||
			(sid != other.sid ||
				lower_copy(sid) != lower_copy(other.sid)) ||
			(comment != other.comment ||
				lower_copy(comment) != lower_copy(other.comment)) ||
			(accountName != other.accountName ||
				lower_copy(accountName) != lower_copy(other.accountName)) ||
			(accountFullname != other.accountFullname ||
				lower_copy(accountFullname) != lower_copy(other.accountFullname)) ||
			(logonScriptPath != other.logonScriptPath ||
				lower_copy(logonScriptPath) != lower_copy(other.logonScriptPath)) ||
			(homeDirPath != other.homeDirPath ||
				lower_copy(homeDirPath) != lower_copy(other.homeDirPath)) ||
			(homeDirDrive != other.homeDirDrive ||
				lower_copy(homeDirDrive) != lower_copy(other.homeDirDrive)) ||
			(profileDirPath != other.profileDirPath ||
				lower_copy(profileDirPath) != lower_copy(other.profileDirPath)) ||
			(params != other.params ||
				lower_copy(params) != lower_copy(other.params)) ||
			(allowedWorkstations != other.allowedWorkstations ||
				lower_copy(allowedWorkstations) != lower_copy(other.allowedWorkstations)) ||
			(logonServer != other.logonServer ||
				lower_copy(logonServer) != lower_copy(other.logonServer)) ||
			(passwordSetStr != other.passwordSetStr ||
				lower_copy(passwordSetStr) != lower_copy(other.passwordSetStr)) ||
			(lastLogOnStr != other.lastLogOnStr ||
				lower_copy(lastLogOnStr) != lower_copy(other.lastLogOnStr)) ||
			(lastLogOffStr != other.lastLogOffStr ||
				lower_copy(lastLogOffStr) != lower_copy(other.lastLogOffStr)) ||
			(privilegesStr != other.privilegesStr ||
				lower_copy(privilegesStr) != lower_copy(other.privilegesStr)) ||
			(expireDateStr != other.expireDateStr ||
				lower_copy(expireDateStr) != lower_copy(other.expireDateStr)) ||
			(maxStorageStr != other.maxStorageStr ||
				lower_copy(maxStorageStr) != lower_copy(other.maxStorageStr)));
	}
	bool ncNormalAcc;
	bool ncTempDupAcc;
	bool ncWorkstationTrustAcc;
	bool ncServerTrustAcc;
	bool ncInterdomainTrustAcc;
	bool accountLocked;
	bool accountDisabled;
	bool accountNotDelegated;
	bool accountDESKeysOnly;
	bool accountNoKerberosPreauth;
	bool accountTrustedToAuthForDelegation;
	bool accountTrustedForDelegation;
	bool authPrintOp;
	bool authCommOp;
	bool authSrvOp;
	bool authAccOp;
	bool logonScriptExecuted;
	bool passwordNotNeeded;
	bool passwordCantChange;
	bool passwordNoExpire;
	bool passwordReverseEncryption;
	bool passwordSmartCardNeeded;
	bool passwordExpired;
	bool passwordEncrypted;
	unsigned long passwordAge;
	unsigned long passwordBadCount;
	unsigned long lastLogOn;
	unsigned long lastLogOff;
	unsigned long privileges;
	unsigned long expireDate;
	unsigned long maxStorage;
	unsigned long logonCount;
	unsigned long countryCode;
	unsigned long codePage;
	unsigned long primaryGroupId;
	std::wstring sid;
	std::wstring comment;
	std::wstring accountName;
	std::wstring accountFullname;
	std::wstring logonScriptPath;
	std::wstring homeDirPath;
	std::wstring homeDirDrive;
	std::wstring profileDirPath;
	std::wstring params;
	std::wstring allowedWorkstations;
	std::wstring logonServer;
	std::wstring passwordSetStr;
	std::wstring lastLogOnStr;
	std::wstring lastLogOffStr;
	std::wstring privilegesStr;
	std::wstring expireDateStr;
	std::wstring maxStorageStr;
	std::vector<GroupDesc> groups;
};

struct GroupDesc {
	GroupDesc();
	GroupDesc(const GroupDesc &other);
	~GroupDesc();
	GroupDesc& operator=(const GroupDesc &other) {
		IsADGroup = other.IsADGroup;
		GroupName = other.GroupName;
		GroupStrSid = other.GroupStrSid;
		Comment = other.Comment;
		return *this;
	}
	bool operator==(const GroupDesc &other) const {
		return ((GroupName == other.GroupName &&
				lower_copy(GroupName) == lower_copy(other.GroupName) &&
			(GroupStrSid == other.GroupStrSid &&
				lower_copy(GroupName) == lower_copy(other.GroupName)) &&
			(Comment == other.Comment &&
				lower_copy(Comment) == lower_copy(other.Comment))) &&
			IsADGroup == other.IsADGroup &&
			Accounts == other.Accounts);
	}
	bool operator!=(const GroupDesc& other) const {
		return ((GroupName != other.GroupName ||
				lower_copy(GroupName) == lower_copy(other.GroupName)) ||
			(GroupStrSid != other.GroupStrSid ||
				lower_copy(GroupName) != lower_copy(other.GroupName)) ||
			(Comment != other.Comment ||
				lower_copy(Comment) != lower_copy(other.Comment)) ||
			IsADGroup != other.IsADGroup ||
			Accounts != other.Accounts);
	}
	bool IsADGroup;
	std::wstring GroupName;
	std::wstring GroupStrSid;
	std::wstring Comment;
	std::vector<AccountDesc> Accounts;
};

class SysHandler {
	public:
		SysHandler();
		SysHandler(const SysHandler &other);
		~SysHandler();
		/* Gets machine name
			Param:
			None
			Returns machine name */
		std::wstring GetMachineName() const;
		/* Gets machine arch - x32 or x64
			Param:
			None
			Returns system architecture (SysArch enum value) */
		SysArch GetMachineArch() const;
		/* Extracts resource from resource section
			Param:
			[in] path to extract resource to
			[in] resource id in resource file
			Returns true on success, false on failure */
		bool ExtractResource(const std::wstring extractPath, const unsigned long resId) const;
		/* Checks if current x86 process runs under Wow64 environment
			Param:
			None
			Returns true if current x86 process runs under Wow64 environment, false otherwise */
		bool IsWow64Proc() const;
		/* Checks if a given x86 process runs under Wow64 environment
			Param:
			[in] [mandatory] process id
			[in] [optional] [default - PROCESS_ALL_ACCESS] roghts to open requested process
			Returns true if current x86 process runs under Wow64 environment, false otherwise */
		bool IsWow64Proc(const unsigned long pid,
			const unsigned long desiredProcRights = PROCESS_ALL_ACCESS) const;
		/* Converts string SID to SID type
			Param:
			[in] [mandatory] account string SID
			Returns PSID related with source string SID
			Returned PSID must be freed with LocalFree */
		PSID SIDFromStrSid(const std::wstring sidstr) const;
		/* Converts PSID to string SID
			Param:
			[in] account PSID
			Returns string SID */
		std::wstring StrSIDFromSID(const PSID sid) const;
		/* Gets account name based on SID
			Param:
			[in] target PSID
			[in] [default - "."] target machine name. Leave it unchanged or empty to get local machine accounts
			Returns account name */
		std::wstring GetAccountNameFromSID(const PSID sid, const std::wstring machineName = L".") const;
		/* Gets account name based on str SID
			Param:
			[in] target str SID
			[in] [default - "."] target machine name. Leave it unchanged or empty to get local machine accounts
			Returns account name */
		std::wstring GetAccountNameFromStrSID(const std::wstring strSid,
			const std::wstring machineName = L".") const;
		/* Gets string SID based on account name
			Param:
			[in] account name
			[in] [default - "."]	target machine name. Leave it unchanged or empty to get local machine accounts
			[in] [default - false]	set to true if specified account is domain acc
			Returns result code of the operation */
		std::wstring GetStrSIDFromAccountName(const std::wstring accName,
			const std::wstring machineName = L".", const bool isDomainAcc = false) const;
		/* Gets PSID from a given account name
			Param:
			[in] [mandatory] account name / login
			[in] [optional] [default - "."]		target machine name. Leave it unchanged or empty to get local machine accounts
			[in] [optional] [default - false]	set to true if requested account belongs to domain
			Returns account PSID
			Returned PSID must be freed with LocalFree */
		PSID GetSIDFromAccountName(const std::wstring accName,
			const std::wstring machineName = L".", const bool isDomainAcc = false) const;
		/* Converts current system time to Unix time
			Param:
			None
			Returns Unix timestamp (number of seconds since 01.01.1970) with current time */
		unsigned long long CurrentTime2Unixtime() const;
		/* Converts Unix timestamp to human - readable format
			Param:
			[in] Unix timestamp
			Returns datetime string based on a timestamp */
		std::wstring UnixTime2StrTime(const time_t &timestamp) const;
		/* Gets TZ offset even without date and time API calls
			Param:
			[in]	[default - null time/use local time] timepoint to get offset from
			Returns TZ offset at Unix time format (number of seconds since 01.01.1970) */
		long GetTZOffset(time_t when = NULL_TIME) const;
		/* Gets number of CPU cores
			Param:
			None
			Returns number of CPU cores */
		unsigned char GetCPUCoreNum() const;
		/* Gets max number of threads available without oversubscription (cpucorenum - 1)
			Param:
			None
			Returns max number of threads available without oversubscription (cpucorenum - 1) */
		unsigned char GetThreadNum() const;
		SysOpResult IsAccountMemberOfGroup(const PSID groupSID, const PSID testSID, bool &isMember,
			const std::wstring machineName = L".") const;
		SysOpResult IsAccountMemberOfGroup(const std::wstring groupName, const std::wstring testAccName,
			bool &isMember, const std::wstring machineName = L".") const;
		SysOpResult GetSIDType(const PSID sid, SidType &sidType, const std::wstring machineName = L".", 
			const std::wstring domainName = L".") const;
		/* Gets RAM info
			Param:
			[out]	free RAM
			[out]	total RAM
			Returns result code of the operation (enum value) */
		SysOpResult GetRam(unsigned long long &freeRam, unsigned long long &totalRam, unsigned char &precentInUse) const;
		/* Gets paging file info
			Param:
			[out]	free paging file
			[out]	total paging file
			Returns result code of the operation (enum value) */
		SysOpResult GetPageFile(unsigned long long& freePageFile, unsigned long long &totalPageFile) const;
		/* Gets virtual memory info
			Param:
			[out]	free virtual memory
			[out]	total virtual memory
			Returns result code of the operation (enum value) */
		SysOpResult GetVirtualMem(unsigned long long &freeVirtMem, unsigned long long &totalVirtMem) const;
		SysOpResult LocalGroupListFromUsername(std::vector<GroupDesc> &outGroupList, const std::wstring userName,
			const std::wstring machineName = L".") const;
		SysOpResult LocalGroupListFromStrSID(std::vector<GroupDesc> &outGroupList, const std::wstring strSID,
			const std::wstring machineName = L".") const;
		/* Enumerates local groups
			Param:
			[out] [mandatory] group desctiption vector to receive data
			[in] [optional] [default - "."]	target machine name. Leave it unchanged or empty to get local accounts
			[in] [optional] [default - true] add a short list of group memners
			[in] [optional] [default - 0] pointer to account list to use. Leaving value as 0 causes account enumeration (EnumAccounts call)
			Returns result code of the operation (enum value) */
		SysOpResult EnumLocalGroups(std::vector<GroupDesc> &groupList,
			const std::wstring machineName = L".", const bool enumAccs = true,
			const std::vector<AccountDesc> *accList = 0) const;
		/* Enumerates local accounts
			Param:
			[out] [mandatory] account desctiption vector to receive data
			[in] [optional] [default - "."]	target machine name. Leave it unchanged or empty to get local accounts
			[in] [optional] [default - true] add a short list of groups to which account belongs
			Returns result code of the operation (enum value) */
		SysOpResult EnumAccounts(std::vector<AccountDesc> &accountList,
			const std::wstring machineName = L".", const bool enumGroups = true) const;
	protected:
	private:
};

#endif // _SYSHELPER_H