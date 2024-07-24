#ifndef _DBHELPER_H
#define _DBHELPER_H

/*       In loving memory of @pixo $oft       */
#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS
#define NOVIRTUALKEYCODES
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOKEYSTATES
#define NOSYSCOMMANDS
#define NORASTEROPS
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOKERNEL
#define NOMEMMGR
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWINOFFSETS
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NODRAWTEXT
#define NOWINSTYLES
#define NOSHOWWINDOW
#define NOWINMESSAGES
#define NOWH

#include <Windows.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <sql.h>
#include <sal.h>
#include "config.h"
#include "strhelper.h"

enum class MSSQLOpResult : unsigned char {
	Success,
	Fail
};

enum class MSSQLDriverType : unsigned char {
	SQLServer
};

enum class MSSQLConnTrust : unsigned char {
	Undefined,
	Trusted,
	NotTrusted
};

enum class PGSQLOpResult : unsigned char {
	Success,
	Fail
};

const std::map<MSSQLDriverType, std::wstring> gc_SQLDriverType = {
	{ MSSQLDriverType::SQLServer, L"SQL Server" }
};

class MSSQLDBHandler {
	public:
		MSSQLDBHandler();
		MSSQLDBHandler(const std::map<unsigned long, std::wstring> connectedDBS,
			const std::map<unsigned long, std::wstring> runningQueries);
		MSSQLDBHandler(const MSSQLDBHandler &other);
		MSSQLDBHandler(MSSQLDBHandler &&other) noexcept;
		~MSSQLDBHandler();
		MSSQLDBHandler& operator=(const MSSQLDBHandler &other);
		MSSQLDBHandler& operator=(MSSQLDBHandler &&other) noexcept;
		bool operator==(const MSSQLDBHandler &other) const;
		bool operator!=(const MSSQLDBHandler &other) const;
		MSSQLOpResult ConnectDB(const std::wstring serverAddr, const std::wstring dbName,
			const std::wstring port = L"50100", MSSQLDriverType driverType = MSSQLDriverType::SQLServer,
			const std::wstring login = L"", const std::wstring pwd = L"",
			const MSSQLConnTrust trustRel = MSSQLConnTrust::Undefined, const size_t connOutBufLen = 1924);
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
		PGSQLDBHandler(const std::map<unsigned long, std::wstring> connectedDBS,
			const std::map<unsigned long, std::wstring> runningQueries);
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