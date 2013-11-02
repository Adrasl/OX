// File: dbplugin_bugs.h
// Purpose: Regression test class for bug testing
// Author: Lukasz Michalski <lm at zork.pl>, Copyright 2007
// License: See COPYING file that comes with this distribution
//
// This file is a part of debea library (http://debea.net)
//
//
#ifndef DBA_TESTSDBPLUGIN_BUGS_H
#define DBA_TESTSDBPLUGIN_BUGS_H

#include "sharedsqlarchive_tests.h"

namespace dba_tests {

/**
Bug regression tests*/
class DbPlugin_Bugs  : public SharedSQLArchive_Tests {
    CPPUNIT_TEST_SUB_SUITE(DbPlugin_Bugs, SharedSQLArchive_Tests);  
    CPPUNIT_TEST_SUITE_END();  
  public:
    DbPlugin_Bugs() {};
    DbPlugin_Bugs(const char* pPluginName, const char* pDbParams);
    ~DbPlugin_Bugs();
};

}

#endif
