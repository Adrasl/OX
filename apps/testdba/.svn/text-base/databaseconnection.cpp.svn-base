// File: databaseconnection.cpp
// Purpose: Regression tests database plugin setup
// Author: Lukasz Michalski <lm at zork.pl>, Copyright 2007
// License: See COPYING file that comes with this distribution
//
// This file is a part of debea library (http://debea.net)
//
//
#include "databaseconnection.h"

namespace dba_tests {

DatabaseConnection::DatabaseConnection(const char* pPluginName, const char* pDbParams)
  : DatabaseTestSuite(pPluginName, pDbParams)
{
}

DatabaseConnection::~DatabaseConnection()
{
}

void 
DatabaseConnection::dbConnection() {
  mDb->setErrorHandler(this,&onError);
  std::auto_ptr<dba::DbConnection> conn(mDb->getConnection(mDbParams));
};


}
