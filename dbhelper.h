#ifndef _DBHELPER_H
#define _DBHELPER_H

#include "config.h"
#include "strhelper.h"

enum class MSSQLOpResult {
	Success,
	Fail
};

enum class PGSQLOpResult {
	Success,
	Fail
};

class MSSQLDBHelper {
	public:
		MSSQLDBHelper();
		MSSQLDBHelper(const MSSQLDBHelper &other);
		MSSQLDBHelper(MSSQLDBHelper &&other) noexcept;
		~MSSQLDBHelper();
		MSSQLDBHelper& operator=(const MSSQLDBHelper &other);
		MSSQLDBHelper& operator=(MSSQLDBHelper &&other) noexcept;
		bool operator==(const MSSQLDBHelper &other) const;
		bool operator!=(const MSSQLDBHelper &other) const;
		MSSQLOpResult ConnectDB();
		MSSQLOpResult DisconnectDB();
		MSSQLOpResult ExecQuery(unsigned long &queyID, const std::wstring query);
		MSSQLOpResult CancelQuery(const unsigned long queyID);
	protected:
	private:
		std::map<unsigned long, std::wstring> m_RunningQueries;
};

class PGSQLDBHelper {
	public:
		PGSQLDBHelper();
		PGSQLDBHelper(const PGSQLDBHelper &other);
		PGSQLDBHelper(PGSQLDBHelper &&other) noexcept;
		~PGSQLDBHelper();
		PGSQLDBHelper& operator=(const PGSQLDBHelper &other);
		PGSQLDBHelper& operator=(PGSQLDBHelper && other) noexcept;
		bool operator==(const PGSQLDBHelper &other) const;
		bool operator!=(const PGSQLDBHelper &other) const;
		PGSQLOpResult ConnectDB();
		PGSQLOpResult DisconnectDB();
		PGSQLOpResult ExecQuery(unsigned long &queyID, const std::wstring query);
		PGSQLOpResult CancelQuery(const unsigned long queyID);
	protected:
	private:
		std::map<unsigned long, std::wstring> m_RunningQueries;
};

#endif