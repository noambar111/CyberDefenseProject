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
