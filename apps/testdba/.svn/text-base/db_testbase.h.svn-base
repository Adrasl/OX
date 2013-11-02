// File: db_testbase.h
// Purpose: Base class for plugin regression tests
// Author: Lukasz Michalski <lm at zork.pl>, Copyright 2007
// License: See COPYING file that comes with this distribution
//
// This file is a part of debea library (http://debea.net)
//
//
#ifndef DBA_TESTSDB_TESTBASE_H
#define DBA_TESTSDB_TESTBASE_H

#include <cppunit/TestCase.h>
#include "dba/database.h"

namespace dba_tests {

/**
base class for Database interface plugin tests */
class Db_testBase  : public CppUnit::TestCase {
  public:
    Db_testBase();
    void setParams(dba::Database* pDb, const char* pConnParams);
    ~Db_testBase();
  protected:
    dba::Database* mDb;  
    const char* mConnParams;
};

}

#endif
