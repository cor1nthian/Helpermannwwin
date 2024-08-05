#include <algorithm>
#include "dbhelper.h"

MSSQLOpResult getAvailableODBCDrivers(std::vector<std::wstring> &drivers, const bool includeAttrs) {
	SQLHENV hEnv = 0;
	wchar_t driver[512] = { 0 }, attr[512] = { 0 };
	short driver_ret = 0, attr_ret, direction = 0, ret = 0;
	short rc = ::SQLAllocHandle(SQL_HANDLE_ENV, 0, &hEnv);
	if (SQL_SUCCESS != rc) {
		if (SQL_SUCCESS_WITH_INFO == rc) {
		} else if (SQL_INVALID_HANDLE == rc) {
			return MSSQLOpResult::Fail;
		} else if (SQL_ERROR == rc) {
			return MSSQLOpResult::Fail;
		}
	}
	rc = ::SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
	if (SQL_SUCCESS != rc) {
		if (SQL_SUCCESS_WITH_INFO == rc) {
		} else if (SQL_INVALID_HANDLE == rc) {
			return MSSQLOpResult::Fail;
		} else if (SQL_ERROR == rc) {
			return MSSQLOpResult::Fail;
		}
	}
	direction = SQL_FETCH_FIRST;
	while (SQL_SUCCEEDED(ret = ::SQLDrivers(hEnv, direction, driver, 512, &driver_ret, attr, 512, &attr_ret))) {
		direction = SQL_FETCH_NEXT;
		if (includeAttrs) {
			drivers.push_back(std::wstring(driver) + L" " + attr);
		} else {
			drivers.push_back(driver);
		}
		if (ret == SQL_SUCCESS_WITH_INFO) {
			continue;
			// printf("\tdata truncation\n");
		}
	}
	rc = ::SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
	if (SQL_SUCCESS != rc) {
		if (SQL_SUCCESS_WITH_INFO == rc) {
		} else if (SQL_INVALID_HANDLE == rc) {
			return MSSQLOpResult::Fail;
		} else if (SQL_ERROR == rc) {
			return MSSQLOpResult::Fail;
		}
	}
	return MSSQLOpResult::Success;
}

MSSQLBinding::MSSQLBinding() {
	cDisplaySize = 0;
	wszBuffer = 0;
	indPtr = 0;
	fChar = false;
	sNext = 0;
}

MSSQLBinding::MSSQLBinding(const MSSQLBinding& other) {
	if (this != &other) {
		cDisplaySize = other.cDisplaySize;
		wszBuffer = other.wszBuffer;
		indPtr = other.indPtr;
		fChar = other.fChar;
		sNext = other.sNext;
	}
}

MSSQLBinding::MSSQLBinding(MSSQLBinding &&other) noexcept {
	if (this != &other) {
		cDisplaySize = other.cDisplaySize;
		other.cDisplaySize = 0;
		wszBuffer = other.wszBuffer;
		other.cDisplaySize = 0;
		indPtr = other.indPtr;
		other.indPtr = 0;
		fChar = other.fChar;
		other.fChar = false;
		sNext = other.sNext;
		other.sNext = 0;
	}
}

MSSQLBinding::~MSSQLBinding() {

}

MSSQLBinding& MSSQLBinding::operator=(const MSSQLBinding &other) {
	if (this != &other) {
		cDisplaySize = other.cDisplaySize;
		wszBuffer = other.wszBuffer;
		indPtr = other.indPtr;
		fChar = other.fChar;
		sNext = other.sNext;
	}
	return *this;
}

MSSQLBinding& MSSQLBinding::operator=(MSSQLBinding &&other) noexcept {
	if (this != &other) {
		cDisplaySize = other.cDisplaySize;
		other.cDisplaySize = 0;
		wszBuffer = other.wszBuffer;
		other.cDisplaySize = 0;
		indPtr = other.indPtr;
		other.indPtr = 0;
		fChar = other.fChar;
		other.fChar = false;
		sNext = other.sNext;
		other.sNext = 0;
	}
	return *this;
}

bool MSSQLBinding::operator==(const MSSQLBinding &other) const {
	if (this != &other) {
		return (cDisplaySize == other.cDisplaySize &&
				wszBuffer == other.wszBuffer &&
				indPtr == other.indPtr &&
				fChar == other.fChar &&
				sNext == other.sNext);
	} else {
		return true;
	}
}

bool MSSQLBinding::operator!=(const MSSQLBinding &other) const {
	if (this != &other) {
		return (cDisplaySize != other.cDisplaySize ||
				wszBuffer != other.wszBuffer ||
				indPtr != other.indPtr ||
				fChar != other.fChar ||
				sNext != other.sNext);
	} else {
		return false;
	}
}

MSSQLOutBuf::MSSQLOutBuf() {
	OutBuf = 0;
	OutBufSz = 0;
}

MSSQLOutBuf::MSSQLOutBuf(const size_t outbufsz) {
	OutBufSz = outbufsz;
	NEW_ARR_NULLIFY_NO_REDEFINE(OutBuf, wchar_t, OutBufSz);
}

MSSQLOutBuf::MSSQLOutBuf(const wchar_t* outbuf, const size_t outbufsz) {
	OutBufSz = outbufsz;
	NEW_ARR_NULLIFY_NO_REDEFINE(OutBuf, wchar_t, OutBufSz);
	if (OutBuf) {
		wsprintf(OutBuf, L"%s", outbuf);
	}
}

MSSQLOutBuf::MSSQLOutBuf(const MSSQLOutBuf &other) {
	if (this != &other) {
		OutBufSz = other.OutBufSz;
		if (OutBuf) {
			SAFE_ARR_DELETE(OutBuf);
		}
		NEW_ARR_NULLIFY_NO_REDEFINE(OutBuf, wchar_t, OutBufSz);
		if (OutBuf) {
			memcpy(OutBuf, other.OutBuf, OutBufSz * sizeof(wchar_t));
		}
	}
}

MSSQLOutBuf::MSSQLOutBuf(MSSQLOutBuf &&other) noexcept {
	if (this != &other) {
		OutBuf = other.OutBuf;
		other.OutBuf = 0;
		OutBufSz = other.OutBufSz;
		other.OutBuf = 0;
	}
}

MSSQLOutBuf::~MSSQLOutBuf() {}

MSSQLOutBuf& MSSQLOutBuf::operator=(const MSSQLOutBuf &other) {
	if (this != &other) {
		OutBufSz = other.OutBufSz;
		if (OutBuf) {
			SAFE_ARR_DELETE(OutBuf);
		}
		NEW_ARR_NULLIFY_NO_REDEFINE(OutBuf, wchar_t, OutBufSz);
		if (OutBuf) {
			memcpy(OutBuf, other.OutBuf, OutBufSz * sizeof(wchar_t));
		}
	}
	return *this;
}

MSSQLOutBuf& MSSQLOutBuf::operator=(MSSQLOutBuf &&other) noexcept {
	if (this != &other) {
		OutBuf = other.OutBuf;
		other.OutBuf = 0;
		OutBufSz = other.OutBufSz;
		other.OutBufSz = 0;
	}
	return *this;
}

bool MSSQLOutBuf::operator==(const MSSQLOutBuf &other) const {
	if (this != &other) {
		return (!memcmp(OutBuf, other.OutBuf, OutBufSz * sizeof(wchar_t)) &&
				OutBufSz == other.OutBufSz);
	} else {
		return true;
	}
}

bool MSSQLOutBuf::operator!=(const MSSQLOutBuf &other) const {
	if (this != &other) {
		return (memcmp(OutBuf, other.OutBuf, OutBufSz * sizeof(wchar_t)) ||
				OutBufSz != other.OutBufSz);
	} else {
		return false;
	}
}

MSSQLQuery::MSSQLQuery() {
	DBID = 0;
	QueryID = 0;
}

MSSQLQuery::MSSQLQuery(const SQLHANDLE dbid, const SQLHSTMT queryid, const std::wstring querystr) {
	DBID = dbid;
	QueryID = queryid;
	QueryStr = querystr;
}

MSSQLQuery::MSSQLQuery(const MSSQLQuery &other) {
	if (this != &other) {
		DBID = other.DBID;
		QueryID = other.QueryID;
		QueryStr = other.QueryStr;
	}
}

MSSQLQuery::MSSQLQuery(MSSQLQuery &&other) noexcept {
	if (this != &other) {
		DBID = other.DBID;
		other.DBID = 0;
		QueryID = other.QueryID;
		other.QueryID = 0;
		QueryStr = other.QueryStr;
		other.QueryStr.~basic_string();
	}
}

MSSQLQuery::~MSSQLQuery() {}

MSSQLQuery& MSSQLQuery::operator=(const MSSQLQuery &other) {
	if (this != &other) {
		DBID = other.DBID;
		QueryID = other.QueryID;
		QueryStr = other.QueryStr;
	}
	return *this;
}

MSSQLQuery& MSSQLQuery::operator=(MSSQLQuery &&other) noexcept {
	if (this != &other) {
		DBID = other.DBID;
		other.DBID = 0;
		QueryID = other.QueryID;
		other.QueryID = 0;
		QueryStr = other.QueryStr;
		other.QueryStr.~basic_string();
	}
	return *this;
}

bool MSSQLQuery::operator==(const MSSQLQuery &other) const {
	if (this != &other) {
		return (DBID == other.DBID &&
				QueryID == other.QueryID &&
				lower_copy(QueryStr) == lower_copy(other.QueryStr));
	} else {
		return true;
	}
}

bool MSSQLQuery::operator!=(const MSSQLQuery &other) const {
	if (this != &other) {
		return (DBID != other.DBID ||
				QueryID != other.QueryID ||
				lower_copy(QueryStr) != lower_copy(other.QueryStr));
	} else {
		return false;
	}
}

MSSQLDBHandler::MSSQLDBHandler() {
	m_hEnv = 0;
}

MSSQLDBHandler::MSSQLDBHandler(const SQLHANDLE henv, const std::map<SQLHANDLE, std::wstring> connectedDBS,
	const std::map<SQLHANDLE, MSSQLOutBuf> outBuffers, const std::vector<MSSQLQuery> runningQueries) {
	m_hEnv = henv;
	m_ConnectedDBs = connectedDBS;
	m_OutBuffers = outBuffers;
	m_RunningQueries = runningQueries;
}

MSSQLDBHandler::MSSQLDBHandler(const MSSQLDBHandler &other) {
	if (this != &other) {
		m_hEnv = other.m_hEnv;
		m_ConnectedDBs = other.m_ConnectedDBs;
		m_OutBuffers = other.m_OutBuffers;
		m_RunningQueries = other.m_RunningQueries;
	}
}

MSSQLDBHandler::MSSQLDBHandler(MSSQLDBHandler &&other) noexcept {
	if (this != &other) {
		m_hEnv = other.m_hEnv;
		other.m_hEnv = 0;
		m_ConnectedDBs = other.m_ConnectedDBs;
		other.m_ConnectedDBs.~map();
		m_OutBuffers = other.m_OutBuffers;
		other.m_OutBuffers.~map();
		m_RunningQueries = other.m_RunningQueries;
		other.m_RunningQueries.~vector();
	}
}

MSSQLDBHandler::~MSSQLDBHandler() {
	short rc = 0;
	if (m_ConnectedDBs.size()) {
		while (m_ConnectedDBs.size()) {
			DisconnectDB(m_ConnectedDBs.begin()->first);
		}
	}
	rc = ::SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
	if (SQL_SUCCESS != rc) {
		if (SQL_SUCCESS_WITH_INFO == rc) {
		} else if (SQL_INVALID_HANDLE == rc) {
			// handle error
		} else if (SQL_ERROR == rc) {
			// handle error
		} else if (SQL_STILL_EXECUTING == rc) {
			// handle error
		}
	}
}

MSSQLDBHandler& MSSQLDBHandler::operator=(const MSSQLDBHandler &other) {
	if (this != &other) {
		m_hEnv = other.m_hEnv;
		m_ConnectedDBs = other.m_ConnectedDBs;
		m_OutBuffers = other.m_OutBuffers;
		m_RunningQueries = other.m_RunningQueries;
	}
	return *this;
}

MSSQLDBHandler& MSSQLDBHandler::operator=(MSSQLDBHandler &&other) noexcept {
	if (this != &other) {
		m_hEnv = other.m_hEnv;
		other.m_hEnv = 0;
		m_ConnectedDBs = other.m_ConnectedDBs;
		other.m_ConnectedDBs.~map();
		m_OutBuffers = other.m_OutBuffers;
		other.m_OutBuffers.~map();
		m_RunningQueries = other.m_RunningQueries;
		other.m_RunningQueries.~vector();
	}
	return *this;
}

bool MSSQLDBHandler::operator==(const MSSQLDBHandler &other) const {
	if (this != &other) {
		return (m_hEnv == other.m_hEnv &&
				m_ConnectedDBs == other.m_ConnectedDBs &&
				m_OutBuffers == other.m_OutBuffers &&
				m_RunningQueries == other.m_RunningQueries);
	} else {
		return true;
	}
}

bool MSSQLDBHandler::operator!=(const MSSQLDBHandler &other) const {
	if (this != &other) {
		return (m_hEnv != other.m_hEnv ||
				m_ConnectedDBs != other.m_ConnectedDBs ||
				m_OutBuffers != other.m_OutBuffers ||
				m_RunningQueries != other.m_RunningQueries);
	} else {
		return false;
	}
}

MSSQLOpResult MSSQLDBHandler::ConnectDB(SQLHANDLE &connID, const std::wstring serverAddr, std::wstring *infoBuf,
	const std::wstring dbName, const std::wstring port, const std::wstring login, const std::wstring pwd,
	const MSSQLConnTrust trustRel, const MSSQLDriverType driverType, const MSSQLDriverPreference driverPref,
	const std::wstring defaultDriver, const unsigned long connOutBufLen) {
	if ((!login.length() && !pwd.length() && (MSSQLConnTrust::Trusted != trustRel)) || (!serverAddr.length()) ||
		(connOutBufLen < MSSQLMINOUTBUF)) {
		return MSSQLOpResult::Fail;
	}
	if (m_ConnectedDBs.size() == MSSQLMAXCONN) {
		return MSSQLOpResult::Fail;
	}
	std::wstring connRec = serverAddr;
	if (port.length()) {
		connRec = connRec + L"|" + port;
	}
	if (dbName.length()) {
		connRec = connRec + L"|" + dbName;
	}
	if (login.length()) {
		connRec = connRec + L"|" + login;
	} else {
		connRec = connRec + L"|Trusted";
	}
	for (auto &it : m_ConnectedDBs) {
		if (lower_copy(connRec) == lower_copy(it.second)) {
			connID = it.first;
			return MSSQLOpResult::Fail;
		}
	}
	SQLHENV hEnv = 0;
	SQLHDBC hDbc = 0;
	short rc = 0;
	NEW_ARR_NULLIFY(errbuf, wchar_t, MSSQLMAXOUTBUF);
	if (!errbuf) {
		return MSSQLOpResult::Fail;
	}
	if (!m_hEnv) {
		rc = ::SQLAllocHandle(SQL_HANDLE_ENV, 0, &m_hEnv);
		if (SQL_SUCCESS != rc) {
			if (SQL_SUCCESS_WITH_INFO == rc) {
				if (infoBuf) {
					if (MSSQLOpResult::Success != SQLInfoDetails(m_hEnv, SQL_HANDLE_ENV, rc, errbuf, MSSQLMAXOUTBUF)) {
						return MSSQLOpResult::Fail;
					}
					*infoBuf = errbuf;
				}
			} else if (SQL_INVALID_HANDLE == rc) {
				if (infoBuf) {
					if (MSSQLOpResult::Success != SQLInfoDetails(m_hEnv, SQL_HANDLE_ENV, rc, errbuf, MSSQLMAXOUTBUF)) {
						return MSSQLOpResult::Fail;
					}
					*infoBuf = errbuf;
				}
				SAFE_ARR_DELETE(errbuf);
				return MSSQLOpResult::Fail;
			} else if (SQL_ERROR == rc) {
				if (infoBuf) {
					if (MSSQLOpResult::Success != SQLInfoDetails(m_hEnv, SQL_HANDLE_ENV, rc, errbuf, MSSQLMAXOUTBUF)) {
						return MSSQLOpResult::Fail;
					}
					*infoBuf = errbuf;
				}
				SAFE_ARR_DELETE(errbuf);
				return MSSQLOpResult::Fail;
			}
		}
		rc = ::SQLSetEnvAttr(m_hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
		if (SQL_SUCCESS != rc) {
			if (SQL_SUCCESS_WITH_INFO == rc) {
				if (infoBuf) {
					if (MSSQLOpResult::Success != SQLInfoDetails(m_hEnv, SQL_HANDLE_ENV, rc, errbuf, MSSQLMAXOUTBUF)) {
						return MSSQLOpResult::Fail;
					}
					*infoBuf = errbuf;
				}
			} else if (SQL_INVALID_HANDLE == rc) {
				if (infoBuf) {
					if (MSSQLOpResult::Success != SQLInfoDetails(m_hEnv, SQL_HANDLE_ENV, rc, errbuf, MSSQLMAXOUTBUF)) {
						return MSSQLOpResult::Fail;
					}
					*infoBuf = errbuf;
				}
				SAFE_ARR_DELETE(errbuf);
				return MSSQLOpResult::Fail;
			} else if (SQL_ERROR == rc) {
				if (infoBuf) {
					if (MSSQLOpResult::Success != SQLInfoDetails(m_hEnv, SQL_HANDLE_ENV, rc, errbuf, MSSQLMAXOUTBUF)) {
						return MSSQLOpResult::Fail;
					}
					*infoBuf = errbuf;
				}
				SAFE_ARR_DELETE(errbuf);
				return MSSQLOpResult::Fail;
			}
		}
	}
	rc = ::SQLAllocHandle(SQL_HANDLE_DBC, m_hEnv, &hDbc);
	if (SQL_SUCCESS != rc) {
		if (SQL_SUCCESS_WITH_INFO == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(hDbc, SQL_HANDLE_DBC, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
		} else if (SQL_INVALID_HANDLE == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(hDbc, SQL_HANDLE_DBC, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			SAFE_ARR_DELETE(errbuf);
			return MSSQLOpResult::Fail;
		} else if (SQL_ERROR == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(hDbc, SQL_HANDLE_DBC, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			SAFE_ARR_DELETE(errbuf);
			return MSSQLOpResult::Fail;
		}
	}
	std::wstring sqlDriver;
	if (MSSQLDriverType::SQLServer == driverType) {
		bool fl = true;
		for (auto &it : gc_SQLDriverType) {
			if (driverType == it.first) {
				sqlDriver = it.second;
				fl = false;
				break;
			}
		}
		if (fl) {
			SAFE_ARR_DELETE(errbuf);
			return MSSQLOpResult::Fail;
		}
	} else if (MSSQLDriverType::Custom == driverType) {
		if (MSSQLDriverPreference::SQLServer == driverPref) {
			std::vector<std::wstring> drivers;
			if (MSSQLOpResult::Success == getAvailableODBCDrivers(drivers)) {
				for (size_t i = 0; i < drivers.size(); ++i) {
					if (startsWith(drivers[i], L"SQL Server")) {
						sqlDriver = drivers[i];
						break;
					}
				}
			} else {
				SAFE_ARR_DELETE(errbuf);
				return MSSQLOpResult::Fail;
			}
		} else if (MSSQLDriverPreference::Any == driverPref) {
			sqlDriver = defaultDriver;
		}
	}
	std::wstring connLine = L"DRIVER={" + sqlDriver + L"};SERVER=" + serverAddr;
	if (port.length()) {
		connLine = connLine + L"," + port;
	}
	if (dbName.length()) {
		connLine = connLine + L";DATABASE=" + dbName;
	}
	if (MSSQLConnTrust::Trusted == trustRel) {
		connLine = connLine + L";Trusted=true";
	} else if (MSSQLConnTrust::NotTrusted == trustRel) {
		connLine = connLine + L";Trusted=false";
	} else if(MSSQLConnTrust::Undefined == trustRel) {
		if (login.length()) {
			connLine = connLine + L";UID=" + login;
		} else {
			return MSSQLOpResult::Fail;
		}
		if (pwd.length()) {
			connLine = connLine + L";PWD=" + pwd;
		}
	}
	if (!endsWith(connLine, L";")) {
		connLine = connLine + L";";
	}
	MSSQLOutBuf buf;
	buf.OutBufSz = MSSQLMAXOUTBUF;
	NEW_ARR_NULLIFY_NO_REDEFINE(buf.OutBuf, wchar_t, buf.OutBufSz);
	if (!buf.OutBuf) {
		SAFE_ARR_DELETE(errbuf);
		return MSSQLOpResult::Fail;
	}
	unsigned char connres = 0;
	rc = ::SQLDriverConnect(hDbc, 0, (SQLWCHAR*)connLine.c_str(), SQL_NTS, buf.OutBuf, buf.OutBufSz, 0,
		SQL_DRIVER_NOPROMPT);
	if (SQL_SUCCESS != rc) {
		if (SQL_SUCCESS_WITH_INFO == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(hDbc, SQL_HANDLE_DBC, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
		} else if (SQL_INVALID_HANDLE == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(hDbc, SQL_HANDLE_DBC, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			connres = 1;
		} else if (SQL_ERROR == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(hDbc, SQL_HANDLE_DBC, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			connres = 2;
		}
	}
	connID = hDbc;
	m_ConnectedDBs[hDbc] = connRec;
	SAFE_ARR_DELETE(errbuf);
	if (!connres) {
		return MSSQLOpResult::Success;
	} else {
		return MSSQLOpResult::Fail;
	}
}

MSSQLOpResult MSSQLDBHandler::ConnectDBWConnLine(SQLHANDLE& connID, const std::wstring connLine, std::wstring *infoBuf) {
	short rc = 0;
	NEW_ARR_NULLIFY(errbuf, wchar_t, MSSQLMAXOUTBUF);
	if (!errbuf) {
		return MSSQLOpResult::Fail;
	}
	if (!m_hEnv) {
		rc = ::SQLAllocHandle(SQL_HANDLE_ENV, 0, &m_hEnv);
		if (SQL_SUCCESS != rc) {
			if (SQL_SUCCESS_WITH_INFO == rc) {
				if (infoBuf) {
					if (MSSQLOpResult::Success != SQLInfoDetails(m_hEnv, SQL_HANDLE_ENV, rc, errbuf, MSSQLMAXOUTBUF)) {
						return MSSQLOpResult::Fail;
					}
					*infoBuf = errbuf;
				}
			} else if (SQL_INVALID_HANDLE == rc) {
				if (infoBuf) {
					if (MSSQLOpResult::Success != SQLInfoDetails(m_hEnv, SQL_HANDLE_ENV, rc, errbuf, MSSQLMAXOUTBUF)) {
						return MSSQLOpResult::Fail;
					}
					*infoBuf = errbuf;
				}
				SAFE_ARR_DELETE(errbuf);
				return MSSQLOpResult::Fail;
			} else if (SQL_ERROR == rc) {
				if (infoBuf) {
					if (MSSQLOpResult::Success != SQLInfoDetails(m_hEnv, SQL_HANDLE_ENV, rc, errbuf, MSSQLMAXOUTBUF)) {
						return MSSQLOpResult::Fail;
					}
					*infoBuf = errbuf;
				}
				SAFE_ARR_DELETE(errbuf);
				return MSSQLOpResult::Fail;
			}
		}
		rc = ::SQLSetEnvAttr(m_hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
		if (SQL_SUCCESS != rc) {
			if (SQL_SUCCESS_WITH_INFO == rc) {
				if (infoBuf) {
					if (MSSQLOpResult::Success != SQLInfoDetails(m_hEnv, SQL_HANDLE_ENV, rc, errbuf, MSSQLMAXOUTBUF)) {
						return MSSQLOpResult::Fail;
					}
					*infoBuf = errbuf;
				}
			} else if (SQL_INVALID_HANDLE == rc) {
				if (infoBuf) {
					if (MSSQLOpResult::Success != SQLInfoDetails(m_hEnv, SQL_HANDLE_ENV, rc, errbuf, MSSQLMAXOUTBUF)) {
						return MSSQLOpResult::Fail;
					}
					*infoBuf = errbuf;
				}
				SAFE_ARR_DELETE(errbuf);
				return MSSQLOpResult::Fail;
			} else if (SQL_ERROR == rc) {
				if (infoBuf) {
					if (MSSQLOpResult::Success != SQLInfoDetails(m_hEnv, SQL_HANDLE_ENV, rc, errbuf, MSSQLMAXOUTBUF)) {
						return MSSQLOpResult::Fail;
					}
					*infoBuf = errbuf;
				}
				SAFE_ARR_DELETE(errbuf);
				return MSSQLOpResult::Fail;
			}
		}
	}
	SQLHDBC hDbc = 0;
	rc = ::SQLAllocHandle(SQL_HANDLE_DBC, m_hEnv, &hDbc);
	if (SQL_SUCCESS != rc) {
		if (SQL_SUCCESS_WITH_INFO == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(hDbc, SQL_HANDLE_DBC, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
		} else if (SQL_INVALID_HANDLE == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(hDbc, SQL_HANDLE_DBC, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			SAFE_ARR_DELETE(errbuf);
			return MSSQLOpResult::Fail;
		} else if (SQL_ERROR == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(hDbc, SQL_HANDLE_DBC, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			SAFE_ARR_DELETE(errbuf);
			return MSSQLOpResult::Fail;
		}
	}
	size_t poss = connLine.find(L"SERVER="), pose = std::wstring::npos;
	std::wstring connRec, connLineSub;
	std::vector<std::wstring> connLnSpl;
	if (std::wstring::npos != poss) {
		poss += 7;
		pose = connLine.find(L";", poss);
		if (std::wstring::npos != pose) {
			connLineSub = connLine.substr(poss, pose - poss);
		}
		if (std::wstring::npos != connLineSub.find(L",")) {
			connLnSpl = splitStr(connLineSub, L",");
			connRec = connLnSpl[0] + L"|" + connLnSpl[1] + L"|";
		} else {
			connRec = connLineSub + L"|";
		}
	}
	poss = connLine.find(L"DATABASE=");
	if (std::wstring::npos != poss) {
		poss += 9;
		pose = connLine.find(L";", poss);
		if (std::wstring::npos != pose) {
			connLineSub = connLine.substr(poss, pose - poss);
			connRec = connRec + connLineSub;
		}
	}
	poss = connLine.find(L"UID=");
	if (std::wstring::npos != poss) {
		poss += 4;
		pose = connLine.find(L";", poss);
		if (std::wstring::npos != pose) {
			connLineSub = connLine.substr(poss, pose - poss);
			connRec = connRec + connLineSub;
		}
	} else {
		connRec = connRec + L"|Trusted";
	}
	MSSQLOutBuf buf;
	buf.OutBufSz = MSSQLMAXOUTBUF;
	NEW_ARR_NULLIFY_NO_REDEFINE(buf.OutBuf, wchar_t, buf.OutBufSz);
	if (!buf.OutBuf) {
		SAFE_ARR_DELETE(errbuf);
		return MSSQLOpResult::Fail;
	}
	unsigned char connres = 0;
	rc = ::SQLDriverConnect(hDbc, 0, (SQLWCHAR*)connLine.c_str(), SQL_NTS, buf.OutBuf, buf.OutBufSz, 0,
		SQL_DRIVER_NOPROMPT);
	if (SQL_SUCCESS != rc) {
		if (SQL_SUCCESS_WITH_INFO == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(hDbc, SQL_HANDLE_DBC, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
		} else if (SQL_INVALID_HANDLE == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(hDbc, SQL_HANDLE_DBC, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			connres = 1;
		} else if (SQL_ERROR == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(hDbc, SQL_HANDLE_DBC, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			connres = 2;
		}
	}
	connID = hDbc;
	m_ConnectedDBs[hDbc] = connRec;
	m_OutBuffers[hDbc] = buf;
	SAFE_ARR_DELETE(errbuf);
	if (!connres) {
		return MSSQLOpResult::Success;
	} else {
		return MSSQLOpResult::Fail;
	}
}

MSSQLOpResult MSSQLDBHandler::DisconnectDB(const SQLHANDLE connDBID, const std::wstring serverAddr,
	std::wstring *infoBuf) {
	NEW_ARR_NULLIFY(errbuf, wchar_t, MSSQLMAXOUTBUF);
	if (!errbuf) {
		return MSSQLOpResult::Fail;
	}
	if (!connDBID && !serverAddr.length()) {
		SAFE_ARR_DELETE(errbuf);
		return MSSQLOpResult::Fail;
	}
	SQLHANDLE dbID = 0;
	if (connDBID) {
		std::map<SQLHANDLE, std::wstring>::iterator it;
		it = m_ConnectedDBs.find(connDBID);
		if (m_ConnectedDBs.end() == it) {
			SAFE_ARR_DELETE(errbuf);
			return MSSQLOpResult::Fail;
		}
		dbID = it->first;
	} else {
		bool fl = true;
		for (auto &it : m_ConnectedDBs) {
			if (partialMatch(it.second, serverAddr)) {
				fl = false;
				dbID = it.first;
				break;
			}
		}
		if (fl) {
			SAFE_ARR_DELETE(errbuf);
			return MSSQLOpResult::Fail;
		}
	}
	short rc = ::SQLDisconnect(dbID);
	if (SQL_SUCCESS != rc) {
		if (SQL_SUCCESS_WITH_INFO == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(dbID, SQL_HANDLE_DBC, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
		} else if (SQL_INVALID_HANDLE == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(dbID, SQL_HANDLE_DBC, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			SAFE_ARR_DELETE(errbuf);
		} else if (SQL_ERROR == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(dbID, SQL_HANDLE_DBC, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			SAFE_ARR_DELETE(errbuf);
		} else if (SQL_STILL_EXECUTING == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(dbID, SQL_HANDLE_DBC, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			SAFE_ARR_DELETE(errbuf);
		}
		
	}
	if (m_OutBuffers[dbID].OutBuf) {
		SAFE_ARR_DELETE(m_OutBuffers[dbID].OutBuf);
	}
	m_OutBuffers.erase(dbID);
	m_ConnectedDBs.erase(dbID);
	rc = ::SQLFreeHandle(SQL_HANDLE_DBC, dbID);
	if (SQL_SUCCESS != rc) {
		if (SQL_SUCCESS_WITH_INFO == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(dbID, SQL_HANDLE_DBC, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
		} else if (SQL_INVALID_HANDLE == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(dbID, SQL_HANDLE_DBC, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			SAFE_ARR_DELETE(errbuf);
			return MSSQLOpResult::Fail;
		} else if (SQL_ERROR == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(dbID, SQL_HANDLE_DBC, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			SAFE_ARR_DELETE(errbuf);
			return MSSQLOpResult::Fail;
		}
	}
	SAFE_ARR_DELETE(errbuf);
	return MSSQLOpResult::Success;
}

MSSQLOpResult MSSQLDBHandler::ExecQuery(SQLHANDLE &queryID, std::vector<std::vector<std::wstring>> &results,
	const std::wstring querystr, const SQLHANDLE dbHadle,
	std::wstring *infoBuf) {
	if (!dbHadle || !querystr.length()) {
		return MSSQLOpResult::Fail;
	}
	short rc = 0;
	NEW_ARR_NULLIFY(errbuf, wchar_t, MSSQLMAXOUTBUF);
	if (!errbuf) {
		return MSSQLOpResult::Fail;
	}
	SQLHSTMT hStmt = 0;
	rc = ::SQLAllocHandle(SQL_HANDLE_STMT, dbHadle, &hStmt);
	if (SQL_SUCCESS != rc) {
		if (SQL_SUCCESS_WITH_INFO == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(hStmt, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
		} else if (SQL_INVALID_HANDLE == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(hStmt, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			SAFE_ARR_DELETE(errbuf);
			return MSSQLOpResult::Fail;
		} else if (SQL_ERROR == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(hStmt, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			SAFE_ARR_DELETE(errbuf);
			return MSSQLOpResult::Fail;
		}
	}
	long stmtBufSz = 1.1 * wcslen_c(querystr.c_str());
	NEW_ARR_NULLIFY(stmtBuf, wchar_t, stmtBufSz);
	if (!stmtBuf) {
		SAFE_ARR_DELETE(errbuf);
		return MSSQLOpResult::Fail;
	}
	wsprintf(stmtBuf, L"%s", querystr.c_str());
	rc = ::SQLPrepare(hStmt, stmtBuf, stmtBufSz);
	if (SQL_SUCCESS != rc) {
		if (SQL_SUCCESS_WITH_INFO == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(hStmt, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
		} else if (SQL_INVALID_HANDLE == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(hStmt, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			SAFE_ARR_DELETE(stmtBuf);
			SAFE_ARR_DELETE(errbuf);
			return MSSQLOpResult::Fail;
		} else if (SQL_ERROR == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(hStmt, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			SAFE_ARR_DELETE(stmtBuf);
			SAFE_ARR_DELETE(errbuf);
			return MSSQLOpResult::Fail;
		} else if (SQL_STILL_EXECUTING == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(hStmt, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			SAFE_ARR_DELETE(stmtBuf);
			SAFE_ARR_DELETE(errbuf);
			return MSSQLOpResult::Fail;
		}
	}
	rc = ::SQLExecDirect(hStmt, stmtBuf, stmtBufSz);
	if (SQL_SUCCESS != rc) {
		if (SQL_SUCCESS_WITH_INFO == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(hStmt, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
		} else if (SQL_INVALID_HANDLE == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(hStmt, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			SAFE_ARR_DELETE(stmtBuf);
			SAFE_ARR_DELETE(errbuf);
			return MSSQLOpResult::Fail;
		} else if (SQL_ERROR == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(hStmt, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			SAFE_ARR_DELETE(stmtBuf);
			SAFE_ARR_DELETE(errbuf);
			return MSSQLOpResult::Fail;
		} else if (SQL_STILL_EXECUTING == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(hStmt, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			SAFE_ARR_DELETE(stmtBuf);
			SAFE_ARR_DELETE(errbuf);
			return MSSQLOpResult::Fail;
		} else if (SQL_NEED_DATA == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(hStmt, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			SAFE_ARR_DELETE(stmtBuf);
			SAFE_ARR_DELETE(errbuf);
			return MSSQLOpResult::Fail;
		} else if (SQL_NEED_DATA == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(hStmt, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			SAFE_ARR_DELETE(stmtBuf);
			SAFE_ARR_DELETE(errbuf);
		} else if (SQL_NO_DATA == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(hStmt, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			SAFE_ARR_DELETE(stmtBuf);
			SAFE_ARR_DELETE(errbuf);
		} else if (SQL_PARAM_DATA_AVAILABLE == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(hStmt, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			SAFE_ARR_DELETE(stmtBuf);
			SAFE_ARR_DELETE(errbuf);
		}
	}
	queryID = hStmt;
	MSSQLQuery tquery;
	tquery.DBID = dbHadle;
	tquery.QueryID = hStmt;
	tquery.QueryStr = querystr;
	m_RunningQueries.push_back(tquery);
	SAFE_ARR_DELETE(stmtBuf);
	SAFE_ARR_DELETE(errbuf);
	return QueryComplete(results, hStmt, querystr, infoBuf);
}

MSSQLOpResult MSSQLDBHandler::CancelQuery(const SQLHSTMT queryID, const std::wstring querystr,
	std::wstring *infoBuf) {
	NEW_ARR_NULLIFY(errbuf, wchar_t, MSSQLMAXOUTBUF);
	if (!errbuf) {
		return MSSQLOpResult::Fail;
	}
	SQLHSTMT qryID = 0;
	if (!queryID && !querystr.length()) {
		return MSSQLOpResult::Fail;
	} else {
		if (queryID && !querystr.length()) {
			for (size_t i = 0; i < m_RunningQueries.size(); ++i) {
				if (m_RunningQueries[i].QueryID == queryID) {
					qryID = m_RunningQueries[i].QueryID;
					break;
				}
			}
		} else if (!queryID && querystr.length()) {
			for (size_t i = 0; i < m_RunningQueries.size(); ++i) {
				if (lower_copy(trim_copy(m_RunningQueries[i].QueryStr)) == lower_copy(trim_copy(querystr))) {
					qryID = m_RunningQueries[i].QueryID;
					break;
				}
			}
		}
	}
	if (!qryID) {
		return MSSQLOpResult::Fail;
	}
	short rc = ::SQLCancel(qryID);
	if (SQL_SUCCESS != rc) {
		if (SQL_SUCCESS_WITH_INFO == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(qryID, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
		} else if (SQL_INVALID_HANDLE == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(qryID, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			return MSSQLOpResult::Fail;
		} else if (SQL_ERROR == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(qryID, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			return MSSQLOpResult::Fail;
		}

	}
	return QueryCancelled();
}

MSSQLOpResult MSSQLDBHandler::QueryComplete(std::vector<std::vector<std::wstring>> &results,
	SQLHSTMT queryHandle, const std::wstring queryStr, std::wstring *infoBuf) {
	if (!queryHandle && !queryStr.length()) {
		return MSSQLOpResult::Fail;
	}
	MSSQLQuery* qptr = 0;
	NEW_ARR_NULLIFY(errbuf, wchar_t, MSSQLMAXOUTBUF);
	if (!errbuf) {
		return MSSQLOpResult::Fail;
	}
	if (queryHandle) {
		for (size_t i = 0; i < m_RunningQueries.size(); ++i) {
			if (m_RunningQueries[i].QueryID == queryHandle) {
				qptr = &(m_RunningQueries[i]);
				break;
			}
		}
	} else {
		std::wstring qlow = lower_copy(queryStr);
		for (size_t i = 0; i < m_RunningQueries.size(); ++i) {
			if (lower_copy(m_RunningQueries[i].QueryStr) == qlow) {
				qptr = &(m_RunningQueries[i]);
				break;
			}
		}
	}
	if (!qptr) {
		SAFE_ARR_DELETE(errbuf);
		return MSSQLOpResult::Fail;
	}
	results.front().clear();
	results.clear();
	size_t i = 0;
	short colNum = 0;
	SQLLEN rowNum = 0;
	short rc = ::SQLNumResultCols(qptr->QueryID, &colNum);
	if (SQL_SUCCESS_WITH_INFO == rc) {
		if (infoBuf) {
			if (MSSQLOpResult::Success != SQLInfoDetails(qptr->QueryID, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
				return MSSQLOpResult::Fail;
			}
			*infoBuf = errbuf;
		}
	} else if (SQL_INVALID_HANDLE == rc) {
		if (infoBuf) {
			if (MSSQLOpResult::Success != SQLInfoDetails(qptr->QueryID, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
				return MSSQLOpResult::Fail;
			}
			*infoBuf = errbuf;
		}
		SAFE_ARR_DELETE(errbuf);
		return MSSQLOpResult::Fail;
	} else if (SQL_ERROR == rc) {
		if (infoBuf) {
			if (MSSQLOpResult::Success != SQLInfoDetails(qptr->QueryID, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
				return MSSQLOpResult::Fail;
			}
			*infoBuf = errbuf;
		}
		SAFE_ARR_DELETE(errbuf);
		return MSSQLOpResult::Fail;
	} else if (SQL_STILL_EXECUTING == rc) {
		if (infoBuf) {
			if (MSSQLOpResult::Success != SQLInfoDetails(qptr->QueryID, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
				return MSSQLOpResult::Fail;
			}
			*infoBuf = errbuf;
		}
		SAFE_ARR_DELETE(errbuf);
		return MSSQLOpResult::Fail;
	}
	rc = ::SQLRowCount(qptr->QueryID, &rowNum);
	if (SQL_SUCCESS_WITH_INFO == rc) {
		if (infoBuf) {
			if (MSSQLOpResult::Success != SQLInfoDetails(qptr->QueryID, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
				return MSSQLOpResult::Fail;
			}
			*infoBuf = errbuf;
		}
	} else if (SQL_INVALID_HANDLE == rc) {
		if (infoBuf) {
			if (MSSQLOpResult::Success != SQLInfoDetails(qptr->QueryID, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
				return MSSQLOpResult::Fail;
			}
			*infoBuf = errbuf;
		}
		SAFE_ARR_DELETE(errbuf);
		return MSSQLOpResult::Fail;
	} else if (SQL_ERROR == rc) {
		if (infoBuf) {
			if (MSSQLOpResult::Success != SQLInfoDetails(qptr->QueryID, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
				return MSSQLOpResult::Fail;
			}
			*infoBuf = errbuf;
		}
		SAFE_ARR_DELETE(errbuf);
		return MSSQLOpResult::Fail;
	}
	short display = 0;
	MSSQLBinding *binding;
	if (colNum) {
		if (MSSQLOpResult::Success != SQLAllocateBindings(qptr->QueryID, colNum, &binding, &display, infoBuf)) {

		}
	} else {
		for (i = 0; i < rowNum; ++i) {

		}
	}
	rc = ::SQLFreeStmt(qptr->QueryID, SQL_CLOSE);
	if (SQL_SUCCESS_WITH_INFO == rc) {
		if (infoBuf) {
			if (MSSQLOpResult::Success != SQLInfoDetails(qptr->QueryID, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
				return MSSQLOpResult::Fail;
			}
			*infoBuf = errbuf;
		}
	} else if (SQL_INVALID_HANDLE == rc) {
		if (infoBuf) {
			if (MSSQLOpResult::Success != SQLInfoDetails(qptr->QueryID, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
				return MSSQLOpResult::Fail;
			}
			*infoBuf = errbuf;
		}
		SAFE_ARR_DELETE(errbuf);
		return MSSQLOpResult::Fail;
	} else if (SQL_ERROR == rc) {
		if (infoBuf) {
			if (MSSQLOpResult::Success != SQLInfoDetails(qptr->QueryID, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
				return MSSQLOpResult::Fail;
			}
			*infoBuf = errbuf;
		}
		SAFE_ARR_DELETE(errbuf);
		return MSSQLOpResult::Fail;
	}
	m_RunningQueries.erase(std::remove(m_RunningQueries.begin(), m_RunningQueries.end(), *qptr),
		m_RunningQueries.end());
	return MSSQLOpResult::Success;
}

MSSQLOpResult MSSQLDBHandler::QueryCancelled(std::wstring *infoBuf) {
	return MSSQLOpResult::Success;
}

MSSQLOpResult MSSQLDBHandler::SQLAllocateBindings(const SQLHSTMT queryHandle,
	const short colNum, MSSQLBinding **binding, short *display, std::wstring *infoBuf) const {
	NEW_ARR_NULLIFY(errbuf, wchar_t, MSSQLMAXOUTBUF);
	if (!errbuf) {
		return MSSQLOpResult::Fail;
	}
	short iCol = 0, cchColumnNameLength = 0, rc = 0;
	long long cchDisplay = 0, ssType = 0;
	MSSQLBinding *pThisBinding = 0, *pLastBinding = 0;
	*display = 0;
	for (iCol = 1; iCol <= colNum; ++iCol) {
		pThisBinding = (MSSQLBinding*)(malloc(sizeof(MSSQLBinding)));
		if (!(pThisBinding)) {
			return MSSQLOpResult::Fail;
		}
		if (iCol == 1) {
			*binding = pThisBinding;
		} else {
			pLastBinding->sNext = pThisBinding;
		}
		pLastBinding = pThisBinding;
		rc = ::SQLColAttribute(queryHandle, iCol, SQL_DESC_DISPLAY_SIZE, 0, 0, 0, &cchDisplay);
		if (SQL_SUCCESS_WITH_INFO == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(queryHandle, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
		} else if (SQL_INVALID_HANDLE == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(queryHandle, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			SAFE_ARR_DELETE(errbuf);
			return MSSQLOpResult::Fail;
		} else if (SQL_ERROR == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(queryHandle, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			SAFE_ARR_DELETE(errbuf);
			return MSSQLOpResult::Fail;
		} else if (SQL_STILL_EXECUTING == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(queryHandle, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			SAFE_ARR_DELETE(errbuf);
			return MSSQLOpResult::Fail;
		}
		rc = ::SQLColAttribute(queryHandle, iCol, SQL_DESC_CONCISE_TYPE, 0, 0, 0, &ssType);
		if (SQL_SUCCESS_WITH_INFO == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(queryHandle, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
		} else if (SQL_INVALID_HANDLE == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(queryHandle, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			SAFE_ARR_DELETE(errbuf);
			return MSSQLOpResult::Fail;
		} else if (SQL_ERROR == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(queryHandle, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			SAFE_ARR_DELETE(errbuf);
			return MSSQLOpResult::Fail;
		} else if (SQL_STILL_EXECUTING == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(queryHandle, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			SAFE_ARR_DELETE(errbuf);
			return MSSQLOpResult::Fail;
		}
		pThisBinding->fChar = (ssType == SQL_CHAR || ssType == SQL_VARCHAR || ssType == SQL_LONGVARCHAR);
		pThisBinding->sNext = 0;
		if (cchDisplay > MSSQLDISPLAYMAX) {
			cchDisplay = MSSQLDISPLAYMAX;
		}
		NEW_ARR_NULLIFY_NO_REDEFINE(pThisBinding->wszBuffer, wchar_t, (cchDisplay + 1));
		if (!(pThisBinding->wszBuffer)) {
			SAFE_ARR_DELETE(errbuf);
			return MSSQLOpResult::Fail;
		}
		rc = ::SQLBindCol(queryHandle, iCol, SQL_C_TCHAR, (SQLPOINTER)pThisBinding->wszBuffer,
			(cchDisplay + 1) * sizeof(WCHAR), &pThisBinding->indPtr);
		if (SQL_SUCCESS_WITH_INFO == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(queryHandle, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
		} else if (SQL_INVALID_HANDLE == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(queryHandle, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			SAFE_ARR_DELETE(errbuf);
			return MSSQLOpResult::Fail;
		} else if (SQL_ERROR == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(queryHandle, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			SAFE_ARR_DELETE(errbuf);
			return MSSQLOpResult::Fail;
		}
		rc = ::SQLColAttribute(queryHandle, iCol, SQL_DESC_NAME, 0, 0, &cchColumnNameLength, 0);
		if (SQL_SUCCESS_WITH_INFO == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(queryHandle, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
		} else if (SQL_INVALID_HANDLE == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(queryHandle, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			SAFE_ARR_DELETE(errbuf);
			return MSSQLOpResult::Fail;
		} else if (SQL_ERROR == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(queryHandle, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			SAFE_ARR_DELETE(errbuf);
			return MSSQLOpResult::Fail;
		} else if (SQL_STILL_EXECUTING == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(queryHandle, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			SAFE_ARR_DELETE(errbuf);
			return MSSQLOpResult::Fail;
		}
		pThisBinding->cDisplaySize = MAX((short)cchDisplay, cchColumnNameLength);
		if (pThisBinding->cDisplaySize < MSSQLNULLSIZE)
			pThisBinding->cDisplaySize = MSSQLNULLSIZE;
		*display += pThisBinding->cDisplaySize + MSSQLDISPLAYFORMATEXTRA;
	}
	SAFE_ARR_DELETE(errbuf);
	return MSSQLOpResult::Success;
}

MSSQLOpResult MSSQLDBHandler::SQLGetTitles(std::vector<std::vector<std::wstring>> &results, const SQLHSTMT queryHandle,
	unsigned long displaySize, MSSQLBinding *binding, std::wstring *infoBuf) {
	wchar_t wszTitle[MSSQLDISPLAYMAX] = { 0 };
	short iCol = 1;
	NEW_ARR_NULLIFY(errbuf, wchar_t, MSSQLMAXOUTBUF);
	if (!errbuf) {
		return MSSQLOpResult::Fail;
	}
	for (; binding; binding = binding->sNext) {
		short rc = SQLColAttribute(queryHandle, ++iCol, SQL_DESC_NAME, wszTitle, sizeof(wszTitle), 0, 0);
		if (SQL_SUCCESS_WITH_INFO == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(queryHandle, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
		} else if (SQL_INVALID_HANDLE == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(queryHandle, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			SAFE_ARR_DELETE(errbuf);
			return MSSQLOpResult::Fail;
		} else if (SQL_ERROR == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(queryHandle, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			SAFE_ARR_DELETE(errbuf);
			return MSSQLOpResult::Fail;
		} else if (SQL_STILL_EXECUTING == rc) {
			if (infoBuf) {
				if (MSSQLOpResult::Success != SQLInfoDetails(queryHandle, SQL_HANDLE_STMT, rc, errbuf, MSSQLMAXOUTBUF)) {
					return MSSQLOpResult::Fail;
				}
				*infoBuf = errbuf;
			}
			SAFE_ARR_DELETE(errbuf);
			return MSSQLOpResult::Fail;
		}
	}
	SAFE_ARR_DELETE(errbuf);
	return MSSQLOpResult::Success;
}

MSSQLOpResult MSSQLDBHandler::SQLInfoDetails(const SQLHANDLE handle, const short recordType, const short code,
	wchar_t* &infoBuf, const size_t infoBufSz) const {
	short iRec = 0; long  iError = 0;
	wchar_t wszMessage[1000] = { 0 }, wszState[SQL_SQLSTATE_SIZE + 1] = { 0 };
	if (!infoBuf) {
		NEW_ARR_NULLIFY_NO_REDEFINE(infoBuf, wchar_t, MSSQLMAXOUTBUF);
		if (!infoBuf) {
			return MSSQLOpResult::Fail;
		}
	} else {
		memset(infoBuf, 0, infoBufSz);
	}
	if (SQL_INVALID_HANDLE == code) {
		wsprintf(infoBuf, L"%s", L"Invalid handle!\n");
		return MSSQLOpResult::Success;
	}
	while (SQL_SUCCESS == ::SQLGetDiagRec(recordType,
		handle,
		++iRec,
		wszState,
		&iError,
		wszMessage,
		(short)(sizeof(wszMessage) / sizeof(wchar_t)), 0)) {
		// Hide data truncated..
		if (wcsncmp(wszState, L"01004", 5)) {
			wsprintf(infoBuf, L"[%5.5s] %s (%d)\n", wszState, wszMessage, iError);
		}
	}
	return MSSQLOpResult::Success;
}

PGSQLDBHandler::PGSQLDBHandler() {}

PGSQLDBHandler::PGSQLDBHandler(const std::map<unsigned long, std::wstring> connectedDBS,
	const std::map<unsigned long, std::wstring> runningQueries) {
	m_ConnectedDBs = connectedDBS;
	m_RunningQueries = runningQueries;
}

PGSQLDBHandler::PGSQLDBHandler(const PGSQLDBHandler &other) {
	if (this != &other) {
		m_ConnectedDBs = other.m_ConnectedDBs;
		m_RunningQueries = other.m_RunningQueries;
	}
}

PGSQLDBHandler::PGSQLDBHandler(PGSQLDBHandler &&other) noexcept {
	if (this != &other) {
		m_ConnectedDBs = other.m_ConnectedDBs;
		other.m_ConnectedDBs.~map();
		m_RunningQueries = other.m_RunningQueries;
		other.m_RunningQueries.~map();
	}
}

PGSQLDBHandler::~PGSQLDBHandler() {}

PGSQLDBHandler& PGSQLDBHandler::operator=(const PGSQLDBHandler &other) {
	if (this != &other) {
		m_ConnectedDBs = other.m_ConnectedDBs;
		m_RunningQueries = other.m_RunningQueries;
	}
	return *this;
}

PGSQLDBHandler& PGSQLDBHandler::operator=(PGSQLDBHandler &&other) noexcept {
	if (this != &other) {
		m_ConnectedDBs = other.m_ConnectedDBs;
		other.m_ConnectedDBs.~map();
		m_RunningQueries = other.m_RunningQueries;
		other.m_RunningQueries.~map();
	}
	return *this;
}

bool PGSQLDBHandler::operator==(const PGSQLDBHandler &other) const {
	if (this != &other) {
		return (m_ConnectedDBs == other.m_ConnectedDBs ||
				m_RunningQueries == other.m_RunningQueries);
	} else {
		return true;
	}
}

bool PGSQLDBHandler::operator!=(const PGSQLDBHandler &other) const {
	if (this != &other) {
		return (m_ConnectedDBs != other.m_ConnectedDBs ||
				m_RunningQueries != other.m_RunningQueries);
	} else {
		return false;
	}
}

PGSQLOpResult PGSQLDBHandler::ConnectDB(const std::wstring serverAddr, const std::wstring login,
	const std::wstring pwd) {
	return PGSQLOpResult::Success;
}

PGSQLOpResult PGSQLDBHandler::DisconnectDB(const std::wstring serverAddr) {
	return PGSQLOpResult::Success;
}

PGSQLOpResult PGSQLDBHandler::ExecQuery(unsigned long &queyID, const std::wstring query) {
	return QueryComplete();
}

PGSQLOpResult PGSQLDBHandler::CancelQuery(const unsigned long queyID) {
	return QueryCancelled();
}

PGSQLOpResult PGSQLDBHandler::QueryComplete() {
	return PGSQLOpResult::Success;
}

PGSQLOpResult PGSQLDBHandler::QueryCancelled() {
	return PGSQLOpResult::Success;
}