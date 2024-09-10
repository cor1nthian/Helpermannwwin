#include "WMIHelper.h"

WMIHandler::WMIHandler() {
	m_pLoc = 0;
	m_pSvc = 0;
	m_WMIInitialized = false;
}

WMIHandler::WMIHandler(const WMIHandler &other) {
	if (this != &other) {
		m_pLoc = other.m_pLoc;
		m_pSvc = other.m_pSvc;
		m_WMIInitialized = other.m_WMIInitialized;
	}
}

#if (COMPILERVER >= 11 && COMPILERVER != 98)
WMIHandler::WMIHandler(WMIHandler &&other) {
	if (this != &other) {
		m_pLoc = std::move(other.m_pLoc);
		other.m_pLoc = 0;
		m_pSvc = std::move(other.m_pSvc);
		other.m_pSvc = 0;
		m_WMIInitialized = std::exchange(other.m_WMIInitialized, false);
	}
}
#endif

WMIHandler& WMIHandler::operator=(const WMIHandler &other) {
	if (this != &other) {
		m_pLoc = other.m_pLoc;
		m_pSvc = other.m_pSvc;
		m_WMIInitialized = other.m_WMIInitialized;
	}
	return *this;
}

#if (COMPILERVER >= 11 && COMPILERVER != 98)
WMIHandler& WMIHandler::operator=(WMIHandler &&other) {
	if (this != &other) {
		m_pLoc = std::move(other.m_pLoc);
		other.m_pLoc = 0;
		m_pSvc = std::move(other.m_pSvc);
		other.m_pSvc = 0;
		m_WMIInitialized = std::exchange(other.m_WMIInitialized, false);
	}
	return *this;
}
#endif

bool WMIHandler::operator==(const WMIHandler &other) {
	if (this != &other) {
		return (m_pLoc == other.m_pLoc &&
				m_pSvc == other.m_pSvc &&
				m_WMIInitialized == other.m_WMIInitialized);
	} else {
		return true;
	}
}

bool WMIHandler::operator!=(const WMIHandler &other) {
	if (this != &other) {
		return (m_pLoc != other.m_pLoc ||
				m_pSvc != other.m_pSvc ||
				m_WMIInitialized != other.m_WMIInitialized);
	} else {
		return false;
	}
}

WMIHandler::~WMIHandler() {
	if (m_WMIInitialized) {
		ShutdownWMI();
	}
}

WMIOpResult WMIHandler::InitWMI(const std::wstring namespacePath) {
	::HRESULT hres = ::CoInitializeEx(0, ::COINIT_MULTITHREADED);
	if (FAILED(hres)) {
		return WMIOpResult::Fail;
	}
	hres = CoInitializeSecurity(
		0,
		-1,								// COM authentication
		0,								// Authentication services
		0,								// Reserved
		RPC_C_AUTHN_LEVEL_DEFAULT,		// Default authentication 
		RPC_C_IMP_LEVEL_IMPERSONATE,	// Default Impersonation  
		0,								// Authentication info
		::EOAC_NONE,					// Additional capabilities 
		0								// Reserved
	);
	if (FAILED(hres)) {
		::CoUninitialize();
		return WMIOpResult::Fail;
	}
	hres = ::CoCreateInstance(::CLSID_WbemLocator, 0, ::CLSCTX_INPROC_SERVER, ::IID_IWbemLocator, (void**)&m_pLoc);
	if (FAILED(hres)) {
		::CoUninitialize();
		return WMIOpResult::Fail;
	}
	hres = m_pLoc->ConnectServer(
		_bstr_t(namespacePath.c_str()),	// Object path of WMI namespace
		0,								// User name. NULL = current user
		0,								// User password. NULL = current
		0,								// Locale. NULL indicates current
		0,								// Security flags.
		0,								// Authority (e.g. Kerberos)
		0,								// Context object 
		&m_pSvc							// pointer to IWbemServices proxy
	);
	if (FAILED(hres)) {
		m_pLoc->Release();
		m_pLoc = 0;
		::CoUninitialize();
		return WMIOpResult::Fail;
	}
	hres = ::CoSetProxyBlanket(
		m_pSvc,							// Indicates the proxy to set
		RPC_C_AUTHN_WINNT,				// RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,				// RPC_C_AUTHZ_xxx
		0,								// Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,			// RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE,	// RPC_C_IMP_LEVEL_xxx
		0,								// client identity
		::EOAC_NONE						// proxy capabilities 
	);
	if (FAILED(hres)) {
		m_pSvc->Release();
		m_pSvc = 0;
		m_pLoc->Release();
		m_pLoc = 0;
		::CoUninitialize();
		return WMIOpResult::Fail;
	}
	m_WMIInitialized = true;
	return WMIOpResult::Success;
}

WMIOpResult WMIHandler::RunWMIQuery(std::map<std::wstring, std::wstring> &results, const std::vector<std::wstring> requiredFields,
	const std::wstring query, const std::wstring namespacePath, const std::wstring queryLanguage,
	const bool initWMI, const bool shutdownWMI) {
	if (!m_WMIInitialized) {
		if (initWMI) {
			if (WMIOpResult::Success != InitWMI(namespacePath)) {
				return WMIOpResult::Fail;
			}
		} else {
			return WMIOpResult::Fail;
		}
	}
	::IEnumWbemClassObject *pEnumerator = 0;
	::HRESULT hres = m_pSvc->ExecQuery(
		_bstr_t(queryLanguage.c_str()),
		_bstr_t(query.c_str()),
		::WBEM_FLAG_FORWARD_ONLY | ::WBEM_FLAG_RETURN_IMMEDIATELY,
		0, &pEnumerator);
	if (FAILED(hres)) {
		return WMIOpResult::Fail;
	}
	::IWbemClassObject *pclsObj = 0;
	unsigned long uReturn = 0;
	size_t cmaxpart = 0, cminpart = 0;
	while (pEnumerator) {
		hres = pEnumerator->Next(::WBEM_INFINITE, 1, &pclsObj, &uReturn);
		if (!uReturn) {
			break;
		}
		::VARIANT vtProp = { 0 };
		cminpart = 0;
		for (size_t i = 0; i < requiredFields.size(); ++i) {
			hres = pclsObj->Get(requiredFields[i].c_str(), 0, &vtProp, 0, 0);
			if (FAILED(hres)) {
				return WMIOpResult::Fail;
			}
			results[requiredFields[i] + std::to_wstring((cmaxpart + cminpart))] = vtProp.bstrVal;
			++cminpart;
		}
		cmaxpart += WH_MAXRESCNTCHANGE;
	}
	if (shutdownWMI) {
		if (WMIOpResult::Success != ShutdownWMI()) {
			return WMIOpResult::Fail;
		}
	}
	return WMIOpResult::Success;
}

WMIOpResult WMIHandler::ShutdownWMI() {
	if (m_pSvc) {
		m_pSvc->Release();
		m_pSvc = 0;
	}
	if (m_pLoc) {
		m_pLoc->Release();
		m_pLoc = 0;
	}
	::CoUninitialize();
	m_WMIInitialized = false;
	return WMIOpResult::Success;
}