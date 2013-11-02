// File: csvtestcase.cpp
// Purpose: Regression tests for CSV driver
// Author: Lukasz Michalski <lm at zork.pl>, Copyright 2007
// License: See COPYING file that comes with this distribution
//
// This file is a part of debea library (http://debea.net)

#ifdef TEST_CSV

#include <fstream>
#include <iostream>
#include "csvtestcase.h"
#include "dba/storeable.h"
#include "dba/stdfilters.h"

namespace dba_tests {

std::string
readFile(const char* pFilename) {
  std::ifstream s(pFilename);
  std::string data;
  char c;
  while (s.good()) {
    s.get(c);
    data += c;
  };
  return data;
};

BEGIN_STORE_TABLE(CSVTestCase::CSVTester, dba::Storeable, "test")
  BIND_STR(CSVTester::a,dba::Int,"a")
  BIND_STR(CSVTester::b,dba::Int,"b")
  BIND_STR(CSVTester::str,dba::String,"str")
END_STORE_TABLE()

/*======================== test cases =====================*/

CPPUNIT_TEST_SUITE_REGISTRATION(CSVTestCase);

void 
CSVTestCase::setUp() {
  mResults.push_back(CSVTester(1,2,"\234sdfsdfd\231\130"));
  mResults.push_back(CSVTester(3,4,"df\"sfsdg"));
  mResults.push_back(CSVTester(5,6,"\""));
  mResults.push_back(CSVTester(7,0,""));
  mResults.push_back(CSVTester(9,0,""));
  mResults.push_back(CSVTester(10,11,"\""));
  mResults.push_back(CSVTester(12,12,","));
  mResults.push_back(CSVTester(13,14,",\","));
  mResults.push_back(CSVTester(1,3,"d"));
  mResults.push_back(CSVTester(2,3,"\t3 "));
  mResults.push_back(CSVTester(14,15,
"this is\n"
"a \n"
"mulitline\n"
"unix \n"
"text"
  ));
};

void 
CSVTestCase::tearDown() {

};


void 
CSVTestCase::manual() {
  unlink("csv-manual.csv");
  dba::CSVArchive ar;
  ar.hasColumnNames(false);
  ar.addMapping(0,"test","a");
  ar.addMapping(1,"test","b");
  ar.addMapping(2,"test","str");
  ar.open("csv-manual.csv");

  dba::CSVOStream ostream = ar.getOStream();
  ostream.open();
  for(std::list<CSVTester>::iterator it = mResults.begin(); it != mResults.end(); it++) {
    CSVTester& t = *it;
//    cerr << "rec: " << " [" << t.a << "] [" << t.b << "] [" << t.str << "]" << endl;
    ostream.put(&t);
  };
  ostream.close();
  
  std::list<CSVTester> lst;
  {
    CSVTester t;
    dba::CSVIStream istream = ar.getIStream();
    istream.open(t);  
    while(istream.getNext(&t)) {
      lst.push_back(t);
    };
    istream.close();
  };
  unlink("csv-manual.csv");
  CPPUNIT_ASSERT(mResults == lst);
};


void 
CSVTestCase::autoCol() {
  unlink("csv-auto.csv");
  dba::CSVArchive ar;
  ar.hasColumnNames(false);
  ar.open("csv-auto.csv");
  ar.hasColumnNames(true);
  dba::CSVOStream ostream = ar.getOStream();
  ostream.open();
  for(std::list<CSVTester>::iterator it = mResults.begin(); it != mResults.end(); it++) {
    CSVTester& t = *it;
//    cerr << "rec: " << " [" << t.a << "] [" << t.b << "] [" << t.str << "]" << endl;
    ostream.put(&t);
  };
  ostream.close();

  ar.hasColumnNames(true);
  ar.open("csv-auto.csv");
//  for (std::list<std::string>::iterator it = cols.begin(); it != cols.end(); it++)
//    cerr << "col: " << *it << endl;
  dba::CSVIStream istream = ar.getIStream();
  std::list<std::string> cols = istream.getColumns();
  CSVTester t;
  istream.open(t);
    
  std::list<CSVTester>::iterator it = mResults.begin();
  bool col = true;
  while (istream.getNext(&t)) {
//    cerr << "rec: [" << t.a << "] [" << t.b << "] [" << t.str << "]" << endl;
    if (col) {
      col = false;
    } else {
      CPPUNIT_ASSERT(it != mResults.end());
      CPPUNIT_ASSERT(t == *it);
    };
    it++;
  };
  istream.close();
  CPPUNIT_ASSERT(it == mResults.end());
  unlink("csv-auto.csv");
};

class CSVBindTester : public dba::Storeable {
    DECLARE_STORE_TABLE();
  public:
    CSVBindTester() : dba::Storeable(InvalidId) {};
};

BEGIN_STORE_TABLE(CSVBindTester,dba::Storeable,"test")
END_STORE_TABLE()


void 
CSVTestCase::autoBindedVarsEmptyObj() {
  unlink("csv-auto-binded.csv");
  dba::CSVArchive ar;
  ar.hasColumnNames(false);
  ar.open("csv-auto-binded.csv");
  ar.hasColumnNames(true);
  {
    dba::CSVOStream ostream = ar.getOStream();
    dba::BindedInt ba(ostream.bindInt("test","a"));
    dba::BindedInt bb(ostream.bindInt("test","b"));
    dba::BindedString bs(ostream.bindString("test","str"));
    ostream.open();
    for(std::list<CSVTester>::iterator it = mResults.begin(); it != mResults.end(); it++) {
      CSVBindTester t;
      ba = it->a;
      bb = it->b;
      bs = it->str;
//      cerr << "out: " << " [" << it->a << "] [" << it->b << "] [" << it->str << "]" << endl;
      ostream.put(&t);
    };
    ostream.close();
  }
  ar.hasColumnNames(true);
  
  ar.open("csv-auto-binded.csv");
  std::list<std::string> cols = ar.getIStream().getColumns();
//  for (std::list<std::string>::iterator it = cols.begin(); it != cols.end(); it++)
    //cerr << "col: " << *it << endl;
  {
    dba::CSVIStream istream = ar.getIStream();
    dba::BindedInt ba(istream.bindInt("test","a"));
    dba::BindedInt bb(istream.bindInt("test","b"));
    dba::BindedString bs(istream.bindString("test","str"));
    CSVBindTester t;
    istream.open(t);
      
    std::list<CSVTester>::iterator it = mResults.begin();
    bool col = true;
    while (istream.getNext(&t)) {
//      cerr << "in: [" << (int)ba << "] [" << (int)bb << "] [" << std::string(bs) << "]" << endl;
      if (col) {
        
        col = false;
      } else {
        CPPUNIT_ASSERT(it != mResults.end());
        CPPUNIT_ASSERT(ba == it->a);
        CPPUNIT_ASSERT(bb == it->b);
        CPPUNIT_ASSERT((std::string)bs == it->str);
      };
      it++;
    };
    CPPUNIT_ASSERT(it == mResults.end());
  };
  unlink("csv-auto-binded.csv");
};

void 
CSVTestCase::manualBindedVars() {
  unlink("csv-manual-binded.csv");
  dba::CSVArchive ar;
  ar.hasColumnNames(false);
  ar.open("csv-manual-binded.csv");
  ar.addMapping(0,"test","a");
  ar.addMapping(1,"test","b");
  ar.addMapping(2,"test","str");
  {
    dba::CSVOStream stream = ar.getOStream();
    dba::BindedInt ba(stream.bindInt("test","a"));
    dba::BindedInt bb(stream.bindInt("test","b"));
    dba::BindedString bs(stream.bindString("test","str"));
    stream.open();
    for(std::list<CSVTester>::iterator it = mResults.begin(); it != mResults.end(); it++) {
      CSVBindTester t;
      ba = it->a;
      bb = it->b;
      bs = it->str;
  //    cerr << "rec: " << " [" << t.a << "] [" << t.b << "] [" << t.str << "]" << endl;
      stream.put(&t);
    };
    stream.close();
  };

  std::list<std::string> cols = ar.getIStream().getColumns();
//  for (std::list<std::string>::iterator it = cols.begin(); it != cols.end(); it++)
    //cerr << "col: " << *it << endl;
  {
    dba::CSVIStream stream = ar.getIStream();
    dba::BindedInt ba(stream.bindInt("test","a"));
    dba::BindedInt bb(stream.bindInt("test","b"));
    dba::BindedString bs(stream.bindString("test","str"));
    CSVBindTester t;
    stream.open(t);
      
    std::list<CSVTester>::iterator it = mResults.begin();
    bool col = true;
    while (stream.getNext(&t)) {
  //    cerr << "rec: [" << (int)ba << "] [" << (int)bb << "] [" << std::string(bs) << "]" << endl;
      if (col) {
        
        col = false;
      } else {
        CPPUNIT_ASSERT(it != mResults.end());
        CPPUNIT_ASSERT(ba == it->a);
        CPPUNIT_ASSERT(bb == it->b);
        CPPUNIT_ASSERT((std::string)bs == it->str);
      };
      it++;
    };
    CPPUNIT_ASSERT(it == mResults.end());
  }
  unlink("csv-manual-binded.csv");
};

void 
CSVTestCase::autoMultiBindedVars() {
  unlink("csv-multi-auto-binded.csv");
  {
    dba::CSVArchive ar;
    ar.hasColumnNames(false);
    ar.open("csv-multi-auto-binded.csv");
    ar.hasColumnNames(true);
    dba::CSVOStream stream = ar.getOStream();
    dba::BindedInt ba(stream.bindInt("test","a"));
    dba::BindedInt bb1(stream.bindInt("test","b"));
    dba::BindedString bs(stream.bindString("test","str"));
    dba::BindedInt bb2(stream.bindInt("test","b"));
    stream.open();
    for(std::list<CSVTester>::iterator it = mResults.begin(); it != mResults.end(); it++) {
      CSVBindTester t;
      ba = it->a;
      bb1 = it->b;
      bs = it->str;
      bb2 = it->b + 1;
//    cerr << "rec: " << " [" << t.a << "] [" << t.b << "] [" << t.str << "]" << endl;
      stream.put(&t);
    };
    stream.close();
  };

  {;
    dba::CSVArchive ar;
    ar.hasColumnNames(true);
  
    ar.open("csv-multi-auto-binded.csv");
    std::list<std::string> cols = ar.getIStream().getColumns();
//  for (std::list<std::string>::iterator it = cols.begin(); it != cols.end(); it++)
    //cerr << "col: " << *it << endl;
    dba::CSVIStream stream = ar.getIStream();
    dba::BindedInt ba(stream.bindInt("test","a"));
    dba::BindedInt bb1(stream.bindInt("test","b"));
    dba::BindedString bs(stream.bindString("test","str"));
    dba::BindedInt bb2(stream.bindInt("test","b"));
    CSVBindTester t;
    stream.open(t);
    
    std::list<CSVTester>::iterator it = mResults.begin();
    bool col = true;
    while (stream.getNext(&t)) {
//    cerr << "rec: [" << (int)ba << "] [" << (int)bb << "] [" << std::string(bs) << "]" << endl;
      if (col) {
        col = false;
      } else {
        CPPUNIT_ASSERT(it != mResults.end());
        CPPUNIT_ASSERT(ba == it->a);
        CPPUNIT_ASSERT(bb1 == it->b);
        CPPUNIT_ASSERT(bb2 == it->b+1);
        CPPUNIT_ASSERT((std::string)bs == it->str);
      };
      it++;
    };
    CPPUNIT_ASSERT(it == mResults.end());
  };
  unlink("csv-multi-auto-binded.csv");
};

void 
CSVTestCase::openNonCSVFileForWriteTest2() {
  dba::CSVArchive ar;
  ar.hasColumnNames(false);
  //should not throw exception
  ar.open("open_for_write_test.xml");
}; //openNonCSVFileForWriteTest2

void 
CSVTestCase::openNonCSVFileForWriteTest() {
  try {
    dba::CSVArchive ar;
    ar.hasColumnNames(true);
    //should throw exception
    ar.open("open_for_write_test.xml");
    dba::CSVIStream stream(ar.getIStream());
    dba::Storeable e;
    stream.open(e);
    CPPUNIT_ASSERT_MESSAGE("istream should throw",false);
  } catch (const dba::DataException&) {
    //
  };
}; //openNonCSVFileForWriteTest

void 
CSVTestCase::sepTest() {
  dba::CSVArchive ar;
  ar.hasColumnNames(true);
  ar.setFieldSeparator(';');
  
  CSVTester t1(1,2,"ole!");
  CSVTester t2(1,2,"o;l,e!");

  unlink("fiedlsep.csv");
  ar.open("fiedlsep.csv");
  {
    dba::CSVOStream stream = ar.getOStream();
    stream.open();
    stream.put(&t1);
    stream.put(&t2);
  }

  CSVTester r1;
  CSVTester r2;
  {
    dba::CSVIStream stream = ar.getIStream();
    stream.open(r1);
    stream.getNext(&r1);
    stream.getNext(&r2);  
  };

  CPPUNIT_ASSERT(t1 == r1);
  CPPUNIT_ASSERT(t2 == r2);
};

void 
CSVTestCase::autoBindedVars() {
  dba::CSVArchive ar;
  ar.hasColumnNames(false);
  ar.open("csv-col-binded.csv");
  ar.hasColumnNames(true);
  ar.addMapping("a","test","a");
  ar.addMapping("b","test","b");
  ar.addMapping("str","test","str");
  {
    dba::CSVOStream stream = ar.getOStream();
    dba::BindedInt ba(stream.bindInt("test","a"));
    dba::BindedInt bb(stream.bindInt("test","b"));
    dba::BindedString bs(stream.bindString("test","str"));
    stream.open();
    for(std::list<CSVTester>::iterator it = mResults.begin(); it != mResults.end(); it++) {
      CSVBindTester t;
      ba = it->a;
      bb = it->b;
      bs = it->str;
  //    cerr << "rec: " << " [" << t.a << "] [" << t.b << "] [" << t.str << "]" << endl;
      stream.put(&t);
    };
    stream.close();
  };
  
  ar.hasColumnNames(true);
  ar.clearMappings();
  ar.addMapping("a","test","a");
  ar.addMapping(1,"test","b");
  ar.addMapping("str","test","str");
  ar.open("csv-col-binded.csv");
  std::list<std::string> cols = ar.getIStream().getColumns();
//  for (std::list<std::string>::iterator it = cols.begin(); it != cols.end(); it++)
    //cerr << "col: " << *it << endl;
  {
    dba::CSVIStream stream = ar.getIStream();
    dba::BindedInt ba(stream.bindInt("test","a"));
    dba::BindedInt bb(stream.bindInt("test","b"));
    dba::BindedString bs(stream.bindString("test","str"));
    CSVBindTester t;
    stream.open(t);
      
    std::list<CSVTester>::iterator it = mResults.begin();
    bool col = true;
    while (stream.getNext(&t)) {
  //    cerr << "rec: [" << (int)ba << "] [" << (int)bb << "] [" << std::string(bs) << "]" << endl;
      if (col) {
        col = false;
      } else {
        CPPUNIT_ASSERT(it != mResults.end());
        CPPUNIT_ASSERT(ba == it->a);
        CPPUNIT_ASSERT(bb == it->b);
        CPPUNIT_ASSERT((std::string)bs == it->str);
      };
      it++;
    };
    CPPUNIT_ASSERT(it == mResults.end());
  };
};

void 
CSVTestCase::spaceSeparator() {
  unlink("csv-auto.csv");
  dba::CSVArchive ar;
  ar.hasColumnNames(false);
  ar.setFieldSeparator(' ');
  ar.open("csv-auto.csv");
  ar.hasColumnNames(true);
  dba::CSVOStream ostream(ar.getOStream());
  ostream.open();
  for(std::list<CSVTester>::iterator it = mResults.begin(); it != mResults.end(); it++) {
    CSVTester& t = *it;
    std::cerr << "rec: " << " [" << t.a << "] [" << t.b << "] [" << t.str << "]" << std::endl;
    ostream.put(&t);
  };
  ostream.destroy();

  ar.hasColumnNames(true);
  ar.open("csv-auto.csv");
  std::list<std::string> cols = ar.getIStream().getColumns();
//  for (list<string>::iterator it = cols.begin(); it != cols.end(); it++)
//    cerr << "col: " << *it << endl;
  dba::CSVIStream istream(ar.getIStream());
  CSVTester t;
  istream.open(t);
    
  std::list<CSVTester>::iterator it = mResults.begin();
  bool col = true;
  while (istream.getNext(&t)) {
    std::cerr << "rec: [" << t.a << "] [" << t.b << "] [" << t.str << "]" << std::endl;
    if (col) {
      col = false;
    } else {
      CPPUNIT_ASSERT(it != mResults.end());
      CPPUNIT_ASSERT(t == *it);
    };
    it++;
  };
  istream.close();
  CPPUNIT_ASSERT(it == mResults.end());
  unlink("csv-auto.csv");
};

void
CSVTestCase::invalidPos() {
  const char* data = 
"1,2";
  { 
    unlink("invalid_pos.csv");
    std::ofstream file("invalid_pos.csv");
    file << data;
  };  
  dba::CSVArchive ar;
  ar.hasColumnNames(false);
  ar.addMapping(0,"test","a");
  ar.addMapping(1,"test","b");
  ar.addMapping(2,"test","str");
  ar.open("invalid_pos.csv");

  std::string binded;

  dba::CSVIStream istream = ar.getIStream();
  istream.bind("test", "str", new dba::String(binded));
  dba::Storeable t;
  istream.open(t);
  try {
    istream.getNext(&t);
    CPPUNIT_ASSERT_MESSAGE("no exception on missing mapping",false);
  } catch (const dba::DataException&) {
    //test passed
  };
};

};//namespace

#endif //TEST_CSV
