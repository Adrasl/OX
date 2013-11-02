// File: dbplugintestsuite.cpp
// Purpose: Base class for plugin regression tests
// Author: Lukasz Michalski <lm at zork.pl>, Copyright 2007
// License: See COPYING file that comes with this distribution
//
// This file is a part of debea library (http://debea.net)
//
//
#include "dbplugintestsuite.h"
#include "utils.h"

namespace dba_tests {

bool 
onError(void*, int pType, const char* pMsg) {
  std::string err;
  switch(pType) {
    case dba::DBA_SQL_ERROR:
      err += "SQL Error: ";
      err += pMsg;
      throw TestSQLException(err);
    break;
    case dba::DBA_DB_ERROR:
      err += "Database Error: ";
      err += pMsg;
      throw TestDbException(err);
    break;
    default:
      err = "Unknown error in dba module";
    break;
  };
  throw dba::DatabaseException(err);
};

void 
PluginTestBase::setUp() {
  mPlugin.load(mPluginName);  
};

void 
PluginTestBase::tearDown() {
  mPlugin.close();
};

void 
DatabaseTestSuite::setUp() {
  PluginTestBase::setUp();
  mDb = mPlugin.createHandle();
};

void 
DatabaseTestSuite::tearDown() {
  mPlugin.destroyHandle(mDb);
  PluginTestBase::tearDown();
};
 
PluginTestBase::PluginTestBase(const char* pPluginName, const char* pDbParams)
  : CppUnit::TestCase(),
    mPluginName(pPluginName),
    mDbParams(pDbParams)
{
}

void 
SQLDatabaseTestSuite::setUp() {
  DatabaseTestSuite::setUp();
  mDb->setErrorHandler(this,&onError);
  std::auto_ptr<dba::DbConnection> conn(mDb->getConnection(mDbParams));
  Utils::prepareDatabaseSchema(*conn, mDbParams);
};

void 
SQLArchiveTestSuite::setUp() {
  mSQLArchive = Utils::initSharedSQLArchive(mPluginName,mDbParams);
  dba::SQLOStream stream = mSQLArchive->getOStream();
  Utils::prepareSharedSQLArchiveSchema(stream,mDbParams);
  mArchive = mSQLArchive;
};

void
SQLArchiveTestSuite::tearDown() {
  delete mSQLArchive;
  mSQLArchive = NULL;
  mArchive = NULL;
};


};
