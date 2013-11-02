// File: connectstringtestcase.cpp
// Purpose: Test for parsing connect string parameters
// Author: Lukasz Michalski <lm at zork.pl>, Copyright 2007
// License: See COPYING file that comes with this distribution
//
// This file is a part of debea library (http://debea.net)
//
//

#include "connectstringtestcase.h"
#include "dba/connectstringparser.h"
#include "dba/connectstring.h"

namespace dba_tests {

CPPUNIT_TEST_SUITE_REGISTRATION(ConnectStringTestCase);

void 
ConnectStringTestCase::noParams() {
  try {
    dba::ConnectString str("n");
    CPPUNIT_ASSERT_MESSAGE("ConnectStringParserException not thrown",false);
  } catch (const dba::ConnectStringParserException&) {
    //"syntax error - no assign after param name
  };
};

void 
ConnectStringTestCase::noParamsWithSpaces() {
  try {
    dba::ConnectString str("n n");
    CPPUNIT_ASSERT_MESSAGE("ConnectStringParserException not thrown",false);
  } catch (const dba::ConnectStringParserException&) {
    //"syntax error - no assign after param name
  };
};

void 
ConnectStringTestCase::noParamsQuoted() {
  try {
    dba::ConnectString str("'n n'");
    CPPUNIT_ASSERT_MESSAGE("ConnectStringParserException not thrown",false);
  } catch (const dba::ConnectStringParserException&) {
    //"syntax error - no assign after param name
  };
};

void 
ConnectStringTestCase::oneParam() {
  dba::ConnectString str("d=m");
  CPPUNIT_ASSERT(str.getParam("d") == "m");
};

void 
ConnectStringTestCase::twoParams() {
  dba::ConnectString str("d=m h=2");
  CPPUNIT_ASSERT(str.getParam("d") == "m");
  CPPUNIT_ASSERT(str.getParam("h") == "2");
};

void 
ConnectStringTestCase::quotedParam() {
  dba::ConnectString str("dbname='my database'");
  CPPUNIT_ASSERT(str.dbname() == "my database");
};

void 
ConnectStringTestCase::quotedParams() {
  dba::ConnectString str("d='m m' h=h p='t s p'");
  CPPUNIT_ASSERT(str.getParam("d") == "m m");
  CPPUNIT_ASSERT(str.getParam("h") == "h");
  CPPUNIT_ASSERT(str.getParam("p") == "t s p");
};

void 
ConnectStringTestCase::quoteInParam() {
  dba::ConnectString str("dbname=O''Connor");
  CPPUNIT_ASSERT(str.dbname() == "O'Connor");
};

void 
ConnectStringTestCase::quoteInQuotedParam() {
  dba::ConnectString str("dbname='O''Connor'");
  CPPUNIT_ASSERT(str.dbname() == "O'Connor");
};

void 
ConnectStringTestCase::quoteInParamName() {
  dba::ConnectString str("db''name=mydb");
  CPPUNIT_ASSERT(str.getParam("db'name") == "mydb");
};

void 
ConnectStringTestCase::assignAfterQuoteInValue() {
  dba::ConnectString str("dbname''=mydb");
  CPPUNIT_ASSERT(str.getParam("dbname'") == "mydb");
};

void 
ConnectStringTestCase::assignBeforeQuoteInName() {
  dba::ConnectString str("dbname=''mydb");
  CPPUNIT_ASSERT(str.getParam("dbname") == "'mydb");
};

void 
ConnectStringTestCase::assignInQuotedName() {
  dba::ConnectString str("'dbname='=mydb");
  CPPUNIT_ASSERT(str.getParam("dbname=") == "mydb");
};

void 
ConnectStringTestCase::assignInQuotedValue() {
  dba::ConnectString str("dbname='=mydb'");
  CPPUNIT_ASSERT(str.getParam("dbname") == "=mydb");
};

void 
ConnectStringTestCase::spaceInQuotedParamAtBegin() {
  dba::ConnectString str("' dbname'=mydb");
  CPPUNIT_ASSERT(str.getParam(" dbname") == "mydb");
};

void 
ConnectStringTestCase::spaceInQuotedParamAtEnd() {
  dba::ConnectString str("'dbname '=mydb");
  CPPUNIT_ASSERT(str.getParam("dbname ") == "mydb");
};

void 
ConnectStringTestCase::spaceInQuotedValueAtBegin() {
  dba::ConnectString str("dbname=' mydb'");
  CPPUNIT_ASSERT(str.getParam("dbname") == " mydb");
};

void 
ConnectStringTestCase::spaceInQuotedValueAtEnd() {
  dba::ConnectString str("dbname='mydb '");
  CPPUNIT_ASSERT(str.getParam("dbname") == "mydb ");
};

void 
ConnectStringTestCase::spaces() {
  dba::ConnectString str("dbname  = mydb");
  CPPUNIT_ASSERT(str.getParam("dbname") == "mydb");
};

void 
ConnectStringTestCase::spaces2() {
  dba::ConnectString str(" dbname  = mydb  ");
  CPPUNIT_ASSERT(str.getParam("dbname") == "mydb");
};

} //namespace
