#ifndef _WMIHELPER_H
#define _WMIHELPER_H

#define _WIN32_DCOM
#include <comdef.h>
#include <Wbemidl.h>
#include "config.h"
#include "syshelper.h"
#include "strhelper.h"

const std::wstring const gc_wmiEmptyval		= L"(null)";
const std::wstring const gc_wmiArray		= L"(array)";
const std::wstring const gc_wmiJoinSymbol	= L" ]|[ ";

enum class WMIOpResult : unsigned char {
	Success,
	Fail
};

class WMIQueryAsyncSink;
class WMIHandler;

// https://learn.microsoft.com/en-us/windows/win32/wmisdk/iwbemobjectsink
class WMIQueryAsyncSink : public ::IWbemObjectSink {
	public:
		WMIQueryAsyncSink();
		WMIQueryAsyncSink(const WMIQueryAsyncSink &other);
#if (defined(STDVER) && STDVER >= 11 && STDVER != 98)
		WMIQueryAsyncSink(WMIQueryAsyncSink &&other) noexcept;
#endif
		WMIQueryAsyncSink& operator=(const WMIQueryAsyncSink &other);
#if (defined(STDVER) && STDVER >= 11 && STDVER != 98)
		WMIQueryAsyncSink& operator=(WMIQueryAsyncSink &&other) noexcept;
#endif
		bool operator==(const WMIQueryAsyncSink &other) const;
		bool operator!=(const WMIQueryAsyncSink &other) const;
		~WMIQueryAsyncSink();
		// https://learn.microsoft.com/en-us/windows/win32/wmisdk/iwbemobjectsink
		virtual unsigned long __stdcall AddRef();
		virtual unsigned long __stdcall Release();
		virtual ::HRESULT __stdcall QueryInterface(const GUID &riid, void** ppv);
		virtual ::HRESULT __stdcall Indicate(long lObjectCount,
			::IWbemClassObject __RPC_FAR* __RPC_FAR* apObjArray);
		virtual ::HRESULT __stdcall SetStatus(long lFlags, ::HRESULT hResult, ::BSTR strParam,
			::IWbemClassObject __RPC_FAR* pObjParam);
		WMIOpResult SetHandler(WMIHandler *wmiHandler);
	protected:
	private:
		long m_lRef;
		int m_bDone;
		WMIHandler* m_WMIHandler;
};

class WMIHandler {
	public:
		WMIHandler();
		WMIHandler(const WMIHandler &other);
#if (defined(STDVER) && STDVER >= 11 && STDVER != 98)
		WMIHandler(WMIHandler &&other) noexcept;
#endif
		WMIHandler& operator=(const WMIHandler &other);
#if (defined(STDVER) && STDVER >= 11 && STDVER != 98)
		WMIHandler& operator=(WMIHandler &&other) noexcept;
#endif
		bool operator==(const WMIHandler &other) const;
		bool operator!=(const WMIHandler &other) const;
		~WMIHandler();
		friend class WMIQueryAsyncSink;
		WMIOpResult InitWMI(const std::wstring namespacePath = L"ROOT\\CIMV2");
		WMIOpResult RunWMIQuery(std::map<std::wstring, std::wstring> &results,
			const std::vector<std::wstring> requiredFields,
			const std::wstring query, const std::wstring namespacePath = L"ROOT\\CIMV2",
			const std::wstring queryLanguage = L"WQL", const bool initWMI = true, const bool shutdownWMI = false);
		WMIOpResult RunWMIQueryAsync(std::map<std::wstring, std::wstring>& results,
			const std::vector<std::wstring> requiredFields,
			const std::wstring query, const std::wstring namespacePath = L"ROOT\\CIMV2",
			const std::wstring queryLanguage = L"WQL", const bool initWMI = true, const bool shutdownWMI = false,
			const unsigned long awaitTimeout = 500);
		WMIOpResult GetFieldsFromQuery(std::vector<std::wstring> &fields, const std::wstring query,
			const std::wstring namespacePath = L"ROOT\\CIMV2", const std::wstring queryLanguage = L"WQL",
			const bool clearFields = true, const bool initWMI = true, const bool shutdownWMI = false);
		WMIOpResult GetFieldsFromObject(std::vector<std::wstring> &fields, const std::wstring objectName,
			const std::wstring namespacePath = L"ROOT\\CIMV2", const bool clearFields = true,
			const bool initWMI = true, const bool shutdownWMI = false);
		WMIOpResult ShutdownWMI();
	protected:
		std::vector<std::wstring> m_asyncRequiredFields;
		std::map<std::wstring, std::wstring> *m_asyncResults;
	private:
		bool m_WMIInitialized;
		::IWbemLocator *m_pLoc;
		::IWbemServices *m_pSvc;
};

#endif // _WMIHELPER_H