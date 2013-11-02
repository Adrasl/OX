// File: csvtestcase.h
// Purpose: Regression tests for CSV driver
// Author: Lukasz Michalski <lm at zork.pl>, Copyright 2007
// License: See COPYING file that comes with this distribution
//
// This file is a part of debea library (http://debea.net)

#ifndef TESTCSVTESTCASE_H
#define TESTCSVTESTCASE_H

#ifdef TEST_CSV

#include "dba/csv.h"

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

namespace dba_tests {

class CSVTester : public dba::Storeable {
    DECLARE_STORE_TABLE();
  public:
    CSVTester(int pA = 0, int pB = 0, const std::string& pStr = "") 
      : Storeable(InvalidId) ,
        a(pA),
        b(pB),
        str(pStr)
    {
    };
    bool operator==(const CSVTester& pTester) const {
      if (a == pTester.a)
        if (b == pTester.b)
         if (str == pTester.str)
          return true;
      return false;
    };
    int a;
    int b;
    std::string str;
};

class CSVTestCase : public CppUnit::TestCase {
  CPPUNIT_TEST_SUITE(CSVTestCase);
      CPPUNIT_TEST(manual);
      CPPUNIT_TEST(autoCol);
      CPPUNIT_TEST(autoBindedVarsEmptyObj);
      CPPUNIT_TEST(manualBindedVars);
      CPPUNIT_TEST(autoMultiBindedVars);
      CPPUNIT_TEST(openNonCSVFileForWriteTest);
      CPPUNIT_TEST(openNonCSVFileForWriteTest2);
      CPPUNIT_TEST(sepTest);
      CPPUNIT_TEST(autoBindedVars);
      CPPUNIT_TEST(invalidPos);
  CPPUNIT_TEST_SUITE_END();

  public:
    void setUp();
    void tearDown();

    void manual();
    void sepTest();
    void autoCol();
    void autoBindedVarsEmptyObj();
    void manualBindedVars();
    void autoBindedVars();
    void autoMultiBindedVars();
    void openNonCSVFileForWriteTest();
    void openNonCSVFileForWriteTest2();
    void spaceSeparator();
    void invalidPos();
  private:
    class CSVTester : public dba::Storeable {
        DECLARE_STORE_TABLE();
      public:
        CSVTester(int pA = 0, int pB = 0, const std::string& pStr = "") 
          : dba::Storeable(InvalidId),
            a(pA),
            b(pB),
            str(pStr)
        {
        };
        bool operator==(const CSVTester& pTester) const {
          if (a == pTester.a)
            if (b == pTester.b)
            if (str == pTester.str)
              return true;
          return false;
        };
        int a;
        int b;
        std::string str;
    };
    std::list<CSVTester> mResults;
};

}; //namespace

#endif //TEST_CSV


#endif
