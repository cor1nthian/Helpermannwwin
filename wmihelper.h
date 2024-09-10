#ifndef _WMIHandler_H
#define _WMIHandler_H

#define _WIN32_DCOM
#include <comdef.h>
#include <Wbemidl.h>
#include "config.h"
#include "strhelper.h"

enum class WMIOpResult : unsigned char {
	Success,
	Fail
};

class WMIHandler {
	public:
		WMIHandler();
		WMIHandler(const WMIHandler &other);
#if (COMPILERVER >= 11 && COMPILERVER != 98)
		WMIHandler(WMIHandler &&other);
#endif
		WMIHandler& operator=(const WMIHandler &other);
#if (COMPILERVER >= 11 && COMPILERVER != 98)
		WMIHandler& operator=(WMIHandler &&other);
#endif
		bool operator==(const WMIHandler &other);
		bool operator!=(const WMIHandler &other);
		~WMIHandler();
		WMIOpResult InitWMI(const std::wstring namespacePath = L"ROOT\\CIMV2");
		WMIOpResult RunWMIQuery(std::map<std::wstring, std::wstring> &results,
			const std::vector<std::wstring> requiredFields,
			const std::wstring query, const std::wstring namespacePath = L"ROOT\\CIMV2",
			const std::wstring queryLanguage = L"WQL", const bool initWMI = true, const bool shutdownWMI = false);
		WMIOpResult ShutdownWMI();
	protected:

	private:
		bool m_WMIInitialized;
		::IWbemLocator *m_pLoc;
		::IWbemServices *m_pSvc;
};

#endif
