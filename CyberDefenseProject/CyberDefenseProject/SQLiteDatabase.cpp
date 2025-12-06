#include "SQLiteDatabase.h"

SQLiteDatabase::SQLiteDatabase(const std::string& fileName)
{
	if (sqlite3_open(fileName.c_str(), &m_db) != SQLITE_OK)
	{
		Logger::getInstance().log(LogLevel::ERROR_LOG, "Failed to open sqlite - DB");
		m_db = nullptr;
	}
	else
	{
		Logger::getInstance().log(LogLevel::INFO_LOG, "SQLite DB opened successfully");
		initSchema();
	}
}

SQLiteDatabase::~SQLiteDatabase()
{
	if (m_db) sqlite3_close(m_db);
}

bool SQLiteDatabase::saveGeo(const GeoRecord& rec)
{
	const char* sql = "INSTERT INTO geo_records (ip, country) VALUES (?, ?);";

	sqlite3_stmt* stmt = nullptr;
	sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr);

	sqlite3_bind_text(stmt, 1, rec.ip.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 2, rec.country.c_str(), -1, SQLITE_TRANSIENT);

	bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
	sqlite3_finalize(stmt);

	return ok;
}

std::optional<std::string> SQLiteDatabase::getCountryByIP(const std::string& ip)
{
	const char* sql = "SELECT country FROM geo_records WHERE ip = ? ORDER BY id DESC LIMIT 1;";

	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr);
	sqlite3_bind_text(stmt, 1, ip.c_str(), -1, SQLITE_TRANSIENT);

	if (sqlite3_step(stmt) == SQLITE_ROW)
	{
		std::string result = (const char*)sqlite3_column_text(stmt, 0);
		sqlite3_finalize(stmt);
		return result;
	}


	sqlite3_finalize(stmt);
	return std::nullopt;
}

std::vector<std::string> SQLiteDatabase::getIPsByCountry(const std::string& country)
{
	return std::vector<std::string>();
}

std::vector<std::pair<std::string, int>> SQLiteDatabase::getTopCountries(int limit)
{
	return std::vector<std::pair<std::string, int>>();
}

void SQLiteDatabase::initSchema()
{
	const char* sql =
		"CREATE TABLE IF NOT EXISTS geo_records ("
		"id INTEGER  PRIMARY KEY AUTOINCREMENT, "
		"ip TEXT NOT NULL, "
		"timestamp DATETIME FEDUALT CURRENT_TIMESTAMP"
		");";

	char* errMsg = nullptr;
	int rc = sqlite3_exec(m_db, sql, nullptr, nullptr, &errMsg);

	if (rc != SQLITE_OK)
	{
		Logger::getInstance().log(LogLevel::ERROR_LOG, "Schema init failes: " + std::string(errMsg));
		sqlite3_free(errMsg);
	}
}
