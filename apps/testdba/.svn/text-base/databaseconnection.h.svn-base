// File: databaseconnection.h
// Purpose: Regression tests database plugin setup
// Author: Lukasz Michalski <lm at zork.pl>, Copyright 2007
// License: See COPYING file that comes with this distribution
//
// This file is a part of debea library (http://debea.net)
//
//
#ifndef DBA_TESTSDATABASECONNECTION_H
#define DBA_TESTSDATABASECONNECTION_H

#include "dbplugintestsuite.h"

namespace dba_tests {

/**
Test suite for checking plugin initializations and connections to database backend*/
class DatabaseConnection  : public DatabaseTestSuite {
    CPPUNIT_TEST_SUITE(DatabaseConnection);
      CPPUNIT_TEST(dbConnection);
    CPPUNIT_TEST_SUITE_END();
  public:
    DatabaseConnection() {};
    DatabaseConnection(const char* pPluginName, const char* pDbParams);
    virtual ~DatabaseConnection();
  
    //tests  
    void initPlugin() {}; //everything is done in setUp of DatabaseTestSuite
    void dbConnection();

};

}

#endif
