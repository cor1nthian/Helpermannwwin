#ifndef _ACL_HELPER_H
#define _ACL_HELPER_H

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
#include "strhelper.h"
#include "syshelper.h"

enum class AceType : unsigned char {
	// same as ACCESS_MIN_MS_ACE_TYPE
	AccessAllowed = ACCESS_ALLOWED_ACE_TYPE,
	AccessDenied = ACCESS_DENIED_ACE_TYPE,
	SystemAudit = SYSTEM_AUDIT_ACE_TYPE,
	// same as ACCESS_MAX_MS_V2_ACE_TYPE
	SystemAlarm = SYSTEM_ALARM_ACE_TYPE
};

enum class SecInfo : unsigned long {
	/* The resource properties of the object being referenced. The resource properties are
	stored in SYSTEM_RESOURCE_ATTRIBUTE_ACE types in the SACL of the security descriptor.
	Windows Server 2008 R2, Windows 7,
	Windows Server 2008, Windows Vista,
	Windows Server 2003 and Windows XP:
	This bit flag is not available. */
	AttributeSecInfo = ATTRIBUTE_SECURITY_INFORMATION,
	/* All parts of the security descriptor. This is useful for backup and restore
	software that needs to preserve the entire security descriptor.
	Windows Server 2008 R2, Windows 7,
	Windows Server 2008, Windows Vista,
	Windows Server 2003 and Windows XP:
	This bit flag is not available. */
	BackupSecInfo = BACKUP_SECURITY_INFORMATION,
	// The DACL of the object is being referenced.
	DACLSecInfo = DACL_SECURITY_INFORMATION,
	// The primary group identifier of the object is being referenced.
	GroupSecInfo = GROUP_SECURITY_INFORMATION,
	/* The mandatory integrity label is being referenced. The mandatory integrity label is an
	ACE in the SACL of the object.
	Windows Server 2003 and Windows XP:
	This bit flag is not available. */
	LabelSecInfo = LABEL_SECURITY_INFORMATION,
	// The owner identifier of the object is being referenced.
	OwnerSecInfo = OWNER_SECURITY_INFORMATION,
	// The DACL cannot inherit access control entries(ACEs).
	ProtectedDACLSecInfo = PROTECTED_DACL_SECURITY_INFORMATION,
	// The SACL cannot inherit ACEs.
	ProtectedSACLSecInfo = PROTECTED_SACL_SECURITY_INFORMATION,
	// The SACL of the object is being referenced.
	SACLSecInfo = SACL_SECURITY_INFORMATION,
	/* The Central Access Policy (CAP) identifier applicable on the object that is being
	referenced. Each CAP identifier is stored in a SYSTEM_SCOPED_POLICY_ID_ACE type in the SACL of the SD.
	Windows Server 2008 R2, Windows 7,
	Windows Server 2008, Windows Vista,
	Windows Server 2003 and Windows XP:
	This bit flag is not available. */
	ScopeSecInfo = SCOPE_SECURITY_INFORMATION,
	// The DACL inherits ACEs from the parent object.
	UnprotectedDACLSecInfo = UNPROTECTED_DACL_SECURITY_INFORMATION,
	// The SACL inherits ACEs from the parent object.
	UnprotectedSACLSecInfo = UNPROTECTED_SACL_SECURITY_INFORMATION
};

enum class ACLOpResult : unsigned char {
	Success,
	Fail
};

struct SecDesc {
	SecDesc();
	SecDesc(const SecDesc &other);
	~SecDesc();
	SecDesc& operator=(const SecDesc &other) {
		daclInfoSz = other.daclInfoSz;
		daclAbsInfoSz = other.daclAbsInfoSz;
		saclInfoSz = other.saclInfoSz;
		saclAbsInfoSz = other.saclAbsInfoSz;
		ownerInfoSz = other.ownerInfoSz;
		primaryGroupInfoSz = other.primaryGroupInfoSz;
		absoluteSDInfoSz = other.absoluteSDInfoSz;
		selfRelativeSDInfoSz = other.selfRelativeSDInfoSz;
		if (other.daclInfo) {
			/* if (daclInfo && daclInfoSz) {
				 SAFE_LOCALFREE(daclInfo);
			}*/
			daclInfo = LocalAlloc(LPTR, daclInfoSz);
			if (daclInfo) {
				memcpy(daclInfo, other.daclInfo, daclInfoSz);
			}
		} else {
			/*if (daclInfo && daclInfoSz) {
				SAFE_LOCALFREE(daclInfo);
			}*/
			daclInfo = 0;
		}
		if (other.daclAbsInfo) {
			/* if (daclAbsInfo && daclAbsInfoSz) {
				 SAFE_LOCALFREE(daclAbsInfo);
			}*/
			daclAbsInfo = LocalAlloc(LPTR, daclAbsInfoSz);
			if (daclAbsInfo) {
				memcpy(daclAbsInfo, other.daclAbsInfo, daclAbsInfoSz);
			}
		} else {
			/*if (daclAbsInfo && daclAbsInfoSz) {
				SAFE_LOCALFREE(daclAbsInfo);
			}*/
			daclAbsInfo = 0;
		}
		if (other.saclInfo) {
			/*if (saclInfo && saclInfoSz) {
				SAFE_LOCALFREE(saclInfo);
			}*/
			saclInfo = LocalAlloc(LPTR, saclInfoSz);
			if (saclInfo) {
				memcpy(saclInfo, other.saclInfo, saclInfoSz);
			}
		} else {
			/*if (saclInfo && saclInfoSz) {
				SAFE_LOCALFREE(saclInfo);
			}*/
			saclInfo = 0;
		}
		if (other.saclAbsInfo) {
			/* if (saclAbsInfo && saclAbsInfoSz) {
				 SAFE_LOCALFREE(saclAbsInfo);
			}*/
			saclAbsInfo = LocalAlloc(LPTR, saclAbsInfoSz);
			if (saclAbsInfo) {
				memcpy(saclAbsInfo, other.saclAbsInfo, saclAbsInfoSz);
			}
		} else {
			/*if (saclAbsInfo && saclAbsInfoSz) {
				SAFE_LOCALFREE(saclAbsInfo);
			}*/
			saclAbsInfo = 0;
		}
		if (other.ownerInfo) {
			/*if (ownerInfo && ownerInfoSz) {
				SAFE_LOCALFREE(ownerInfo);
			}*/
			ownerInfo = LocalAlloc(LPTR, saclInfoSz);
			if (ownerInfo) {
				memcpy(ownerInfo, other.ownerInfo, ownerInfoSz);
			}
		} else {
			/*if (ownerInfo && ownerInfoSz) {
				SAFE_LOCALFREE(ownerInfo);
			}*/
			ownerInfo = 0;
		}
		if (other.primaryGroupInfo) {
			/*if (primaryGroupInfo && primaryGroupInfoSz) {
				SAFE_LOCALFREE(primaryGroupInfo);
			}*/
			primaryGroupInfo = LocalAlloc(LPTR, primaryGroupInfoSz);
			if (primaryGroupInfo) {
				memcpy(primaryGroupInfo, other.primaryGroupInfo, primaryGroupInfoSz);
			}
		} else {
			/*if (primaryGroupInfo && primaryGroupInfoSz) {
				SAFE_LOCALFREE(primaryGroupInfo);
			}*/
			primaryGroupInfo = 0;
		}
		if (other.absoluteSDInfo) {
			/*if (absoluteSDInfo && absoluteSDInfoSz) {
				SAFE_LOCALFREE(absoluteSDInfo);
			}*/
			absoluteSDInfo = LocalAlloc(LPTR, absoluteSDInfoSz);
			if (absoluteSDInfo) {
				memcpy(absoluteSDInfo, other.absoluteSDInfo, absoluteSDInfoSz);
			}
		} else {
			/*if (absoluteSDInfo && absoluteSDInfoSz) {
				SAFE_LOCALFREE(absoluteSDInfo);
			}*/
			absoluteSDInfo = 0;
		}
		if (other.selfRelativeSDInfo) {
			/*if (selfRelativeSDInfo && selfRelativeSDInfoSz) {
				SAFE_LOCALFREE(absoluteSDInfo);
			}*/
			selfRelativeSDInfo = LocalAlloc(LPTR, absoluteSDInfoSz);
			if (selfRelativeSDInfo) {
				memcpy(selfRelativeSDInfo, other.selfRelativeSDInfo, selfRelativeSDInfoSz);
			}
		} else {
			/*if (selfRelativeSDInfo && selfRelativeSDInfoSz) {
				SAFE_LOCALFREE(selfRelativeSDInfo);
			}*/
			selfRelativeSDInfo = 0;
		}
		return *this;
	}
	bool operator==(const SecDesc& other) const {
		bool bufeq = false, otheq = false, sideq = false;
		if (daclInfo && other.daclInfo) {
			if (daclInfoSz && other.daclInfoSz) {
				bufeq = !memcmp(daclInfo, other.daclInfo, other.daclInfoSz);
			} else {
				bufeq = false;
			}
		} else {
			if (daclInfo && !other.daclInfo) {
				bufeq = false;
			} else if (!daclInfo && other.daclInfo) {
				bufeq = false;
			} else if (!daclInfo && !other.daclInfo) {
				bufeq = true;
			}
		}
		if (daclAbsInfo && other.daclAbsInfo) {
			if (daclAbsInfoSz && other.daclAbsInfoSz) {
				bufeq &= !memcmp(daclAbsInfo, other.daclAbsInfo, other.daclAbsInfoSz);
			} else {
				bufeq &= false;
			}
		} else {
			if (daclAbsInfo && !other.daclAbsInfo) {
				bufeq &= false;
			} else if (!daclAbsInfo && other.daclAbsInfo) {
				bufeq &= false;
			} else if (!daclAbsInfo && !other.daclAbsInfo) {
				bufeq &= true;
			}
		}
		if (saclInfo && other.saclInfo) {
			if (saclInfoSz && other.saclInfoSz) {
				bufeq &= !memcmp(saclInfo, other.saclInfo, other.saclInfoSz);
			} else {
				bufeq &= false;
			}
		} else {
			if (saclInfo && !other.saclInfo) {
				bufeq &= false;
			} else if (!saclInfo && other.saclInfo) {
				bufeq &= false;
			} else if (!saclInfo && !other.saclInfo) {
				bufeq &= true;
			}
		}
		if (saclAbsInfo && other.saclAbsInfo) {
			if (saclAbsInfoSz && other.saclAbsInfoSz) {
				bufeq &= !memcmp(saclAbsInfo, other.saclAbsInfo, other.saclAbsInfoSz);
			} else {
				bufeq &= false;
			}
		} else {
			if (saclAbsInfo && !other.saclAbsInfo) {
				bufeq &= false;
			} else if (!saclAbsInfo && other.saclAbsInfo) {
				bufeq &= false;
			} else if (!saclAbsInfo && !other.saclAbsInfo) {
				bufeq &= true;
			}
		}
		if (absoluteSDInfo && other.absoluteSDInfo) {
			if (absoluteSDInfoSz && other.absoluteSDInfoSz) {
				bufeq &= !memcmp(absoluteSDInfo, other.absoluteSDInfo, other.absoluteSDInfoSz);
			} else {
				bufeq &= false;
			}
		} else {
			if (absoluteSDInfo && !other.absoluteSDInfo) {
				bufeq &= false;
			} else if (!absoluteSDInfo && other.absoluteSDInfo) {
				bufeq &= false;
			} else if (!absoluteSDInfo && !other.absoluteSDInfo) {
				bufeq &= true;
			}
		}
		if (selfRelativeSDInfo && other.selfRelativeSDInfo) {
			if (selfRelativeSDInfoSz && other.selfRelativeSDInfoSz) {
				bufeq &= !memcmp(selfRelativeSDInfo, other.selfRelativeSDInfo, other.selfRelativeSDInfoSz);
			} else {
				bufeq &= false;
			}
		} else {
			if (selfRelativeSDInfo && !other.selfRelativeSDInfo) {
				bufeq &= false;
			} else if (!selfRelativeSDInfo && other.selfRelativeSDInfo) {
				bufeq &= false;
			}
			else if (!selfRelativeSDInfo && !other.selfRelativeSDInfo) {
				bufeq &= true;
			}
		}
		otheq = (daclInfoSz == other.daclInfoSz &&
			saclInfoSz == other.saclInfoSz &&
			ownerInfoSz == other.ownerInfoSz &&
			primaryGroupInfoSz == other.primaryGroupInfoSz &&
			absoluteSDInfoSz == other.absoluteSDInfoSz &&
			selfRelativeSDInfoSz == other.selfRelativeSDInfoSz);
		sideq = (EqualSid(ownerInfo, other.ownerInfo) && EqualSid(primaryGroupInfo, other.primaryGroupInfo));
		return (bufeq && otheq && sideq);
	}
	bool operator!=(const SecDesc& other) const {
		bool bufneq = false, othneq = false, sidneq = false;
		if (daclInfo && other.daclInfo) {
			if (daclInfoSz && other.daclInfoSz) {
				bufneq = memcmp(daclInfo, other.daclInfo, other.daclInfoSz);
			} else {
				bufneq = true;
			}
		} else {
			if (daclInfo && !other.daclInfo) {
				bufneq = true;
			} else if (!daclInfo && other.daclInfo) {
				bufneq = true;
			} else if (!daclInfo && !other.daclInfo) {
				bufneq = false;
			}
		}
		if (daclAbsInfo && other.daclAbsInfo) {
			if (daclAbsInfoSz && other.daclAbsInfoSz) {
				bufneq |= memcmp(daclAbsInfo, other.daclAbsInfo, other.daclAbsInfoSz);
			} else {
				bufneq |= true;
			}
		} else {
			if (daclAbsInfo && !other.daclAbsInfo) {
				bufneq |= true;
			} else if (!daclAbsInfo && other.daclAbsInfo) {
				bufneq |= true;
			} else if (!daclAbsInfo && !other.daclAbsInfo) {
				bufneq |= false;
			}
		}
		if (saclInfo && other.saclInfo) {
			if(saclInfoSz && other.saclInfoSz) {
				bufneq |= memcmp(saclInfo, other.saclInfo, other.saclInfoSz);
			} else {
				bufneq |= true;
			}
		} else {
			if (saclInfo && !other.saclInfo) {
				bufneq |= true;
			} else if (!saclInfo && other.saclInfo) {
				bufneq |= true;
			} else if (!saclInfo && !other.saclInfo) {
				bufneq |= false;
			}
		}
		if (saclAbsInfo && other.saclAbsInfo) {
			if (saclAbsInfoSz && other.saclAbsInfoSz) {
				bufneq |= memcmp(saclAbsInfo, other.saclAbsInfo, other.saclAbsInfoSz);
			} else {
				bufneq |= true;
			}
		} else {
			if (saclAbsInfo && !other.saclAbsInfo) {
				bufneq |= true;
			} else if (!saclAbsInfo && other.saclAbsInfo) {
				bufneq |= true;
			} else if (!saclAbsInfo && !other.saclAbsInfo) {
				bufneq |= false;
			}
		}
		if (absoluteSDInfo && other.absoluteSDInfo) {
			if (absoluteSDInfoSz && other.absoluteSDInfoSz) {
				bufneq |= memcmp(absoluteSDInfo, other.absoluteSDInfo, other.absoluteSDInfoSz);
			} else {
				bufneq |= true;
			}
		} else {
			if (absoluteSDInfo && !other.absoluteSDInfo) {
				bufneq |= true;
			} else if (!absoluteSDInfo && other.absoluteSDInfo) {
				bufneq |= true;
			} else if (!absoluteSDInfo && !other.absoluteSDInfo) {
				bufneq |= false;
			}
		}
		if (selfRelativeSDInfo && other.selfRelativeSDInfo) {
			if (selfRelativeSDInfoSz && other.selfRelativeSDInfoSz) {
				bufneq |= memcmp(selfRelativeSDInfo, other.selfRelativeSDInfo, other.selfRelativeSDInfoSz);
			} else {
				bufneq |= true;
			}
		} else {
			if (selfRelativeSDInfo && !other.selfRelativeSDInfo) {
				bufneq |= true;
			} else if (!selfRelativeSDInfo && other.selfRelativeSDInfo) {
				bufneq |= true;
			} else if (!selfRelativeSDInfo && !other.selfRelativeSDInfo) {
				bufneq |= false;
			}
		}
		othneq = (daclInfoSz != other.daclInfoSz ||
			saclInfoSz != other.saclInfoSz ||
			ownerInfoSz != other.ownerInfoSz ||
			primaryGroupInfoSz != other.primaryGroupInfoSz ||
			absoluteSDInfoSz != other.absoluteSDInfoSz ||
			selfRelativeSDInfoSz != other.selfRelativeSDInfoSz);
		sidneq = (!EqualSid(ownerInfo, other.ownerInfo) || !EqualSid(primaryGroupInfo, other.primaryGroupInfo));
		return (bufneq || othneq || sidneq);
	}
	unsigned long daclInfoSz;
	unsigned long daclAbsInfoSz;
	unsigned long saclInfoSz;
	unsigned long saclAbsInfoSz;
	unsigned long ownerInfoSz;
	unsigned long primaryGroupInfoSz;
	unsigned long absoluteSDInfoSz;
	unsigned long selfRelativeSDInfoSz;
	PSID ownerInfo;
	PSID primaryGroupInfo;
	void* daclInfo;
	void* daclAbsInfo;
	void* saclInfo;
	void* saclAbsInfo;
	void* absoluteSDInfo;
	void* selfRelativeSDInfo;
};

class ACLHandler {
	public:
		ACLHandler();
		ACLHandler(const ACLHandler &other);
		~ACLHandler();
		ACLOpResult SecurityDescriptor2StringSecurityDescriptor(unsigned char* &secDesc,
			std::wstring &textSecDesc, SecInfo secInfo = SecInfo::DACLSecInfo) const;
		ACLOpResult StringSecurityDescriptor2SecurityDescriptor(const std::wstring textSecDesc,
			unsigned char* &secDesc, unsigned long &secDeszSz) const;
		ACLOpResult DACLReadAllowed(bool &allowed, ACL* testACL, PSID sid) const;
		ACLOpResult DACLWriteAllowed(bool &allowed, ACL* testACL, PSID sid) const;
		ACLOpResult DACLExecuteAllowed(bool &allowed, ACL* testACL, PSID sid) const;
		ACLOpResult DACLDeleteAllowed(bool &allowed, ACL* testACL, PSID sid) const;
		ACLOpResult DACLFromSecurityDescriptor(SECURITY_DESCRIPTOR* secDesc, ACL* &dacl) const;
		ACLOpResult DACLAddReadPermissions(ACL* dacl, const PSID sid, const bool removeExistingBan = true) const;
		ACLOpResult DACLRemoveSIDACE(ACL* &dacl, const PSID sid, const bool includeGroups = true) const;
		ACLOpResult CreateAbsoluteSecDesc(SecDesc &secDesc) const;
		ACLOpResult DACL2AbsoluteSD(SECURITY_DESCRIPTOR* secDesc, ACL* dacl) const;
	protected:

	private:
		ACE_HEADER* BuildACE(PSID sid, AceType aceType, unsigned char aceFlags,
			ACCESS_MASK accessMask) const;
		ACL* AddACE(ACL *acl, ACE_HEADER *newHeader) const;
		ACL* RemoveACE(ACL *acl, ACE_HEADER *newHeader) const;
		unsigned long AddRegACE(SID *sid, HKEY hKey, unsigned long aceType, ACCESS_MASK accessMask) const;
		unsigned long RemoveRegACE(SID *sid, HKEY hKey, unsigned long aceType, ACCESS_MASK accessMask) const;
};

#endif // _ACL_HELPER_H