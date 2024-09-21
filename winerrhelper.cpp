#include "winerrhelper.h"

void errorMsgBox(const std::wstring errormsg) {
	::MessageBox(0, errormsg.c_str(), L"ERROR", MB_ICONERROR);
}

void setLastError(const unsigned long code) {
	::SetLastError(code);
}

unsigned long getLastErrorCode() {
	return ::GetLastError();
}

std::wstring getErrorDetails(const unsigned long errcode, const std::wstring funcName,
	const std::wstring additionalInfo) {
	unsigned long errCode = errcode;
	if (!errCode) {
		errCode = ::GetLastError();
	}
	wchar_t descBuf[256] = { 0 };
	wchar_t msgbuf[1024] = { 0 };
	if (funcName.length()) {
		wcscpy(msgbuf, L"Error calling function ");
		wcscat(msgbuf, funcName.c_str());
	} else {
		wcscpy(msgbuf, L"Call failed");
	}
	wcscat(msgbuf, L"\n\nError code: ");
	wcscat(msgbuf, std::to_wstring(errCode).c_str());
	wcscat(msgbuf, L"\n\nDetails:\n");
	::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		descBuf, 256, NULL);
	wcscat(msgbuf, descBuf);
	if (additionalInfo.length()) {
		wcscat(msgbuf, L"\nAdditional info:\n");
		wcscat(msgbuf, additionalInfo.c_str());
	}
	return msgbuf;
}

bool getErrorInfo(::IErrorInfo* errorInfo) {
	return (S_OK == ::GetErrorInfo(0, &errorInfo));
}

bool getIObjectErrorDetails(std::wstring &errorDescription, ::IUnknown* object, ::IErrorInfo* errinf) {
	// ::IErrorInfo* errinf = 0;
	::IErrorRecords* pErrorRecords = 0;
	/*if (S_OK != ::GetErrorInfo(0, &errinf)) {
		return false;
	}*/
	/*::HRESULT hres = object->QueryInterface(IID_IErrorRecords, (void**)&pErrorRecords);
	if (FAILED(hres)) {
		return false;
	}*/
	// Get the IErrorRecord interface, and get the count
		 // of error recs.
	::HRESULT hres = object->QueryInterface(IID_IErrorRecords, (void**)&pErrorRecords);
	if (FAILED(hres)) {
		return false;
	}
	unsigned long recCount = 0;
	hres = pErrorRecords->GetRecordCount(&recCount);
	if (FAILED(hres)) {
		return false;
	}
	::IErrorInfo* pErrorInfoRec = 0;
	::ERRORINFO ErrorInfo = { 0 };
	for (unsigned long i = 0; i < recCount; ++i) {
		hres = pErrorRecords->GetBasicErrorInfo(i, &ErrorInfo);
		if (FAILED(hres)) {
			return false;
		}
		// Get error description and source through the
		// IErrorInfo interface pointer on a particular record.
		pErrorRecords->GetErrorInfo(i, ::GetUserDefaultUILanguage(), &pErrorInfoRec);
		::BSTR bstrDescriptionOfError = 0;
		::BSTR bstrSourceOfError = 0;
		pErrorInfoRec->GetDescription(&bstrDescriptionOfError);
		pErrorInfoRec->GetSource(&bstrSourceOfError);
		// At this point, you could call GetCustomErrorObject
		// and query for additional interfaces to determine
		// what else happened.
		wchar_t* errBuf = (wchar_t*)malloc(1024 * sizeof(wchar_t));
		if (!errBuf) {
			return false;
		}
		wsprintf(errBuf, L"HRESULT: %lx, Minor Code: %lu, Source: %s, Description: %s", ErrorInfo.hrError,
			ErrorInfo.dwMinor, bstrSourceOfError, bstrDescriptionOfError);
		if (errorDescription.length()) {
			errorDescription = errorDescription + L"\n" + errBuf;
		} else {
			errorDescription = errBuf;
		}
		SAFE_FREE(errBuf);
		// Free the resources.
		::SysFreeString(bstrDescriptionOfError);
		::SysFreeString(bstrSourceOfError);
		pErrorInfoRec->Release();
	}
	// Release the error object.
	pErrorRecords->Release();
	return true;
}