#include "dbhelper.h"

MSSQLDBHandler::MSSQLDBHandler() {}

MSSQLDBHandler::MSSQLDBHandler(const std::map<unsigned long, std::wstring> connectedDBS,
	const std::map<unsigned long, std::wstring> runningQueries) {
	m_ConnectedDBs = connectedDBS;
	m_RunningQueries = runningQueries;
}

MSSQLDBHandler::MSSQLDBHandler(const MSSQLDBHandler &other) {
	if (this != &other) {
		m_ConnectedDBs = other.m_ConnectedDBs;
		m_RunningQueries = other.m_RunningQueries;
	}
}

MSSQLDBHandler::MSSQLDBHandler(MSSQLDBHandler &&other) noexcept {
	if (this != &other) {
		m_ConnectedDBs = other.m_ConnectedDBs;
		other.m_ConnectedDBs.~map();
		m_RunningQueries = other.m_RunningQueries;
		other.m_RunningQueries.~map();
	}
}

MSSQLDBHandler::~MSSQLDBHandler() {}

MSSQLDBHandler& MSSQLDBHandler::operator=(const MSSQLDBHandler &other) {
	if (this != &other) {
		m_ConnectedDBs = other.m_ConnectedDBs;
		m_RunningQueries = other.m_RunningQueries;
	}
	return *this;
}

MSSQLDBHandler& MSSQLDBHandler::operator=(MSSQLDBHandler &&other) noexcept {
	if (this != &other) {
		m_ConnectedDBs = other.m_ConnectedDBs;
		other.m_ConnectedDBs.~map();
		m_RunningQueries = other.m_RunningQueries;
		other.m_RunningQueries.~map();
	}
	return *this;
}

bool MSSQLDBHandler::operator==(const MSSQLDBHandler &other) const {
	if (this != &other) {
		return (m_ConnectedDBs == other.m_ConnectedDBs &&
				m_RunningQueries == other.m_RunningQueries);
	} else {
		return true;
	}
}

bool MSSQLDBHandler::operator!=(const MSSQLDBHandler &other) const {
	if (this != &other) {
		return (m_ConnectedDBs != other.m_ConnectedDBs &&
				m_RunningQueries != other.m_RunningQueries);
	} else {
		return false;
	}
}

MSSQLOpResult MSSQLDBHandler::ConnectDB(const std::wstring serverAddr, const std::wstring dbName,
	const std::wstring port, MSSQLDriverType driverType, const std::wstring login, const std::wstring pwd,
	const MSSQLConnTrust trustRel, const size_t connOutBufLen) {
	if ((!login.length() && !pwd.length() && MSSQLConnTrust::NotTrusted == trustRel) || (!connOutBufLen)) {
		return MSSQLOpResult::Fail;
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
	std::wstring sqlDriver;
	for (auto &it : gc_SQLDriverType) {
		if (driverType == it.first) {
			sqlDriver = it.second;
			break;
		}
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
		connLine = connLine + L";Trusted=true";
	} else if (MSSQLConnTrust::NotTrusted == trustRel) {
		connLine = connLine + L";Trusted=false";
	}
	NEW_ARR_NULLIFY(connOut, wchar_t, connOutBufLen);
	if (!connOut) {
		return MSSQLOpResult::Fail;
	}
	rc = SQLDriverConnect(hDbc, 0, (SQLWCHAR*)connLine.c_str(), SQL_NTS, connOut, connOutBufLen, 0,
		SQL_DRIVER_NOPROMPT);
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

MSSQLOpResult MSSQLDBHandler::DisconnectDB(const std::wstring serverAddr) {
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