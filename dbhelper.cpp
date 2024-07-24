#include "dbhelper.h"

MSSQLDBHandler::MSSQLDBHandler() {}

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

MSSQLOpResult MSSQLDBHandler::ConnectDB() {
	return MSSQLOpResult::Success;
}

MSSQLOpResult MSSQLDBHandler::DisconnectDB() {
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

PGSQLOpResult PGSQLDBHandler::ConnectDB() {
	return PGSQLOpResult::Success;
}

PGSQLOpResult PGSQLDBHandler::DisconnectDB() {
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