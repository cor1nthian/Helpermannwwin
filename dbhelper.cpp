#include "dbhelper.h"

MSSQLDBHelper::MSSQLDBHelper() {}

MSSQLDBHelper::MSSQLDBHelper(const MSSQLDBHelper &other) {}

MSSQLDBHelper::MSSQLDBHelper(MSSQLDBHelper &&other) noexcept {}

MSSQLDBHelper::~MSSQLDBHelper() {}

MSSQLDBHelper& MSSQLDBHelper::operator=(const MSSQLDBHelper &other) {}

MSSQLDBHelper& MSSQLDBHelper::operator=(MSSQLDBHelper &&other) noexcept {}

bool MSSQLDBHelper::operator==(const MSSQLDBHelper &other) const {}

bool MSSQLDBHelper::operator!=(const MSSQLDBHelper &other) const {}

MSSQLOpResult MSSQLDBHelper::ConnectDB() {}

MSSQLOpResult MSSQLDBHelper::DisconnectDB() {}

MSSQLOpResult MSSQLDBHelper::ExecQuery(unsigned long &queyID, const std::wstring query) {}

MSSQLOpResult MSSQLDBHelper::CancelQuery(const unsigned long queyID) {}

PGSQLDBHelper::PGSQLDBHelper() {}

PGSQLDBHelper::PGSQLDBHelper(const PGSQLDBHelper &other) {}

PGSQLDBHelper::PGSQLDBHelper(PGSQLDBHelper &&other) noexcept {}

PGSQLDBHelper::~PGSQLDBHelper() {}

PGSQLDBHelper& PGSQLDBHelper::operator=(const PGSQLDBHelper &other) {}

PGSQLDBHelper& PGSQLDBHelper::operator=(PGSQLDBHelper &&other) noexcept {}

bool PGSQLDBHelper::operator==(const PGSQLDBHelper &other) const {}

bool PGSQLDBHelper::operator!=(const PGSQLDBHelper &other) const {}

PGSQLOpResult PGSQLDBHelper::ConnectDB() {}

PGSQLOpResult PGSQLDBHelper::DisconnectDB() {}

PGSQLOpResult PGSQLDBHelper::ExecQuery(unsigned long &queyID, const std::wstring query) {}

PGSQLOpResult PGSQLDBHelper::CancelQuery(const unsigned long queyID) {}