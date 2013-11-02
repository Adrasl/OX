// File: utils.h
// Purpose: Utilities for testing
// Author: Lukasz Michalski <lm at zork.pl>, Copyright 2007
// License: See COPYING file that comes with this distribution
//
// This file is a part of debea library (http://debea.net)
//
//
#ifndef DBA_TESTSUTILS_H
#define DBA_TESTSUTILS_H

#include "dba/sharedsqlarchive.h"
#include "dba/dbplugin.h"

namespace dba_tests {

/**
Various utilities*/
class Utils {
  public:
    //!types of tested databases
    typedef enum {
      DB_UNKNOWN = 0, //!unknown databasse
      DB_MSSQL //!Microsoft SQL Server
    } dbtype;

    static tm getNow(int pDays = 0);    
    static tm getDate(int pYear, int pMonth, int pDay, int pHour, int pMin, int pSec);    
    static dba::SharedSQLArchive* initSharedSQLArchive(dba::DbPlugin& pPlugin, const char* pDbParams);
    static dba::SharedSQLArchive* initSharedSQLArchive(const char* pPlugin, const char* pDbParams);
    static void cleanupSharedSQLArchive(dba::DbPlugin& pPlugin, dba::SQLArchive* pArchive);
    static void prepareDatabaseSchema(dba::DbConnection& pConn, const char* pConnParams);
    static void prepareSharedSQLArchiveSchema(dba::SQLOStream& pStream, const char* pConnParams);
    static void dropTable(dba::DbConnection& pConn, const char* pTableName);
    static void dropTable(dba::SQLOStream& pStream, const char* pTableName);

  private:
    /**
      Detect type of tested database
      @return database type or DB_UNKNOWN if type is not detected
    */
    static dbtype getBackendType(const char* pConnParams);
    /**
      Modify column types in %SQL CREATE TABLE query for databases 
      that does not allow default types. MSSQL disallows to set timestamp,
      we have to use non-standard datetime there

      For tests, database type is encoded in ODBC DSN or dbname argument. Database
      name contains substrings specified below:
      PostgresSQL: pgsql
      Sqlite3: sqlite3
      MSSQL: mssql

      @param pTableDefinition default query
      @param pConnParams connection parameters
      @return original or modified query depending of detected database type
    */
    static std::string alterTypes(const char* pTableDefinition, const char* pConnParams);

};

}

#endif
