#include <iostream>
#include "syshelper.h"

bool IsBadReadPtr(void* p) {
	MEMORY_BASIC_INFORMATION mbi = { 0 };
	if (::VirtualQuery(p, &mbi, sizeof(mbi))) {
		unsigned long mask = (PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY);
		bool b = !(mbi.Protect & mask);
		// check the page is not a guard page
		if (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS)) {
			b = true;
		}
		return b;
	}
	return true;
}

bool IsBadWritePtr(void* p) {
	MEMORY_BASIC_INFORMATION mbi = { 0 };
	if (::VirtualQuery(p, &mbi, sizeof(mbi))) {
		unsigned long mask = (PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY);
		bool b = !(mbi.Protect & mask);
		// check the page is not a guard page
		if (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS)) {
			b = true;
		}
		return b;
	}
	return true;
}

bool IsSIDWellKnown(const std::wstring strsid) {
	for (size_t i = 0; i < gc_WellKnownStrSIDs.size(); ++i) {
		if (gc_WellKnownStrSIDs[i].StrSID == strsid) {
			return true;
		}
	}
	return false;
}

bool IsSIDWellKnown(const PSID sid) {
	SysHandler sys;
	std::wstring twsid = sys.StrSIDFromSID(sid);
	for (size_t i = 0; i < gc_WellKnownStrSIDs.size(); ++i) {
		if (gc_WellKnownStrSIDs[i].StrSID == twsid) {
			return true;
		}
	}
	return false;
}

std::vector<WKSid> GetWellKnownStrSIDs(PSID domainSID) {
	std::vector<WKSid> ret;
	unsigned char wksids[] = {
		WELL_KNOWN_SID_TYPE::WinNullSid,
		WELL_KNOWN_SID_TYPE::WinWorldSid,
		WELL_KNOWN_SID_TYPE::WinLocalSid,
		WELL_KNOWN_SID_TYPE::WinCreatorOwnerSid,
		WELL_KNOWN_SID_TYPE::WinCreatorGroupSid,
		WELL_KNOWN_SID_TYPE::WinCreatorOwnerServerSid,
		WELL_KNOWN_SID_TYPE::WinCreatorGroupServerSid,
		WELL_KNOWN_SID_TYPE::WinNtAuthoritySid,
		WELL_KNOWN_SID_TYPE::WinDialupSid,
		WELL_KNOWN_SID_TYPE::WinNetworkSid,
		WELL_KNOWN_SID_TYPE::WinBatchSid,
		WELL_KNOWN_SID_TYPE::WinInteractiveSid,
		WELL_KNOWN_SID_TYPE::WinServiceSid,
		WELL_KNOWN_SID_TYPE::WinAnonymousSid,
		WELL_KNOWN_SID_TYPE::WinProxySid,
		WELL_KNOWN_SID_TYPE::WinEnterpriseControllersSid,
		WELL_KNOWN_SID_TYPE::WinSelfSid,
		WELL_KNOWN_SID_TYPE::WinAuthenticatedUserSid,
		WELL_KNOWN_SID_TYPE::WinRestrictedCodeSid,
		WELL_KNOWN_SID_TYPE::WinTerminalServerSid,
		WELL_KNOWN_SID_TYPE::WinRemoteLogonIdSid,
		WELL_KNOWN_SID_TYPE::WinLogonIdsSid,
		WELL_KNOWN_SID_TYPE::WinLocalSystemSid,
		WELL_KNOWN_SID_TYPE::WinLocalServiceSid,
		WELL_KNOWN_SID_TYPE::WinNetworkServiceSid,
		WELL_KNOWN_SID_TYPE::WinBuiltinDomainSid,
		WELL_KNOWN_SID_TYPE::WinBuiltinAdministratorsSid,
		WELL_KNOWN_SID_TYPE::WinBuiltinUsersSid,
		WELL_KNOWN_SID_TYPE::WinBuiltinGuestsSid,
		WELL_KNOWN_SID_TYPE::WinBuiltinPowerUsersSid,
		WELL_KNOWN_SID_TYPE::WinBuiltinAccountOperatorsSid,
		WELL_KNOWN_SID_TYPE::WinBuiltinSystemOperatorsSid,
		WELL_KNOWN_SID_TYPE::WinBuiltinPrintOperatorsSid,
		WELL_KNOWN_SID_TYPE::WinBuiltinBackupOperatorsSid,
		WELL_KNOWN_SID_TYPE::WinBuiltinReplicatorSid,
		WELL_KNOWN_SID_TYPE::WinBuiltinPreWindows2000CompatibleAccessSid,
		WELL_KNOWN_SID_TYPE::WinBuiltinRemoteDesktopUsersSid,
		WELL_KNOWN_SID_TYPE::WinBuiltinNetworkConfigurationOperatorsSid,
		WELL_KNOWN_SID_TYPE::WinAccountAdministratorSid,
		WELL_KNOWN_SID_TYPE::WinAccountGuestSid,
		WELL_KNOWN_SID_TYPE::WinAccountKrbtgtSid,
		WELL_KNOWN_SID_TYPE::WinAccountDomainAdminsSid,
		WELL_KNOWN_SID_TYPE::WinAccountDomainUsersSid,
		WELL_KNOWN_SID_TYPE::WinAccountDomainGuestsSid,
		WELL_KNOWN_SID_TYPE::WinAccountComputersSid,
		WELL_KNOWN_SID_TYPE::WinAccountControllersSid,
		WELL_KNOWN_SID_TYPE::WinAccountCertAdminsSid,
		WELL_KNOWN_SID_TYPE::WinAccountSchemaAdminsSid,
		WELL_KNOWN_SID_TYPE::WinAccountEnterpriseAdminsSid,
		WELL_KNOWN_SID_TYPE::WinAccountPolicyAdminsSid,
		WELL_KNOWN_SID_TYPE::WinAccountRasAndIasServersSid,
		WELL_KNOWN_SID_TYPE::WinNTLMAuthenticationSid,
		WELL_KNOWN_SID_TYPE::WinDigestAuthenticationSid,
		WELL_KNOWN_SID_TYPE::WinSChannelAuthenticationSid,
		WELL_KNOWN_SID_TYPE::WinThisOrganizationSid,
		WELL_KNOWN_SID_TYPE::WinOtherOrganizationSid,
		WELL_KNOWN_SID_TYPE::WinBuiltinIncomingForestTrustBuildersSid,
		WELL_KNOWN_SID_TYPE::WinBuiltinPerfMonitoringUsersSid,
		WELL_KNOWN_SID_TYPE::WinBuiltinPerfLoggingUsersSid,
		WELL_KNOWN_SID_TYPE::WinBuiltinAuthorizationAccessSid,
		WELL_KNOWN_SID_TYPE::WinBuiltinTerminalServerLicenseServersSid,
		WELL_KNOWN_SID_TYPE::WinBuiltinDCOMUsersSid,
		WELL_KNOWN_SID_TYPE::WinBuiltinIUsersSid,
		WELL_KNOWN_SID_TYPE::WinIUserSid,
		WELL_KNOWN_SID_TYPE::WinBuiltinCryptoOperatorsSid,
		WELL_KNOWN_SID_TYPE::WinUntrustedLabelSid,
		WELL_KNOWN_SID_TYPE::WinLowLabelSid,
		WELL_KNOWN_SID_TYPE::WinMediumLabelSid,
		WELL_KNOWN_SID_TYPE::WinHighLabelSid,
		WELL_KNOWN_SID_TYPE::WinSystemLabelSid,
		WELL_KNOWN_SID_TYPE::WinWriteRestrictedCodeSid,
		WELL_KNOWN_SID_TYPE::WinCreatorOwnerRightsSid,
		WELL_KNOWN_SID_TYPE::WinCacheablePrincipalsGroupSid,
		WELL_KNOWN_SID_TYPE::WinNonCacheablePrincipalsGroupSid,
		WELL_KNOWN_SID_TYPE::WinEnterpriseReadonlyControllersSid,
		WELL_KNOWN_SID_TYPE::WinAccountReadonlyControllersSid,
		WELL_KNOWN_SID_TYPE::WinBuiltinEventLogReadersGroup,
		WELL_KNOWN_SID_TYPE::WinNewEnterpriseReadonlyControllersSid,
		WELL_KNOWN_SID_TYPE::WinBuiltinCertSvcDComAccessGroup,
		WELL_KNOWN_SID_TYPE::WinMediumPlusLabelSid,
		WELL_KNOWN_SID_TYPE::WinLocalLogonSid,
		WELL_KNOWN_SID_TYPE::WinConsoleLogonSid,
		WELL_KNOWN_SID_TYPE::WinThisOrganizationCertificateSid,
		WELL_KNOWN_SID_TYPE::WinApplicationPackageAuthoritySid,
		WELL_KNOWN_SID_TYPE::WinBuiltinAnyPackageSid,
		WELL_KNOWN_SID_TYPE::WinCapabilityInternetClientSid,
		WELL_KNOWN_SID_TYPE::WinCapabilityInternetClientServerSid,
		WELL_KNOWN_SID_TYPE::WinCapabilityPrivateNetworkClientServerSid,
		WELL_KNOWN_SID_TYPE::WinCapabilityPicturesLibrarySid,
		WELL_KNOWN_SID_TYPE::WinCapabilityVideosLibrarySid,
		WELL_KNOWN_SID_TYPE::WinCapabilityMusicLibrarySid,
		WELL_KNOWN_SID_TYPE::WinCapabilityDocumentsLibrarySid,
		WELL_KNOWN_SID_TYPE::WinCapabilitySharedUserCertificatesSid,
		WELL_KNOWN_SID_TYPE::WinCapabilityEnterpriseAuthenticationSid,
		WELL_KNOWN_SID_TYPE::WinCapabilityRemovableStorageSid,
		WELL_KNOWN_SID_TYPE::WinBuiltinRDSRemoteAccessServersSid,
		WELL_KNOWN_SID_TYPE::WinBuiltinRDSEndpointServersSid,
		WELL_KNOWN_SID_TYPE::WinBuiltinRDSManagementServersSid,
		WELL_KNOWN_SID_TYPE::WinUserModeDriversSid,
		WELL_KNOWN_SID_TYPE::WinBuiltinHyperVAdminsSid,
		WELL_KNOWN_SID_TYPE::WinAccountCloneableControllersSid,
		WELL_KNOWN_SID_TYPE::WinBuiltinAccessControlAssistanceOperatorsSid,
		WELL_KNOWN_SID_TYPE::WinBuiltinRemoteManagementUsersSid,
		WELL_KNOWN_SID_TYPE::WinAuthenticationAuthorityAssertedSid,
		WELL_KNOWN_SID_TYPE::WinAuthenticationServiceAssertedSid,
		WELL_KNOWN_SID_TYPE::WinLocalAccountSid,
		WELL_KNOWN_SID_TYPE::WinLocalAccountAndAdministratorSid,
		WELL_KNOWN_SID_TYPE::WinAccountProtectedUsersSid,
		WELL_KNOWN_SID_TYPE::WinCapabilityAppointmentsSid,
		WELL_KNOWN_SID_TYPE::WinCapabilityContactsSid,
		WELL_KNOWN_SID_TYPE::WinAccountDefaultSystemManagedSid,
		WELL_KNOWN_SID_TYPE::WinBuiltinDefaultSystemManagedGroupSid,
		WELL_KNOWN_SID_TYPE::WinBuiltinStorageReplicaAdminsSid,
		WELL_KNOWN_SID_TYPE::WinAccountKeyAdminsSid,
		WELL_KNOWN_SID_TYPE::WinAccountEnterpriseKeyAdminsSid,
		WELL_KNOWN_SID_TYPE::WinAuthenticationKeyTrustSid,
		WELL_KNOWN_SID_TYPE::WinAuthenticationKeyPropertyMFASid,
		WELL_KNOWN_SID_TYPE::WinAuthenticationKeyPropertyAttestationSid,
		WELL_KNOWN_SID_TYPE::WinAuthenticationFreshKeyAuthSid,
		WELL_KNOWN_SID_TYPE::WinBuiltinDeviceOwnersSid
	};
	const wchar_t* wksidnames[] = {
		L"WinNullSid",
		L"WinWorldSid",
		L"WinLocalSid",
		L"WinCreatorOwnerSid",
		L"WinCreatorGroupSid",
		L"WinCreatorOwnerServerSid",
		L"WinCreatorGroupServerSid",
		L"WinNtAuthoritySid",
		L"WinDialupSid",
		L"WinNetworkSid",
		L"WinBatchSid",
		L"WinInteractiveSid",
		L"WinServiceSid",
		L"WinAnonymousSid",
		L"WinProxySid",
		L"WinEnterpriseControllersSid",
		L"WinSelfSid",
		L"WinAuthenticatedUserSid",
		L"WinRestrictedCodeSid",
		L"WinTerminalServerSid",
		L"WinRemoteLogonIdSid",
		L"WinLogonIdsSid",
		L"WinLocalSystemSid",
		L"WinLocalServiceSid",
		L"WinNetworkServiceSid",
		L"WinBuiltinDomainSid",
		L"WinBuiltinAdministratorsSid",
		L"WinBuiltinUsersSid",
		L"WinBuiltinGuestsSid",
		L"WinBuiltinPowerUsersSid",
		L"WinBuiltinAccountOperatorsSid",
		L"WinBuiltinSystemOperatorsSid",
		L"WinBuiltinPrintOperatorsSid",
		L"WinBuiltinBackupOperatorsSid",
		L"WinBuiltinReplicatorSid",
		L"WinBuiltinPreWindows2000CompatibleAccessSid",
		L"WinBuiltinRemoteDesktopUsersSid",
		L"WinBuiltinNetworkConfigurationOperatorsSid",
		L"WinAccountAdministratorSid",
		L"WinAccountGuestSid",
		L"WinAccountKrbtgtSid",
		L"WinAccountDomainAdminsSid",
		L"WinAccountDomainUsersSid",
		L"WinAccountDomainGuestsSid",
		L"WinAccountComputersSid",
		L"WinAccountControllersSid",
		L"WinAccountCertAdminsSid",
		L"WinAccountSchemaAdminsSid",
		L"WinAccountEnterpriseAdminsSid",
		L"WinAccountPolicyAdminsSid",
		L"WinAccountRasAndIasServersSid",
		L"WinNTLMAuthenticationSid",
		L"WinDigestAuthenticationSid",
		L"WinSChannelAuthenticationSid",
		L"WinThisOrganizationSid",
		L"WinOtherOrganizationSid",
		L"WinBuiltinIncomingForestTrustBuildersSid",
		L"WinBuiltinPerfMonitoringUsersSid",
		L"WinBuiltinPerfLoggingUsersSid",
		L"WinBuiltinAuthorizationAccessSid",
		L"WinBuiltinTerminalServerLicenseServersSid",
		L"WinBuiltinDCOMUsersSid",
		L"WinBuiltinIUsersSid",
		L"WinIUserSid",
		L"WinBuiltinCryptoOperatorsSid",
		L"WinUntrustedLabelSid",
		L"WinLowLabelSid",
		L"WinMediumLabelSid",
		L"WinHighLabelSid",
		L"WinSystemLabelSid",
		L"WinWriteRestrictedCodeSid",
		L"WinCreatorOwnerRightsSid",
		L"WinCacheablePrincipalsGroupSid",
		L"WinNonCacheablePrincipalsGroupSid",
		L"WinEnterpriseReadonlyControllersSid",
		L"WinAccountReadonlyControllersSid",
		L"WinBuiltinEventLogReadersGroup",
		L"WinNewEnterpriseReadonlyControllersSid",
		L"WinBuiltinCertSvcDComAccessGroup",
		L"WinMediumPlusLabelSid",
		L"WinLocalLogonSid",
		L"WinConsoleLogonSid",
		L"WinThisOrganizationCertificateSid",
		L"WinApplicationPackageAuthoritySid",
		L"WinBuiltinAnyPackageSid",
		L"WinCapabilityInternetClientSid",
		L"WinCapabilityInternetClientServerSid",
		L"WinCapabilityPrivateNetworkClientServerSid",
		L"WinCapabilityPicturesLibrarySid",
		L"WinCapabilityVideosLibrarySid",
		L"WinCapabilityMusicLibrarySid",
		L"WinCapabilityDocumentsLibrarySid",
		L"WinCapabilitySharedUserCertificatesSid",
		L"WinCapabilityEnterpriseAuthenticationSid",
		L"WinCapabilityRemovableStorageSid",
		L"WinBuiltinRDSRemoteAccessServersSid",
		L"WinBuiltinRDSEndpointServersSid",
		L"WinBuiltinRDSManagementServersSid",
		L"WinUserModeDriversSid",
		L"WinBuiltinHyperVAdminsSid",
		L"WinAccountCloneableControllersSid",
		L"WinBuiltinAccessControlAssistanceOperatorsSid",
		L"WinBuiltinRemoteManagementUsersSid",
		L"WinAuthenticationAuthorityAssertedSid",
		L"WinAuthenticationServiceAssertedSid",
		L"WinLocalAccountSid",
		L"WinLocalAccountAndAdministratorSid",
		L"WinAccountProtectedUsersSid",
		L"WinCapabilityAppointmentsSid",
		L"WinCapabilityContactsSid",
		L"WinAccountDefaultSystemManagedSid",
		L"WinBuiltinDefaultSystemManagedGroupSid",
		L"WinBuiltinStorageReplicaAdminsSid",
		L"WinAccountKeyAdminsSid",
		L"WinAccountEnterpriseKeyAdminsSid",
		L"WinAuthenticationKeyTrustSid",
		L"WinAuthenticationKeyPropertyMFASid",
		L"WinAuthenticationKeyPropertyAttestationSid",
		L"WinAuthenticationFreshKeyAuthSid",
		L"WinBuiltinDeviceOwnersSid"
	};
	SysHandler sys;
	PSID tpsid = 0;
	unsigned long sidlen = SECURITY_MAX_SID_SIZE;
	for (size_t i = 0; i < 120; ++i) {
		sidlen = SECURITY_MAX_SID_SIZE;
		tpsid = malloc(sidlen);
		if (tpsid) {
			if (::CreateWellKnownSid((WELL_KNOWN_SID_TYPE)wksids[i], domainSID, tpsid, &sidlen)) {
				ret.emplace_back(sys.StrSIDFromSID(tpsid), sys.GetAccountNameFromSID(tpsid), wksidnames[i]);
			}
			SAFE_FREE(tpsid);
		} else {
			return ret;
		}
	}
	return ret;
}

AccountDesc::AccountDesc() {
	ncNormalAcc = false;
	ncTempDupAcc = false;
	ncWorkstationTrustAcc = false;
	ncServerTrustAcc = false;
	ncInterdomainTrustAcc = false;
	accountLocked = false;
	accountDisabled = false;
	accountNotDelegated = false;
	accountDESKeysOnly = false;
	accountNoKerberosPreauth = false;
	accountTrustedToAuthForDelegation = false;
	accountTrustedForDelegation = false;
	logonScriptExecuted = false;
	passwordNotNeeded = false;
	passwordCantChange = false;
	passwordNoExpire = false;
	passwordReverseEncryption = false;
	passwordSmartCardNeeded = false;
	passwordExpired = false;
	passwordEncrypted = false;
	authPrintOp = false;
	authCommOp = false;
	authSrvOp = false;
	authAccOp = false;
	privileges = 0;
	passwordAge = 0;
	passwordBadCount = 0;
	lastLogOn = 0;
	lastLogOff = 0;
	expireDate = 0;
	maxStorage = 0;
	logonCount = 0;
	countryCode = 0;
	codePage = 0;
	primaryGroupId = 0;
}

AccountDesc::AccountDesc(const AccountDesc &other) {
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
}

AccountDesc::~AccountDesc() {}

GroupDesc::GroupDesc() {
	IsADGroup = false;
}

GroupDesc::GroupDesc(const GroupDesc &other) {
	IsADGroup = other.IsADGroup;
	GroupName = other.GroupName;
	GroupStrSid = other.GroupStrSid;
	Comment = other.Comment;
}

GroupDesc::~GroupDesc() {}

SysHandler::SysHandler() {}

SysHandler::SysHandler(const SysHandler &other) {}

SysHandler::~SysHandler() {}

std::wstring SysHandler::GetMachineName() const {
	unsigned long nameLen = MAX_COMPUTERNAME_LENGTH;
	wchar_t compName[MAX_COMPUTERNAME_LENGTH + 1] = { 0 };
	if (::GetComputerName(compName, &nameLen)) {
		return compName;
	} else {
		return L"";
	}
}

SysArch SysHandler::GetMachineArch() const {
	unsigned int x64test = ::GetSystemWow64DirectoryA(0, 0);
	if (::GetLastError() == ERROR_CALL_NOT_IMPLEMENTED) {
		return SysArch::X32;
	} else {
		return SysArch::X64;
	}
}

bool SysHandler::IsWow64Proc () const {
	int isWow64 = 0;
	LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
		::GetModuleHandle(L"kernel32"), "IsWow64Process");
	if(fnIsWow64Process) {
		if (fnIsWow64Process(GetCurrentProcess(), isWow64)) {
			if (isWow64) {
				return true;
			} else {
				return false;
			}
		} else {
			return false;
		}
	} else {
		return false;
	}
	return false;
}

bool SysHandler::IsWow64Proc(const unsigned long pid, const unsigned long desiredProcRights) const {
	int isWow64 = 0;
	LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
		::GetModuleHandle(L"kernel32"), "IsWow64Process");
	if (fnIsWow64Process) {
		HANDLE hProc = ::OpenProcess(desiredProcRights, true, pid);
		if (hProc && INVALID_HANDLE_VALUE != hProc && hProc) {
			if (fnIsWow64Process(hProc, isWow64)) {
				if (isWow64) {
					::CloseHandle(hProc);
					return true;
				} else {
					::CloseHandle(hProc);
					return false;
				}
			} else {
				::CloseHandle(hProc);
				return false;
			}
		} else {
			return false;
		}
	} else {
		return false;
	}
	return false;
}

bool SysHandler::ExtractResource(const std::wstring extractPath, const unsigned long resId) const {
	HRSRC hResource = 0;
	hResource = FindResource(NULL, MAKEINTRESOURCE(resId), RT_RCDATA);
	if (hResource) {
		HGLOBAL hGlobal = ::LoadResource(NULL, hResource);
		if (hGlobal) {
			DWORD exeSize = ::SizeofResource(NULL, hResource);
			if (exeSize) {
				void* exeBuf = ::LockResource(hGlobal);
				if (exeBuf) {
					std::ofstream outfile(extractPath.c_str(), std::ios::binary);
					if (outfile.is_open()) {
						int res = (outfile.write((char*)exeBuf, exeSize)) ? 0 : 7;
						outfile.close();
						if (!res) {
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}

std::wstring SysHandler::GetStrSIDFromAccountName(const std::wstring accName, const std::wstring machineName,
	const bool isDomainAcc) const {
	unsigned long domainNameLen = 256, sidlen = 128;
	NEW_ARR_NULLIFY(buf, wchar_t, sidlen);
	NEW_ARR_NULLIFY(domainName, wchar_t, domainNameLen);
	// wchar_t buf[128] = { 0 };
	// wchar_t domainName[256] = { 0 };
	wchar_t* sidstr = { 0 };
	SID_NAME_USE sidType;
	/*std::wstring fullName = machineName.length() ?
		machineName + L"\\" + accName : L".\\" + accName;*/
	if (::LookupAccountName(machineName.c_str(), accName.c_str(), buf, &sidlen, domainName,
		&domainNameLen, &sidType)) {
		SID* sid = (SID*)buf;
		if (::ConvertSidToStringSid(sid, &sidstr)) {
			std::wstring ret = sidstr;
			SAFE_NetApiBufferFree(sidstr);
			SAFE_ARR_DELETE(buf);
			SAFE_ARR_DELETE(domainName);
			return ret;
		} else {
			return L"";
		}
	} else {
		return L"";
	}
}

PSID SysHandler::GetSIDFromAccountName(const std::wstring accName,
	const std::wstring machineName, const bool isDomainAcc) const {
	// unsigned char* buf = (unsigned char*)malloc(128 * sizeof(unsigned char));
	NEW_ARR_NULLIFY(buf, unsigned char, 128);
	// unsigned char buf[128] = { 0 };
	wchar_t domainName[128] = { 0 };
	SID_NAME_USE sidType;
	unsigned long domainNameLen = 128, sidlen = 128;
	/*std::wstring fullName = machineName.length() ?
		machineName + L"\\" + accName : L".\\" + accName;*/
	if (::LookupAccountName(machineName.c_str(), accName.c_str(), (PSID)buf, &sidlen, domainName,
		&domainNameLen, &sidType)) {
		return (PSID)buf;
	} else {
		return 0;
	}
}

unsigned long long SysHandler::CurrentTime2Unixtime() const {
	time_t ltime;
	return time(&ltime);
}

long SysHandler::GetTZOffset(time_t when) const {
	if (when == NULL_TIME) {
		when = std::time(nullptr);
	}
	auto const tm = *std::localtime(&when);
	std::ostringstream os;
	os << std::put_time(&tm, "%z");
	std::string s = os.str();
	// s is in ISO 8601 format: "±HHMM"
	char h = std::stoi(s.substr(0, 3), nullptr, 10);
	char m = std::stoi(s[0] + s.substr(3), nullptr, 10);
	return ((3600 * h) + (60 * m));
}

std::wstring SysHandler::UnixTime2StrTime(const time_t& timestamp) const {
	enum { numCStringSize = 0x80 };
	const char strTimestampFmt[] = "%a, %Y-%b-%d, %H:%M:%S";
	char timeCStr[numCStringSize];
	std::tm* tmInfo = gmtime(&timestamp);
	std::strftime(&timeCStr[0], numCStringSize, strTimestampFmt, tmInfo);
	return str2wstr(std::string(timeCStr));
}

unsigned char SysHandler::GetCPUCoreNum() const {
	return (unsigned char)std::thread::hardware_concurrency();
}

unsigned char SysHandler::GetThreadNum() const {
	return (unsigned char)(std::thread::hardware_concurrency() - 1);
}

SysOpResult SysHandler::GetSIDType(const PSID sid, SidType &sidType, const std::wstring machineName, const std::wstring domainName) const {
	if (sid) {
		unsigned long nameLen = 256, domainNameLen = 256;
		NEW_ARR_NULLIFY(nameBuf, wchar_t, nameLen);
		if (nameBuf) {
			NEW_ARR_NULLIFY(domainNameBuf, wchar_t, domainNameLen);
			if (domainNameBuf) {
				swprintf(domainNameBuf, L"%s", domainName.c_str());
				SID_NAME_USE sidUse;
				if (::LookupAccountSid(machineName.c_str(), sid, nameBuf, &nameLen, domainNameBuf, &domainNameLen, &sidUse)) {
					SAFE_ARR_DELETE(nameBuf);
					SAFE_ARR_DELETE(domainNameBuf);
					sidType = (SidType)sidUse;
					return SysOpResult::Success;
				} else {
					SAFE_ARR_DELETE(nameBuf);
					SAFE_ARR_DELETE(domainNameBuf);
					sidType = SidType::SidUnknown;
					return SysOpResult::Fail;
				}
			} else {
				SAFE_ARR_DELETE(nameBuf);
				return SysOpResult::Fail;
			}
		} else {
			return SysOpResult::Fail;
		}
	} else {
		return SysOpResult::Fail;
	}
}

SysOpResult SysHandler::GetRam(unsigned long long &freeRam,
	unsigned long long &totalRam, unsigned char &precentInUse) const {
	MEMORYSTATUSEX statex;
	memset(&statex, 0, sizeof(MEMORYSTATUSEX));
	statex.dwLength = sizeof(statex);
	if (::GlobalMemoryStatusEx(&statex)) {
		freeRam = statex.ullAvailPhys;
		totalRam = statex.ullTotalPhys;
		precentInUse = statex.dwMemoryLoad;
		return SysOpResult::Success;
	} else {
		return SysOpResult::Fail;
	}
}

SysOpResult SysHandler::GetPageFile(unsigned long long &freePageFile,
	unsigned long long &totalPageFile) const {
	MEMORYSTATUSEX statex;
	memset(&statex, 0, sizeof(MEMORYSTATUSEX));
	statex.dwLength = sizeof(statex);
	if (::GlobalMemoryStatusEx(&statex)) {
		freePageFile = statex.ullAvailPageFile;
		totalPageFile = statex.ullTotalPageFile;
		return SysOpResult::Success;
	} else {
		return SysOpResult::Fail;
	}
}

SysOpResult SysHandler::GetVirtualMem(unsigned long long &freeVirtMem,
	unsigned long long &totalVirtMem) const {
	MEMORYSTATUSEX statex;
	memset(&statex, 0, sizeof(MEMORYSTATUSEX));
	statex.dwLength = sizeof(statex);
	if (::GlobalMemoryStatusEx(&statex)) {
		freeVirtMem = statex.ullAvailVirtual;
		totalVirtMem = statex.ullTotalVirtual;
		return SysOpResult::Success;
	} else {
		return SysOpResult::Fail;
	}
}

PSID SysHandler::SIDFromStrSid(const std::wstring sidstr) const {
	if (sidstr.length()) {
		PSID tsid = 0;
		if (::ConvertStringSidToSid(sidstr.c_str(), &tsid)) {
			return tsid;
		} else {
			return 0;
		}
	} else {
		return 0;
	}
}

std::wstring SysHandler::StrSIDFromSID(const PSID sid) const {
	wchar_t* sbuf = 0;
	std::wstring ret;
	if (::ConvertSidToStringSid(sid, &sbuf)) {
		ret = sbuf;
		SAFE_LOCALFREE(sbuf);
		return ret;
	}
	return L"";
}

std::wstring SysHandler::GetAccountNameFromSID(const PSID sid, const std::wstring machineName) const {
	if (sid) {
		unsigned long nameLen = MAX_PATH, domainNameLen = MAX_PATH;
		SID_NAME_USE sidUse;
		NEW_ARR_NULLIFY(nameBuf, wchar_t, nameLen + 1);
		NEW_ARR_NULLIFY(domainNameBuf, wchar_t, domainNameLen + 1);
		if (nameBuf && domainNameBuf) {
			if (::LookupAccountSid(machineName.c_str(), sid, nameBuf, &nameLen,
				domainNameBuf, &domainNameLen, &sidUse)) {
				std::wstring ret = nameBuf;
				SAFE_ARR_DELETE(nameBuf);
				SAFE_ARR_DELETE(domainNameBuf);
				return ret;
			} else {
				SAFE_ARR_DELETE(nameBuf);
				SAFE_ARR_DELETE(domainNameBuf);
				return L"";
			}
		}
		SAFE_ARR_DELETE(nameBuf);
		SAFE_ARR_DELETE(domainNameBuf);
		return L"";
	} else {
		return L"";
	}
}

std::wstring SysHandler::GetAccountNameFromStrSID(const std::wstring strSid,
	const std::wstring  machineName) const {
	std::wstring sid;
	if (strSid.length()) {
		if (startsWith(strSid, L"{")) {
			sid = removeFromStart_copy(strSid, L"{");
		}
		if (endsWith(strSid, L"}")) {
			sid = removeFromEnd_copy(strSid, L"}");
		}
		PSID tpsid = SIDFromStrSid(strSid);
		unsigned long nameLen = MAX_PATH, domainNameLen = MAX_PATH;
		SID_NAME_USE sidUse;
		NEW_ARR_NULLIFY(nameBuf, wchar_t, nameLen + 1);
		NEW_ARR_NULLIFY(domainNameBuf, wchar_t, domainNameLen + 1);
		// wchar_t* nameBuf = (wchar_t*)calloc(nameLen + 1, (nameLen + 1) * sizeof(wchar_t));
		// wchar_t* domainNameBuf = (wchar_t*)calloc(domainNameLen + 1, (domainNameLen + 1) * sizeof(wchar_t));
		if (nameBuf && domainNameBuf) {
			if (::LookupAccountSid(machineName.c_str(), tpsid, nameBuf, &nameLen,
				domainNameBuf, &domainNameLen, &sidUse)) {
				std::wstring ret = nameBuf;
				SAFE_LOCALFREE(tpsid);
				SAFE_ARR_DELETE(nameBuf);
				SAFE_ARR_DELETE(domainNameBuf);
				return ret;
			} else {
				SAFE_LOCALFREE(tpsid);
				SAFE_ARR_DELETE(nameBuf);
				SAFE_ARR_DELETE(domainNameBuf);
				return L"";
			}
		}
		SAFE_LOCALFREE(tpsid);
		SAFE_ARR_DELETE(nameBuf);
		SAFE_ARR_DELETE(domainNameBuf);
		return L"";
	} else {
		return L"";
	}
}

SysOpResult SysHandler::LocalGroupListFromUsername(std::vector<GroupDesc> &outGroupList,
	const std::wstring userName, const std::wstring machineName) const {
	size_t i = 0;
	unsigned char* buf = 0;
	unsigned char* tmpbuf = 0;
	unsigned long entriesRead = 0, totalEntries = 0, res = 0;
	do {
		res = ::NetUserGetLocalGroups(machineName.c_str(), userName.c_str(), 0, LG_INCLUDE_INDIRECT , &buf,
			MAX_PREFERRED_LENGTH, &entriesRead, &totalEntries);
		if (NERR_Success == res || ERROR_MORE_DATA == res) {
			if ((tmpbuf = buf) != 0) {
				LOCALGROUP_USERS_INFO_0* groups = (LOCALGROUP_USERS_INFO_0*)tmpbuf;
				for (i = 0; i < entriesRead; ++i) {
					GroupDesc elem;
					elem.IsADGroup = false;
					elem.GroupName = groups[i].lgrui0_name;
					elem.GroupStrSid = GetStrSIDFromAccountName(elem.GroupName, machineName);
					outGroupList.push_back(elem);
					++tmpbuf;
					++totalEntries;
				}
			}
			SAFE_NetApiBufferFree(buf);
		}
	} while (ERROR_MORE_DATA == res || NERR_Success != res);
	SAFE_NetApiBufferFree(buf);
	return SysOpResult::Success;
}

SysOpResult SysHandler::LocalGroupListFromStrSID(std::vector<GroupDesc> &outGroupList, const std::wstring strSID,
	const std::wstring machineName) const {
	std::wstring username = GetAccountNameFromStrSID(strSID, machineName);
	if (username.length()) {
		SysOpResult res = SysOpResult::Success;
		res = LocalGroupListFromUsername(outGroupList, username, machineName);
		if (SysOpResult::Success != res) {
			return res;
		}
	}
	return SysOpResult::Success;
}

SysOpResult SysHandler::UserLogon(HANDLE &token, std::wstring userName, std::wstring password,
	std::wstring domain) const {
	if (LogonUser(userName.c_str(), domain.c_str(), password.c_str(), LOGON32_LOGON_NEW_CREDENTIALS,
		LOGON32_PROVIDER_DEFAULT, &token)) {
		return SysOpResult::Success;
	} else {
		return SysOpResult::Fail;
	}
}

SysOpResult SysHandler::ImpersonateUser(HANDLE &token) const {
	if (ImpersonateLoggedOnUser(token)) {
		return SysOpResult::Success;
	} else {
		return SysOpResult::Fail;
	}
}

SysOpResult SysHandler::EnumLocalGroups(std::vector<GroupDesc> &groupList,
	const std::wstring machineName, const bool enumAccs,
	const std::vector<AccountDesc> *accList) const {
	size_t i = 0;
	unsigned char* buf = 0;
	unsigned char* tmpbuf = 0;
	unsigned long entriesRead = 0, totalEntries = 0, res = 0;
#ifdef _WIN64
	unsigned long long resptr = 0;
#else
	unsigned long resptr = 0;
#endif
	do {
		res = ::NetLocalGroupEnum(machineName.c_str(), 1, &buf, MAX_PREFERRED_LENGTH, &entriesRead,
			&totalEntries, &resptr);
		if (NERR_Success == res || ERROR_MORE_DATA == res) {
			if ((tmpbuf = buf) != 0) {
				LOCALGROUP_INFO_1* groups = (LOCALGROUP_INFO_1*)tmpbuf;
				for (i = 0; i < entriesRead; ++i) {
					GroupDesc elem;
					elem.IsADGroup = false;
					elem.GroupName = groups[i].lgrpi1_name;
					elem.Comment = groups[i].lgrpi1_comment;
					elem.GroupStrSid = GetStrSIDFromAccountName(elem.GroupName, machineName);
					groupList.push_back(elem);
					++tmpbuf;
					++totalEntries;
				}
			}
		}
		SAFE_NetApiBufferFree(buf);
	} while (ERROR_MORE_DATA == res || NERR_Success != res);
	SAFE_NetApiBufferFree(buf);
	if (enumAccs) {
		std::vector<AccountDesc> accs;
		if (enumAccs) {
			if (accList) {
				accs = *accList;
			} else {
				SysOpResult res = SysOpResult::Success;
				res = EnumAccounts(accs, machineName);
				if (SysOpResult::Success != res) {
					return res;
				}
			}
		}
		size_t j = 0, k = 0;
		buf = 0;
		tmpbuf = 0;
		entriesRead = 0;
		totalEntries = 0;
		res = 0;
		resptr = 0;
		for (i = 0; i < groupList.size(); ++i) {
			do {
				res = ::NetLocalGroupGetMembers(machineName.c_str(), groupList[i].GroupName.c_str(), 1,
					&buf, MAX_PREFERRED_LENGTH, &entriesRead, &totalEntries, &resptr);
				if (NERR_Success == res || ERROR_MORE_DATA == res) {
					if ((tmpbuf = buf) != 0) {
						LOCALGROUP_MEMBERS_INFO_1* groupmembers = (LOCALGROUP_MEMBERS_INFO_1*)tmpbuf;
						for (j = 0; j < entriesRead; ++j) {
							for (k = 0; k < accs.size(); ++k) {
								if (StrSIDFromSID(groupmembers[j].lgrmi1_sid) == accs[k].sid) {
									if (std::find(groupList[i].Accounts.begin(), groupList[i].Accounts.end(),
										accs[k]) == groupList[i].Accounts.end()) {
										groupList[i].Accounts.push_back(accs[k]);
									}
								}
							}
						}
						++tmpbuf;
						++totalEntries;
					}
				}
				SAFE_NetApiBufferFree(buf);
			} while (ERROR_MORE_DATA == res || NERR_Success != res);
			SAFE_NetApiBufferFree(buf);
		}
	}
	return SysOpResult::Success;
}

SysOpResult SysHandler::EnumAccounts(std::vector<AccountDesc> &accountList,
	const std::wstring machineName, const bool enumGroups) const {
	unsigned long entriesRead = 0, totalEntries = 0, resHandle = 0, res = 0;
	USER_INFO_20* buf = 0;
	USER_INFO_20* tmpbuf = 0;
	do {
		SAFE_NetApiBufferFree(buf);
		res = ::NetUserEnum(machineName.c_str(), 20,
			FILTER_TEMP_DUPLICATE_ACCOUNT | FILTER_NORMAL_ACCOUNT | FILTER_INTERDOMAIN_TRUST_ACCOUNT |
			FILTER_WORKSTATION_TRUST_ACCOUNT | FILTER_SERVER_TRUST_ACCOUNT,
			(unsigned char**)&buf, MAX_PREFERRED_LENGTH, &entriesRead, &totalEntries, &resHandle);
		if (NERR_Success == res || ERROR_MORE_DATA == res) {
			if ((tmpbuf = buf) != 0) {
				for (size_t i = 0; i < entriesRead; ++i) {
					AccountDesc elem;
					elem.ncNormalAcc = tmpbuf->usri20_flags & UF_NORMAL_ACCOUNT;
					elem.ncTempDupAcc = tmpbuf->usri20_flags & UF_TEMP_DUPLICATE_ACCOUNT;
					elem.ncWorkstationTrustAcc = tmpbuf->usri20_flags & UF_WORKSTATION_TRUST_ACCOUNT;
					elem.ncServerTrustAcc = tmpbuf->usri20_flags & UF_SERVER_TRUST_ACCOUNT;
					elem.ncInterdomainTrustAcc = tmpbuf->usri20_flags & UF_INTERDOMAIN_TRUST_ACCOUNT;
					elem.logonScriptExecuted = tmpbuf->usri20_flags & UF_SCRIPT;
					elem.accountDisabled = tmpbuf->usri20_flags & UF_ACCOUNTDISABLE;
					elem.passwordNotNeeded = tmpbuf->usri20_flags & UF_PASSWD_NOTREQD;
					elem.passwordCantChange = tmpbuf->usri20_flags & UF_PASSWD_CANT_CHANGE;
					elem.accountLocked = tmpbuf->usri20_flags & UF_LOCKOUT;
					elem.passwordNoExpire = tmpbuf->usri20_flags & UF_DONT_EXPIRE_PASSWD;
					elem.passwordEncrypted = tmpbuf->usri20_flags & UF_ENCRYPTED_TEXT_PASSWORD_ALLOWED;
					elem.accountNotDelegated = tmpbuf->usri20_flags & UF_NOT_DELEGATED;
					elem.passwordSmartCardNeeded = tmpbuf->usri20_flags & UF_SMARTCARD_REQUIRED;
					elem.accountDESKeysOnly = tmpbuf->usri20_flags & UF_USE_DES_KEY_ONLY;
					elem.accountNoKerberosPreauth = tmpbuf->usri20_flags & UF_DONT_REQUIRE_PREAUTH;
					elem.accountTrustedForDelegation = tmpbuf->usri20_flags & UF_TRUSTED_FOR_DELEGATION;
					elem.passwordExpired = tmpbuf->usri20_flags & UF_PASSWORD_EXPIRED;
					elem.accountTrustedToAuthForDelegation = tmpbuf->usri20_flags & UF_TRUSTED_TO_AUTHENTICATE_FOR_DELEGATION;
					if (!IsBadReadPtr(tmpbuf->usri20_name)) {
						elem.accountName = tmpbuf->usri20_name;
						elem.sid = GetStrSIDFromAccountName(elem.accountName, machineName);
					}
					/*if (elem.sid.length() && !elem.accountName.length()) {
						elem.accountName = GetAccountNameFromSID(SIDFromStrSid(elem.sid), machineName);
					}*/
					if (!IsBadReadPtr(tmpbuf->usri20_full_name)) {
						elem.accountFullname = tmpbuf->usri20_full_name;
					}
					if (!IsBadReadPtr(tmpbuf->usri20_comment)) {
						elem.comment = tmpbuf->usri20_comment;
					}
					if (enumGroups) {
						if (SysOpResult::Success != LocalGroupListFromUsername(elem.groups,
							elem.accountName, machineName)) {
							return SysOpResult::Fail;
						}
					}
					accountList.push_back(elem);
					++tmpbuf;
					++totalEntries;
				}
			}
		}
	} while (ERROR_MORE_DATA == res || NERR_Success != res);
	SAFE_NetApiBufferFree(buf);
	USER_INFO_4* buf4 = 0;
	AccountDesc* rec = 0;
	for (size_t i = 0; i < accountList.size(); ++i) {
		res = ::NetUserGetInfo(machineName.c_str(), accountList[i].accountName.c_str(), 4,
			(unsigned char**)&buf4);
		if (NERR_Success == res) {
			if (accountList[i].accountName == buf4->usri4_name) {
				rec = &accountList[i];
				rec->privileges = buf4->usri4_priv;
				if (0 == rec->privileges) {
					rec->privilegesStr = L"Guest (" + std::to_wstring(rec->privileges) + L")";
				} else if (1 == rec->privileges) {
					rec->privilegesStr = L"User (" + std::to_wstring(rec->privileges) + L")";
				} else if (2 == rec->privileges) {
					rec->privilegesStr = L"Admin (" + std::to_wstring(rec->privileges) + L")";
				}
				rec->logonScriptPath = buf4->usri4_script_path;
				rec->homeDirPath = buf4->usri4_home_dir;
				rec->homeDirPath = buf4->usri4_home_dir_drive;
				rec->profileDirPath = buf4->usri4_profile;
				rec->params = buf4->usri4_parms;
				rec->allowedWorkstations = buf4->usri4_workstations;
				/*if (!wcslen_c(rec->allowedWorkstations.c_str())) {
					rec->allowedWorkstations = L"\\\\*";
				}*/
				rec->passwordAge = buf4->usri4_password_age;
				if (rec->passwordAge) {
					rec->passwordSetStr = UnixTime2StrTime((CurrentTime2Unixtime() - rec->passwordAge) + GetTZOffset());
				} else {
					rec->passwordSetStr = L"Password never changed";
				}
				rec->lastLogOn = buf4->usri4_last_logon;
				if (rec->lastLogOn) {
					rec->lastLogOnStr = UnixTime2StrTime(rec->lastLogOn + GetTZOffset());
				} else {
					rec->lastLogOnStr = L"User never logged on";
				}
				rec->lastLogOff = buf4->usri4_last_logoff;
				if (rec->lastLogOff) {
					rec->lastLogOffStr = UnixTime2StrTime(rec->lastLogOff);
				} else {
					// rec->lastLogOffStr = UnixTime2StrTime(CurrentTime2Unixtime() + GetTZOffset());
					if (rec->lastLogOn) {
						// Is user session running
						rec->lastLogOffStr = L"Session running";
					} else {
						rec->lastLogOffStr = L"User never logged on";
					}
				}
				rec->authPrintOp = buf4->usri4_auth_flags & AF_OP_PRINT;
				rec->authCommOp = buf4->usri4_auth_flags & AF_OP_COMM;
				rec->authSrvOp = buf4->usri4_auth_flags & AF_OP_SERVER;
				rec->authAccOp = buf4->usri4_auth_flags & AF_OP_ACCOUNTS;
				rec->expireDate = buf4->usri4_acct_expires;
				if ((rec->expireDate != TIMEQ_FOREVER) && (rec->expireDate > 0) && rec->expireDate) {
					rec->expireDateStr = UnixTime2StrTime(rec->expireDate);
				} else {
					rec->expireDateStr = L"Never";
				}
				rec->maxStorage = buf4->usri4_max_storage;
				if (USER_MAXSTORAGE_UNLIMITED == rec->maxStorage) {
					rec->maxStorageStr = L"Unlimited";
				} else {
					rec->maxStorageStr = std::to_wstring(rec->maxStorage / 1048576) + L" MB";
				}
				rec->passwordBadCount = buf4->usri4_bad_pw_count;
				rec->logonCount = buf4->usri4_num_logons;
				rec->countryCode = buf4->usri4_country_code;
				rec->codePage = buf4->usri4_code_page;
				rec->primaryGroupId = buf4->usri4_primary_group_id;
				rec->logonServer = buf4->usri4_logon_server;
			}
		}
		SAFE_NetApiBufferFree(buf4);
	}
	return SysOpResult::Success;
}

SysOpResult SysHandler::IsAccountMemberOfGroup(const PSID groupSID, const PSID testSID, bool &isMember,
	const std::wstring machineName) const {
	isMember = false;
	std::vector<GroupDesc> groups;
	std::wstring teststrsid = StrSIDFromSID(testSID);
	std::wstring groupstrsid = StrSIDFromSID(groupSID);
	if (SysOpResult::Success == LocalGroupListFromStrSID(groups, groupstrsid, machineName)) {
		for (size_t i = 0; i < groups.size(); ++i) {
			if (lower_copy(teststrsid) == lower_copy(groups[i].GroupStrSid)) {
				isMember = true;
				return SysOpResult::Success;
			}
		}
	} else {
		return SysOpResult::Fail;
	}
	return SysOpResult::Success;
}

SysOpResult SysHandler::IsAccountMemberOfGroup(const std::wstring groupName, const std::wstring testAccName,
	bool &isMember, const std::wstring machineName) const {
	isMember = false;
	std::vector<GroupDesc> groups;
	if (SysOpResult::Success == LocalGroupListFromUsername(groups, testAccName, machineName)) {
		for (size_t i = 0; i < groups.size(); ++i) {
			if (lower_copy(groupName) == lower_copy(groups[i].GroupName)) {
				isMember = true;
				return SysOpResult::Success;
			}
		}
	} else {
		return SysOpResult::Fail;
	}
	return SysOpResult::Success;
}