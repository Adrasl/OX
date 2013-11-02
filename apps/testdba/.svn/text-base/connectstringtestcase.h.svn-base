// File: connectstringtestcase.h
// Purpose: Test for parsing connect string parameters
// Author: Lukasz Michalski <lm at zork.pl>, Copyright 2007
// License: See COPYING file that comes with this distribution
//
// This file is a part of debea library (http://debea.net)
//
//
#ifndef DBA_TESTSCONNECTSTRINGTESTCASE_H
#define DBA_TESTSCONNECTSTRINGTESTCASE_H

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

namespace dba_tests {

/**
Tests for connect string parser
*/
class ConnectStringTestCase : public CppUnit::TestCase {
    CPPUNIT_TEST_SUITE(ConnectStringTestCase);
      CPPUNIT_TEST(noParams);
      CPPUNIT_TEST(noParamsWithSpaces);
      CPPUNIT_TEST(noParamsQuoted);
      CPPUNIT_TEST(oneParam);
      CPPUNIT_TEST(twoParams);
      CPPUNIT_TEST(quotedParam);
      CPPUNIT_TEST(quotedParams);
      CPPUNIT_TEST(quoteInParam);
      CPPUNIT_TEST(quoteInQuotedParam);
      CPPUNIT_TEST(quoteInParamName);
      CPPUNIT_TEST(assignAfterQuoteInValue);
      CPPUNIT_TEST(assignInQuotedName);
      CPPUNIT_TEST(assignInQuotedValue);
      CPPUNIT_TEST(assignBeforeQuoteInName);
      CPPUNIT_TEST(spaceInQuotedParamAtBegin);
      CPPUNIT_TEST(spaceInQuotedParamAtEnd);
      CPPUNIT_TEST(spaceInQuotedValueAtBegin);
      CPPUNIT_TEST(spaceInQuotedValueAtEnd);
      CPPUNIT_TEST(spaces);
      CPPUNIT_TEST(spaces2);
    CPPUNIT_TEST_SUITE_END();
  public:
    void noParams();
    void noParamsWithSpaces();
    void noParamsQuoted();
    void oneParam();
    void twoParams();
    void quotedParam();
    void quotedParams();
    void quoteInParam();
    void quoteInQuotedParam();
    void quoteInParamName();
    void assignAfterQuoteInValue();
    void assignInQuotedName();
    void assignInQuotedValue();
    void assignBeforeQuoteInName();
    void spaceInQuotedParamAtBegin();
    void spaceInQuotedParamAtEnd();
    void spaceInQuotedValueAtBegin();
    void spaceInQuotedValueAtEnd();
    void spaces();
    void spaces2();
};

} //namespace

#endif
