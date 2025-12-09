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
	const char* sql = "INSERT INTO geo_records (ip, country) VALUES (?, ?);";

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
	const char* sql = "SELECT ip FROM geo_records WHERE country = ? ORDER BY id DESC;";
	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr);
	std::vector<std::string> results;

	if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK)
	{
		Logger::getInstance().log(LogLevel::ERROR_LOG, "Failed to prepare statement in getIPsByCountry");
		return results;
	}

	sqlite3_bind_text(stmt, 1, country.c_str(), -1, SQLITE_TRANSIENT);

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		results.push_back((const char*)sqlite3_column_text(stmt, 0));
	}

	sqlite3_finalize(stmt);
	return results;

}

std::vector<std::pair<std::string, int>> SQLiteDatabase::getTopCountries(int limit)
{
	const char* sql = "SELECT country, COUNT(*) as cnt "
		"FROM geo_records "
		"GROUP BY country "
		"ORDER BY cnt DESC "
		"LIMIT ?;";
		
	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr);
	sqlite3_bind_int(stmt, 1, limit);

	std::vector<std::pair<std::string, int>> results;

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		std::string country = (const char*)sqlite3_column_text(stmt, 0);
		int count = sqlite3_column_int(stmt, 1);
		results.emplace_back(country, count);
	}

	sqlite3_finalize(stmt);
	return results;
}

void SQLiteDatabase::initSchema()
{
	const char* sql =
		"CREATE TABLE IF NOT EXISTS geo_records ("
		"id INTEGER  PRIMARY KEY AUTOINCREMENT, "
		"ip TEXT NOT NULL, "
		"country TEXT NOT NULL, "
		"timestamp DATETIME DEFAULT CURRENT_TIMESTAMP"
		");";

	char* errMsg = nullptr;
	int rc = sqlite3_exec(m_db, sql, nullptr, nullptr, &errMsg);

	if (rc != SQLITE_OK)
	{
		Logger::getInstance().log(LogLevel::ERROR_LOG, "Schema init failes: " + std::string(errMsg));
		sqlite3_free(errMsg);
	}
}
