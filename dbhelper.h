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
		MSSQLOpResult ConnectDB(const std::wstring serverAddr, const std::wstring login, const std::wstring pwd);
		MSSQLOpResult DisconnectDB(const std::wstring serverAddr);
		MSSQLOpResult ExecQuery(unsigned long &queyID, const std::wstring query);
		MSSQLOpResult CancelQuery(const unsigned long queyID);
	protected:

	private:
		/*       FUNCTIONS       */
		MSSQLOpResult QueryCompleteCallback();
		MSSQLOpResult QueryCamcelledCallback();
		/*       VARIABLES       */
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
		PGSQLOpResult ConnectDB(const std::wstring serverAddr, const std::wstring login, const std::wstring pwd);
		PGSQLOpResult DisconnectDB(const std::wstring serverAddr);
		PGSQLOpResult ExecQuery(unsigned long &queyID, const std::wstring query);
		PGSQLOpResult CancelQuery(const unsigned long queyID);
	protected:

	private:
		/*       FUNCTIONS       */
		PGSQLOpResult QueryCompleteCallback();
		PGSQLOpResult QueryCamcelledCallback();
		/*       VARIABLES       */
		std::map<unsigned long, std::wstring> m_ConnectedDBs;
		std::map<unsigned long, std::wstring> m_RunningQueries;
};

#endif