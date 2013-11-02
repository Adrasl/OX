// File: sqlarchive_lists.cpp
// Purpose: Regression tests for references
// Author: Lukasz Michalski <lm at zork.pl>, Copyright 2007
// License: See COPYING file that comes with this distribution
//
// This file is a part of debea library (http://debea.net)
//
//
#include "sqlarchive_lists.h"
#include "utils.h"
#include "dba/datetime_filter.h"

namespace dba_tests {

SQLArchive_Lists::SQLArchive_Lists(const char* pPluginName, const char* pDbParams)
 : dba_tests::SQLArchive_Basic(pPluginName, pDbParams)
{
}

struct bindedMatcher {
  bindedMatcher(const TestData& pData) : mData(pData) {};
  bool operator() (const TestData& pData) {
    return mData.bi == pData.bi && mData.bs == pData.bs;
  };
  
  const TestData& mData;
};

void 
SQLArchive_Lists::compare(const std::list<TestData>& pOrg, const std::list<TestData>& pLoaded, bool pBindedOnly) {
  std::list<TestData> org(pOrg);
  for (std::list<TestData>::const_iterator it = pLoaded.begin(); it != pLoaded.end(); it++) {
    std::list<TestData>::iterator oit;
    if (pBindedOnly)
      oit = std::find_if(org.begin(),org.end(),bindedMatcher(*it));
    else
      oit = std::find(org.begin(),org.end(),*it);
    CPPUNIT_ASSERT(oit != org.end());
    org.erase(oit);
  };
  CPPUNIT_ASSERT(org.size() == 0);
};

void
SQLArchive_Lists::compareList(const std::list<TestData>& pData) {
  std::list<TestData> olst(pData);
  {
    dba::SQLOStream ostream = mSQLArchive->getOStream();
    ostream.open();
    dba::BindedInt bi = ostream.bindInt("test_objects","bindedint");
    dba::BindedString bs = ostream.bindString("test_objects","bindedstring");
    TestObject obj;
    ostream.open();
    for(std::list<TestData>::iterator it = olst.begin(); it != olst.end(); it++) {
      bi = it->bi;
      bs = it->bs;
      TestObject* o(&(it->o));
      ostream.put(o);
    };
  };  
  
  {
    std::list<TestData> binded_only;
    std::list<TestData> full_data;
    
    dba::SQLIStream is = mSQLArchive->getIStream();
    dba::BindedInt bi = is.bindInt("test_objects","bindedint");
    dba::BindedString bs = is.bindString("test_objects","bindedstring");
    bi = -1;
    TestObject t;
    is.open(t);
    do {
      TestObject tt;
      //update binded vars WITHOUT getting object data (no row fetch)
      if (!is.updateVars())
        break;
      TestData binded;
      binded.bi = bi;
      binded.bs = bs;
      binded_only.push_back(binded);
      //update binded vars and object data (row fetch)
      if (!is.getNext(&tt))
        break;
      TestData full;
      full.o = tt;
      full.bi = bi;
      full.bs = bs;      
      full_data.push_back(full);
    } while(true);
  
    compare(olst,binded_only,true);
    compare(olst,full_data,false);
  };
};

/////////////////// Tests from here ////////////////////////////

void
SQLArchive_Lists::oneNonNullRecord() {
  std::list<TestData> olst;
  tm date(Utils::getNow());
  
  TestObject obj;
  obj = TestObject(1,1.1,"str1",date);
  olst.push_back(TestData(obj,1,"1"));

  compareList(olst);
};

void
SQLArchive_Lists::simpleData() {
  std::list<TestData> olst;
  tm date(Utils::getNow());
  
  TestObject obj;
  obj = TestObject(1,1.1,"str1",date);
  olst.push_back(TestData(obj,1,"1"));
  
  date = Utils::getNow(1);
  obj = TestObject(2,2.1,"str2",date);
  olst.push_back(TestData(obj,2,"2"));
  compareList(olst);
};

void
SQLArchive_Lists::weirdChars() {
  std::list<TestData> olst;
  tm date(Utils::getNow());
  
  TestObject obj;
  obj = TestObject(1,1.1,"\x01",date);
  olst.push_back(TestData(obj,1,"1"));
  
  date = Utils::getNow(1);
  obj = TestObject(2,2.1,"\x02",date);
  olst.push_back(TestData(obj,2,"2"));
  compareList(olst);
};

void
SQLArchive_Lists::nulls() {
  std::list<TestData> olst;
  tm date;
  dba::DateTime filter(date);
  filter.fromNull();
  
  TestObject obj;
  obj = TestObject(0,0,"",date);
  olst.push_back(TestData(obj,0,""));
  
  compareList(olst);
};


void
SQLArchive_Lists::bigListTest() {
  tm date(Utils::getNow());
  TestObject obj;
  obj = TestObject(1,1.1,"str1",date);
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  for(int i = 0; i < 30000; i++) {
    ostream.put(&obj);      
    obj = TestObject(1,1.1,"1234567890123456789023456789234567892345678923456789wertyuisdfghjkcvbnmxcvbnmdfghjwertyuio",date);
  };  
  ostream.destroy();
  dba::SQLIStream istream = mSQLArchive->getIStream();
  std::auto_ptr<dba::DbResult> res(istream.sendQuery("select count(*) from test_objects"));
  CPPUNIT_ASSERT(res->fetchRow());
  CPPUNIT_ASSERT(res->getInt(0) == 30000);
};

}
