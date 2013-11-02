// File: filtertestcase.cpp
// Purpose: Regression test application
// Author: Lukasz Michalski <lm at zork.pl>, Copyright 2007
// License: See COPYING file that comes with this distribution
//
// This file is a part of debea library (http://debea.net)

#ifdef _USE_VLD
#include <vld.h>
#endif

#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestResult.h>

#include "dbplugintestsuite.h"
#include "csvtestcase.h"
#include "xmltestcase.h"
#include "connectstringtestcase.h"

//tests
#include "db_basic.h"
#include "archive_lists.h"
#include "dbplugin_bugs.h"
#include "databaseconnection.h"
#include "csvtestcase.h"
#include "stdfilters_test.h"
#include "sqlparamparsertestcase.h"

#define TEST_POSTGRES

#if !defined(DEBEA_USINGDLL)
  #define TEST_PL_SUFFIX "-static"
#else
  #define TEST_PL_SUFFIX 
#endif

std::string plugin;

#ifdef TEST_SQLITE3
const char* sqlite3_connect_string = "dbname=libdbasqlite3test.sqt3";
//SQL database functions
class SQLite3DbTestCase : public dba_tests::Db_Basic {
    CPPUNIT_TEST_SUB_SUITE(SQLite3DbTestCase,dba_tests::Db_Basic);
    CPPUNIT_TEST_SUITE_END();
  public:
    SQLite3DbTestCase() : dba_tests::Db_Basic("dbasqlite3"TEST_PL_SUFFIX,sqlite3_connect_string) {}
};

CPPUNIT_TEST_SUITE_REGISTRATION(SQLite3DbTestCase);

//Basic Archive functions
class SQLite3ArchiveTestCase : public dba_tests::Archive_SQLPlugin {
    CPPUNIT_TEST_SUB_SUITE(SQLite3ArchiveTestCase,dba_tests::Archive_SQLPlugin);
    CPPUNIT_TEST_SUITE_END();
  public:
    SQLite3ArchiveTestCase() : dba_tests::Archive_SQLPlugin("dbasqlite3"TEST_PL_SUFFIX,sqlite3_connect_string) {}
};

CPPUNIT_TEST_SUITE_REGISTRATION(SQLite3ArchiveTestCase);

//SQLArchive functions
class SQLite3SQLArchiveTestCase : public dba_tests::DbPlugin_Bugs {
    CPPUNIT_TEST_SUB_SUITE(SQLite3SQLArchiveTestCase,dba_tests::SharedSQLArchive_Tests);
    CPPUNIT_TEST_SUITE_END();
  public:
    SQLite3SQLArchiveTestCase() : dba_tests::DbPlugin_Bugs("dbasqlite3"TEST_PL_SUFFIX,sqlite3_connect_string) {}
};

CPPUNIT_TEST_SUITE_REGISTRATION(SQLite3SQLArchiveTestCase);
#endif

#ifdef TEST_POSTGRES
const char* pg_connect_string = "dbname=libdbapgsqltest";
class PostgresDbTestCase : public dba_tests::Db_Basic {
    CPPUNIT_TEST_SUB_SUITE(PostgresDbTestCase,dba_tests::Db_Basic);
    CPPUNIT_TEST_SUITE_END();
  public:
    PostgresDbTestCase() : dba_tests::Db_Basic("dbapgsql"TEST_PL_SUFFIX,pg_connect_string) {}
};

CPPUNIT_TEST_SUITE_REGISTRATION(PostgresDbTestCase);

//Basic Archive functions
class PostgresArchiveTestCase : public dba_tests::Archive_SQLPlugin {
    CPPUNIT_TEST_SUB_SUITE(PostgresArchiveTestCase,dba_tests::Archive_SQLPlugin);
    CPPUNIT_TEST_SUITE_END();
  public:
    PostgresArchiveTestCase() : dba_tests::Archive_SQLPlugin("dbapgsql"TEST_PL_SUFFIX,pg_connect_string) {}
};

CPPUNIT_TEST_SUITE_REGISTRATION(PostgresArchiveTestCase);

//SQLArchive functions
class PostgresSQLArchiveTestCase : public dba_tests::DbPlugin_Bugs {
    CPPUNIT_TEST_SUB_SUITE(PostgresSQLArchiveTestCase,dba_tests::DbPlugin_Bugs);
    CPPUNIT_TEST_SUITE_END();
  public:
    PostgresSQLArchiveTestCase() : dba_tests::DbPlugin_Bugs("dbapgsql"TEST_PL_SUFFIX,pg_connect_string) {}
};

CPPUNIT_TEST_SUITE_REGISTRATION(PostgresSQLArchiveTestCase);
#endif

#ifdef TEST_ODBC
#ifdef _WIN32
const char* odbcDSN = "odbcparams='DSN=mssql_debea_test' unicode=0";
#else
const char* odbcDSN = "DSN=test";
#endif

class OdbcPluginTestCase : public dba_tests::DatabaseConnection {
    CPPUNIT_TEST_SUB_SUITE(OdbcPluginTestCase,dba_tests::DatabaseConnection);
    CPPUNIT_TEST_SUITE_END();
  public:
    OdbcPluginTestCase() : dba_tests::DatabaseConnection("dbaodbc"TEST_PL_SUFFIX,odbcDSN) {}
};

CPPUNIT_TEST_SUITE_REGISTRATION(OdbcPluginTestCase);

class OdbcDbTestCase : public dba_tests::Db_Basic {
    CPPUNIT_TEST_SUB_SUITE(OdbcDbTestCase,dba_tests::Db_Basic);
    CPPUNIT_TEST_SUITE_END();
  public:
    OdbcDbTestCase() : dba_tests::Db_Basic("dbaodbc"TEST_PL_SUFFIX,odbcDSN) {}
};

CPPUNIT_TEST_SUITE_REGISTRATION(OdbcDbTestCase);

class ODBCSQLArchiveTestCase : public dba_tests::DbPlugin_Bugs {
    CPPUNIT_TEST_SUB_SUITE(ODBCSQLArchiveTestCase,dba_tests::DbPlugin_Bugs);
    CPPUNIT_TEST_SUITE_END();
  public:
    ODBCSQLArchiveTestCase() : dba_tests::DbPlugin_Bugs("dbaodbc"TEST_PL_SUFFIX,odbcDSN) {}
};

CPPUNIT_TEST_SUITE_REGISTRATION(ODBCSQLArchiveTestCase);
#endif

int
main() {
  CppUnit::TextTestRunner runner;
  CppUnit::BriefTestProgressListener listener;
  runner.eventManager().addListener(&listener);
  runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );
  //runner.addTest(new CppUnit::TestCaller<SQLite3SQLArchiveTestCase>("debug_test",&SQLite3SQLArchiveTestCase::sqlError));
  //runner.addTest(new CppUnit::TestCaller<PostgresSQLArchiveTestCase>("debug_test",&PostgresSQLArchiveTestCase::sqlError));
  //runner.addTest(new CppUnit::TestCaller<dba_tests::StringFilter>("debug_test",&dba_tests::StringFilter::test17));
  //runner.addTest(new CppUnit::TestCaller<dba_tests::CSVTestCase>("debug_test",&dba_tests::CSVTestCase::invalidPos));
  //runner.addTest(new CppUnit::TestCaller<SQLite3SQLArchiveTestCase>("debug_test",&SQLite3SQLArchiveTestCase::colbug_sublist_erase));
  //runner.addTest(dba_tests::SQLParamParserTestCase::suite());
  //runner.addTest(new CppUnit::TestCaller<dba_tests::XMLTestCase>("debug_test",&dba_tests::XMLTestCase::storeBug4));
  //runner.addTest(new CppUnit::TestCaller<dba_tests::XMLTestCase>("debug_test",&dba_tests::XMLTestCase::store_two));
  //runner.addTest(new CppUnit::TestCaller<OdbcPluginTestCase>("debug_test",&OdbcPluginTestCase::dbConnection));
  //runner.addTest(new CppUnit::TestCaller<PostgresSQLArchiveTestCase>("debug_test",&PostgresSQLArchiveTestCase::sql_escape_param));
  //runner.addTest(new CppUnit::TestCaller<OdbcDbTestCase>("debug_test",&OdbcDbTestCase::conv_date_to_date));
  //runner.addTest(new CppUnit::TestCaller<PostgresDbTestCase>("debug_test",&PostgresDbTestCase::conv_str_to_double_pl));
  //runner.addTest(new CppUnit::TestCaller<ODBCSQLArchiveTestCase>("debug_test",&ODBCSQLArchiveTestCase::agregationTest));
  //runner.addTest(new CppUnit::TestCaller<dba_tests::ConnectStringTestCase>("debug_test",&dba_tests::ConnectStringTestCase::noParams));
  //runner.addTest(new CppUnit::TestCaller<SQLite3SQLArchiveTestCase>("debug_test",&SQLite3SQLArchiveTestCase::multifetcher_forced_root_table));
  //runner.addTest(new CppUnit::TestCaller<dba_tests::SQLParamParserTestCase>("debug_test",&dba_tests::SQLParamParserTestCase::likeOneParam));
  runner.run();
  return runner.result().wasSuccessful() ? 0 : 1;
};

 
