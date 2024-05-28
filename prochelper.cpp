#include "prochelper.h"

ProcResource::ProcResource() {
	pid = 0;
	exitCode = 0;
	memset(&si, 0, sizeof(STARTUPINFO));
	memset(&pi, 0, sizeof(PROCESS_INFORMATION));
}

ProcResource::ProcResource(const ProcResource& other) {
	pid = other.pid;
	exitCode = other.exitCode;
	memcpy(&si, &other.si, sizeof(STARTUPINFO));
	memcpy(&pi, &other.pi, sizeof(PROCESS_INFORMATION));
}

ProcResource::~ProcResource() {}

ProcDesc::ProcDesc() {
	size = 0;
	usage = 0;
	pid = 0;
	threadnum = 0;
	parentPid = 0;
	flags = 0;
	moduleId = 0;
	defHeapId = 0;
	threadPriority = 0;
}

ProcDesc::ProcDesc(const unsigned long sz,
	const unsigned long use,
	const unsigned long procid,
	const unsigned long threadn,
	const unsigned long parentpid,
	const unsigned long flgs,
	const unsigned long moduleid,
	const unsigned long long defheapId,
	const long threadpriority,
	const std::wstring pathexe) {
	size = sz;
	usage = use;
	pid = procid;
	threadnum = threadn;
	parentPid = parentpid;
	flags = flgs;
	moduleId = moduleid;
	defHeapId = defheapId;
	threadPriority = threadpriority;
	exepath = pathexe;
}

ProcDesc::ProcDesc(const ProcDesc& other) {
	size = other.size;
	usage = other.usage;
	pid = other.pid;
	threadnum = other.threadnum;
	parentPid = other.parentPid;
	flags = other.flags;
	moduleId = other.moduleId;
	defHeapId = other.defHeapId;
	threadPriority = other.threadPriority;
	exepath = other.exepath;
}

ProcDesc::~ProcDesc() {}

ProcessHandler::ProcessHandler() {

}

ProcessHandler::ProcessHandler(const ProcessHandler &other) {

}

ProcessHandler::~ProcessHandler() {}

ProcResource ProcessHandler::StartProc(const std::wstring exepath, const std::wstring args,
	const ProcAwait awaitTime, const unsigned long procAwaitTime, const bool freeRes) {
	std::wstring cmdline = args.length() ? startsWith(args, L" ") ? args : L" " + args : L"";
	ProcResource ret;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	memset(&si, 0, sizeof(STARTUPINFO));
	memset(&pi, 0, sizeof(PROCESS_INFORMATION));
	if (CreateProcess((wchar_t*)exepath.c_str(), // Exe path
		(wchar_t*)cmdline.c_str(),				 // Command line (prior space is mandatory)
		NULL,									 // Process handle not inheritable
		NULL,									 // Thread handle not inheritable
		true,							         // Set handle inheritance to TRUE
		CREATE_NEW_PROCESS_GROUP,				 // Creates new process group
		0,										 // Use parent's environment block
		0,										 // Use parent's starting directory 
		&si,							         // Pointer to STARTUPINFO structure
		&pi)									 // Pointer to PROCESS_INFORMATION structure
		) {
		ret.pid = pi.dwProcessId;
		ret.si = si;
		ret.pi = pi;
		if (ProcAwait::Infinite == awaitTime) {
			WaitForSingleObject(pi.hProcess, INFINITE);
			GetExitCodeProcess(pi.hProcess, &ret.exitCode);
			if (freeRes) {
				FreeProcResources(ret);
			}
		} else if (ProcAwait::ExactTime == awaitTime) {
			WaitForSingleObject(pi.hProcess, procAwaitTime);
			GetExitCodeProcess(pi.hProcess, &ret.exitCode);
			if (freeRes) {
				FreeProcResources(ret);
			}
		} else if (ProcAwait::NoWait == awaitTime) {
			if (freeRes) {
				FreeProcResources(ret, false);
			}
		}
	}
	return ret;
}

bool ProcessHandler::FreeProcResources(const ProcResource& procRes, bool stopProc) {
	bool funcres = CloseHandle(procRes.pi.hProcess) && CloseHandle(procRes.pi.hThread);
	if (stopProc) {
		return StopProc(procRes.pid);
	}
	return funcres;
}

bool ProcessHandler::StopProc(const unsigned long pid,
	const std::vector<ProcDesc>* proclist) {
	std::vector<ProcDesc> procs;
	if (proclist) {
		procs = *proclist;
	} else {
		if (ProcOpResult::Fail == ListProcesses(procs)) {
			return false;
		}
	}
	for (size_t i = 0; i < procs.size(); ++i) {
		if (procs[i].pid == pid) {
			HANDLE hProc = OpenProcess(PROCESS_TERMINATE, false, pid);
			if (INVALID_HANDLE_VALUE != hProc) {
				if (!TerminateProcess(hProc, 1)) {
					CloseHandle(hProc);
					return false;
				}
				return CloseHandle(hProc);
			} else {
				return false;
			}
		}
	}
	return true;
}

bool ProcessHandler::StopProc(const std::wstring exepath,
	const std::vector<ProcDesc>* proclist) {
	bool ret = false;
	std::vector<ProcDesc> procs;
	if (proclist) {
		procs = *proclist;
	} else {
		if (ProcOpResult::Fail == ListProcesses(procs)) {
			return false;
		}
	}
	for (size_t i = 0; i < procs.size(); ++i) {
		if (lower_copy(procs[i].exepath) == lower_copy(exepath)) {
			HANDLE hProc = ::OpenProcess(PROCESS_TERMINATE, false, procs[i].pid);
			if (INVALID_HANDLE_VALUE != hProc) {
				if (!::TerminateProcess(hProc, 1)) {
					::CloseHandle(hProc);
					return false;
				}
				return ::CloseHandle(hProc);
			} else {
				return false;
			}
		}
	}
	return true;
}

bool ProcessHandler::EnableDebugPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::EnableBackupPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_BACKUP_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::EnableRestorePrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_RESTORE_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::EnableLockMemoryPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_LOCK_MEMORY_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::EnableIncreaseQuotaPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_INCREASE_QUOTA_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::EnableUnsolicitedInputPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_UNSOLICITED_INPUT_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::EnableMachineAccountPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_MACHINE_ACCOUNT_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::EnableTcbPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_TCB_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::EnableSecurityPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_SECURITY_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::EnableTakeOwnershipPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_TAKE_OWNERSHIP_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::EnableLoadDriverPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_LOAD_DRIVER_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::EnableSystemProfilePrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_SYSTEM_PROFILE_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::EnableSystemTimePrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_SYSTEMTIME_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::EnableProfSingleProcessPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_PROF_SINGLE_PROCESS_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::EnableIncBasePriorutyPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_INC_BASE_PRIORITY_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::EnableCreatePageFilePrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_CREATE_PAGEFILE_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::EnableCreatePermanentPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_CREATE_PERMANENT_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::EnableShutdownPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::EnableAuditPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_AUDIT_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::EnableSystemEnvironmentPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_SYSTEM_ENVIRONMENT_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::EnableChangeNotifyPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_CHANGE_NOTIFY_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::EnableRemoteShutdownPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_REMOTE_SHUTDOWN_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::EnableUndockPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_UNDOCK_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::EnableSyncAgentPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_SYNC_AGENT_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::EnableDelegationPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_ENABLE_DELEGATION_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::EnableManageVolumePrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_MANAGE_VOLUME_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::EnableImpersonatePrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_IMPERSONATE_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::EnableCreateGlobalPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_CREATE_GLOBAL_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::EnableTrustedCredManAccessPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_TRUSTED_CREDMAN_ACCESS_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::EnableRelabelPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_RELABEL_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::EnableIncWorkingSetPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_INC_WORKING_SET_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::EnableTimezonePrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_TIME_ZONE_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::EnableCreateSymbolicLinkPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_CREATE_SYMBOLIC_LINK_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::EnableDelegateSessionUserImpersonatePrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_DELEGATE_SESSION_USER_IMPERSONATE_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::EnableCreateTokenPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_CREATE_TOKEN_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::EnableAssignPrimaryTokenPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_ASSIGNPRIMARYTOKEN_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::DisableAssignPrimaryTokenPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_ASSIGNPRIMARYTOKEN_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::DisableCreateTokenPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_CREATE_TOKEN_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::DisableDelegateSessionUserImpersonatePrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_DELEGATE_SESSION_USER_IMPERSONATE_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::DisableCreateSymbolicLinkPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_CREATE_SYMBOLIC_LINK_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::DisableTimezonePrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_TIME_ZONE_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::DisableIncWorkingSetPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_INC_WORKING_SET_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::DisableRelabelPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_RELABEL_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::DisableTrustedCredManAccessPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_TRUSTED_CREDMAN_ACCESS_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::DisableCreateGlobalPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_CREATE_GLOBAL_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::DisableImpersonatePrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_IMPERSONATE_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::DisableManageVolumePrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_MANAGE_VOLUME_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::DisableDelegationPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_ENABLE_DELEGATION_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::DisableSyncAgentPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_SYNC_AGENT_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::DisableUndockPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_UNDOCK_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::DisableRemoteShutdownPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_REMOTE_SHUTDOWN_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::DisableChangeNotifyPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_CHANGE_NOTIFY_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::DisableSystemEnvironmentPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_SYSTEM_ENVIRONMENT_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::DisableAuditPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_AUDIT_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::DisableShutdownPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::DisableCreatePermanentPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_CREATE_PERMANENT_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::DisableCreatePageFilePrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_CREATE_PAGEFILE_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::DisableIncBasePriorutyPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_INC_BASE_PRIORITY_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::DisableProfSingleProcessPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_PROF_SINGLE_PROCESS_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::DisableSystemTimePrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_SYSTEMTIME_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::DisableSystemProfilePrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_SYSTEM_PROFILE_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::DisableLoadDriverPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_LOAD_DRIVER_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::DisableTakeOwnershipPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_TAKE_OWNERSHIP_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::DisableSecurityPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_SECURITY_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::DisableTcbPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_TCB_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::DisableMachineAccountPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_MACHINE_ACCOUNT_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::DisableUnsolicitedInputPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_UNSOLICITED_INPUT_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::DisableBackupPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_BACKUP_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::DisableRestorePrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_RESTORE_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::DisableLockMemoryPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_LOCK_MEMORY_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::DisableIncreaseQuotaPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_INCREASE_QUOTA_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::DisableDebugPrivilege(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		LUID luid;
		TOKEN_PRIVILEGES tkp;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			::LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid);
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
			if (::AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return true;
			} else {
				::CloseHandle(hToken);
				::CloseHandle(hProc);
				return false;
			}
		}
		::CloseHandle(hProc);
		return false;
	}
	return false;
}

bool ProcessHandler::IsProcRunning(const std::wstring exepath,
	const std::vector<ProcDesc> *proclist) {
	std::vector<ProcDesc> vec;
	if (proclist) {
		vec = *proclist;
	} else {
		if (ProcOpResult::Fail == ListProcesses(vec)) {
			return false;
		}
	}
	for (size_t i = 0; i < vec.size(); ++i) {
		if (lower_copy(vec[i].exepath) == lower_copy(exepath)) {
			return true;
		}
	}
	return false;
}

std::wstring ProcessHandler::GetProcessExePathByPid(const unsigned long pid,
	const unsigned long desiredProcRights) {
	std::wstring ret;
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (INVALID_HANDLE_VALUE != hProc) {
		NEW_ARR_NULLIFY(buf, wchar_t, 2048);
		// wchar_t* buf = (wchar_t*)calloc(2048, 2048 * sizeof(wchar_t));
		if (buf) {
			if (::GetProcessImageFileName(hProc, buf, 2048)) {
				if(startsWith(lower_copy(buf), L"\\device\\harddiskvolume")) {
					FSHandler fsh;
					ret = fsh.ReplaceDrivePathWithPartLetter(buf);
				} else {
					ret = buf;
				}
			}
			SAFE_ARR_DELETE(buf);
			// SAFE_FREE(buf);
		}
		::CloseHandle(hProc);
	}
	return ret;
}

ProcDesc ProcessHandler::GetProcessByPid(const unsigned long pid,
	std::vector<ProcDesc>* procList) {
	std::vector<ProcDesc> procs;
	ProcDesc ret;
	if (procList) {
		procs = *procList;
	} else {
		if (ProcOpResult::Fail == ListProcesses(procs)) {
			return ret;
		}
	}
	for (size_t i = 0; i < procs.size(); ++i) {
		if (procs[i].pid == pid) {
			return procs[i];
		}
	}
	return ret;
}

std::vector<ProcDesc> ProcessHandler::GetProcessByExePath(const std::wstring exePath,
	std::vector<ProcDesc>* procList) {
	std::vector<ProcDesc> procs;
	std::vector<ProcDesc> ret;
	if (procList) {
		procs = *procList;
	} else {
		if (ProcOpResult::Fail == ListProcesses(procs)) {
			return ret;
		}
	}
	for (size_t i = 0; i < procs.size(); ++i) {
		if (lower_copy(procs[i].exepath) == lower_copy(exePath)) {
			ret.push_back(procs[i]);
		}
	}
	return ret;
}

unsigned long ProcessHandler::GetCurrentProcPid() const {
	return GetCurrentProcessId();
}

std::vector<std::wstring> ProcessHandler::GetProcPrivileges(const unsigned long pid,
	const unsigned long desiredProcRights) const {
	std::vector<std::wstring> ret;
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			unsigned long bufSize = 0, retval = NO_ERROR;
			if (!::GetTokenInformation(hToken, TokenPrivileges, 0, 0, &bufSize)) {
				retval = ::GetLastError();
				if (retval == ERROR_INSUFFICIENT_BUFFER) {
					NEW_ARR_NULLIFY(buf, unsigned char, bufSize * sizeof(unsigned char));
					if (buf) {
						if (::GetTokenInformation(hToken, TokenPrivileges, (void*)buf, bufSize, &bufSize)) {
							TOKEN_PRIVILEGES* priv = (TOKEN_PRIVILEGES*)buf;
							LUID luid = { 0 };
							wchar_t pbuf[64] = { 0 };
							for (size_t i = 0; i < priv->PrivilegeCount; ++i) {
								bufSize = 64;
								if (priv->Privileges[i].Attributes) {
									luid = priv->Privileges[i].Luid;
									if (::LookupPrivilegeName(0, &luid, pbuf, &bufSize)) {
										ret.push_back(pbuf);
									} else {
										SAFE_ARR_DELETE(buf);
										::CloseHandle(hToken);
										::CloseHandle(hProc);
										return ret;
									}
								}
							}
							SAFE_ARR_DELETE(buf);
							::CloseHandle(hToken);
							::CloseHandle(hProc);
							return ret;
						} else {
							::CloseHandle(hToken);
							::CloseHandle(hProc);
							return ret;
						}
					} else {
						::CloseHandle(hToken);
						::CloseHandle(hProc);
						return ret;
					}
				} else {
					::CloseHandle(hToken);
					::CloseHandle(hProc);
					return ret;
				}
			}
		}
		::CloseHandle(hProc);
		return ret;
	}
	return ret;
}

ProcOpResult ProcessHandler::GetProcUserSID(const unsigned long pid, PSID &sid,
	const unsigned long desiredProcRights) const {
	HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
	if (hProc && INVALID_HANDLE_VALUE != hProc) {
		HANDLE hToken = 0;
		::OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (INVALID_HANDLE_VALUE != hToken) {
			unsigned long bufSize = 0, retval = NO_ERROR;
			if (!::GetTokenInformation(hToken, TokenUser, 0, 0, &bufSize)) {
				retval = ::GetLastError();
				if (retval == ERROR_INSUFFICIENT_BUFFER) {
					TOKEN_USER* buf = (TOKEN_USER*)malloc(bufSize);
					// NEW_ARR_NULLIFY(buf, unsigned char, bufSize * sizeof(unsigned char));
					if (buf) {
						if (::GetTokenInformation(hToken, TokenUser, (void*)buf, bufSize, &bufSize)) {
							sid = buf->User.Sid;
							// free(buf);
							// memcpy(&sid, &(((TOKEN_USER*)buf)->User.Sid), sizeof(SID));
							// SAFE_ARR_DELETE(buf);
							return ProcOpResult::Success;
						}
						// SAFE_ARR_DELETE(buf);
						free(buf);
						return ProcOpResult::Fail;
					}
					return ProcOpResult::Fail;
				}
				return ProcOpResult::Fail;
			}
			return ProcOpResult::Fail;
		}
		return ProcOpResult::Fail;
	}
	return ProcOpResult::Fail;
}

ProcOpResult ProcessHandler::ListProcesses(std::vector<ProcDesc> &procList) {
	bool recPlaced = false;
	std::vector<ProcDesc> ret;
	ProcDesc elem;
	HANDLE hProcSnap = INVALID_HANDLE_VALUE;
	PROCESSENTRY32 pe32;
	hProcSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE != hProcSnap) {
		pe32.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hProcSnap, &pe32)) {
			do {
				elem.pid = pe32.th32ProcessID;
				std::wstring tstr = GetProcessExePathByPid(elem.pid);
				if (tstr.length()) {
					elem.exepath = tstr;
				} else {
					elem.exepath = pe32.szExeFile;
				}
				elem.flags = pe32.dwFlags;
				elem.parentPid = pe32.th32ParentProcessID;
				elem.size = pe32.dwSize;
				elem.threadnum = pe32.cntThreads;
				elem.threadPriority = pe32.pcPriClassBase;
				elem.usage = pe32.cntUsage;
				elem.moduleId = pe32.th32ModuleID;
				elem.defHeapId = pe32.th32DefaultHeapID;
				procList.push_back(elem);
				recPlaced = true;
			} while (Process32Next(hProcSnap, &pe32));
			::CloseHandle(hProcSnap);
		} else {
			::CloseHandle(hProcSnap);
		}
	}
	if (recPlaced) {
		return ProcOpResult::Success;
	} else {
		return ProcOpResult::Fail;
	}
}