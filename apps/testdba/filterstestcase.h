// File: filtertestcase.h
// Purpose: Base class for plugin regression tests
// Author: Lukasz Michalski <lm at zork.pl>, Copyright 2004
// License: See COPYING file that comes with this distribution
//
// This file is a part of debea library (http://debea.net)
//
//
#ifndef TESTFILTERSTESTCASE_H
#define TESTFILTERSTESTCASE_H

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>
#include "dba/storeablefilter.h"
#include "dba/convspec.h"
#include "dba/defs.h"

#include <iostream>

namespace dba_tests {

/**
test case for dba filters
*/
template <typename T, typename F>
class FiltersTestCase : public CppUnit::TestCase {
  public:
    FiltersTestCase() {
      date_2000.tm_year = 107;
      date_2000.tm_mon = 4;
      date_2000.tm_mday = 17;
      date_2000.tm_hour = 3;
      date_2000.tm_min = 0;
      date_2000.tm_sec = 45;
    #ifdef HAVE_GTMOFF_IN_TM
      date_2000.tm_isdst = -1;
      date_2000.tm_gmtoff = 0;
      date_2000.tm_zone = 0x0;
    #endif
    
      date_1815.tm_year = -85;
      date_1815.tm_mon = 11;
      date_1815.tm_mday = 12;
      date_1815.tm_hour = 23;
      date_1815.tm_min = 12;
      date_1815.tm_sec = 48;
    #ifdef HAVE_GTMOFF_IN_TM
      date_1815.tm_isdst = -1;
      date_1815.tm_gmtoff = 0;
      date_1815.tm_zone = 0x0;
    #endif
    
      date_2212.tm_year = 312;
      date_2212.tm_mon = 7;
      date_2212.tm_mday = 9;
      date_2212.tm_hour = 15;
      date_2212.tm_min = 16;
      date_2212.tm_sec = 23;
    #ifdef HAVE_GTMOFF_IN_TM
      date_2212.tm_isdst = -1;
      date_2212.tm_gmtoff = 0;
      date_2212.tm_zone = 0x0;
    #endif
    
      date_invalid.tm_year = -1;
      date_invalid.tm_mon = -1;
      date_invalid.tm_mday = -1;
      date_invalid.tm_hour = -1;
      date_invalid.tm_min = -1;
      date_invalid.tm_sec = -1;
    #ifdef HAVE_GTMOFF_IN_TM
      date_invalid.tm_isdst = -1;
      date_invalid.tm_gmtoff = -1;
      date_invalid.tm_zone = 0x0;
    #endif
    };
    virtual void setUp() { sDefaultSpecs = dba::ConvSpec(); };

  protected:
    typedef enum {
      TE_INT = 1,
      TE_DBL = 2,
      TE_DAT = 4
    } flags;
    
    static dba::ConvSpec sDefaultSpecs;
    
    //!2007-05-17 03:00:45 date for testing
    tm date_2000;
    
    //!1815-12-12 23:12:48 date for testing
    tm date_1815;
    
    //!2212-08-09 15:16:23 date for testing
    tm date_2212;
    
    //!invalid (null) date
    tm date_invalid;
    
    void toString(const T& pData, const std::string& pExpectedResult, const dba::ConvSpec& pSpecs = sDefaultSpecs);
    void exceptionFrom(const std::string& pData, const dba::ConvSpec& pSpecs = sDefaultSpecs);
    void exceptionFrom(int pData, const dba::ConvSpec& pSpecs = sDefaultSpecs);
    void exceptionFrom(double pData, const dba::ConvSpec& pSpecs = sDefaultSpecs);
    void exceptionFrom(const tm& pDate, const dba::ConvSpec& pSpecs = sDefaultSpecs);
    void fromInt(int pData, const T& pExpectedResult, const dba::ConvSpec& pSpecs = sDefaultSpecs);
    void fromDouble(double pData, const T& pExpectedResult, const dba::ConvSpec& pSpecs = sDefaultSpecs);
    void fromString(const std::string& pData, const T& pExpectedResult, const dba::ConvSpec& pSpecs = sDefaultSpecs);
    void fromDate(const tm& pDateTime, const T& pExpectedResult, const dba::ConvSpec& pSpecs = sDefaultSpecs);
    void fromNull(const T& pNotNullValue);
    void null(const T& pData);
    void notNull(const T& pData);
    void testString(const T& pIn, const dba::ConvSpec& pSpecs = sDefaultSpecs);
};

bool
operator==(const tm& d1, const tm& d2);

template <typename T, typename F> 
dba::ConvSpec FiltersTestCase<T,F>::sDefaultSpecs;

template <typename T, typename F>
void 
FiltersTestCase<T,F>::toString(const T& pData, const std::string& pExpectedResult, const dba::ConvSpec& pSpecs) {
  //create internal value and filter for it
  T in(pData);
  F filter(in);
  
  std::string val(filter.toString(pSpecs));
  CPPUNIT_ASSERT_MESSAGE("toString failed",val == pExpectedResult);
};

template <typename T, typename F>
void 
FiltersTestCase<T,F>::fromString(const std::string& pData, const T& pExpectedResult, const dba::ConvSpec& pSpecs) {
  //create internal value and filter for it
  T in;
  F filter(in);
  
  filter.fromString(pSpecs,pData);
  CPPUNIT_ASSERT_MESSAGE("fromString failed",in == pExpectedResult);
};

template <typename T, typename F>
void 
FiltersTestCase<T,F>::testString(const T& pData, const dba::ConvSpec& pSpecs) {
  //create internal value and filter for it
  T in(pData);
  F in_filter(in);
  std::string val(in_filter.toString(pSpecs));

  T out;

  CPPUNIT_ASSERT_MESSAGE("Cannot use this test for value equal to default constructor",out != in);

  F out_filter(out);
  out_filter.fromString(pSpecs,val);
  CPPUNIT_ASSERT_MESSAGE("testString failed",out == pData);
};


template <typename T, typename F>
void 
FiltersTestCase<T,F>::fromInt(int pData, const T& pExpectedResult, const dba::ConvSpec& pSpecs) {
  //create internal value and filter for it
  T in;
  F filter(in);
  
  filter.fromInt(pSpecs,pData);
  CPPUNIT_ASSERT_MESSAGE("fromInt failed",in == pExpectedResult);
};

template <typename T, typename F>
void 
FiltersTestCase<T,F>::fromDouble(double pData, const T& pExpectedResult, const dba::ConvSpec& pSpecs) {
  //create internal value and filter for it
  T in;
  F filter(in);
  
  filter.fromDouble(pSpecs,pData);
  CPPUNIT_ASSERT_MESSAGE("fromDouble failed",in == pExpectedResult);
};

template <typename T, typename F>
void 
FiltersTestCase<T,F>::fromDate(const tm& pData, const T& pExpectedResult, const dba::ConvSpec& pSpecs) {
  //create internal value and filter for it
  T in;
  F filter(in);
  
  filter.fromDate(pSpecs,pData);
  CPPUNIT_ASSERT_MESSAGE("fromDate failed",in == pExpectedResult);
};

template <typename T, typename F>
void 
FiltersTestCase<T,F>::fromNull(const T& pNotNullValue) {
  //create internal value and filter for it
  T in(pNotNullValue);
  F filter(in);
  
  CPPUNIT_ASSERT_MESSAGE("inital value must be reported as not null by filter, fix this test",!filter.isNull());
  
  filter.fromNull();
  CPPUNIT_ASSERT_MESSAGE("filter does not report null as it should",filter.isNull());
};

template <typename T, typename F>
void 
FiltersTestCase<T,F>::notNull(const T& pData) {
  //create internal value and filter for it
  T in(pData);
  F filter(in);
  
  CPPUNIT_ASSERT_MESSAGE("filter reports null but it should not",!filter.isNull());
};

template <typename T, typename F>
void 
FiltersTestCase<T,F>::null(const T& pData) {
  //create internal value and filter for it
  T in(pData);
  F filter(in);
  
  CPPUNIT_ASSERT_MESSAGE("filter does not report null as it should",filter.isNull());
};


template <typename T, typename F>
void 
FiltersTestCase<T,F>::exceptionFrom(const std::string& pData, const dba::ConvSpec& pSpecs) {
  try {
    T in;
    F filter(in);
    filter.fromString(pSpecs,pData);
    CPPUNIT_ASSERT_MESSAGE("exception not thrown on string conversion",false);
  } catch (const dba::StoreableFilterException& pFilter) {
  };
};

template <typename T, typename F>
void 
FiltersTestCase<T,F>::exceptionFrom(int pData, const dba::ConvSpec& pSpecs) {
  try {
    T in;
    F filter(in);
    filter.fromInt(pSpecs,pData);
    CPPUNIT_ASSERT_MESSAGE("exception not thrown on int conversion",false);
  } catch (const dba::StoreableFilterException& pFilter) {
  };
};

template <typename T, typename F>
void 
FiltersTestCase<T,F>::exceptionFrom(double pData, const dba::ConvSpec& pSpecs) {
  try {
    T in;
    F filter(in);
    filter.fromDouble(pSpecs,pData);
    CPPUNIT_ASSERT_MESSAGE("exception not thrown on double conversion",false);
  } catch (const dba::StoreableFilterException& pFilter) {
  };
};

template <typename T, typename F>
void 
FiltersTestCase<T,F>::exceptionFrom(const tm& pDate, const dba::ConvSpec& pSpecs) {
  try {
    T in;
    F filter(in);
    filter.fromDate(pSpecs,pDate);
    CPPUNIT_ASSERT_MESSAGE("exception not thrown on date conversion",false);
  } catch (const dba::StoreableFilterException& pFilter) {
  };
};

}; //namespace

#endif
