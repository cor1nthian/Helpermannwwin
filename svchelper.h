#ifndef _SERVICE_HELPER_H
#define _SERVICE_HELPER_H

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
// #define NOSERVICE
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
#include <map>
#include <vector>
#include "prochelper.h"
#include "strhelper.h"
#include "syshelper.h"
#include "winerrhelper.h"
#include "winverhelper.h"

enum class ScmOpResult : unsigned char {
	OpenManagerFail = 0,
	CloseManagerFail,
	EnumSvcsFail,
	StartSvcFail,
	StartSvcFailAlreadyStarted,
	StopSvcFail,
	StopSvcFailAlreadyStopped,
	InstallFail,
	RemoveFail,
	ChangeServiceFail,
	Success
};

enum class DepCompareType : unsigned char {
	equal = 0,
	notEqual,
	greater,
	less,
	greaterOrEqual,
	lessOrEqual
};

enum class StrCompareType : unsigned char {
	equal = 0,
	equalIgnoreCase,
	partialMatch,
	partialMatchIgnoreCase
};

enum class ServiceState : unsigned long {
	Active = SERVICE_ACTIVE,
	Stopped = SERVICE_INACTIVE,
	Any = SERVICE_STATE_ALL
};

enum class SvcStartUpType : unsigned long {
	AutoStart = SERVICE_AUTO_START,
	BootStart = SERVICE_BOOT_START,
	DemandStart = SERVICE_DEMAND_START,
	Disabled = SERVICE_DISABLED,
	SystemStart = SERVICE_SYSTEM_START
};

enum class SvcType : unsigned long {
	KernelDriver = SERVICE_KERNEL_DRIVER,
	FSDriver = SERVICE_FILE_SYSTEM_DRIVER,
	SvcOwnProc = SERVICE_WIN32_OWN_PROCESS,
	SvcShareProc = SERVICE_WIN32_SHARE_PROCESS,
	Druver = SERVICE_DRIVER | SERVICE_FILE_SYSTEM_DRIVER | SERVICE_KERNEL_DRIVER,
	Service = SERVICE_WIN32
};

enum class ServiceType : unsigned long {
	Driver = SERVICE_DRIVER | SERVICE_FILE_SYSTEM_DRIVER | SERVICE_KERNEL_DRIVER,
	Service = SERVICE_WIN32,
	Any = SERVICE_WIN32 | SERVICE_DRIVER | SERVICE_FILE_SYSTEM_DRIVER | SERVICE_KERNEL_DRIVER
};

enum class SvcErrControl : unsigned char {
	Ignore = SERVICE_ERROR_IGNORE,
	Boenal = SERVICE_ERROR_NORMAL,
	Severe = SERVICE_ERROR_SEVERE,
	Critical = SERVICE_ERROR_CRITICAL
};

LIBCOMPONENT struct SvcRunState {
	SvcRunState();
	SvcRunState(const SvcRunState& other);
	~SvcRunState();
	SvcRunState* operator=(const SvcRunState& other) {
		svcRunning = other.svcRunning;
		svcProcRunning = other.svcProcRunning;
	}
	bool operator==(const SvcRunState& other) const {
		return (svcRunning == other.svcRunning &&
				svcProcRunning == other.svcProcRunning);
	}
	bool operator!=(const SvcRunState& other) const {
		return (svcRunning != other.svcRunning ||
			svcProcRunning != other.svcProcRunning);
	}
	bool svcRunning;
	bool svcProcRunning;
};

LIBCOMPONENT struct SvcTriggerData {
	SvcTriggerData();
	SvcTriggerData(const unsigned short triggerdatasize,
		const unsigned long triggerdatatype,
		std::vector<unsigned char> triggerdata);
	SvcTriggerData(const SvcTriggerData& other);
	~SvcTriggerData();
	SvcTriggerData& operator=(const SvcTriggerData& other) {
		triggerDataSize = other.triggerDataSize;
		triggerDataType = other.triggerDataType;
		triggerData = other.triggerData;
		return *this;
	}
	bool operator ==(const SvcTriggerData& other) const {
		std::string origstr = std::string(triggerData.begin(), triggerData.end());
		std::string otherstr = std::string(other.triggerData.begin(), other.triggerData.end());
		return (triggerDataSize == other.triggerDataSize &&
			triggerDataType == other.triggerDataType &&
			(origstr == otherstr &&
				lower_copy(origstr) == lower_copy(otherstr)));
	}
	bool operator !=(const SvcTriggerData& other) const {
		std::string origstr = std::string(triggerData.begin(), triggerData.end());
		std::string otherstr = std::string(other.triggerData.begin(), other.triggerData.end());
		return (triggerDataSize != other.triggerDataSize ||
			triggerDataType != other.triggerDataType ||
			(origstr != otherstr ||
				lower_copy(origstr) != lower_copy(otherstr)));
	}
	unsigned short triggerDataSize;
	unsigned long triggerDataType;
	std::vector<unsigned char> triggerData;
};

LIBCOMPONENT struct SvcTrigger {
	SvcTrigger();
	SvcTrigger(const unsigned long triggertype,
		const unsigned long triggeraction,
		const std::wstring triggerguid,
		std::vector <SvcTriggerData> trigdata);
	SvcTrigger(const SvcTrigger& other);
	~SvcTrigger();
	SvcTrigger& operator=(const SvcTrigger &other) {
		triggerType = other.triggerType;
		triggerAction = other.triggerAction;
		triggerGuid = other.triggerGuid;
		triggerData = other.triggerData;
		return *this;
	}
	bool operator==(const SvcTrigger& other) const {
		return(triggerType == other.triggerType &&
			triggerAction == other.triggerAction &&
			(triggerGuid == other.triggerGuid &&
				lower_copy(triggerGuid) == lower_copy(other.triggerGuid)) &&
			triggerData == other.triggerData);
	}
	bool operator!=(const SvcTrigger& other) const {
		return(triggerType != other.triggerType ||
			triggerAction != other.triggerAction ||
			(triggerGuid != other.triggerGuid || 
				lower_copy(triggerGuid) != lower_copy(other.triggerGuid)) ||
			triggerData != other.triggerData);
	}
	unsigned long triggerType;
	unsigned long triggerAction;
	std::wstring triggerGuid;
	std::vector<SvcTriggerData> triggerData;
};

LIBCOMPONENT struct SvcRecord {
	SvcRecord();
	SvcRecord(const std::wstring svcname,
		const std::wstring svcdisplayname,
		const std::wstring svcdesc,
		const std::wstring binarypath,
		const std::wstring startsfrom,
		const std::wstring loadordergroup,
		const std::wstring failresetmsg,
		const std::wstring failcommand,
		const std::wstring reqprivileges,
		const unsigned long servicetype,
		const unsigned long starttype,
		const unsigned long errorcontrol,
		const unsigned long tagid,
		const unsigned long failresetperiod,
		const unsigned long failactions,
		const unsigned long failactiondelay,
		const unsigned long preshutdowntimeout,
		const unsigned long svcsidtype,
		const unsigned long launchprotected,
		const unsigned short preferrednode,
		const bool preferrednodedeleteflag,
		const bool failureactionsonnoncrashfailures,
		const bool delayedautostart);
	SvcRecord(const std::wstring svcname,
		const std::wstring svcdisplayname,
		const std::wstring svcdesc,
		const std::wstring binarypath,
		const std::wstring startsfrom,
		const std::wstring loadordergroup,
		const std::wstring failresetmsg,
		const std::wstring failcommand,
		const std::wstring reqprivileges,
		const unsigned long servicetype,
		const unsigned long starttype,
		const unsigned long errorcontrol,
		const unsigned long tagid,
		const unsigned long failresetperiod,
		const unsigned long failactions,
		const unsigned long failactiondelay,
		const unsigned long preshutdowntimeout,
		const unsigned long svcsidtype,
		const unsigned long launchprotected,
		const unsigned short preferrednode,
		const bool preferrednodedeleteflag,
		const bool delayedautostart,
		const bool failureactionsonnoncrashfailures,
		const std::vector<std::wstring> deps,
		const std::vector<SvcTrigger> trgs,
		const SC_ACTION_TYPE failactiontype,
		const SERVICE_STATUS_PROCESS svcprocdetial);
	SvcRecord(const SvcRecord &other);
	~SvcRecord();
	SvcRecord& operator =(const SvcRecord &other) {
		svcName = other.svcName;
		svcDisplayName = other.svcDisplayName;
		svcDesc = other.svcDesc;
		binaryPath = other.binaryPath;
		startsFrom = other.startsFrom;
		dependencies = other.dependencies;
		loadOrderGroup = other.loadOrderGroup;
		failResetMsg = other.failResetMsg;
		failCommand = other.failCommand;
		privileges = other.privileges;
		serviceType = other.serviceType;
		startType = other.startType;
		errorControl = other.errorControl;
		tagId = other.tagId;
		failResetPeriod = other.failResetPeriod;
		failActions = other.failActions;
		failActionDelay = other.failActionDelay;
		preshutdownTimeout = other.preshutdownTimeout;
		svcSidType = other.svcSidType;
		launchProtected = other.launchProtected;
		preferredNode = other.preferredNode;
		preferredNodeDeleteFlag = other.preferredNodeDeleteFlag;
		delayedAutoStart = other.delayedAutoStart;
		failureActionsOnNonCrashFailures = other.failureActionsOnNonCrashFailures;
		triggers = other.triggers;
		memcpy(&failActionType, &other.failActionType, sizeof(SC_ACTION_TYPE));
		memcpy(&svcProcDetail, &other.svcProcDetail, sizeof(SERVICE_STATUS_PROCESS));
		return *this;
	}
	bool operator ==(const SvcRecord& other) const {
		return ((svcName == other.svcName &&
				lower_copy(svcName) == lower_copy(other.svcName)) &&
			(svcDisplayName == other.svcDisplayName &&
				lower_copy(svcDisplayName) == lower_copy(other.svcDisplayName)) &&
			(svcDesc == other.svcDesc &&
				lower_copy(svcDesc) == lower_copy(other.svcDesc)) &&
			(binaryPath == other.binaryPath &&
				lower_copy(binaryPath) == lower_copy(other.binaryPath)) &&
			(startsFrom == other.startsFrom &&
				lower_copy(startsFrom) == lower_copy(other.startsFrom)) &&
			dependencies == other.dependencies &&
			(loadOrderGroup == other.loadOrderGroup &&
				lower_copy(loadOrderGroup) == lower_copy(other.loadOrderGroup)) &&
			(failResetMsg == other.failResetMsg &&
				lower_copy(failResetMsg) == lower_copy(other.failResetMsg)) &&
			(failCommand == other.failCommand &&
				lower_copy(failResetMsg) == lower_copy(other.failResetMsg)) &&
			privileges == other.privileges &&
			serviceType == other.serviceType &&
			startType == other.startType &&
			errorControl == other.errorControl &&
			tagId == other.tagId &&
			failResetPeriod == other.failResetPeriod &&
			failActions == other.failActions &&
			failActionDelay == other.failActionDelay &&
			preshutdownTimeout == other.preshutdownTimeout &&
			svcSidType == other.svcSidType &&
			launchProtected == other.launchProtected &&
			preferredNode == other.preferredNode &&
			preferredNodeDeleteFlag == other.preferredNodeDeleteFlag &&
			delayedAutoStart == other.delayedAutoStart &&
			failureActionsOnNonCrashFailures == other.failureActionsOnNonCrashFailures &&
			triggers == other.triggers &&
			!memcmp(&failActionType, &other.failActionType, sizeof(SC_ACTION_TYPE)) &&
			!memcmp(&svcProcDetail, &other.svcProcDetail, sizeof(SERVICE_STATUS_PROCESS)));
	}
	bool operator !=(const SvcRecord& other) const {
		return ((svcName != other.svcName ||
				lower_copy(svcName) != lower_copy(other.svcName)) ||
			(svcDisplayName != other.svcDisplayName ||
				lower_copy(svcDisplayName) != lower_copy(other.svcDisplayName)) ||
			(svcDesc != other.svcDesc ||
				lower_copy(svcDesc) != lower_copy(other.svcDesc)) ||
			(binaryPath != other.binaryPath ||
				lower_copy(binaryPath) != lower_copy(other.binaryPath)) ||
			(startsFrom != other.startsFrom ||
				lower_copy(startsFrom) != lower_copy(other.startsFrom)) ||
			dependencies != other.dependencies ||
			(loadOrderGroup != other.loadOrderGroup ||
				lower_copy(loadOrderGroup) != lower_copy(other.loadOrderGroup)) ||
			(failResetMsg != other.failResetMsg ||
				lower_copy(failResetMsg) != lower_copy(other.failResetMsg)) ||
			(failCommand != other.failCommand ||
				lower_copy(failResetMsg) != lower_copy(other.failResetMsg)) ||
			privileges != other.privileges ||
			serviceType != other.serviceType ||
			startType != other.startType ||
			errorControl != other.errorControl ||
			tagId != other.tagId ||
			failResetPeriod != other.failResetPeriod ||
			failActions != other.failActions ||
			failActionDelay != other.failActionDelay ||
			preshutdownTimeout != other.preshutdownTimeout ||
			svcSidType != other.svcSidType ||
			launchProtected != other.launchProtected ||
			preferredNode != other.preferredNode ||
			preferredNodeDeleteFlag != other.preferredNodeDeleteFlag ||
			delayedAutoStart != other.delayedAutoStart ||
			failureActionsOnNonCrashFailures != other.failureActionsOnNonCrashFailures ||
			triggers != other.triggers ||
			failActionType != other.failActionType ||
			memcmp(&failActionType, &other.failActionType, sizeof(SC_ACTION_TYPE)) ||
			memcmp(&svcProcDetail, &other.svcProcDetail, sizeof(SERVICE_STATUS_PROCESS)));
	}
	std::vector<std::wstring> dependencies;
	std::vector<SvcTrigger> triggers;
	std::wstring svcName;
	std::wstring svcDisplayName;
	std::wstring svcDesc;
	std::wstring binaryPath;
	std::wstring startsFrom;
	std::wstring loadOrderGroup;
	std::wstring failResetMsg;
	std::wstring failCommand;
	std::wstring privileges;
	unsigned long serviceType;
	unsigned long startType;
	unsigned long errorControl;
	unsigned long tagId;
	unsigned long failResetPeriod;
	unsigned long failActions;
	unsigned long failActionDelay;
	unsigned long preshutdownTimeout;
	unsigned long svcSidType;
	unsigned long launchProtected;
	unsigned short preferredNode;;
	bool preferredNodeDeleteFlag;
	bool delayedAutoStart;
	bool failureActionsOnNonCrashFailures;
	SC_ACTION_TYPE failActionType;
	SERVICE_STATUS_PROCESS svcProcDetail;
};

LIBCOMPONENT class SvcHandler {
	public:
		SvcHandler();
		SvcHandler(const SvcHandler &other);
		~SvcHandler();
		ScmOpResult SCM_OpenManager(unsigned long desiredRghts = 0);
		ScmOpResult SCM_CloseManager();
		ScmOpResult InstallSvc(const std::wstring svcName, const std::wstring displayName,
			const std::wstring binPath, const unsigned long desiredRights = 0, const unsigned long tagId = 0,
			const SvcType svcType = SvcType::SvcOwnProc,
			const SvcStartUpType startupType = SvcStartUpType::AutoStart,
			const SvcErrControl errControlType = SvcErrControl::Ignore,
			std::wstring loadOrderGroup = L"", std::wstring startupAccLogin = L"",
			std::wstring startupAccPwd = L"", const std::vector<std::wstring> *depenendencies = 0);
		ScmOpResult RemoveSvc(const std::wstring svcName,
			const StrCompareType compareType = StrCompareType::equalIgnoreCase,
			const unsigned long desiredRights = 0, const std::vector<SvcRecord> *svcRecords = 0);
		ScmOpResult StartSvc(const std::wstring displayName, const unsigned long desiredRights = 0,
			const std::vector<std::wstring>* svcArgs = 0, const std::vector<SvcRecord> *svcRecords = 0);
		ScmOpResult StopSvc(const std::wstring displayName, const bool stopDependentSvcs = false,
			const bool stopDrivers = false, const unsigned long desiredRights = 0,
			const unsigned long timeout = 30000, const std::vector<SvcRecord> *svcRecords = 0);
		bool IsSvcDriver(const SvcRecord &svcrec);
		bool IsSvcDriver(const std::wstring serviceName, const unsigned long desiredRights = 0,
			const std::vector<SvcRecord> *svcRecords = 0);
		ScmOpResult EnumSvcsStatus(std::vector<SvcRecord> &svcrecs,
			const unsigned long desiredRights = 0, ServiceType svcType = ServiceType::Any,
			ServiceState = ServiceState::Any);
		ScmOpResult SetSvcStartupType(const std::wstring svcName,
			SvcStartUpType startupType = SvcStartUpType::DemandStart,
			const StrCompareType compareType = StrCompareType::equalIgnoreCase,
			const unsigned long desiredRights = 0, const std::vector<SvcRecord> *svcRecords = 0);
		ScmOpResult SetSvcStartName(const std::wstring svcName, const std::wstring accLogin,
			const std::wstring accPwd, const StrCompareType compareType = StrCompareType::equalIgnoreCase,
			const unsigned long desiredRights = 0, const std::vector<SvcRecord> *svcRecords = 0);
		ScmOpResult SetSvcDisplayName(const std::wstring svcName, const std::wstring newDisplayName,
			const StrCompareType compareType = StrCompareType::equalIgnoreCase,
			const unsigned long desiredRights = 0, const std::vector<SvcRecord> *svcRecords = 0);
		ScmOpResult SetSvcDependencies(const std::wstring svcName, const std::vector<std::wstring> dependencies,
			const StrCompareType compareType = StrCompareType::equalIgnoreCase,
			const unsigned long desiredRights = 0, const std::vector<SvcRecord> *svcRecords = 0);
		ScmOpResult SetSvcDelayedAutostart(const std::wstring svcName, const bool enableDelayedAutostart,
			const StrCompareType compareType = StrCompareType::equalIgnoreCase,
			const unsigned long desiredRights = 0, const std::vector<SvcRecord> *svcRecords = 0);
		ScmOpResult SetSvcDescription(const std::wstring svcName, const std::wstring svcDescription,
			const StrCompareType compareType = StrCompareType::equalIgnoreCase,
			const unsigned long desiredRights = 0, const std::vector<SvcRecord> *svcRecords = 0);
		ScmOpResult SetSvcType(const std::wstring svcName, const SvcType svcType = SvcType::SvcOwnProc,
			const StrCompareType compareType = StrCompareType::equalIgnoreCase,
			const unsigned long desiredRights = 0, const std::vector<SvcRecord> *svcRecords = 0);
		ScmOpResult SetSvcBinary(const std::wstring svcName, const std::wstring svcBinPath,
			const StrCompareType compareType = StrCompareType::equalIgnoreCase,
			const unsigned long desiredRights = 0, const std::vector<SvcRecord> *svcRecords = 0);
		SvcRunState IsServiceRunning(const std::wstring svcDisplayName,
			const unsigned long desiredRights = 0,
			const std::vector<SvcRecord> *svcRecords = 0);
		std::vector<SvcRecord> GetSvcsWithNTriggers(size_t trigCount,
			const DepCompareType depCompareType = DepCompareType::equal,
			const bool uniqueResults = true, const unsigned long desiredRights = 0,
			const std::vector<SvcRecord> *svcRecords = 0);
		std::vector<SvcRecord> GetSvcsWithNDependencies(size_t depCount,
			const DepCompareType depCompareType = DepCompareType::equal,
			const bool uniqueResults = true, const unsigned long desiredRights = 0,
			const std::vector<SvcRecord> *svcRecords = 0);
		std::vector<SvcRecord> GetSvcByDisplayName(const std::wstring displayName,
			const StrCompareType compareType = StrCompareType::equalIgnoreCase,
			bool uniqueResults = true, const unsigned long desiredRights = 0,
			const std::vector<SvcRecord> *svcRecords = 0);
		std::vector<SvcRecord> GetSvcBySvcName(const std::wstring svcName,
			const StrCompareType compareType = StrCompareType::equalIgnoreCase,
			const bool uniqueResults = true, const unsigned long desiredRights = 0,
			const std::vector<SvcRecord> *svcRecords = 0);
		std::vector<std::wstring> GetSvcExecutableByDisplayName(const std::wstring displayName,
			const StrCompareType compareType = StrCompareType::equalIgnoreCase,
			const bool uniqueResults = true, const bool removeParameters = true,
			const unsigned long desiredRights = 0, const std::vector<SvcRecord> *svcRecords = 0);
		std::vector<std::wstring> GetSvcExecutableBySvcName(const std::wstring svcName,
			const StrCompareType compareType = StrCompareType::equalIgnoreCase,
			const bool uniqueResults = true, const bool removeParameters = true,
			const unsigned long desiredRights = 0, const std::vector<SvcRecord> *svcRecords = 0);
		std::vector<std::wstring> GetDisplayNameBySvcName(const std::wstring svcName,
			const StrCompareType compareType = StrCompareType::equalIgnoreCase,
			const bool uniqueResults = true, const unsigned long desiredRights = 0,
			const std::vector<SvcRecord> *svcRecords = 0);
		std::vector<std::wstring> GetSvcNameByDisplayName(const std::wstring displayName,
			const StrCompareType compareType = StrCompareType::equalIgnoreCase,
			const bool uniqueResults = true, const unsigned long desiredRights = 0,
			const std::vector<SvcRecord> *svcRecords = 0);
	protected:

	private:
		ScmOpResult QuerySvcDesc(SC_HANDLE& svcHandle, SvcRecord& rec);
		ScmOpResult QuerySvcDelayedAutostart(SC_HANDLE& svcHandle, SvcRecord& rec);
		ScmOpResult QuerySvcFailureActions(SC_HANDLE& svcHandle, SvcRecord& rec);
		ScmOpResult QuerySvcFailureActionsFlag(SC_HANDLE& svcHandle, SvcRecord& rec);
		ScmOpResult QuerySvcPreferredNode(SC_HANDLE& svcHandle, SvcRecord& rec);
		ScmOpResult QuerySvcPreshutdownInfo(SC_HANDLE& svcHandle,  SvcRecord& rec);
		ScmOpResult QuerySvcPrivileges(SC_HANDLE& svcHandle,  SvcRecord& rec);
		ScmOpResult QuerySvcSIDInfo(SC_HANDLE& svcHandle,  SvcRecord& rec);
		ScmOpResult QuerySvcTriggerInfo(SC_HANDLE& svcHandle,  SvcRecord& rec);
		ScmOpResult QuerySvcLaunchProtected(SC_HANDLE& svcHandle,  SvcRecord& rec);
		ScmOpResult StopDependentSvc(const SvcRecord& svcRec, const bool stopDrivers = false,
			const unsigned long desiredRights = 0, const unsigned long timeout = 30000, const std::vector<SvcRecord>*svcRecords = 0);
		inline ScmOpResult ObtainSCMHanele(const unsigned long desiredRights = 0);
		bool m_scmOpen;
		unsigned long m_scmRights;
		SC_HANDLE m_scmHandle;
};

#endif // _SERVICE_HELPER_H