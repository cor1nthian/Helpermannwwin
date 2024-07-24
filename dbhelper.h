#ifndef _DBHELPER_H
#define _DBHELPER_H

#include "config.h"
#include "strhelper.h"

enum class MSSQLOpResult : unsigned char {
	Success,
	Fail
};

enum class PGSQLOpResult : unsigned char {
	Success,
	Fail
};

class MSSQLDBHandler {
	public:
		MSSQLDBHandler();
		MSSQLDBHandler(const MSSQLDBHandler &other);
		MSSQLDBHandler(MSSQLDBHandler &&other) noexcept;
		~MSSQLDBHandler();
		MSSQLDBHandler& operator=(const MSSQLDBHandler &other);
		MSSQLDBHandler& operator=(MSSQLDBHandler &&other) noexcept;
		bool operator==(const MSSQLDBHandler &other) const;
		bool operator!=(const MSSQLDBHandler &other) const;
		MSSQLOpResult ConnectDB();
		MSSQLOpResult DisconnectDB();
		MSSQLOpResult ExecQuery(unsigned long &queyID, const std::wstring query);
		MSSQLOpResult CancelQuery(const unsigned long queyID);
	protected:

	private:
		MSSQLOpResult QueryCompleteCallback();
		MSSQLOpResult QueryCamcelledCallback();
		std::map<unsigned long, std::wstring> m_ConnectedDBs;
		std::map<unsigned long, std::wstring> m_RunningQueries;
};

class PGSQLDBHandler {
	public:
		PGSQLDBHandler();
		PGSQLDBHandler(const PGSQLDBHandler &other);
		PGSQLDBHandler(PGSQLDBHandler &&other) noexcept;
		~PGSQLDBHandler();
		PGSQLDBHandler& operator=(const PGSQLDBHandler &other);
		PGSQLDBHandler& operator=(PGSQLDBHandler && other) noexcept;
		bool operator==(const PGSQLDBHandler &other) const;
		bool operator!=(const PGSQLDBHandler &other) const;
		PGSQLOpResult ConnectDB();
		PGSQLOpResult DisconnectDB();
		PGSQLOpResult ExecQuery(unsigned long &queyID, const std::wstring query);
		PGSQLOpResult CancelQuery(const unsigned long queyID);
	protected:

	private:
		PGSQLOpResult QueryCompleteCallback();
		PGSQLOpResult QueryCamcelledCallback();
		std::map<unsigned long, std::wstring> m_ConnectedDBs;
		std::map<unsigned long, std::wstring> m_RunningQueries;
};

#endif