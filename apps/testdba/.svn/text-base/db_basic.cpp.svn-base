// File: db_basic.cpp
// Purpose: Regression tests for Database interface
// Author: Lukasz Michalski <lm at zork.pl>, Copyright 2007
// License: See COPYING file that comes with this distribution
//
// This file is a part of debea library (http://debea.net)
//
//
#include "db_basic.h"
#include <algorithm>
#include <sstream>
#include "dba/localechanger.h"

namespace dba_tests {

Db_Basic::Db_Basic(const char* pPluginName, const char* pDbParams)
  : SQLDatabaseTestSuite(pPluginName, pDbParams)
{
  time_t now = time(NULL);
  struct tm tmp = *localtime(&now);
  tmp.tm_year = 107;
  tmp.tm_mon = 11;
  tmp.tm_mday = 12;
  tmp.tm_hour = 14;
  tmp.tm_min = 15;
  tmp.tm_sec = 16;
  mTimeForConv = mktime(&tmp);
}


Db_Basic::~Db_Basic()
{
}

void
Db_Basic::noRowsQuery() {
  std::auto_ptr<dba::DbConnection> conn(mDb->getConnection(mDbParams));
  std::auto_ptr<dba::DbResult> res(conn->sendQuery("SELECT intval FROM db_norows"));  
  if (res->fetchRow()) {
    CPPUNIT_ASSERT(false);
  };
};

void
Db_Basic::oneRowQuery() {
  std::auto_ptr<dba::DbConnection> conn(mDb->getConnection(mDbParams));
  std::auto_ptr<dba::DbResult> res(conn->sendQuery("SELECT intval FROM db_onerow"));  
  if (!res->fetchRow()) {
    CPPUNIT_ASSERT(false);
  };
  CPPUNIT_ASSERT(!res->isNull(0));
  int val = res->getInt(0);
  CPPUNIT_ASSERT(val == 777);
  CPPUNIT_ASSERT(!res->fetchRow());
};

void
Db_Basic::fiveRowsQuery() {
  std::auto_ptr<dba::DbConnection> conn(mDb->getConnection(mDbParams));
  std::auto_ptr<dba::DbResult> res(conn->sendQuery("SELECT intval FROM db_fiverows"));  
  int i = 0;
  while(res->fetchRow()) {
    CPPUNIT_ASSERT(!res->isNull(0));
    int val = res->getInt(0);
    CPPUNIT_ASSERT(i == val);
    i++;    
  };
  CPPUNIT_ASSERT(i == 5);
};

void
Db_Basic::namedColsQuery() {
  std::auto_ptr<dba::DbConnection> conn(mDb->getConnection(mDbParams));
  std::auto_ptr<dba::DbResult> res(conn->sendQuery("SELECT intval, intval2 as val FROM db_namedcols"));  
  if (!res->fetchRow()) {
    CPPUNIT_ASSERT(false);
  };
  CPPUNIT_ASSERT(!res->isNull("intval"));
  CPPUNIT_ASSERT(!res->isNull("intval2"));
  int val1 = res->getInt("intval");
  int val2 = res->getInt("intval2");
  CPPUNIT_ASSERT(val1 == val2);
  CPPUNIT_ASSERT(!res->fetchRow());
};

void 
Db_Basic::nulls() {
  std::auto_ptr<dba::DbConnection> conn(mDb->getConnection(mDbParams));
  std::auto_ptr<dba::DbResult> res(conn->sendQuery("SELECT intval, intval2 FROM db_nulls"));  
  if (!res->fetchRow()) {
    CPPUNIT_ASSERT(false);
  };
  CPPUNIT_ASSERT(res->isNull("intval"));
  CPPUNIT_ASSERT(!res->isNull("intval2"));
  CPPUNIT_ASSERT(!res->fetchRow());
};

void 
Db_Basic::affectedRow() {
  std::auto_ptr<dba::DbConnection> conn(mDb->getConnection(mDbParams));
  conn->sendUpdate("INSERT INTO db_affectedrows(val1,val2,val3) VALUES (1,1,1)");  
  conn->sendUpdate("INSERT INTO db_affectedrows(val1,val2,val3) VALUES (2,2,2)");  
  conn->sendUpdate("INSERT INTO db_affectedrows(val1,val2,val3) VALUES (3,3,3)");  
  conn->sendUpdate("INSERT INTO db_affectedrows(val1,val2,val3) VALUES (4,4,4)");  

  int rows = conn->sendUpdate("UPDATE db_affectedrows SET val2 = 7 WHERE val2 = 3");
  CPPUNIT_ASSERT(rows == 1);
};

void 
Db_Basic::affectedRows() {
  std::auto_ptr<dba::DbConnection> conn(mDb->getConnection(mDbParams));
  conn->sendUpdate("INSERT INTO db_affectedrows(val1,val2,val3) VALUES (1,1,1)");  
  conn->sendUpdate("INSERT INTO db_affectedrows(val1,val2,val3) VALUES (2,2,2)");  
  conn->sendUpdate("INSERT INTO db_affectedrows(val1,val2,val3) VALUES (3,3,3)");  
  conn->sendUpdate("INSERT INTO db_affectedrows(val1,val2,val3) VALUES (4,4,4)");  

  //sqlite3 implements DELETE FROM <table> without a WHERE clause by dropping and recreating the table
  //and this will always return 0
  int rows = conn->sendUpdate("DELETE FROM db_affectedrows WHERE val1 != 5");
  CPPUNIT_ASSERT(rows == 4);
};

void
Db_Basic::numCols() {
  std::auto_ptr<dba::DbConnection> conn(mDb->getConnection(mDbParams));
  std::auto_ptr<dba::DbResult> res(conn->sendQuery("SELECT * FROM db_numcols"));
  CPPUNIT_ASSERT(res->columns() == 7);
};

void
Db_Basic::sqlQueryError() {
  try {
    std::auto_ptr<dba::DbConnection> conn(mDb->getConnection(mDbParams));
    conn->sendQuery("this is really bad sql :-)");  
    CPPUNIT_ASSERT(false);
  } catch (const TestSQLException&) {
  };
};

void
Db_Basic::sqlUpdateError() {
  try {
    std::auto_ptr<dba::DbConnection> conn(mDb->getConnection(mDbParams));
    conn->sendUpdate("this is really bad sql :-)");  
    CPPUNIT_ASSERT(false);
  } catch (const TestSQLException&) {
    return;
  };
};

void 
Db_Basic::convSpec_inheritConn() {
  dba::ConvSpec spec;
  spec.mDecimalPoint = 'z';
  mDb->setConversionSpecs(spec);
  std::auto_ptr<dba::DbConnection> conn(mDb->getConnection(mDbParams));
  CPPUNIT_ASSERT(conn->getConversionSpecs().mDecimalPoint == 'z');
};

void 
Db_Basic::convSpec_inheritResult() {
  std::auto_ptr<dba::DbConnection> conn(mDb->getConnection(mDbParams));
  dba::ConvSpec spec;
  spec.mDecimalPoint = 'z';
  conn->setConversionSpecs(spec);
  
  std::auto_ptr<dba::DbResult> res(conn->sendQuery("SELECT * FROM db_conv"));
  CPPUNIT_ASSERT(res->getConversionSpecs().mDecimalPoint == 'z');
};

void
Db_Basic::convSpec_inheritCol() {
  std::auto_ptr<dba::DbConnection> conn(mDb->getConnection(mDbParams));
  dba::ConvSpec spec;
  spec.mDbCharset = dba::ConvSpec::UTF7;
  conn->setConversionSpecs(spec);
  std::auto_ptr<dba::DbResult> res(conn->sendQuery("SELECT * FROM db_conv"));
  
  CPPUNIT_ASSERT(res->getColumn(0).getDbCharset() == dba::ConvSpec::UTF7);
};

void 
Db_Basic::relationList() {
  std::auto_ptr<dba::DbConnection> conn(mDb->getConnection(mDbParams));
  std::list<std::string> relations = conn->getRelationNames();

  //do not fail if next tests adds new tables to sql_queries.h
  CPPUNIT_ASSERT(relations.size() > 7); 

  //check one relation name
  std::list<std::string>::const_iterator it = std::find(relations.begin(), relations.end(), "db_conv");
  CPPUNIT_ASSERT(it != relations.end());
};

void 
Db_Basic::query_utf8() {
  dba::ConvSpec specs(mDb->getConversionSpecs());
  specs.mDbCharset = dba::ConvSpec::UTF8;
  mDb->setConversionSpecs(specs);
  std::auto_ptr<dba::DbConnection> conn(mDb->getConnection(mDbParams));
  std::stringstream query;
  query << "INSERT INTO db_strdata VALUES ('";
  std::string data("\305\274\303\263\305\202\167"); //"turtle" in polish
  //std::string data("zołw");
  query << data << "')"; 
  conn->sendUpdate(query.str().c_str());

  std::auto_ptr<dba::DbResult> res(conn->sendQuery("SELECT strval FROM db_strdata"));
  CPPUNIT_ASSERT(res->fetchRow());
  std::string outdata = res->getString(0);
  CPPUNIT_ASSERT(data == outdata);
};

void 
Db_Basic::query_cp1250() {
  dba::ConvSpec specs(mDb->getConversionSpecs());
  specs.mDbCharset = dba::ConvSpec::CP1250;
  mDb->setConversionSpecs(specs);
  std::auto_ptr<dba::DbConnection> conn(mDb->getConnection(mDbParams));
  std::stringstream query;
  query << "INSERT INTO db_strdata VALUES ('";
  std::string data("\277\363\263\167"); //"turtle" in polish
  query << data << "')"; 
  conn->sendUpdate(query.str().c_str());

  std::auto_ptr<dba::DbResult> res(conn->sendQuery("SELECT strval FROM db_strdata"));
  CPPUNIT_ASSERT(res->fetchRow());
  std::string outdata = res->getString(0);
  CPPUNIT_ASSERT(data == outdata);
};

void 
Db_Basic::query_iso8859_2() {
  dba::ConvSpec specs(mDb->getConversionSpecs());
  specs.mDbCharset = dba::ConvSpec::ISO8859_2;
  mDb->setConversionSpecs(specs);
  std::auto_ptr<dba::DbConnection> conn(mDb->getConnection(mDbParams));
  std::stringstream query;
  query << "INSERT INTO db_strdata VALUES ('";
  std::string data("\277\363\263\167"); //"turtle pist" in polish
  query << data << "')"; 
  conn->sendUpdate(query.str().c_str());

  std::auto_ptr<dba::DbResult> res(conn->sendQuery("SELECT strval FROM db_strdata"));
  CPPUNIT_ASSERT(res->fetchRow());
  std::string outdata = res->getString(0);
  CPPUNIT_ASSERT(data == outdata);
};


void 
Db_Basic::bigstrdata_load() {
  std::auto_ptr<dba::DbConnection> conn(mDb->getConnection(mDbParams));
  std::auto_ptr<dba::DbResult> res(conn->sendQuery("SELECT strval FROM db_bigstrdata"));
  CPPUNIT_ASSERT(res->fetchRow());
  std::string outdata = res->getString(0);
  int size = outdata.size();
  CPPUNIT_ASSERT(size == 1000);
};

void 
Db_Basic::conv_int_to_str() {
  std::auto_ptr<dba::DbConnection> conn(mDb->getConnection(mDbParams));
  std::auto_ptr<dba::DbResult> res(conn->sendQuery("SELECT i_val FROM db_conv"));
  CPPUNIT_ASSERT(res->fetchRow());
  std::string outdata = res->getString(0);
  CPPUNIT_ASSERT(outdata == "1");
};

void 
Db_Basic::conv_double_to_str() {
  dba::ConvSpec specs(mDb->getConversionSpecs());
  specs.mDecimalPoint = '.';
  mDb->setConversionSpecs(specs);
  std::auto_ptr<dba::DbConnection> conn(mDb->getConnection(mDbParams));
  std::auto_ptr<dba::DbResult> res(conn->sendQuery("SELECT f_val FROM db_conv"));
  CPPUNIT_ASSERT(res->fetchRow());
  std::string outdata = res->getString(0);
  //when converted from floating point value string data can be not exact as
  //written in database
  CPPUNIT_ASSERT(outdata.substr(0,3) == "2.2");
};

void 
Db_Basic::conv_date_to_str() {
  dba::ConvSpec specs(mDb->getConversionSpecs());
  specs.mTimestampFormat = "%Y-%m-%d %H:%M:%S";
  mDb->setConversionSpecs(specs);
  std::auto_ptr<dba::DbConnection> conn(mDb->getConnection(mDbParams));
  std::auto_ptr<dba::DbResult> res(conn->sendQuery("SELECT d_val FROM db_conv"));
  CPPUNIT_ASSERT(res->fetchRow());
  std::string outdata = res->getString(0);
  CPPUNIT_ASSERT(outdata == "2007-12-12 14:15:16");
};

void 
Db_Basic::conv_str_to_str() {
  std::auto_ptr<dba::DbConnection> conn(mDb->getConnection(mDbParams));
  std::auto_ptr<dba::DbResult> res(conn->sendQuery("SELECT s_val FROM db_conv"));
  CPPUNIT_ASSERT(res->fetchRow());
  std::string outdata = res->getString(0);
  CPPUNIT_ASSERT(outdata == "3.3");
};

void 
Db_Basic::conv_str_to_int() {
  std::auto_ptr<dba::DbConnection> conn(mDb->getConnection(mDbParams));
  conn->sendUpdate("UPDATE db_conv SET s_val='3'");
  std::auto_ptr<dba::DbResult> res(conn->sendQuery("SELECT s_val FROM db_conv"));
  CPPUNIT_ASSERT(res->fetchRow());
  int outdata = res->getInt(0);
  CPPUNIT_ASSERT(outdata == 3);
};

void 
Db_Basic::conv_double_to_int() {
  std::auto_ptr<dba::DbConnection> conn(mDb->getConnection(mDbParams));
  std::auto_ptr<dba::DbResult> res(conn->sendQuery("SELECT f_val FROM db_conv"));
  CPPUNIT_ASSERT(res->fetchRow());
  int outdata = res->getInt(0);
  CPPUNIT_ASSERT(outdata == 2);
};

void 
Db_Basic::conv_int_to_int() {
  std::auto_ptr<dba::DbConnection> conn(mDb->getConnection(mDbParams));
  std::auto_ptr<dba::DbResult> res(conn->sendQuery("SELECT i_val FROM db_conv"));
  CPPUNIT_ASSERT(res->fetchRow());
  int outdata = res->getInt(0);
  CPPUNIT_ASSERT(outdata == 1);
};

void 
Db_Basic::conv_str_to_double() {
  std::auto_ptr<dba::DbConnection> conn(mDb->getConnection(mDbParams));
  std::auto_ptr<dba::DbResult> res(conn->sendQuery("SELECT s_val FROM db_conv"));
  CPPUNIT_ASSERT(res->fetchRow());
  double outdata = res->getDouble(0);
  CPPUNIT_ASSERT(outdata == 3.3);
};

void 
Db_Basic::conv_str_to_double_pl() {
#ifdef _WIN32
  dba::LocaleChanger l(LC_ALL,"Polish_Poland.1250");
#else
  dba::LocaleChanger l(LC_ALL,"pl_PL");
#endif
  CPPUNIT_ASSERT(l.isOk());
  std::auto_ptr<dba::DbConnection> conn(mDb->getConnection(mDbParams));
  std::auto_ptr<dba::DbResult> res(conn->sendQuery("SELECT s_val FROM db_conv"));
  CPPUNIT_ASSERT(res->fetchRow());
  double outdata = res->getDouble(0);
  CPPUNIT_ASSERT(outdata == 3.3);
};

void 
Db_Basic::conv_int_to_double() {
  std::auto_ptr<dba::DbConnection> conn(mDb->getConnection(mDbParams));
  std::auto_ptr<dba::DbResult> res(conn->sendQuery("SELECT i_val FROM db_conv"));
  CPPUNIT_ASSERT(res->fetchRow());
  double outdata = res->getDouble(0);
  CPPUNIT_ASSERT(outdata == 1);
};


void 
Db_Basic::conv_double_to_double() {
  std::auto_ptr<dba::DbConnection> conn(mDb->getConnection(mDbParams));
  std::auto_ptr<dba::DbResult> res(conn->sendQuery("SELECT f_val FROM db_conv"));
  CPPUNIT_ASSERT(res->fetchRow());
  double outdata = res->getDouble(0);
  CPPUNIT_ASSERT(outdata == 2.2);
};


void 
Db_Basic::conv_str_to_date() {
  std::auto_ptr<dba::DbConnection> conn(mDb->getConnection(mDbParams));
  int rows = conn->sendUpdate("UPDATE db_conv SET s_val='2007-12-12 14:15:16'");
  std::auto_ptr<dba::DbResult> res(conn->sendQuery("SELECT s_val FROM db_conv"));
  CPPUNIT_ASSERT(res->fetchRow());
  struct tm outdata = res->getDate(0);
  CPPUNIT_ASSERT(outdata.tm_year == 107);
  CPPUNIT_ASSERT(outdata.tm_mon == 11);
  CPPUNIT_ASSERT(outdata.tm_mday == 12);
  CPPUNIT_ASSERT(outdata.tm_hour == 14);
  CPPUNIT_ASSERT(outdata.tm_min == 15);
  CPPUNIT_ASSERT(outdata.tm_sec == 16);
};

void 
Db_Basic::conv_date_to_date() {
  std::auto_ptr<dba::DbConnection> conn(mDb->getConnection(mDbParams));
  std::auto_ptr<dba::DbResult> res(conn->sendQuery("SELECT d_val FROM db_conv"));
  CPPUNIT_ASSERT(res->fetchRow());
  struct tm outdata = res->getDate(0);
  CPPUNIT_ASSERT(outdata.tm_year == 107);
  CPPUNIT_ASSERT(outdata.tm_mon == 11);
  CPPUNIT_ASSERT(outdata.tm_mday == 12);
  CPPUNIT_ASSERT(outdata.tm_hour == 14);
  CPPUNIT_ASSERT(outdata.tm_min == 15);
  CPPUNIT_ASSERT(outdata.tm_sec == 16);
};

} //namespace
