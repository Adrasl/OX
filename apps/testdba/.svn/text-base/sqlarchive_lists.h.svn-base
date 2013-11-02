// File: sqlarchive_lists.h
// Purpose: Regression tests for references
// Author: Lukasz Michalski <lm at zork.pl>, Copyright 2007
// License: See COPYING file that comes with this distribution
//
// This file is a part of debea library (http://debea.net)
//
//
#ifndef DBA_TESTSSQLARCHIVE_LISTS_H
#define DBA_TESTSSQLARCHIVE_LISTS_H

#include "sqlarchive_basic.h"
#include "testobject.h"

namespace dba_tests {

/**
tests on lists of storeable objects*/
class SQLArchive_Lists  : public dba_tests::SQLArchive_Basic {
    CPPUNIT_TEST_SUB_SUITE(SQLArchive_Lists, SQLArchive_Basic);  
      //CPPUNIT_TEST(bigListTest);  
      CPPUNIT_TEST(nulls);  
      CPPUNIT_TEST(oneNonNullRecord);  
      CPPUNIT_TEST(simpleData);  
      CPPUNIT_TEST(weirdChars);  
    CPPUNIT_TEST_SUITE_END();  
  public:
    SQLArchive_Lists() {};
    SQLArchive_Lists(const char* pPluginName, const char* pDbParams);
    
    void bigListTest();
    void oneNonNullRecord();
    void simpleData();
    void weirdChars();
    void nulls();
  private:
    void compare(const std::list<TestData>& pOrg, const std::list<TestData>& pLoaded, bool pBindedOnly);
    void compareList(const std::list<TestData>& pData);
};

}

#endif
