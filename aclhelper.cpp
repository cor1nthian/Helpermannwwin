#include <iostream>
#include "aclhelper.h"

SecDesc::SecDesc() {
    daclInfoSz = 0;
    saclInfoSz = 0;
    ownerInfoSz = 0;
    primaryGroupInfoSz = 0;
    daclInfo = 0;
    saclInfo = 0;
    ownerInfo = 0;
    primaryGroupInfo = 0;
}

SecDesc::SecDesc(const SecDesc &other) {
    daclInfoSz = other.daclInfoSz;
    saclInfoSz = other.saclInfoSz;
    ownerInfoSz = other.ownerInfoSz;
    primaryGroupInfoSz = other.primaryGroupInfoSz;
    ownerInfo = other.ownerInfo;
    primaryGroupInfo = other.primaryGroupInfo;
    if (other.daclInfo) {
        daclInfo = LocalAlloc(LPTR, daclInfoSz);
        if (daclInfo) {
            memcpy(daclInfo, other.daclInfo, daclInfoSz);
        }
    } else {
        if (daclInfo) {
            SAFE_LOCALFREE(daclInfo);
        }
        daclInfo = 0;
    }
    if (other.saclInfo) {
        saclInfo = LocalAlloc(LPTR, saclInfoSz);
        if (saclInfo) {
            memcpy(saclInfo, other.saclInfo, saclInfoSz);
        }
    } else {
        if (saclInfo) {
            SAFE_LOCALFREE(saclInfo);
        }
        saclInfo = 0;
    }
}

SecDesc::~SecDesc() {
    if (daclInfo) {
        SAFE_LOCALFREE(daclInfo);
    }
    if (saclInfo) {
        SAFE_LOCALFREE(saclInfo);
    }
    if (ownerInfo) {
        SAFE_LOCALFREE(ownerInfo);
    }
    if (primaryGroupInfo) {
        SAFE_LOCALFREE(primaryGroupInfo);
    }
}

ACLHandler::ACLHandler() {}

ACLHandler::ACLHandler(const ACLHandler& other) {}

ACLHandler::~ACLHandler() {}

ACLOpResult ACLHandler::SecurityDescriptor2StringSecurityDescriptor(unsigned char* &secDesc,
	std::wstring &textSecDesc, SecInfo secInfo) const  {
	wchar_t* buf = 0;
	if (::ConvertSecurityDescriptorToStringSecurityDescriptor((SECURITY_DESCRIPTOR*)secDesc,
		SDDL_REVISION_1, static_cast<unsigned long>(secInfo), &buf, 0)) {
		textSecDesc = buf;
        SAFE_LOCALFREE(buf);
		return ACLOpResult::Success;
	}
	return ACLOpResult::Fail;
}

ACLOpResult ACLHandler::StringSecurityDescriptor2SecurityDescriptor(const std::wstring textSecDesc,
    unsigned char* &secDesc, unsigned long &secDeszSz) const {
    PSECURITY_DESCRIPTOR sd = 0;
    if (::ConvertStringSecurityDescriptorToSecurityDescriptor(textSecDesc.c_str(),
        SDDL_REVISION_1, &sd, &secDeszSz)) {
        if (!secDesc) {
            NEW_ARR_NULLIFY_NO_REDEFINE(secDesc, unsigned char, secDeszSz);
            if (secDesc) {
                memcpy(secDesc, sd, secDeszSz);
                SAFE_LOCALFREE(sd);
                return ACLOpResult::Success;
            } else {
                SAFE_LOCALFREE(sd);
                return ACLOpResult::Fail;
            }
        } else {
            memcpy(secDesc, sd, secDeszSz);
            SAFE_LOCALFREE(sd);
            return ACLOpResult::Success;
        }
    }
    return ACLOpResult::Fail;
}

ACLOpResult ACLHandler::DACLReadAllowed(bool &allowed, ACL* testACL, PSID sid) const {
    void *testace = 0;
    SysHandler sys;
    SidType specsidype;
    if (SysOpResult::Success != sys.GetSIDType(sid, specsidype)) {
        return ACLOpResult::Fail;
    }
    EXPLICIT_ACCESS *eaEntry = 0;
    for (size_t i = 0; i < testACL->AceCount; ++i) {
        if (i == 6) {
            Sleep(1);
        }
        if (!GetAce(testACL, i, (void**)&testace)) {
            return ACLOpResult::Fail;
        }
        ACE_HEADER* vace = (ACE_HEADER*)testace;
        if (vace->AceType == ACCESS_ALLOWED_ACE_TYPE) {
            std::cout << "(" << i << ")\t Access allowed ACE" << std::endl;
            ACCESS_ALLOWED_ACE *aceAllowed = (ACCESS_ALLOWED_ACE*)testace;
            PSID accsid = (PSID)&aceAllowed->SidStart;
            SidType sidtype;
            if (SysOpResult::Success != sys.GetSIDType(accsid, sidtype)) {
                LocalFree(&testace);
                continue;
                // return ACLOpResult::Fail;
            }
            if (SidType::User == specsidype) {
                if (SidType::User == sidtype) {
                    if (EqualSid(accsid, sid)) {
                        if (READ_CONTROL & aceAllowed->Mask) {
                            allowed = true;
                        }
                    }
                } else if (SidType::Group == sidtype) {
                    bool ismember = false;
                    if (SysOpResult::Success == sys.IsAccountMemberOfGroup(accsid, sid, ismember)) {
                        if (ismember) {
                            if (READ_CONTROL & aceAllowed->Mask) {
                                allowed = true;
                            }
                        }
                    } else {
                        return ACLOpResult::Fail;
                    }
                } else {
                    if (EqualSid(accsid, sid)) {
                        if (READ_CONTROL & aceAllowed->Mask) {
                            allowed = true;
                        }
                    }
                }
            } else if (SidType::Group == specsidype) {
                if (SidType::User == sidtype) {
                    bool ismember = false;
                    if (SysOpResult::Success == sys.IsAccountMemberOfGroup(sid, accsid, ismember)) {
                        if (ismember) {
                            if (READ_CONTROL & aceAllowed->Mask) {
                                allowed = true;
                            }
                        }
                    } else {
                        return ACLOpResult::Fail;
                    }
                } else if (SidType::Group == sidtype) {
                    if (EqualSid(accsid, sid)) {
                        if (READ_CONTROL & aceAllowed->Mask) {
                            allowed = false;
                        }
                    }
                } else {
                    if (EqualSid(accsid, sid)) {
                        if (READ_CONTROL & aceAllowed->Mask) {
                            allowed = true;
                        }
                    }
                }
            } else {
                if (EqualSid(accsid, sid)) {
                    if (READ_CONTROL & aceAllowed->Mask) {
                        allowed = true;
                    }
                }
            }
        } else  if (vace->AceType == ACCESS_DENIED_ACE_TYPE) {
            std::cout << "(" << i << ")\t Access denied ACE" << std::endl;
            ACCESS_DENIED_ACE *aceDenied = (ACCESS_DENIED_ACE*)testace;
            PSID accsid = (PSID)&aceDenied->SidStart;
            SidType sidtype;
            if (SysOpResult::Success != sys.GetSIDType(accsid, sidtype)) {
                LocalFree(&testace);
                continue;
                // return ACLOpResult::Fail;
            }
            if (SidType::User == specsidype) {
                if (SidType::User == sidtype) {
                    if (EqualSid(accsid, sid)) {
                        if (READ_CONTROL & aceDenied->Mask) {
                            allowed = false;
                        }
                    }
                } else if (SidType::Group == sidtype) {
                    bool ismember = false;
                    if (SysOpResult::Success == sys.IsAccountMemberOfGroup(accsid, sid, ismember)) {
                        if (ismember) {
                            if (READ_CONTROL & aceDenied->Mask) {
                                allowed = false;
                            }
                        }
                    } else {
                        return ACLOpResult::Fail;
                    }
                } else {
                    if (EqualSid(accsid, sid)) {
                        if (READ_CONTROL & aceDenied->Mask) {
                            allowed = true;
                        }
                    }
                }
            } else if (SidType::Group == specsidype) {
                if (SidType::User == sidtype) {
                    bool ismember = false;
                    if (SysOpResult::Success == sys.IsAccountMemberOfGroup(sid, accsid, ismember)) {
                        if (ismember) {
                            if (READ_CONTROL & aceDenied->Mask) {
                                allowed = false;
                            }
                        }
                    } else {
                        return ACLOpResult::Fail;
                    }
                } else if (SidType::Group == sidtype) {
                    if (EqualSid(accsid, sid)) {
                        if (READ_CONTROL & aceDenied->Mask) {
                            allowed = false;
                        }
                    }
                } else {
                    if (EqualSid(accsid, sid)) {
                        if (READ_CONTROL & aceDenied->Mask) {
                            allowed = true;
                        }
                    }
                }
            } else {
                if (EqualSid(accsid, sid)) {
                    if (READ_CONTROL & aceDenied->Mask) {
                        allowed = true;
                    }
                }
            }
        } else {
            std::cout << "(" << i << ")\t Undefined ACE" << std::endl;
        }
        vace = 0;
        LocalFree(&testace);
        testace = 0;
    }
    if (testace) {
        SAFE_LOCALFREE(testace);
    }
    return ACLOpResult::Success;
}

ACLOpResult ACLHandler::DACLWriteAllowed(bool &allowed, ACL* testACL, PSID sid) const {
    void* testace = 0;
    SysHandler sys;
    SidType specsidype;
    if (SysOpResult::Success != sys.GetSIDType(sid, specsidype)) {
        return ACLOpResult::Fail;
    }
    EXPLICIT_ACCESS* eaEntry = 0;
    for (size_t i = 0; i < testACL->AceCount; ++i) {
        if (!GetAce(testACL, i, (void**)&testace)) {
            return ACLOpResult::Fail;
        }
        ACE_HEADER* vace = (ACE_HEADER*)testace;
        if (vace->AceType == ACCESS_ALLOWED_ACE_TYPE) {
            std::cout << "(" << i << ")\t Access allowed ACE" << std::endl;
            ACCESS_ALLOWED_ACE* aceAllowed = (ACCESS_ALLOWED_ACE*)testace;
            PSID accsid = (PSID)&aceAllowed->SidStart;
            SidType sidtype;
            if (SysOpResult::Success != sys.GetSIDType(accsid, sidtype)) {
                LocalFree(&testace);
                continue;
                // return ACLOpResult::Fail;
            }
            if (SidType::User == specsidype) {
                if (SidType::User == sidtype) {
                    if (EqualSid(accsid, sid)) {
                        if (WRITE_DAC & aceAllowed->Mask) {
                            allowed = true;
                        }
                    }
                } else if (SidType::Group == sidtype) {
                    bool ismember = false;
                    if (SysOpResult::Success == sys.IsAccountMemberOfGroup(accsid, sid, ismember)) {
                        if (ismember) {
                            if (WRITE_DAC & aceAllowed->Mask) {
                                allowed = true;
                            }
                        }
                    } else {
                        return ACLOpResult::Fail;
                    }
                } else {
                    if (EqualSid(accsid, sid)) {
                        if (WRITE_DAC & aceAllowed->Mask) {
                            allowed = true;
                        }
                    }
                }
            } else if (SidType::Group == specsidype) {
                if (SidType::User == sidtype) {
                    bool ismember = false;
                    if (SysOpResult::Success == sys.IsAccountMemberOfGroup(sid, accsid, ismember)) {
                        if (ismember) {
                            if (WRITE_DAC & aceAllowed->Mask) {
                                allowed = true;
                            }
                        }
                    } else {
                        return ACLOpResult::Fail;
                    }
                } else if (SidType::Group == sidtype) {
                    if (EqualSid(accsid, sid)) {
                        if (WRITE_DAC & aceAllowed->Mask) {
                            allowed = false;
                        }
                    }
                } else {
                    if (EqualSid(accsid, sid)) {
                        if (WRITE_DAC & aceAllowed->Mask) {
                            allowed = true;
                        }
                    }
                }
            } else {
                if (EqualSid(accsid, sid)) {
                    if (WRITE_DAC & aceAllowed->Mask) {
                        allowed = true;
                    }
                }
            }
        } else  if (vace->AceType == ACCESS_DENIED_ACE_TYPE) {
            std::cout << "(" << i << ")\t Access denied ACE" << std::endl;
            ACCESS_DENIED_ACE* aceDenied = (ACCESS_DENIED_ACE*)testace;
            PSID accsid = (PSID)&aceDenied->SidStart;
            SidType sidtype;
            if (SysOpResult::Success != sys.GetSIDType(accsid, sidtype)) {
                LocalFree(&testace);
                continue;
                // return ACLOpResult::Fail;
            }
            if (SidType::User == specsidype) {
                if (SidType::User == sidtype) {
                    if (EqualSid(accsid, sid)) {
                        if (WRITE_DAC & aceDenied->Mask) {
                            allowed = false;
                        }
                    }
                }
                else if (SidType::Group == sidtype) {
                    bool ismember = false;
                    if (SysOpResult::Success == sys.IsAccountMemberOfGroup(accsid, sid, ismember)) {
                        if (ismember) {
                            if (WRITE_DAC & aceDenied->Mask) {
                                allowed = false;
                            }
                        }
                    } else {
                        return ACLOpResult::Fail;
                    }
                } else {
                    if (EqualSid(accsid, sid)) {
                        if (WRITE_DAC & aceDenied->Mask) {
                            allowed = true;
                        }
                    }
                }
            } else if (SidType::Group == specsidype) {
                if (SidType::User == sidtype) {
                    bool ismember = false;
                    if (SysOpResult::Success == sys.IsAccountMemberOfGroup(sid, accsid, ismember)) {
                        if (ismember) {
                            if (WRITE_DAC & aceDenied->Mask) {
                                allowed = false;
                            }
                        }
                    } else {
                        return ACLOpResult::Fail;
                    }
                } else if (SidType::Group == sidtype) {
                    if (EqualSid(accsid, sid)) {
                        if (WRITE_DAC & aceDenied->Mask) {
                            allowed = false;
                        }
                    }
                } else {
                    if (EqualSid(accsid, sid)) {
                        if (WRITE_DAC & aceDenied->Mask) {
                            allowed = true;
                        }
                    }
                }
            } else {
                if (EqualSid(accsid, sid)) {
                    if (WRITE_DAC & aceDenied->Mask) {
                        allowed = true;
                    }
                }
            }
        } else {
            std::cout << "(" << i << ")\t Undefined ACE" << std::endl;
        }
        vace = 0;
        LocalFree(&testace);
        testace = 0;
    }
    if (testace) {
        SAFE_LOCALFREE(testace);
    }
    return ACLOpResult::Success;
}

ACLOpResult ACLHandler::DACLExecuteAllowed(bool &allowed, ACL* testACL, PSID sid) const {
    return ACLOpResult::Success;
}

ACLOpResult ACLHandler::DACLDeleteAllowed(bool &allowed, ACL* testACL, PSID sid) const {
    void* testace = 0;
    SysHandler sys;
    SidType specsidype;
    if (SysOpResult::Success != sys.GetSIDType(sid, specsidype)) {
        return ACLOpResult::Fail;
    }
    EXPLICIT_ACCESS* eaEntry = 0;
    for (size_t i = 0; i < testACL->AceCount; ++i) {
        if (!GetAce(testACL, i, (void**)&testace)) {
            return ACLOpResult::Fail;
        }
        ACE_HEADER* vace = (ACE_HEADER*)testace;
        if (vace->AceType == ACCESS_ALLOWED_ACE_TYPE) {
            std::cout << "(" << i << ")\t Access allowed ACE" << std::endl;
            ACCESS_ALLOWED_ACE* aceAllowed = (ACCESS_ALLOWED_ACE*)testace;
            PSID accsid = (PSID)&aceAllowed->SidStart;
            SidType sidtype;
            if (SysOpResult::Success != sys.GetSIDType(accsid, sidtype)) {
                LocalFree(&testace);
                continue;
                // return ACLOpResult::Fail;
            }
            if (SidType::User == specsidype) {
                if (SidType::User == sidtype) {
                    if (EqualSid(accsid, sid)) {
                        if (DELETE & aceAllowed->Mask) {
                            allowed = true;
                        }
                    }
                } else if (SidType::Group == sidtype) {
                    bool ismember = false;
                    if (SysOpResult::Success == sys.IsAccountMemberOfGroup(accsid, sid, ismember)) {
                        if (ismember) {
                            if (DELETE & aceAllowed->Mask) {
                                allowed = true;
                            }
                        }
                    } else {
                        return ACLOpResult::Fail;
                    }
                } else {
                    if (EqualSid(accsid, sid)) {
                        if (DELETE & aceAllowed->Mask) {
                            allowed = true;
                        }
                    }
                }
            } else if (SidType::Group == specsidype) {
                if (SidType::User == sidtype) {
                    bool ismember = false;
                    if (SysOpResult::Success == sys.IsAccountMemberOfGroup(sid, accsid, ismember)) {
                        if (ismember) {
                            if (DELETE & aceAllowed->Mask) {
                                allowed = true;
                            }
                        }
                    } else {
                        return ACLOpResult::Fail;
                    }
                } else if (SidType::Group == sidtype) {
                    if (EqualSid(accsid, sid)) {
                        if (DELETE & aceAllowed->Mask) {
                            allowed = false;
                        }
                    }
                } else {
                    if (EqualSid(accsid, sid)) {
                        if (DELETE & aceAllowed->Mask) {
                            allowed = true;
                        }
                    }
                }
            } else {
                if (EqualSid(accsid, sid)) {
                    if (DELETE & aceAllowed->Mask) {
                        allowed = true;
                    }
                }
            }
        } else  if (vace->AceType == ACCESS_DENIED_ACE_TYPE) {
            std::cout << "(" << i << ")\t Access denied ACE" << std::endl;
            ACCESS_DENIED_ACE* aceDenied = (ACCESS_DENIED_ACE*)testace;
            PSID accsid = (PSID)&aceDenied->SidStart;
            SidType sidtype;
            if (SysOpResult::Success != sys.GetSIDType(accsid, sidtype)) {
                LocalFree(&testace);
                continue;
                // return ACLOpResult::Fail;
            }
            if (SidType::User == specsidype) {
                if (SidType::User == sidtype) {
                    if (EqualSid(accsid, sid)) {
                        if (DELETE & aceDenied->Mask) {
                            allowed = false;
                        }
                    }
                } else if (SidType::Group == sidtype) {
                    bool ismember = false;
                    if (SysOpResult::Success == sys.IsAccountMemberOfGroup(accsid, sid, ismember)) {
                        if (ismember) {
                            if (DELETE & aceDenied->Mask) {
                                allowed = false;
                            }
                        }
                    } else {
                        return ACLOpResult::Fail;
                    }
                } else {
                    if (EqualSid(accsid, sid)) {
                        if (DELETE & aceDenied->Mask) {
                            allowed = true;
                        }
                    }
                }
            } else if (SidType::Group == specsidype) {
                if (SidType::User == sidtype) {
                    bool ismember = false;
                    if (SysOpResult::Success == sys.IsAccountMemberOfGroup(sid, accsid, ismember)) {
                        if (ismember) {
                            if (DELETE & aceDenied->Mask) {
                                allowed = false;
                            }
                        }
                    } else {
                        return ACLOpResult::Fail;
                    }
                } else if (SidType::Group == sidtype) {
                    if (EqualSid(accsid, sid)) {
                        if (DELETE & aceDenied->Mask) {
                            allowed = false;
                        }
                    }
                } else {
                    if (EqualSid(accsid, sid)) {
                        if (DELETE & aceDenied->Mask) {
                            allowed = true;
                        }
                    }
                }
            } else {
                if (EqualSid(accsid, sid)) {
                    if (DELETE & aceDenied->Mask) {
                        allowed = true;
                    }
                }
            }
        } else {
            std::cout << "(" << i << ")\t Undefined ACE" << std::endl;
        }
        vace = 0;
        LocalFree(&testace);
        testace = 0;
    }
    if (testace) {
        SAFE_LOCALFREE(testace);
    }
    return ACLOpResult::Success;
}

ACLOpResult ACLHandler::DACLFromSecurityDescriptor(SECURITY_DESCRIPTOR* secDesc, ACL* &dacl) const {
    int daclPresent = false, daclDefaulted = false;
    if (GetSecurityDescriptorDacl(secDesc, &daclPresent, &dacl, &daclDefaulted)) {
        if (daclPresent && dacl) {
            return ACLOpResult::Success;
        } else {
            return ACLOpResult::Fail;
        }
    } else {
        return ACLOpResult::Fail;
    }
}

ACLOpResult ACLHandler::CreateAbsoluteSecDesc(SecDesc secDesc, SECURITY_DESCRIPTOR* secDescriptor,
    SECURITY_DESCRIPTOR* absoluteSecDescriptor, unsigned long &absDescSz) const {
    if (MakeAbsoluteSD(secDescriptor, absoluteSecDescriptor, &absDescSz, (ACL*)secDesc.daclInfo, &secDesc.daclInfoSz,
        (ACL*)secDesc.saclInfo, &secDesc.saclInfoSz, secDesc.ownerInfo, &secDesc.ownerInfoSz, secDesc.primaryGroupInfo,
        &secDesc.primaryGroupInfoSz)) {
        if (SetSecurityDescriptorOwner(absoluteSecDescriptor, secDesc.ownerInfo, true)) {
            if (SetSecurityDescriptorGroup(absoluteSecDescriptor, secDesc.primaryGroupInfo, true)) {
                return ACLOpResult::Success;
            }
        }
    }
    return ACLOpResult::Fail;
}

ACLOpResult ACLHandler::DACLAddReadPermissions(ACL* dacl, PSID sid, const bool removeExistingBan) const {
    return ACLOpResult::Success;
}

ACLOpResult ACLHandler::DACLRemoveSIDACE(ACL* dacl, ACL* &outDacl, const PSID sid,
    const bool includeGroups) const {
    if (!dacl->AceCount) {
        return ACLOpResult::Fail;
    }
    void* testace = 0;
    SysHandler sys;
    std::vector<GroupDesc> groups;
    if (includeGroups) {
        sys.LocalGroupListFromStrSID(groups, sys.StrSIDFromSID(sid));
    }
    std::vector<bool> delmarks(dacl->AceCount);
    size_t i = 0, j = 0;
    for (i = 0; i < delmarks.size(); ++i) {
        delmarks[i] = false;
    }
    for (size_t i = 0; i < dacl->AceCount; ++i) {
        if (!GetAce(dacl, i, (void**)&testace)) {
            return ACLOpResult::Fail;
        }
        ACE_HEADER* vace = (ACE_HEADER*)testace;
        if (vace->AceType == ACCESS_ALLOWED_ACE_TYPE) {
            ACCESS_ALLOWED_ACE* aceAllowed = (ACCESS_ALLOWED_ACE*)testace;
            PSID accsid = (PSID)&aceAllowed->SidStart;
            if (includeGroups && groups.size()) {
                for (j = 0; j < groups.size(); ++j) {
                    PSID tpsid = sys.SIDFromStrSid(groups[j].GroupStrSid);
                    if (EqualSid(accsid, tpsid)) {
                        delmarks[i] = true;
                    }
                    SAFE_LOCALFREE(tpsid);
                }
            }
            if (EqualSid(accsid, sid)) {
                delmarks[i] = true;
            }
        } else  if (vace->AceType == ACCESS_DENIED_ACE_TYPE) {
            ACCESS_DENIED_ACE* aceDenied = (ACCESS_DENIED_ACE*)testace;
            PSID accsid = (PSID)&aceDenied->SidStart;
            if (includeGroups && groups.size()) {
                for (j = 0; j < groups.size(); ++j) {
                    PSID tpsid = sys.SIDFromStrSid(groups[j].GroupStrSid);
                    if (EqualSid(accsid, tpsid)) {
                        delmarks[i] = true;
                    }
                    SAFE_LOCALFREE(tpsid);
                }
            }
            if (EqualSid(accsid, sid)) {
                delmarks[i] = true;
            }
        }
        vace = 0;
        // LocalFree(&testace);
        testace = 0;
    }
    unsigned long sz = 0;
    for (i = 0; i < delmarks.size(); ++i) {
        if (delmarks[i]) {
            if (!GetAce(dacl, i, (void**)&testace)) {
                return ACLOpResult::Fail;
            }
            ACE_HEADER* vace = (ACE_HEADER*)testace;
            if (vace->AceType == ACCESS_ALLOWED_ACE_TYPE) {
                ACCESS_ALLOWED_ACE* aceAllowed = (ACCESS_ALLOWED_ACE*)testace;
                PSID accsid = (PSID)&aceAllowed->SidStart;
                sz = sizeof(ACCESS_ALLOWED_ACE);
            } else if (vace->AceType == ACCESS_DENIED_ACE_TYPE) {
                ACCESS_DENIED_ACE* aceDenied = (ACCESS_DENIED_ACE*)testace;
                PSID accsid = (PSID)&aceDenied->SidStart;
                sz = sizeof(ACCESS_DENIED_ACE);
            }
            if(!::DeleteAce(dacl, i)) {
                return ACLOpResult::Fail;
            }
            dacl->AclSize -= (sz + GetLengthSid(sid));
            delmarks.erase(delmarks.begin() + i);
        }
    }
    if (testace) {
        SAFE_LOCALFREE(testace);
    }
    return ACLOpResult::Success;
}

ACLOpResult ACLHandler::DACL2SD(SECURITY_DESCRIPTOR* secDesc, ACL* dacl) const {
    if (SetSecurityDescriptorDacl(secDesc, true, dacl, false)) {
        return ACLOpResult::Success;
    } else {
        return ACLOpResult::Fail;
    }
}

ACE_HEADER* ACLHandler::BuildACE(PSID sid, AceType aceType, unsigned char aceFlags,
    ACCESS_MASK accessMask) const {
    unsigned long sidLen = GetLengthSid(sid);
    unsigned long aceLen = sizeof(ACCESS_ALLOWED_ACE) - sizeof(unsigned long) + sidLen;
    ACCESS_ALLOWED_ACE *ace = (ACCESS_ALLOWED_ACE*)malloc(aceLen);
    if (ace) {
        memset(ace, 0, aceLen);
        ace->Header.AceType = static_cast<unsigned char>(aceType);
        ace->Header.AceFlags = aceFlags;
        ace->Header.AceSize = aceLen;
        ace->Mask = accessMask;
        memcpy(&ace->SidStart, sid, sidLen);
        return (ACE_HEADER*)ace;
    }
    return 0;
}

// Add an ACE to an ACL
// Returns a copy of the original ACL with the relevant changes made; caller must free
// Sequence of ACE's must be:
//      not inherited, denied
//      not inherited, allowed
//      inherited, denied
//      inherited, allowed
ACL* ACLHandler::AddACE(ACL *acl, ACE_HEADER *newHeader) const {
    unsigned long aclSz = acl->AclSize + newHeader->AceSize;
    ACL* newAcl = (ACL*)malloc(aclSz);
    if (newAcl) {
        if (InitializeAcl(newAcl, aclSz, ACL_REVISION)) {
            // Add new denied ACE at start of list
            if (newHeader->AceType == ACCESS_DENIED_ACE_TYPE) {
                if (!AddAce(newAcl, ACL_REVISION, MAXDWORD, newHeader, newHeader->AceSize)) {
                    return 0;
                }
            }
            // Copy all non-inherited ACE's, removing anything matching new_ace
            ACE_HEADER* oldACE = 0;
            for (size_t i = 0; ; ++i) {
                if (!GetAce(acl, i, (void**)&oldACE)) {
                    break;
                }
                if ((oldACE->AceFlags & INHERITED_ACE) == 0 &&
                    (oldACE->AceSize != newHeader->AceSize ||
                        memcmp(oldACE, newHeader, oldACE->AceSize))) {
                    if (!AddAce(newAcl, ACL_REVISION, MAXDWORD, oldACE, oldACE->AceSize)) {
                        return 0;
                    }
                }
                memset(oldACE, 0, sizeof(ACE_HEADER));
            }
            // Add new allowed ACE at end of list
            if (newHeader->AceType == ACCESS_ALLOWED_ACE_TYPE) {
                if (!AddAce(newAcl, ACL_REVISION, MAXDWORD, newHeader, newHeader->AceSize)) {
                    return 0;
                }
            }
            // Copy all inherited ACE's
            for (size_t j = 0; ; ++j) {
                if (!GetAce(acl, j, (void**)&oldACE)) {
                    break;
                }
                if (oldACE->AceFlags & INHERITED_ACE) {
                    if (!AddAce(newAcl, ACL_REVISION, MAXDWORD, oldACE, oldACE->AceSize)) {
                        return 0;
                    }
                }
                memset(oldACE, 0, sizeof(ACE_HEADER));
            }
            // Store the actual size of the acl data (not reversible)
            ACL_SIZE_INFORMATION asi;
            if (GetAclInformation(newAcl, &asi, sizeof(asi), AclSizeInformation)) {
                newAcl->AclSize = (unsigned short)asi.AclBytesInUse;
                return newAcl;
            } else {
                return 0;
            }
        }
        return 0;
    }
    return 0;
}

// Remove an ACE from an ACL
// Returns a copy of the original ACL with the relevant changes made; caller must free
// Sequence of ACE's must be:
//      not inherited, denied
//      not inherited, allowed
//      inherited, denied
//      inherited, allowed
ACL* ACLHandler::RemoveACE(ACL *acl, ACE_HEADER *newHeader) const {
    unsigned long aclSz = acl->AclSize;
    ACL* newAcl = (ACL*)malloc(aclSz);
    if (newAcl) {
        if (InitializeAcl(newAcl, aclSz, ACL_REVISION)) {
            // Copy all non-inherited ACE's, removing anything matching newHeader
            ACE_HEADER* oldACE = 0;
            for (size_t i = 0; ; ++i) {
                if (!GetAce(acl, i, (void**)&oldACE)) {
                    break;
                }
                if ((oldACE->AceFlags & INHERITED_ACE) == 0 &&
                    (oldACE->AceSize != newHeader->AceSize ||
                        memcmp(oldACE, newHeader, oldACE->AceSize))) {
                    if (!AddAce(newAcl, ACL_REVISION, MAXDWORD, oldACE, oldACE->AceSize)) {
                        return 0;
                    }
                }
                memset(oldACE, 0, sizeof(ACE_HEADER));
            }
            // Copy all inherited ACE's
            for (size_t j = 0; ; ++j) {
                if (!GetAce(acl, j, (void**)&oldACE)) {
                    break;
                }
                if (oldACE->AceFlags & INHERITED_ACE) {
                    if (!AddAce(newAcl, ACL_REVISION, MAXDWORD, oldACE, oldACE->AceSize)) {
                        return 0;
                    }
                }
                memset(oldACE, 0, sizeof(ACE_HEADER));
            }
            // Store the actual size of the acl data (not reversible)
            ACL_SIZE_INFORMATION asi;
            if (GetAclInformation(newAcl, &asi, sizeof(asi), AclSizeInformation)) {
                newAcl->AclSize = (unsigned short)asi.AclBytesInUse;
                return newAcl;
            } else {
                return 0;
            }
        }
        return 0;
    }
    return 0;
}

unsigned long ACLHandler::AddRegACE(SID *sid, HKEY hKey, unsigned long aceType, ACCESS_MASK accessMask) const {
    long err = 0;
    SECURITY_DESCRIPTOR *psd = 0;
    unsigned long buflen = 1024;
    // Read current security information    
    for (;;) {
        psd = (SECURITY_DESCRIPTOR*)malloc(buflen);
        err = RegGetKeySecurity(hKey, DACL_SECURITY_INFORMATION, psd, &buflen);
        if (err == 0) {
            break;
        }
        free(psd);
        if (err == ERROR_INSUFFICIENT_BUFFER) {
            continue;
        }
        // return err;
    }
    SECURITY_DESCRIPTOR_RELATIVE *sdr = (SECURITY_DESCRIPTOR_RELATIVE*)psd;
    if (sdr) {
        ACL* pdacl = (ACL*)((unsigned char*)sdr); // + psd->Dacl);
        ACE_HEADER* ace = BuildACE(sid, (AceType)aceType, CONTAINER_INHERIT_ACE, accessMask);
        ACL* newACL = AddACE(pdacl, ace);
        free(ace);
        free(psd);
        SECURITY_DESCRIPTOR sd;
        if (InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION)) {
            if (SetSecurityDescriptorControl(&sd,
                SE_DACL_AUTO_INHERIT_REQ | SE_DACL_AUTO_INHERITED,
                SE_DACL_AUTO_INHERIT_REQ | SE_DACL_AUTO_INHERITED)) {
                if (SetSecurityDescriptorDacl(&sd, TRUE, newACL, FALSE)) {
                    // apply the security descriptor to the registry key
                    err = RegSetKeySecurity(hKey, DACL_SECURITY_INFORMATION, &sd);
                    free(newACL);
                    return 1;
                }
                return 0;
            }
            return 0;
        }
        return 0;
    }
    return 0;
}

unsigned long ACLHandler::RemoveRegACE(SID *sid, HKEY hKey, unsigned long aceType, ACCESS_MASK accessMask) const {
    return 0;
}


// Build an access allowed, access denied, system alarm or system audit ACE.  Caller must free.
ACE_HEADER* BuildACE2(PSID sid, int ace_type, int ace_flags, ACCESS_MASK ace_mask) {
    int sid_len = GetLengthSid(sid);
    int ace_len = sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD) + sid_len;
    ACCESS_ALLOWED_ACE* ace = (ACCESS_ALLOWED_ACE*)malloc(ace_len);
    memset(ace, 0, ace_len);
    ace->Header.AceType = ace_type;
    ace->Header.AceFlags = ace_flags;
    ace->Header.AceSize = ace_len;
    ace->Mask = ace_mask;
    memcpy(&ace->SidStart, sid, sid_len);
    return (ACE_HEADER*)ace;
}

// Add or remove an ACE to/from an ACL
// Returns a copy of the original ACL with the relevant changes made; caller must free
// Sequence of ACE's must be:
//      not inherited, denied
//      not inherited, allowed
//      inherited, denied
//      inherited, allowed
ACL* AddOrRemoveACE(ACL* acl, ACE_HEADER* new_ace, bool add)
{
    int acl_size = acl->AclSize;
    if (add)
        acl_size += new_ace->AceSize;

    ACL* new_acl = (ACL*)malloc(acl_size);
    BOOL ok = InitializeAcl(new_acl, acl_size, ACL_REVISION);

    // Add new denied ACE at start of list
    if (add && new_ace->AceType == ACCESS_DENIED_ACE_TYPE)
    {
        ok = AddAce(new_acl, ACL_REVISION, MAXDWORD, new_ace, new_ace->AceSize);
    }

    // Copy all non-inherited ACE's, removing anything matching new_ace
    for (int i = 0; ; ++i)
    {
        ACE_HEADER* old_ace;
        if (!GetAce(acl, i, (VOID**)&old_ace))
            break;
        if ((old_ace->AceFlags & INHERITED_ACE) == 0 &&
            (old_ace->AceSize != new_ace->AceSize ||
                memcmp(old_ace, new_ace, old_ace->AceSize) != 0))
        {
            ok = AddAce(new_acl, ACL_REVISION, MAXDWORD, old_ace, old_ace->AceSize);
        }
    }

    // Add new allowed ACE at end of list
    if (add && new_ace->AceType == ACCESS_ALLOWED_ACE_TYPE)
    {
        ok = AddAce(new_acl, ACL_REVISION, MAXDWORD, new_ace, new_ace->AceSize);
    }

    // Copy all inherited ACE's
    for (int j = 0; ; ++j)
    {
        ACE_HEADER* old_ace;
        if (!GetAce(acl, j, (VOID**)&old_ace))
            break;
        if (old_ace->AceFlags & INHERITED_ACE)
        {
            ok = AddAce(new_acl, ACL_REVISION, MAXDWORD, old_ace, old_ace->AceSize);
        }
    }

    // Store the actual size of the acl data (not reversible)
    ACL_SIZE_INFORMATION asi;
    ok = GetAclInformation(new_acl, &asi, sizeof(asi), AclSizeInformation);
    new_acl->AclSize = (WORD)asi.AclBytesInUse;
    return new_acl;
}

// Add or remove a registry key ACE.  hKey must have WRITE_DAC access
DWORD AddOrRemoveRegKeyACE
(HKEY hKey, bool add, int ace_type, PSID sid, ACCESS_MASK access_mask)
{
    LONG    err;
    PSECURITY_DESCRIPTOR psd;
    DWORD buflen = 1024;

    // Read current security information    
    for (; ; )
    {
        psd = malloc(buflen);
        err = RegGetKeySecurity(hKey, DACL_SECURITY_INFORMATION, psd, &buflen);
        if (err == 0)
            break;
        free(psd);
        if (err == ERROR_INSUFFICIENT_BUFFER)
            continue;
        return err;
    }

    SECURITY_DESCRIPTOR_RELATIVE* sdr = (SECURITY_DESCRIPTOR_RELATIVE*)psd;
    ACL* pdacl = (ACL*)((BYTE*)sdr + sdr->Dacl);

    ACE_HEADER* ace = BuildACE2(sid, ace_type, CONTAINER_INHERIT_ACE, access_mask);
    ACL* new_acl = AddOrRemoveACE(pdacl, ace, add);
    free(ace);
    free(psd);

    SECURITY_DESCRIPTOR sd;
    BOOL ok = InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);

    ok = SetSecurityDescriptorControl(&sd,
        SE_DACL_AUTO_INHERIT_REQ | SE_DACL_AUTO_INHERITED,
        SE_DACL_AUTO_INHERIT_REQ | SE_DACL_AUTO_INHERITED);

    ok = SetSecurityDescriptorDacl(&sd, TRUE, new_acl, FALSE);

    // apply the security descriptor to the registry key
    err = RegSetKeySecurity(hKey, DACL_SECURITY_INFORMATION, &sd);
    free(new_acl);

    return err;
}

BOOL grant_access_to_registry_key(HKEY hKey)
{
    // Give the Users group access to hKey
    SID_IDENTIFIER_AUTHORITY sia = SECURITY_NT_AUTHORITY;
    PSID pUsersSid;
    BOOL ok = AllocateAndInitializeSid(&sia, 2,
        SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_USERS,
        0, 0, 0, 0, 0, 0, &pUsersSid);
    if (!ok)
        return FALSE;
    ok = AddOrRemoveRegKeyACE(hKey, true, ACCESS_ALLOWED_ACE_TYPE,
        pUsersSid, KEY_ALL_ACCESS) == 0;
    FreeSid(pUsersSid);
    return ok;
}