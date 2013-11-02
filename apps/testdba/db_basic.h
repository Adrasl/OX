// File: db_basic.h
// Purpose: Regression tests for Database interface
// Author: Lukasz Michalski <lm at zork.pl>, Copyright 2007
// License: See COPYING file that comes with this distribution
//
// This file is a part of debea library (http://debea.net)
//
//
#ifndef DBA_TESTSDB_BASIC_H
#define DBA_TESTSDB_BASIC_H

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>
#include "dbplugintestsuite.h"

namespace dba_tests {

/**
Basic Database tests*/
class Db_Basic  : public SQLDatabaseTestSuite {
    CPPUNIT_TEST_SUITE(Db_Basic);
      CPPUNIT_TEST(relationList);
      CPPUNIT_TEST(noRowsQuery);
      CPPUNIT_TEST(oneRowQuery);
      CPPUNIT_TEST(fiveRowsQuery);
      //CPPUNIT_TEST(namedColsQuery); //FIXME causes sqlite3 test to crash
      CPPUNIT_TEST(nulls);
      CPPUNIT_TEST(sqlQueryError);
      CPPUNIT_TEST(sqlUpdateError);
      CPPUNIT_TEST(affectedRow);
      CPPUNIT_TEST(affectedRows);
      CPPUNIT_TEST(numCols);
      CPPUNIT_TEST(convSpec_inheritConn);
      CPPUNIT_TEST(convSpec_inheritResult);
      CPPUNIT_TEST(convSpec_inheritCol);
      CPPUNIT_TEST(query_utf8);
      //those two tests needs libdbaXXXtest created in non-utf8 encoding.
      //CPPUNIT_TEST(query_iso8859_2);
      //CPPUNIT_TEST(query_cp1250);

      CPPUNIT_TEST(bigstrdata_load);
      //all possible conversions
      CPPUNIT_TEST(conv_int_to_str);
      CPPUNIT_TEST(conv_double_to_str);
      CPPUNIT_TEST(conv_date_to_str);
      CPPUNIT_TEST(conv_str_to_str);
      CPPUNIT_TEST(conv_str_to_int);
      CPPUNIT_TEST(conv_double_to_int);
      CPPUNIT_TEST(conv_int_to_int);
      CPPUNIT_TEST(conv_str_to_double);
      CPPUNIT_TEST(conv_int_to_double);
      CPPUNIT_TEST(conv_double_to_double);
      CPPUNIT_TEST(conv_str_to_date);
      CPPUNIT_TEST(conv_date_to_date);
      CPPUNIT_TEST_SUITE_END();
  public:
    Db_Basic() {};
    Db_Basic(const char* pPluginName, const char* pDbParams);
    ~Db_Basic();
    //tests
    void relationList();
    void noRowsQuery();
    void oneRowQuery();
    void fiveRowsQuery();
    void namedColsQuery();
    void nulls();
    void sqlQueryError();
    void sqlUpdateError();
    void affectedRow();
    void affectedRows();
    void numCols();
    void convSpec_inheritConn();
    void convSpec_inheritResult();
    void convSpec_inheritCol();

    void conv_int_to_str();
    void conv_double_to_str();
    void conv_date_to_str();
    void conv_str_to_str();
    void conv_str_to_int();
    void conv_double_to_int();
    void conv_int_to_int();
    void conv_str_to_double();
    void conv_int_to_double();
    void conv_double_to_double();
    void conv_str_to_date();
    void conv_date_to_date();
    void conv_str_to_double_pl(); //polish uses "," as decimal point
    //those conversions are invalid.
    //void conv_int_to_date();
    //void conv_double_to_date();
    //void conv_date_to_int();
    //void conv_date_to_double();

    void query_utf8();
    void query_cp1250();
    void query_iso8859_2();
    void bigstrdata_load();

    time_t mTimeForConv;
};

} //namespace

#endif
