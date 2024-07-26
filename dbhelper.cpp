#include "dbhelper.h"

MSSQLOutBuf::MSSQLOutBuf() {
	OutBuf = 0;
	OutBufSz = 0;
}

MSSQLOutBuf::MSSQLOutBuf(size_t outbufsz) {
	OutBufSz = outbufsz;
	NEW_ARR_NULLIFY_NO_REDEFINE(OutBuf, wchar_t, OutBufSz);
}

MSSQLOutBuf::MSSQLOutBuf(wchar_t* outbuf, size_t outbufsz) {
	OutBuf = outbuf;
	OutBufSz = outbufsz;
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

MSSQLOutBuf::~MSSQLOutBuf() {
	if (OutBuf) {
		SAFE_ARR_DELETE(OutBuf);
	}
}

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

MSSQLDBHandler::MSSQLDBHandler() {}

MSSQLDBHandler::MSSQLDBHandler(const std::map<SQLHANDLE, std::wstring> connectedDBS,
	const std::map<SQLHANDLE, MSSQLOutBuf> outBuffers, const std::map<SQLHANDLE, std::wstring> runningQueries) {
	m_ConnectedDBs = connectedDBS;
	m_OutBuffers = outBuffers;
	m_RunningQueries = runningQueries;
}

MSSQLDBHandler::MSSQLDBHandler(const MSSQLDBHandler &other) {
	if (this != &other) {
		m_ConnectedDBs = other.m_ConnectedDBs;
		m_OutBuffers = other.m_OutBuffers;
		m_RunningQueries = other.m_RunningQueries;
	}
}

MSSQLDBHandler::MSSQLDBHandler(MSSQLDBHandler &&other) noexcept {
	if (this != &other) {
		m_ConnectedDBs = other.m_ConnectedDBs;
		other.m_ConnectedDBs.~map();
		m_OutBuffers = other.m_OutBuffers;
		other.m_OutBuffers.~map();
		m_RunningQueries = other.m_RunningQueries;
		other.m_RunningQueries.~map();
	}
}

MSSQLDBHandler::~MSSQLDBHandler() {}

MSSQLDBHandler& MSSQLDBHandler::operator=(const MSSQLDBHandler &other) {
	if (this != &other) {
		m_ConnectedDBs = other.m_ConnectedDBs;
		m_OutBuffers = other.m_OutBuffers;
		m_RunningQueries = other.m_RunningQueries;
	}
	return *this;
}

MSSQLDBHandler& MSSQLDBHandler::operator=(MSSQLDBHandler &&other) noexcept {
	if (this != &other) {
		m_ConnectedDBs = other.m_ConnectedDBs;
		other.m_ConnectedDBs.~map();
		m_OutBuffers = other.m_OutBuffers;
		other.m_OutBuffers.~map();
		m_RunningQueries = other.m_RunningQueries;
		other.m_RunningQueries.~map();
	}
	return *this;
}

bool MSSQLDBHandler::operator==(const MSSQLDBHandler &other) const {
	if (this != &other) {
		return (m_ConnectedDBs == other.m_ConnectedDBs &&
				m_OutBuffers == other.m_OutBuffers &&
				m_RunningQueries == other.m_RunningQueries);
	} else {
		return true;
	}
}

bool MSSQLDBHandler::operator!=(const MSSQLDBHandler &other) const {
	if (this != &other) {
		return (m_ConnectedDBs != other.m_ConnectedDBs ||
				m_OutBuffers != other.m_OutBuffers ||
				m_RunningQueries != other.m_RunningQueries);
	} else {
		return false;
	}
}

MSSQLOpResult MSSQLDBHandler::ConnectDB(SQLHANDLE &connID, const std::wstring serverAddr, const std::wstring dbName,
	const std::wstring port, const std::wstring login, const std::wstring pwd, const MSSQLConnTrust trustRel,
	const MSSQLDriverType driverType, const unsigned long connOutBufLen) {
	if ((!login.length() && !pwd.length() && (MSSQLConnTrust::Trusted != trustRel)) || (!connOutBufLen) ||
		(!serverAddr.length())) {
		return MSSQLOpResult::Fail;
	}
	std::wstring connRec;
	if (login.length()) {
		connRec = dbName + L"|" + port + L"|" + login;
	} else {
		connRec = dbName + L"|" + port + L"|Trusted";
	}
	for (auto &it : m_ConnectedDBs) {
		if (lower_copy(connRec) == lower_copy(it.second)) {
			return MSSQLOpResult::Fail;
		}
	}
	SQLHENV hEnv = 0;
	SQLHDBC hDbc = 0;
	SQLHSTMT hStmt = 0;
	if (SQL_ERROR == SQLAllocHandle(SQL_HANDLE_ENV, 0, &hEnv)) {
		return MSSQLOpResult::Fail;
	}
	
	short rc = SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
	if (SQL_SUCCESS != rc) {
		if (SQL_SUCCESS_WITH_INFO == rc) {
		} else if (SQL_INVALID_HANDLE == rc) {
			return MSSQLOpResult::Fail;
		} else if (SQL_ERROR == rc) {
			return MSSQLOpResult::Fail;
		}
		// HandleDiagnosticRecord(h, ht, rc);
	}
	rc = SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
	if (SQL_SUCCESS != rc) {
		if (SQL_SUCCESS_WITH_INFO == rc) {
		} else if (SQL_INVALID_HANDLE == rc) {
			return MSSQLOpResult::Fail;
		} else if (SQL_ERROR == rc) {
			return MSSQLOpResult::Fail;
		}
	}
	bool fl = true;
	std::wstring sqlDriver;
	for (auto &it : gc_SQLDriverType) {
		if (driverType == it.first) {
			sqlDriver = it.second;
			fl = false;
			break;
		}
	}
	if (fl) {
		return MSSQLOpResult::Fail;
	}
	std::wstring connLine = L"DRIVER={" + sqlDriver + L"};SERVER=" + serverAddr + L"," + port + L";DATABASE=" + dbName;
	if (login.length()) {
		connLine = connLine + L";UID=" + login;
	}
	if (pwd.length()) {
		connLine = connLine + L";PWD=" + pwd;
	}
	if (!endsWith(connLine, L";")) {
		connLine = connLine + L";";
	}
	if (MSSQLConnTrust::Trusted == trustRel) {
		connLine = connLine + L"Trusted=true;";
	} else if (MSSQLConnTrust::NotTrusted == trustRel) {
		connLine = connLine + L"Trusted=false;";
	}
	MSSQLOutBuf buf;
	buf.OutBufSz = connOutBufLen;
	NEW_ARR_NULLIFY_NO_REDEFINE(buf.OutBuf, wchar_t, buf.OutBufSz);
	if (!buf.OutBuf) {
		return MSSQLOpResult::Fail;
	}
	rc = SQLDriverConnect(hDbc, ::GetDesktopWindow(), (SQLWCHAR*)connLine.c_str(), SQL_NTS, 0, 0, 0,
		SQL_DRIVER_NOPROMPT);
	if (SQL_SUCCESS != rc) {
		if (SQL_SUCCESS_WITH_INFO == rc) {
		} else if (SQL_INVALID_HANDLE == rc) {
			return MSSQLOpResult::Fail;
		} else if (SQL_ERROR == rc) {
			return MSSQLOpResult::Fail;
		}
	}
	m_ConnectedDBs[hDbc] = connRec;
	return MSSQLOpResult::Success;
}

MSSQLOpResult MSSQLDBHandler::DisconnectDB(SQLHANDLE connDBID, const std::wstring serverAddr) {
	if (!connDBID && !serverAddr.length()) {
		return MSSQLOpResult::Fail;
	}
	SQLHANDLE dbID = 0;
	if (connDBID) {
		std::map<SQLHANDLE, std::wstring>::iterator it;
		// short rc = ::SQLDisconnect();
		it = m_ConnectedDBs.find(connDBID);
		if (m_ConnectedDBs.end() == it) {
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
			return MSSQLOpResult::Fail;
		}
	}
	short rc = ::SQLDisconnect(dbID);
	if (SQL_SUCCESS != rc) {
		if (SQL_SUCCESS_WITH_INFO == rc) {
		} else if (SQL_INVALID_HANDLE == rc) {
			return MSSQLOpResult::Fail;
		} else if (SQL_ERROR == rc) {
			return MSSQLOpResult::Fail;
		}
	}
	m_OutBuffers.erase(dbID);
	m_ConnectedDBs.erase(dbID);
	return MSSQLOpResult::Success;
}

MSSQLOpResult MSSQLDBHandler::ExecQuery(unsigned long &queyID, const std::wstring query) {
	return MSSQLOpResult::Success;
}

MSSQLOpResult MSSQLDBHandler::CancelQuery(const unsigned long queyID) {
	return MSSQLOpResult::Success;
}

MSSQLOpResult MSSQLDBHandler::QueryCompleteCallback() {
	return MSSQLOpResult::Success;
}

MSSQLOpResult MSSQLDBHandler::QueryCamcelledCallback() {
	return MSSQLOpResult::Success;
}

unsigned long MSSQLDBHandler::assignConnDBID() {
	short connDBSz = m_ConnectedDBs.size();
	if (MSSQLMAXCONN == connDBSz) {
		return MSSQLCONNTOOMANY;
	} else {
		return connDBSz;
	}
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
	return PGSQLOpResult::Success;
}

PGSQLOpResult PGSQLDBHandler::CancelQuery(const unsigned long queyID) {
	return PGSQLOpResult::Success;
}

PGSQLOpResult PGSQLDBHandler::QueryCompleteCallback() {
	return PGSQLOpResult::Success;
}

PGSQLOpResult PGSQLDBHandler::QueryCamcelledCallback() {
	return PGSQLOpResult::Success;
}