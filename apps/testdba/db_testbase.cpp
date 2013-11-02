// File: db_testbase.cpp
// Purpose: Base class for plugin regression tests
// Author: Lukasz Michalski <lm at zork.pl>, Copyright 2007
// License: See COPYING file that comes with this distribution
//
// This file is a part of debea library (http://debea.net)
//
//
#include "db_testbase.h"

namespace dba_tests {

Db_testBase::Db_testBase()
  : CppUnit::TestCase(),
    mDb(NULL),
    mConnParams(NULL)
{
}

void 
Db_testBase::setParams(dba::Database* pDb, const char* pConnParams) {
  mDb = pDb;
  mConnParams = pConnParams;
};

Db_testBase::~Db_testBase()
{
}




}
