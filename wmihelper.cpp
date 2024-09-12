#include "WMIHelper.h"

WMIQueryAsyncSink::WMIQueryAsyncSink() {
	m_lRef = 0;
	m_bDone = 0;
	m_WMIHandler = 0;
}

WMIQueryAsyncSink::WMIQueryAsyncSink(const WMIQueryAsyncSink& other) {
	if (this !=&other) {
		m_lRef = other.m_lRef;
		m_bDone = other.m_bDone;
		m_WMIHandler = other.m_WMIHandler;
	}
}

#if (defined(STDVER) && STDVER >= 11 && STDVER != 98)
WMIQueryAsyncSink::WMIQueryAsyncSink(WMIQueryAsyncSink &&other) noexcept {
	if (this != &other) {
		m_lRef = std::exchange(other.m_lRef, 0);
		m_bDone = std::exchange(other.m_bDone, false);
		m_WMIHandler = std::move(other.m_WMIHandler);
		other.m_WMIHandler = 0;
	}
}
#endif

WMIQueryAsyncSink& WMIQueryAsyncSink::operator=(const WMIQueryAsyncSink &other) {
	if (this != &other) {
		m_lRef = other.m_lRef;
		m_bDone = other.m_bDone;
		m_WMIHandler = other.m_WMIHandler;
	}
	return *this;
}

#if (defined(STDVER) && STDVER >= 11 && STDVER != 98)
WMIQueryAsyncSink& WMIQueryAsyncSink::operator=(WMIQueryAsyncSink &&other) noexcept {
	if (this != &other) {
		m_lRef = std::exchange(other.m_lRef, 0);
		m_bDone = std::exchange(other.m_bDone, false);
		m_WMIHandler = std::move(other.m_WMIHandler);
		other.m_WMIHandler = 0;
	}
	return *this;
}
#endif

bool WMIQueryAsyncSink::operator==(const WMIQueryAsyncSink& other) const {
	if (this != &other) {
		return (m_lRef == other.m_lRef &&
				m_bDone == other.m_bDone &&
				m_WMIHandler == other.m_WMIHandler);
	} else {
		return true;
	}
}

bool WMIQueryAsyncSink::operator!=(const WMIQueryAsyncSink &other) const {
	if (this != &other) {
		return (m_lRef != other.m_lRef ||
				m_bDone != other.m_bDone ||
				m_WMIHandler != other.m_WMIHandler);
	} else {
		return false;
	}
}

WMIQueryAsyncSink::~WMIQueryAsyncSink() {
	m_bDone = 1;
}

unsigned long __stdcall WMIQueryAsyncSink::AddRef() {
	return ::InterlockedIncrement(&m_lRef);
}

unsigned long __stdcall WMIQueryAsyncSink::Release() {
	long lRef = ::InterlockedDecrement(&m_lRef);
	if (!lRef) {
		delete this;
	}
	return lRef;
}

::HRESULT __stdcall WMIQueryAsyncSink::QueryInterface(const GUID &riid, void** ppv) {
	if (::IID_IUnknown == riid || ::IID_IWbemObjectSink == riid) {
		*ppv = (::IWbemObjectSink*)this;
		AddRef();
		return ::WBEMSTATUS::WBEM_S_NO_ERROR;
	} else {
		return E_NOINTERFACE;
	}
}

::HRESULT __stdcall WMIQueryAsyncSink::Indicate(long lObjectCount,
	::IWbemClassObject __RPC_FAR* __RPC_FAR* apObjArray) {
	std::vector<std::wstring> requiredFields = m_WMIHandler->m_asyncRequiredFields;
	if (!requiredFields.size()) {
		return ::WBEMSTATUS::WBEM_E_FAILED;
	}
	size_t ind = 0;
	::VARIANT vtProp = { 0 };
	::BSTR t = 0;
	std::wstring reskey, temp;
	for (long i = 0; i < lObjectCount; ++i) {
		::IWbemClassObject* pObj = apObjArray[i];
		for (unsigned long j = 0; j < requiredFields.size(); ++j) {
			::HRESULT hres = pObj->Get(requiredFields[j].c_str(), 0, &vtProp, 0, 0);
			if (FAILED(hres)) {
				// temp = gc_wmiEmptyval;
				continue;
			} else {
				t = ::_bstr_t(vtProp);
				if (t) {
					temp = t;
				} else {
					temp = gc_wmiEmptyval;
				}
			}
			ind = 0;
			while (true) {
				reskey = requiredFields[j] + std::to_wstring(ind);
				if (!m_WMIHandler->m_asyncResults->count(reskey)) {
					m_WMIHandler->m_asyncResults->insert(std::make_pair(reskey, temp));
					break;
				} else {
					++ind;
				}
			}
			if (t) {
				::VariantClear((::VARIANTARG*)t);
			}
		}
		// ... use the object.

		// AddRef() is only required if the object will be held after
		// the return to the caller.
	}
	return ::WBEMSTATUS::WBEM_S_NO_ERROR;
}

::HRESULT __stdcall WMIQueryAsyncSink::SetStatus(long lFlags, ::HRESULT hResult, ::BSTR strParam,
	::IWbemClassObject __RPC_FAR *pObjParam) {
	return ::WBEMSTATUS::WBEM_S_NO_ERROR;
}

WMIOpResult WMIQueryAsyncSink::SetHandler(WMIHandler *wmiHandler) {
	if (wmiHandler) {
		m_WMIHandler = wmiHandler;
		return WMIOpResult::Success;
	} else {
		return WMIOpResult::Fail;
	}
}

WMIHandler::WMIHandler() {
	m_pLoc = 0;
	m_pSvc = 0;
	m_WMIInitialized = false;
	m_asyncResults = 0;
}

WMIHandler::WMIHandler(const WMIHandler &other) {
	if (this != &other) {
		m_pLoc = other.m_pLoc;
		m_pSvc = other.m_pSvc;
		m_WMIInitialized = other.m_WMIInitialized;
		m_asyncResults = other.m_asyncResults;
		m_asyncRequiredFields = other.m_asyncRequiredFields;
	}
}

#if (defined(STDVER) && STDVER >= 11 && STDVER != 98)
WMIHandler::WMIHandler(WMIHandler &&other) noexcept {
	if (this != &other) {
		m_pLoc = std::move(other.m_pLoc);
		other.m_pLoc = 0;
		m_pSvc = std::move(other.m_pSvc);
		other.m_pSvc = 0;
		m_asyncResults = std::move(other.m_asyncResults);
		m_asyncRequiredFields = std::move(other.m_asyncRequiredFields);
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

#if (defined(STDVER) && STDVER >= 11 && STDVER != 98)
WMIHandler& WMIHandler::operator=(WMIHandler &&other) noexcept {
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

bool WMIHandler::operator==(const WMIHandler &other) const  {
	if (this != &other) {
		return (m_pLoc == other.m_pLoc &&
				m_pSvc == other.m_pSvc &&
				m_asyncResults == other.m_asyncResults &&
				m_WMIInitialized == other.m_WMIInitialized &&
				m_asyncRequiredFields == other.m_asyncRequiredFields);
	} else {
		return true;
	}
}

bool WMIHandler::operator!=(const WMIHandler &other) const {
	if (this != &other) {
		return (m_pLoc != other.m_pLoc ||
				m_pSvc != other.m_pSvc ||
				m_asyncResults != other.m_asyncResults ||
				m_WMIInitialized != other.m_WMIInitialized ||
				m_asyncRequiredFields != other.m_asyncRequiredFields);
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
	hres = ::CoInitializeSecurity(
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
		::CoUninitialize();
		m_pLoc = 0;
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
		m_pLoc->Release();
		::CoUninitialize();
		m_pSvc = 0;
		m_pLoc = 0;
		return WMIOpResult::Fail;
	}
	m_WMIInitialized = true;
	return WMIOpResult::Success;
}

WMIOpResult WMIHandler::RunWMIQuery(std::map<std::wstring, std::wstring> &results,
	const std::vector<std::wstring> requiredFields, const std::wstring query,
	const std::wstring namespacePath, const std::wstring queryLanguage,
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
	::BSTR qLang = ::_bstr_t(queryLanguage.c_str());
	if (!qLang) {
		return WMIOpResult::Fail;
	}
	::BSTR q = ::_bstr_t(query.c_str());
	if (!q) {
		::VariantClear((::VARIANTARG*)qLang);
		return WMIOpResult::Fail;
	}
	::IEnumWbemClassObject *pEnumerator = 0;
	::HRESULT hres = m_pSvc->ExecQuery(qLang, q, ::WBEM_GENERIC_FLAG_TYPE::WBEM_FLAG_FORWARD_ONLY |
		::WBEM_GENERIC_FLAG_TYPE::WBEM_FLAG_RETURN_IMMEDIATELY, 0, &pEnumerator);
	if (FAILED(hres)) {
		::VariantClear((::VARIANTARG*)qLang);
		::VariantClear((::VARIANTARG*)q);
		return WMIOpResult::Fail;
	}
	results.clear();
	::IWbemClassObject *pclsObj = 0;
	unsigned long uReturn = 0;
	size_t cmaxpart = 0, cminpart = 0;
	while (pEnumerator) {
		hres = pEnumerator->Next(::WBEM_INFINITE, 1, &pclsObj, &uReturn);
		if (!uReturn) {
			break;
		}
		size_t ind = 0;
		::VARIANT vtProp = { 0 };
		::CIMTYPE type = { 0 };
		::BSTR t = 0;
		std::wstring reskey, temp;
		for (size_t i = 0; i < requiredFields.size(); ++i) {
			hres = pclsObj->Get(requiredFields[i].c_str(), 0, &vtProp, &type, 0);
			if(FAILED(hres)) {
#ifdef WH_ADDEMPTYVALS
				temp = gc_wmiEmptyval;
#else
				continue;
#endif
			} else {
				if (vtProp.vt != ::VARENUM::VT_NULL) {
					if (::CIMTYPE_ENUMERATION::CIM_FLAG_ARRAY & type) {
#ifdef WH_SKIPARRAYS
						temp = gc_wmiArray;
#else
						long lLower = 0, lUpper = 0;
						void* Element = malloc(1024);
						if (!Element) {
							::VariantClear((::VARIANTARG*)qLang);
							::VariantClear((::VARIANTARG*)q);
							return WMIOpResult::Fail;
						}
						::SAFEARRAY* pSafeArray = vtProp.parray;
						hres = ::SafeArrayGetLBound(pSafeArray, 1, &lLower);
						if (FAILED(hres)) {
							::VariantClear((::VARIANTARG*)qLang);
							::VariantClear((::VARIANTARG*)q);
							::SafeArrayDestroy(pSafeArray);
							return WMIOpResult::Fail;
						}
						hres = ::SafeArrayGetUBound(pSafeArray, 1, &lUpper);
						if (FAILED(hres)) {
							::VariantClear((::VARIANTARG*)qLang);
							::VariantClear((::VARIANTARG*)q);
							::SafeArrayDestroy(pSafeArray);
							return WMIOpResult::Fail;
						}
						for (long i = lLower; i <= lUpper; ++i) {
							memset(Element, 0, 1024);
							hres = ::SafeArrayGetElement(pSafeArray, &i, Element);
							if (FAILED(hres)) {
								::VariantClear((::VARIANTARG*)qLang);
								::VariantClear((::VARIANTARG*)q);
								::SafeArrayDestroy(pSafeArray);
								return WMIOpResult::Fail;
							}
							if (i < lUpper) {
								// temp = temp + std::wstring(::_bstr_t(Element)) + gc_wmiJoinSymbol;
								temp = temp + char2wchar((char*)Element) + gc_wmiJoinSymbol;
							} else {
								// temp = temp + std::wstring(::_bstr_t(Element));
								temp = temp + char2wchar((char*)Element);
							}
						}
						::SafeArrayDestroy(pSafeArray);
#endif
					} else {
						t = ::_bstr_t(vtProp);
						if (t) {
#ifdef WH_TRIMDATA
							temp = trim_copy(t);
#else
							temp = t;
#endif
							::SysFreeString(t);
						} else {
							temp = gc_wmiEmptyval;
						}
						hres = ::VariantClear(&vtProp);
						if (S_OK != hres) {
							::VariantClear((::VARIANTARG*)qLang);
							::VariantClear((::VARIANTARG*)q);
							return WMIOpResult::Fail;
						}
					}
				} else {
#ifdef WH_ADDEMPTYVALS
					temp = gc_wmiEmptyval;
#endif
					hres = ::VariantClear(&vtProp);
					if (S_OK != hres) {
						::VariantClear((::VARIANTARG*)qLang);
						::VariantClear((::VARIANTARG*)q);
						return WMIOpResult::Fail;
					}
#ifndef WH_ADDEMPTYVALS
					continue;
#endif
				}
			}
			ind = 0;
			while (true) {
				reskey = requiredFields[i] + std::to_wstring(ind);
				if (!results.count(reskey)) {
					results.insert(std::make_pair(reskey, temp));
					temp = L"";
					break;
				} else {
					++ind;
				}
			}
		}
	}
	if (shutdownWMI) {
		if (WMIOpResult::Success != ShutdownWMI()) {
			::VariantClear((::VARIANTARG*)qLang);
			::VariantClear((::VARIANTARG*)q);
			return WMIOpResult::Fail;
		}
	}
	::VariantClear((::VARIANTARG*)qLang);
	::VariantClear((::VARIANTARG*)q);
	return WMIOpResult::Success;
}

WMIOpResult WMIHandler::RunWMIQueryAsync(std::map<std::wstring, std::wstring> &results,
	const std::vector<std::wstring> requiredFields,
	const std::wstring query, const std::wstring namespacePath,
	const std::wstring queryLanguage, const bool initWMI, const bool shutdownWMI,
	const unsigned long awaitTimeout) {
	if (!m_WMIInitialized) {
		if (initWMI) {
			if (WMIOpResult::Success != InitWMI(namespacePath)) {
				return WMIOpResult::Fail;
			}
		} else {
			return WMIOpResult::Fail;
		}
	}
	unsigned long awaitTime = awaitTimeout;
	if (WH_MIMAWAITTIMEOUT > awaitTime) {
		awaitTime = WH_MIMAWAITTIMEOUT;
	} else if (WH_MAXAWAITTIMEOUT < awaitTime) {
		awaitTime = WH_MAXAWAITTIMEOUT;
	}
	::BSTR qLang = ::_bstr_t(queryLanguage.c_str());
	if (!qLang) {
		return WMIOpResult::Fail;
	}
	::BSTR q = ::_bstr_t(query.c_str());
	if (!q) {
		::VariantClear((::VARIANTARG*)qLang);
		return WMIOpResult::Fail;
	}
	WMIQueryAsyncSink* pSink = new WMIQueryAsyncSink;
	if (!pSink) {
		::VariantClear((::VARIANTARG*)qLang);
		::VariantClear((::VARIANTARG*)q);
		return WMIOpResult::Fail;
	}
	if (WMIOpResult::Success != pSink->SetHandler(this)) {
		return WMIOpResult::Fail;
	}
	results.clear();
	m_asyncResults = &results;
	m_asyncRequiredFields = requiredFields;
	::HRESULT hres = m_pSvc->ExecQueryAsync(qLang, q, 0, 0, pSink);
	Sleep(awaitTime);
	if (FAILED(hres)) {
		::VariantClear((::VARIANTARG*)qLang);
		::VariantClear((::VARIANTARG*)q);
		pSink->Release();
		SAFE_DELETE(pSink);
		return WMIOpResult::Fail;
	}
	hres = m_pSvc->CancelAsyncCall(pSink);
	if (FAILED(hres)) {
		::VariantClear((::VARIANTARG*)qLang);
		::VariantClear((::VARIANTARG*)q);
		pSink->Release();
		SAFE_DELETE(pSink);
		return WMIOpResult::Fail;
	}
	if (shutdownWMI) {
		if (WMIOpResult::Success != ShutdownWMI()) {
			::VariantClear((::VARIANTARG*)qLang);
			::VariantClear((::VARIANTARG*)q);
			pSink->Release();
			SAFE_DELETE(pSink);
			return WMIOpResult::Fail;
		}
	}
	::VariantClear((::VARIANTARG*)qLang);
	::VariantClear((::VARIANTARG*)q);
	// pSink->Release();
	// SAFE_DELETE(pSink);
	return WMIOpResult::Success;
}

WMIOpResult WMIHandler::GetFieldsFromQuery(std::vector<std::wstring> &fields, const std::wstring query,
	const std::wstring namespacePath, const std::wstring queryLanguage,
	const bool clearFields, const bool initWMI, const bool shutdownWMI) {
	if (!m_WMIInitialized) {
		if (initWMI) {
			if (WMIOpResult::Success != InitWMI(namespacePath)) {
				return WMIOpResult::Fail;
			}
		} else {
			return WMIOpResult::Fail;
		}
	}
	::BSTR qLang = ::_bstr_t(queryLanguage.c_str());
	if (!qLang) {
		return WMIOpResult::Fail;
	}
	::BSTR q = ::_bstr_t(query.c_str());
	if (!q) {
		::VariantClear((::VARIANTARG*)qLang);
		return WMIOpResult::Fail;
	}
	unsigned long uReturn = 0;
	::IWbemClassObject* pObj = 0;
	::IEnumWbemClassObject* pEnumerator = 0;
	::HRESULT hres = m_pSvc->ExecQuery(qLang, q, ::WBEM_GENERIC_FLAG_TYPE::WBEM_FLAG_FORWARD_ONLY |
		::WBEM_GENERIC_FLAG_TYPE::WBEM_FLAG_RETURN_IMMEDIATELY, 0, &pEnumerator);
	if (FAILED(hres)) {
		::VariantClear((::VARIANTARG*)qLang);
		::VariantClear((::VARIANTARG*)q);
		return WMIOpResult::Fail;
	}
	while (pEnumerator) {
		hres = pEnumerator->Next(::WBEM_INFINITE, 1, &pObj, &uReturn);
		if (!uReturn) {
			break;
		}
		::SAFEARRAY* psaNames = NULL;
		hres = pObj->GetNames(0, ::WBEM_CONDITION_FLAG_TYPE::WBEM_FLAG_ALWAYS |
			::WBEM_CONDITION_FLAG_TYPE::WBEM_FLAG_NONSYSTEM_ONLY, 0, &psaNames);
		if (FAILED(hres)) {
			::VariantClear((::VARIANTARG*)qLang);
			::VariantClear((::VARIANTARG*)q);
			return WMIOpResult::Fail;
		}
		long lLower = 0, lUpper = 0;
		::BSTR PropName = 0;
		hres = ::SafeArrayGetLBound(psaNames, 1, &lLower);
		if (FAILED(hres)) {
			::VariantClear((::VARIANTARG*)qLang);
			::VariantClear((::VARIANTARG*)q);
			::SafeArrayDestroy(psaNames);
			return WMIOpResult::Fail;
		}
		hres = ::SafeArrayGetUBound(psaNames, 1, &lUpper);
		if (FAILED(hres)) {
			::VariantClear((::VARIANTARG*)qLang);
			::VariantClear((::VARIANTARG*)q);
			::SafeArrayDestroy(psaNames);
			return WMIOpResult::Fail;
		}
		if (clearFields) {
			fields.clear();
		}
		for (long i = lLower; i <= lUpper; i++) {
			// Get this property.
			hres = SafeArrayGetElement(psaNames, &i, &PropName);
			if (FAILED(hres)) {
				::VariantClear((::VARIANTARG*)qLang);
				::VariantClear((::VARIANTARG*)q);
				::SafeArrayDestroy(psaNames);
				return WMIOpResult::Fail;
			}
			if (PropName) {
				if (std::find(fields.begin(), fields.end(), PropName) == fields.end()) {
					fields.push_back(PropName);
				}
				::VariantClear((::VARIANTARG*)PropName);
			}
		}
		::SafeArrayDestroy(psaNames);
	}
	if (shutdownWMI) {
		if (WMIOpResult::Success != ShutdownWMI()) {
			::VariantClear((::VARIANTARG*)qLang);
			::VariantClear((::VARIANTARG*)q);
			return WMIOpResult::Fail;
		}
	}
	// ::VariantClear((::VARIANTARG*)qLang);
	::VariantClear((::VARIANTARG*)q);
	return WMIOpResult::Success;
}

WMIOpResult WMIHandler::GetFieldsFromObject(std::vector<std::wstring> &fields, const std::wstring objectName,
	const std::wstring namespacePath, const bool clearFields, const bool initWMI,
	const bool shutdownWMI) {
	if (!m_WMIInitialized) {
		if (initWMI) {
			if (WMIOpResult::Success != InitWMI(namespacePath)) {
				return WMIOpResult::Fail;
			}
		} else {
			return WMIOpResult::Fail;
		}
	}
	::BSTR wmiobj = ::_bstr_t(objectName.c_str());
	if (!wmiobj) {
		return WMIOpResult::Fail;
	}
	::IWbemClassObject* pClass = 0;
	::HRESULT hres = m_pSvc->GetObject(wmiobj, 0, 0, &pClass, 0);
	if (FAILED(hres)) {
		::VariantClear((::VARIANTARG*)wmiobj);
		return WMIOpResult::Fail;
	}
	::SAFEARRAY* psaNames = 0;
	hres = pClass->GetNames(0,
		::WBEM_CONDITION_FLAG_TYPE::WBEM_FLAG_ALWAYS | ::WBEM_CONDITION_FLAG_TYPE::WBEM_FLAG_NONSYSTEM_ONLY,
		0, &psaNames);
	if (FAILED(hres)) {
		::VariantClear((::VARIANTARG*)wmiobj);
		return WMIOpResult::Fail;
	}
	long lLower = 0, lUpper = 0;
	::BSTR PropName = 0;
	hres = ::SafeArrayGetLBound(psaNames, 1, &lLower);
	if (FAILED(hres)) {
		::VariantClear((::VARIANTARG*)wmiobj);
		::SafeArrayDestroy(psaNames);
		return WMIOpResult::Fail;
	}
	hres = ::SafeArrayGetUBound(psaNames, 1, &lUpper);
	if (FAILED(hres)) {
		::VariantClear((::VARIANTARG*)wmiobj);
		::SafeArrayDestroy(psaNames);
		return WMIOpResult::Fail;
	}
	if (clearFields) {
		fields.clear();
	}
	for (long i = lLower; i <= lUpper; i++) {
		// Get this property.
		hres = ::SafeArrayGetElement(psaNames, &i, &PropName);
		if (FAILED(hres)) {
			::VariantClear((::VARIANTARG*)wmiobj);
			::SafeArrayDestroy(psaNames);
			return WMIOpResult::Fail;
		}
		if (PropName) {
			if (std::find(fields.begin(), fields.end(), PropName) == fields.end()) {
				fields.push_back(PropName);
			}
			::VariantClear((::VARIANTARG*)PropName);
		}
	}
	::SafeArrayDestroy(psaNames);
	if (shutdownWMI) {
		if (WMIOpResult::Success != ShutdownWMI()) {
			::VariantClear((::VARIANTARG*)wmiobj);
			return WMIOpResult::Fail;
		}
	}
	::VariantClear((::VARIANTARG*)wmiobj);
	::VariantClear((::VARIANTARG*)wmiobj);
	return WMIOpResult::Success;
}

WMIOpResult WMIHandler::ShutdownWMI() {
	if (m_pSvc) {
		m_pSvc->Release();
	}
	if (m_pLoc) {
		m_pLoc->Release();
	}
	::CoUninitialize();
	m_pSvc = 0;
	m_pLoc = 0;
	m_WMIInitialized = false;
	return WMIOpResult::Success;
}