// File: stdfilters_test.h
// Purpose: Regression tests for filters
// Author: Lukasz Michalski <lm at zork.pl>, Copyright 2007
// License: See COPYING file that comes with this distribution
//
// This file is a part of debea library (http://debea.net)
//
//
#ifndef DBA_TESTSSTDFILTERS_TEST_H
#define DBA_TESTSSTDFILTERS_TEST_H

#include "filterstestcase.h"
#include "dba/stdfilters.h"

namespace dba_tests {

class DoubleFilter : public FiltersTestCase<double, dba::Double> {
    CPPUNIT_TEST_SUITE(DoubleFilter);
      CPPUNIT_TEST(test1);
      CPPUNIT_TEST(test2);
      CPPUNIT_TEST(test3);
      CPPUNIT_TEST(test4);
      CPPUNIT_TEST(test5);
      CPPUNIT_TEST(test6);
      CPPUNIT_TEST(test7);
      CPPUNIT_TEST(test8);
      CPPUNIT_TEST(test9);
      CPPUNIT_TEST(test10);
      CPPUNIT_TEST(test11);
      CPPUNIT_TEST(test12);
      CPPUNIT_TEST(test13);
      CPPUNIT_TEST(test14);
      CPPUNIT_TEST(test15);
      CPPUNIT_TEST(test16);
      CPPUNIT_TEST(test17);
    CPPUNIT_TEST_SUITE_END();
  public:
    void test1() { notNull(0.0); };
    void test2() { toString(1,"1"); };
    void test3() { toString(1.1,"1.1"); };
    void test4() { toString(1.0,"1"); };
    void test5() { toString(-1.0,"-1"); };
    void test6() { toString(-12.0,"-12"); };
    void test7() { toString(12.12345,"12.12345"); };
    void test8() { toString(-12.12345,"-12.12345"); };
    void test9() { fromString("1",1); };
    void test10() { fromString("1.1",1.1); };
    void test11() { fromString("-1.1",-1.1); };
    void test12() { fromString("123.12345",123.12345); };
    void test13() { fromString("-123.12345",-123.12345); };
    void test14() { fromDouble(1.1,1.1); };
    void test15() { fromInt(1,1); };
    void test16() { exceptionFrom("a"); };
    void test17() { exceptionFrom(""); };
};

class StringFilter : public FiltersTestCase<std::string,dba::String> {
    CPPUNIT_TEST_SUITE(StringFilter);
      CPPUNIT_TEST(test1);
      CPPUNIT_TEST(test2);
      CPPUNIT_TEST(test3);
      CPPUNIT_TEST(test4);
      CPPUNIT_TEST(test5);
      CPPUNIT_TEST(test6);
      CPPUNIT_TEST(test7);
      CPPUNIT_TEST(test8);
      CPPUNIT_TEST(test11);
      CPPUNIT_TEST(test12);
      CPPUNIT_TEST(test13);
      CPPUNIT_TEST(test14);
// FIXME MSW mktime and localtime don't work with dates < 1970
// replace mktime and localtime with win32 API calls
#ifndef _WIN32 
      CPPUNIT_TEST(test15);
#endif
      CPPUNIT_TEST(test16);
      CPPUNIT_TEST(test17);
    CPPUNIT_TEST_SUITE_END();
    
  public:
    virtual void setUp() {
      FiltersTestCase<std::string,dba::String>::setUp();
      sDefaultSpecs.mTimestampFormat = "%Y-%m-%d %H:%M:%S";
    };
    void test1() { notNull("not null"); };
    void test2() { null(""); };
    void test3() { toString("",""); };
    void test4() { toString("a","a"); };
    void test5() { toString("abcd","abcd"); };
    void test6() { fromDouble(1.1,"1.1"); };
    void test7() { fromDouble(1.12345,"1.12345"); };
    void test8() { fromDouble(-1.12345,"-1.12345"); };
    void test9() { fromInt(0,"0"); };
    void test10() { fromInt(1,"1"); };
    void test11() { fromInt(-1,"-1"); };
    void test12() { fromInt(-12,"-12"); };
    void test13() { fromInt(12,"12"); };
    void test14() { fromDate(date_2000,"2007-05-17 03:00:45"); };
    void test15() { fromDate(date_1815,"1815-12-12 23:12:48"); };
    void test16() { fromDate(date_2212,"2212-08-09 15:16:23"); };
    void test17() { testString("no women no cry"); };
}; 
 
class DateTimeFilter : public FiltersTestCase<struct tm,dba::DateTime> {
    CPPUNIT_TEST_SUITE(DateTimeFilter);
      CPPUNIT_TEST(test1);
      CPPUNIT_TEST(test2);
      CPPUNIT_TEST(test3);
// FIXME MSW mktime and localtime don't work with dates < 1970
// replace mktime and localtime with win32 API calls
#ifndef _WIN32 
      CPPUNIT_TEST(test4);
#endif
      CPPUNIT_TEST(test5);
      CPPUNIT_TEST(test6);
      CPPUNIT_TEST(test7);
      CPPUNIT_TEST(test8);
    CPPUNIT_TEST_SUITE_END();
  public:    
    virtual void setUp() {
      FiltersTestCase<struct tm, dba::DateTime>::setUp();
      sDefaultSpecs.mTimestampFormat="%Y-%m-%d %H:%M:%S";
    };
    void test1() { notNull(date_2000); };
    void test2() { null(date_invalid); };
    void test3() { toString(date_2000,"2007-05-17 03:00:45"); };
    void test5() { toString(date_2212,"2212-08-09 15:16:23"); };
    void test4() { toString(date_1815,"1815-12-12 23:12:48"); };
    void test7() { fromString("1815-12-12 23:12:48",date_1815); };
    void test6() { fromString("2007-05-17 03:00:45",date_2000); };
    void test8() { fromString("2212-08-09 15:16:23",date_2212); };
    void test9() { fromNull(date_2212); };
}; 
  
  
class DateTimeGMTOffset : public FiltersTestCase<struct tm,dba::DateTime> {
    CPPUNIT_TEST_SUITE(DateTimeGMTOffset);
      CPPUNIT_TEST(test1);
//      CPPUNIT_TEST(test2);
//      CPPUNIT_TEST(test3);
      CPPUNIT_TEST(test4);
//      CPPUNIT_TEST(test5);
//      CPPUNIT_TEST(test6);
    CPPUNIT_TEST_SUITE_END();
  public:    
    virtual void setUp() {
      FiltersTestCase<struct tm, dba::DateTime>::setUp();
      sDefaultSpecs.mGMTOffset = 5*60*60;
      sDefaultSpecs.mTimestampFormat="%Y-%m-%d %H:%M:%S";
    };
    void test1() { toString(date_2000,"2007-05-17 08:00:45"); };
//    void test2() { toString(date_1815,"1815-12-12 23:12:48"); };
//    void test3() { toString(date_2212,"2212-08-09 20:16:23"); };
    void test4() { fromString("2007-05-17 08:00:45",date_2000); };
//    void test5() { fromString("1815-12-12 23:12:48",date_1815); };
//    void test6() { fromString("2212-08-09 10:16:23",date_2212); };
}; 
 
}

#endif
