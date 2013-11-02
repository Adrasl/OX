// File: dbplugintestsuite.h
// Purpose: Base class for plugin regression tests
// Author: Lukasz Michalski <lm at zork.pl>, Copyright 2007
// License: See COPYING file that comes with this distribution
//
// This file is a part of debea library (http://debea.net)
//
//
#ifndef DBA_TESTSDBPLUGINTESTSUITE_H
#define DBA_TESTSDBPLUGINTESTSUITE_H

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include "dba/sharedsqlarchive.h"
#include "dba/dbplugin.h"

namespace dba_tests {

//generic error handler for test cases
bool onError(void*, int pType, const char* pMsg);


class PluginTestBase : public CppUnit::TestCase {
  public:
    PluginTestBase() {};
    PluginTestBase(const char* pPluginName, const char* pDbParams);
    virtual void setUp();
    virtual void tearDown();
  protected:
    const char* mPluginName;
    const char* mDbParams;
    dba::DbPlugin mPlugin;
};

class DatabaseTestSuite : public PluginTestBase {
  public:
    DatabaseTestSuite() : mDb(NULL) {};
    DatabaseTestSuite(const char* pPluginName, const char* pDbParams)
      : PluginTestBase(pPluginName,pDbParams),
        mDb(NULL)
      {}; 
    virtual void setUp();
    virtual void tearDown();
  protected:
    dba::Database* mDb;
};

class TestSQLException : public dba::Exception {
  public:
    TestSQLException(const std::string& pStr) 
      : dba::Exception(pStr)
    {
    };
};

class TestDbException : public dba::Exception {
  public:
    TestDbException(const std::string& pStr) 
      : dba::Exception(pStr)
    {
    };
};


class SQLDatabaseTestSuite : public DatabaseTestSuite {
  public:
    SQLDatabaseTestSuite() {};
    SQLDatabaseTestSuite(const char* pPluginName, const char* pDbParams)
      : DatabaseTestSuite(pPluginName,pDbParams) {}; 
    virtual void setUp();
  protected:
  private:
};



class ArchiveTestSuite : public CppUnit::TestCase {
  public:
    ArchiveTestSuite() : mArchive(NULL) {}
    ArchiveTestSuite(const char* pPluginName, const char* pDbParams)
      : mPluginName(pPluginName),
        mDbParams(pDbParams), 
        mArchive(NULL)
    {}; 
  protected:
    const char* mPluginName;
    const char* mDbParams;
    dba::Archive* mArchive;
  private:
    
};

/**
Test suite for dba plugins
*/
class SQLArchiveTestSuite  : public ArchiveTestSuite {
  public:
    SQLArchiveTestSuite() : mSQLArchive(NULL) {};
    SQLArchiveTestSuite(const char* pPluginName, const char* pDbParams)
      : ArchiveTestSuite(pPluginName,pDbParams), 
        mSQLArchive(NULL)
    {};       
    virtual void setUp();
    virtual void tearDown();
  protected:
    dba::SharedSQLArchive* mSQLArchive;
  private:    
};


}

#endif
