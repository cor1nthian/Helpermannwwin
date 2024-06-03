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
		saclInfoSz = other.saclInfoSz;
		ownerInfoSz = other.ownerInfoSz;
		primaryGroupInfoSz = other.primaryGroupInfoSz;
		ownerInfo = other.ownerInfo;
		primaryGroupInfo = other.primaryGroupInfo;
		if (other.daclInfo) {
			daclInfo = malloc(daclInfoSz);
			if (daclInfo) {
				memcpy(daclInfo, other.daclInfo, daclInfoSz);
			}
		} else {
			if (daclInfo) {
				SAFE_FREE(daclInfo);
			}
			daclInfo = 0;
		}
		if (other.saclInfo) {
			saclInfo = malloc(saclInfoSz);
			if (saclInfo) {
				memcpy(saclInfo, other.saclInfo, saclInfoSz);
			}
		} else {
			if (saclInfo) {
				SAFE_FREE(saclInfo);
			}
			saclInfo = 0;
		}
		return *this;
	}
	bool operator==(const SecDesc& other) const {
		bool bufeq = false, otheq = false;
		if (daclInfo && other.daclInfo) {
			bufeq = !memcmp(daclInfo, other.daclInfo, other.daclInfoSz);
		} else {
			if (daclInfo && !other.daclInfo) {
				bufeq = false;
			} else if (!daclInfo && other.daclInfo) {
				bufeq = false;
			} else if (!daclInfo && !other.daclInfo) {
				bufeq = true;
			}
		}
		if (saclInfo && other.saclInfo) {
			bufeq = !memcmp(saclInfo, other.saclInfo, other.saclInfoSz);
		} else {
			if (saclInfo && !other.saclInfo) {
				bufeq = false;
			} else if (!saclInfo && other.saclInfo) {
				bufeq = false;
			} else if (!saclInfo && !other.saclInfo) {
				bufeq = true;
			}
		}
		otheq = (daclInfoSz == other.daclInfoSz &&
			saclInfoSz == other.saclInfoSz &&
			ownerInfoSz == other.ownerInfoSz &&
			primaryGroupInfoSz == other.primaryGroupInfoSz &&
			ownerInfo == other.ownerInfo &&
			primaryGroupInfo == other.primaryGroupInfo);
		return (bufeq && otheq);
	}
	bool operator!=(const SecDesc& other) const {
		bool bufneq = false, othneq = false;
		if (daclInfo && other.daclInfo) {
			bufneq = memcmp(daclInfo, other.daclInfo, other.daclInfoSz);
		} else {
			if (daclInfo && !other.daclInfo) {
				bufneq = true;
			} else if (!daclInfo && other.daclInfo) {
				bufneq = true;
			} else if (!daclInfo && !other.daclInfo) {
				bufneq = false;
			}
		}
		if (saclInfo && other.saclInfo) {
			bufneq = memcmp(saclInfo, other.saclInfo, other.saclInfoSz);
		} else {
			if (saclInfo && !other.saclInfo) {
				bufneq = true;
			} else if (!saclInfo && other.saclInfo) {
				bufneq = true;
			} else if (!saclInfo && !other.saclInfo) {
				bufneq = false;
			}
		}
		othneq = (daclInfoSz != other.daclInfoSz ||
			saclInfoSz != other.saclInfoSz ||
			ownerInfoSz != other.ownerInfoSz ||
			primaryGroupInfoSz != other.primaryGroupInfoSz ||
			ownerInfo != other.ownerInfo ||
			primaryGroupInfo != other.primaryGroupInfo);
		return (bufneq || othneq);
	}
	unsigned long daclInfoSz;
	unsigned long saclInfoSz;
	unsigned long ownerInfoSz;
	unsigned long primaryGroupInfoSz;
	PSID ownerInfo;
	PSID primaryGroupInfo;
	void* daclInfo;
	void* saclInfo;
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
		ACLOpResult DACLRemoveSIDACE(ACL* dacl, ACL* &outDacl, const PSID sid, const bool includeGroups = true) const;
		ACLOpResult CreateAbsoluteSecDesc(SecDesc secDesc, SECURITY_DESCRIPTOR* secDesceiptor,
			SECURITY_DESCRIPTOR* absoluteSecDescriptor, unsigned long &absDescSz) const;
		ACLOpResult DACL2SD(SECURITY_DESCRIPTOR* secDesc, ACL* dacl) const;
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
