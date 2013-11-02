// File: sqlarchive_basic.cpp
// Purpose: Regression tests for SQLArchive
// Author: Lukasz Michalski <lm at zork.pl>, Copyright 2007
// License: See COPYING file that comes with this distribution
//
// This file is a part of debea library (http://debea.net)
//
//
#include "sqlarchive_basic.h"
#include "testobject.h"
#include "dba/string_filter.h"
#include "dba/int_filter.h"
#include "dba/bool_filter.h"
#include "dba/stdlist.h"
#include "dba/conversion.h"
#include "dba/sqlidfetcher.h"

namespace dba_tests {

SQLArchive_Basic::SQLArchive_Basic(const char* pPluginName, const char* pDbParams)
  : SQLArchiveTestSuite(pPluginName,pDbParams)
{
}

void
SQLArchive_Basic::sqlError() {
  try {
    AgregatedObject obj;
    dba::SQLIStream istream = mSQLArchive->getIStream();
    istream.openFromQuery(obj,"bad_query");
    istream.getNext(&obj);
  } catch (const dba::SQLException& pEx) {
    CPPUNIT_ASSERT(pEx.getQuery() == std::string("bad_query"));
    CPPUNIT_ASSERT(pEx.what() != std::string());
  };
};

void
SQLArchive_Basic::noTables() {
  dba::Storeable obj;
  dba::SQLOStream ostream(mSQLArchive->getOStream());
  ostream.put(&obj);
  CPPUNIT_ASSERT(obj.isNew());
};

void
SQLArchive_Basic::noFields() {
  NoFieldsObject in,out;
  { 
    dba::SQLOStream ostream(mSQLArchive->getOStream());
    ostream.put(&out);
    CPPUNIT_ASSERT(out.isOk());
  };
  { 
    dba::SQLIStream istream(mSQLArchive->getIStream());
    istream.get(&in);
    CPPUNIT_ASSERT(in.isOk());
    CPPUNIT_ASSERT(in.getId() == out.getId());
  };
};

void
SQLArchive_Basic::advAgregationTest() {
  DoubleAgregatedObject obj1;
  {
    dba::SQLOStream ostream = mSQLArchive->getOStream();
    ostream.open();
    ostream.put(&obj1);      
  }

  DoubleAgregatedObject obj2(true);
  {
    dba::SQLIStream istream = mSQLArchive->getIStream();
    istream.open(obj2);
    CPPUNIT_ASSERT(istream.getNext(&obj2));
  }
  CPPUNIT_ASSERT(obj1 == obj2);
};

void
SQLArchive_Basic::agregationTest() {
  AgregatedObject obj1;
  {
    dba::SQLOStream ostream = mSQLArchive->getOStream();
    ostream.open();
    ostream.put(&obj1);      
  }

  AgregatedObject obj2(true);
  {
    dba::SQLIStream istream = mSQLArchive->getIStream();
    istream.open(obj2);
    CPPUNIT_ASSERT(istream.getNext(&obj2));
  }
  CPPUNIT_ASSERT(obj1 == obj2);
};

void 
SQLArchive_Basic::unbindTable() {
  try {
    std::list<TestObject> olst;
    dba::SQLOStream ostream = mSQLArchive->getOStream();
    ostream.open();
    tm date(Utils::getNow());
    olst.push_back(TestObject(1,0.1,"1",date));
    olst.push_back(TestObject(2,0.2,"2",date));
    //bad bindings
    dba::BindedInt bi = ostream.bindInt("test_objects","no_field_no_cry");
    dba::BindedString bs = ostream.bindString("test_objects","another_bad_binding_with_good_table");
    dba::BindedString bs2 = ostream.bindString("other_table","field_ok_but_table_is_wrong");
    TestObject obj;
    ostream.open();
    ostream.unbindAll("test_objects");
    for(std::list<TestObject>::iterator it = olst.begin(); it != olst.end(); it++) {
      ostream.put(&(*it));
    };
  } catch (const dba::SQLException&) {
    //if bad bindings were not removed then first put should throw excepiton  
    CPPUNIT_FAIL("got sql exception unbind test failed");
  };
};

void 
SQLArchive_Basic::unbindField() {
  try {
    std::list<TestObject> olst;
    dba::SQLOStream ostream = mSQLArchive->getOStream();
    ostream.open();
    tm date(Utils::getNow());
    olst.push_back(TestObject(1,0.1,"1",date));
    olst.push_back(TestObject(2,0.2,"2",date));
    //bad bindings
    dba::BindedInt bi = ostream.bindInt("test_objects","no_field_no_cry");
    dba::BindedString bs = ostream.bindString("test_objects","bindedstring");
    bs = "test";
    TestObject obj;
    ostream.open();
    ostream.unbind("test_objects","no_field_no_cry");
    for(std::list<TestObject>::iterator it = olst.begin(); it != olst.end(); it++) {
      ostream.put(&(*it));
    };
  } catch (const dba::SQLException&) {
    //if bad bindings were not removed then first put should throw excepiton  
    CPPUNIT_FAIL("got sql exception unbind test failed");
  };
};

void
SQLArchive_Basic::MIObjectTest() {
  MIObject obj1(1,"a",2,"b");
  {
    dba::SQLOStream ostream = mSQLArchive->getOStream();
    ostream.open();
    ostream.put(&obj1);
  }

  MIObject obj2;
  {
    dba::SQLIStream istream = mSQLArchive->getIStream();
    istream.open(obj2);
    CPPUNIT_ASSERT(istream.getNext(&obj2));
  }
  CPPUNIT_ASSERT(obj1 == obj2);
};

void
SQLArchive_Basic::MIAdvObjectTest() {
  MIAdvObject obj1(1,"a",2,"b",3,"c",4,"d");
  {
    dba::SQLOStream ostream = mSQLArchive->getOStream();
    ostream.open();
    ostream.put(&obj1);
  }

  MIAdvObject obj2;
  {
    dba::SQLIStream istream = mSQLArchive->getIStream();
    istream.open(obj2);
    CPPUNIT_ASSERT(istream.getNext(&obj2));
  }
  CPPUNIT_ASSERT(obj1 == obj2);
};

void
SQLArchive_Basic::storeEnum() {
  EnumObject obj1(EnumObject::ENUM_THREE);
  {
    dba::SQLOStream ostream = mSQLArchive->getOStream();
    ostream.open();
    ostream.put(&obj1);      
  }

  EnumObject obj2;
  {
    dba::SQLIStream istream = mSQLArchive->getIStream();
    istream.open(obj2);
    CPPUNIT_ASSERT(istream.getNext(&obj2));
  }
  CPPUNIT_ASSERT(obj1 == obj2);
};

void
SQLArchive_Basic::bindVar() {
  struct tm date(Utils::getDate(2007,1,1,20,12,12));
  TestObject t1(1,1,"str",date);
  std::string out_val = "test_val";
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.bind("test_objects","bindedstring",new dba::String(out_val));
  ostream.open();
  ostream.put(&t1);
  ostream.unbindAll();
  ostream.destroy();
  
  std::string in_val("");
  dba::SQLIStream istream = mSQLArchive->getIStream();
  istream.bind("test_objects","bindedstring",new dba::String(in_val));
  istream.open(t1);
  istream.getNext(&t1);
  istream.unbindAll();
  
  CPPUNIT_ASSERT(in_val == out_val);
};

void
SQLArchive_Basic::bind_var_with_custom_table() {
  struct tm date(Utils::getDate(2007,1,1,20,12,12));
  NullEmptyTestObject t1(1,1,"str",date);
  std::string out_val = "test_val";
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.bind("test_objects","bindedstring",new dba::String(out_val));
  ostream.open("test_objects_only");
  ostream.put(&t1);
  ostream.unbindAll();
  ostream.destroy();
  
  std::string in_val("");
  dba::SQLIStream istream = mSQLArchive->getIStream();
  istream.bind("test_objects","bindedstring",new dba::String(in_val));
  istream.open(t1,"test_objects_only");
  istream.getNext(&t1);
  istream.unbindAll();
  
  CPPUNIT_ASSERT(in_val == out_val);
};

class GenericMultiFetcher : public dba::SQLIdFetcher { 
  public: 
    GenericMultiFetcher() {}; 

    virtual int getNextId(dba::DbConnection& pConn, const char* pRootTableName = NULL) { 
      if(pRootTableName != NULL) { 
        std::string str_table(pRootTableName); 
        std::auto_ptr<dba::DbResult> res(pConn.sendQuery("SELECT id FROM debea_object_count WHERE root_table = '" + str_table + "'")); 
        int id = 1; 
        if (!res->fetchRow()) {
          pConn.sendUpdate("INSERT INTO debea_object_count(root_table,id) VALUES('" + str_table + "',1)"); 
          return 1;
        } else {
          id = res->getInt(0); 
        }
        res.reset(); 
        pConn.sendUpdate("UPDATE debea_object_count SET id = id + 1 WHERE root_table = '" + str_table + "'"); 
        return id; 
      } else { 
        throw dba::DataException("Unable to allocate next object id"); 
      };
  } 
  virtual ~GenericMultiFetcher() {}; 
};

void
SQLArchive_Basic::multifetcher_inheritance() {
  InheritedObject i;
  
  mSQLArchive->setIdFetcher(new GenericMultiFetcher);

  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  ostream.put(&i);
  ostream.destroy();

  std::auto_ptr<dba::DbResult>res(mSQLArchive->getIStream().sendQuery("SELECT id FROM debea_object_count WHERE root_table='test_inherited'"));
  CPPUNIT_ASSERT(res->fetchRow());
  int ret = res->getInt(0);
  CPPUNIT_ASSERT(ret == 1);
}

void
SQLArchive_Basic::multifetcher_forced_root_table() {
  NullEmptyTestObject i;
  
  mSQLArchive->setIdFetcher(new GenericMultiFetcher);

  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open("test_objects");
  ostream.put(&i);
  ostream.destroy();

  std::auto_ptr<dba::DbResult>res(mSQLArchive->getIStream().sendQuery("SELECT id FROM debea_object_count WHERE root_table='test_objects'"));
  CPPUNIT_ASSERT(res->fetchRow());
  int ret = res->getInt(0);
  CPPUNIT_ASSERT(ret == 1);
}


void
SQLArchive_Basic::sublist_store() {
  ObjWithList t1("obj",1);
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  ostream.put(&t1);
  ostream.destroy();

  dba::SQLIStream istream = mSQLArchive->getIStream();
  std::string query("SELECT count(*) FROM test_objects WHERE fk_owner=" + dba::toStr(t1.getId()));
  std::auto_ptr<dba::DbResult> res(istream.sendQuery(query.c_str()));
  res->fetchRow();

  CPPUNIT_ASSERT(res->getInt(0) == 1);
};

void
SQLArchive_Basic::sublist_erase() {
  ObjWithList t1("obj",1);
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  ostream.put(&t1);

  t1.mList.clear();
  ostream.put(&t1);
  ostream.destroy();

  dba::SQLIStream istream = mSQLArchive->getIStream();
  std::string query("SELECT count(*) FROM test_objects WHERE fk_owner=" + dba::toStr(t1.getId()));
  std::auto_ptr<dba::DbResult> res(istream.sendQuery(query.c_str()));
  res->fetchRow();

  CPPUNIT_ASSERT_MESSAGE("TestObject on sublist was not deleted",res->getInt(0) == 0);
};

void
SQLArchive_Basic::sublist_store_3() {
  ThreeLists t1("obj",1,1);
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  ostream.put(&t1);
  ostream.destroy();

  dba::SQLIStream istream = mSQLArchive->getIStream();
  std::string query1("SELECT count(*) FROM obj_with_list WHERE fk_three_owner=" + dba::toStr(t1.getId()));
  std::auto_ptr<dba::DbResult> res1(istream.sendQuery(query1.c_str()));
  res1->fetchRow();
  CPPUNIT_ASSERT_MESSAGE("No object stored on second level",res1->getInt(0) == 1);
  res1.reset();

  std::string query2("SELECT count(*) FROM test_objects WHERE fk_owner=(SELECT id FROM obj_with_list WHERE fk_three_owner=" + dba::toStr(t1.getId()));
  query2 += ")";
  std::auto_ptr<dba::DbResult> res2(istream.sendQuery(query2.c_str()));
  res2->fetchRow();
  CPPUNIT_ASSERT_MESSAGE("no object stored on third level",res2->getInt(0) == 1);
};

void
SQLArchive_Basic::sublist_store_4() {
  StoreTest4Top c;
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  CPPUNIT_ASSERT(ostream.put(&c));
  ostream.destroy();

  std::list<StoreTest4Middle>::const_iterator mt = c.mBList.begin();
  CPPUNIT_ASSERT(mt->getState()==dba::Storeable::OK);

  std::list<NoFieldsObject>::const_iterator it = c.mBList.front().mAList.begin();
  CPPUNIT_ASSERT(it->getState()==dba::Storeable::OK);
};

void
SQLArchive_Basic::sublist_erase_3() {
  ThreeLists t1("obj",1,1);
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  ostream.put(&t1);

  t1.mList.clear();
  ostream.put(&t1);
  ostream.destroy();

  dba::SQLIStream istream = mSQLArchive->getIStream();
  std::string query1("SELECT count(*) FROM obj_with_list");
  std::auto_ptr<dba::DbResult> res1(istream.sendQuery(query1.c_str()));
  res1->fetchRow();
  CPPUNIT_ASSERT_MESSAGE("Object on second level not erased",res1->getInt(0) == 0);
  res1.reset();

  std::string query2("SELECT count(*) FROM test_objects");
  std::auto_ptr<dba::DbResult> res2(istream.sendQuery(query2.c_str()));
  res2->fetchRow();
  CPPUNIT_ASSERT_MESSAGE("Object on third level not erased",res2->getInt(0) == 0);
};

void
SQLArchive_Basic::sublist_erase_3_sec_deleted() {
  ThreeLists t1("obj",1,1);
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  ostream.put(&t1);

  t1.mList.begin()->setDeleted();
  ostream.put(&t1);
  ostream.destroy();

  dba::SQLIStream istream = mSQLArchive->getIStream();
  std::string query1("SELECT count(*) FROM obj_with_list");
  std::auto_ptr<dba::DbResult> res1(istream.sendQuery(query1.c_str()));
  res1->fetchRow();
  CPPUNIT_ASSERT_MESSAGE("Object on second level not erased",res1->getInt(0) == 0);
  res1.reset();

  std::string query2("SELECT count(*) FROM test_objects");
  std::auto_ptr<dba::DbResult> res2(istream.sendQuery(query2.c_str()));
  res2->fetchRow();
  CPPUNIT_ASSERT_MESSAGE("Object on third level not erased",res2->getInt(0) == 0);
};

void
SQLArchive_Basic::sublist_erase_one_from_two() {
  ObjWithList t1("obj",1);
  ObjWithList t2("for_erase",1);
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  ostream.put(&t1);
  ostream.put(&t2);
  t2.setDeleted();
  ostream.put(&t2);
  ostream.destroy();

  dba::SQLIStream istream = mSQLArchive->getIStream();
  std::string query("SELECT count(*) FROM test_objects");
  std::auto_ptr<dba::DbResult> res(istream.sendQuery(query.c_str()));
  res->fetchRow();

  CPPUNIT_ASSERT(res->getInt(0) == 1);
};

void
SQLArchive_Basic::sublist_store_inherited() {
  ObjWithInheritedList t1("obj",1);
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  ostream.put(&t1);
  ostream.destroy();

  dba::SQLIStream istream = mSQLArchive->getIStream();
  std::string query("SELECT count(*) FROM test_objects");
  std::auto_ptr<dba::DbResult> res1(istream.sendQuery(query.c_str()));
  res1->fetchRow();
  CPPUNIT_ASSERT(res1->getInt(0) == 1);
  res1.reset();

  query = ("SELECT count(*) FROM test_inherited WHERE fk_owner=" + dba::toStr(t1.getId()));
  std::auto_ptr<dba::DbResult> res2(istream.sendQuery(query.c_str()));
  res2->fetchRow();
  CPPUNIT_ASSERT(res2->getInt(0) == 1);
};

void
SQLArchive_Basic::sublist_erase_inherited() {
  ObjWithInheritedList t1("obj",1);
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  ostream.put(&t1);

  t1.mList.clear();
  ostream.put(&t1);
  ostream.destroy();

  dba::SQLIStream istream = mSQLArchive->getIStream();
  std::string query("SELECT count(*) FROM test_objects");
  std::auto_ptr<dba::DbResult> res(istream.sendQuery(query.c_str()));
  res->fetchRow();
  CPPUNIT_ASSERT_MESSAGE("TestObject on sublist was not deleted",res->getInt(0) == 0);

  query = ("SELECT count(*) FROM test_inherited");
  res.reset(istream.sendQuery(query.c_str()));
  res->fetchRow();
  CPPUNIT_ASSERT_MESSAGE("InheritedObject on sublist was not deleted",res->getInt(0) == 0);
};

void
SQLArchive_Basic::sublist_load() {
  ObjWithList t1("obj",1);
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  ostream.put(&t1);
  ostream.destroy();

  ObjWithList t2;
  dba::SQLIStream istream = mSQLArchive->getIStream();
  istream.get(&t2);

  CPPUNIT_ASSERT(t1 == t2);
};

void
SQLArchive_Basic::sublist_load_bug3024() {
  ObjWithList t1("obj",1);
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  ostream.put(&t1);
  ostream.destroy();

  ObjWithList t2("obj",1);
  dba::SQLOStream ostream2 = mSQLArchive->getOStream();
  ostream2.open();
  ostream2.put(&t2);
  ostream2.destroy();

  try {
    ObjWithList t3;
    dba::SQLIStream istream = mSQLArchive->getIStream();
    istream.open(t3);
    istream.get(&t3); // exception caused by open() before get().
  } catch (const dba::APIException& pEx) {
    CPPUNIT_ASSERT(std::string(pEx.what()) == std::string("IStream must be closed when get is called"));
  };
};

void
SQLArchive_Basic::sublist_load_3() {
  ThreeLists t1("obj",1,1);
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  ostream.put(&t1);
  ostream.destroy();

  ThreeLists t2;
  dba::SQLIStream istream = mSQLArchive->getIStream();
  istream.get(&t2);

  CPPUNIT_ASSERT(t1 == t2);
};

void
SQLArchive_Basic::sublist_load_dirty() {
  ObjWithList t1("obj",1);
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  ostream.put(&t1);
  ostream.destroy();

  ObjWithList t2("test",13);
  dba::SQLIStream istream = mSQLArchive->getIStream();
  istream.get(&t2);

  CPPUNIT_ASSERT(t1 == t2);
};

void
SQLArchive_Basic::sublist_load_two_on_sublist() {
  ObjWithList t1("obj",2);
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  ostream.put(&t1);
  ostream.destroy();

  ObjWithList t2;
  dba::SQLIStream istream = mSQLArchive->getIStream();
  istream.get(&t2);

  CPPUNIT_ASSERT(t1 == t2);
};

void
SQLArchive_Basic::sublist_load_3_with_two() {
  ThreeLists t1("obj",2,2);
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  ostream.put(&t1);
  ostream.destroy();

  ThreeLists t2;
  dba::SQLIStream istream = mSQLArchive->getIStream();
  istream.get(&t2);

  CPPUNIT_ASSERT(t1 == t2);
};

void
SQLArchive_Basic::sublist_load_list() {
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  ObjWithList t1("obj1",1);
  ostream.put(&t1);
  ObjWithList t2("obj2",2);
  ostream.put(&t2);
  ObjWithList t3("obj3",3);
  ostream.put(&t3);
  ostream.destroy();

  std::list<ObjWithList> lst;
  dba::SQLIStream istream = mSQLArchive->getIStream();
  dba::stdList<ObjWithList> filter(lst);
  istream.get(&filter);

  CPPUNIT_ASSERT(lst.size() == 3);
};

void
SQLArchive_Basic::sublist_load_one() {
  dba::SQLOStream ostream(mSQLArchive->getOStream());
  ostream.open();
  ObjWithList t1("obj1",1);
  ostream.put(&t1);
  ObjWithList t2("obj2",2);
  ostream.put(&t2);
  ObjWithList t3("obj3",3);
  ostream.put(&t3);
  ostream.destroy();

  ObjWithList in;
  dba::SQLIStream istream(mSQLArchive->getIStream());
  istream.setWhereId(t2.getId());
  istream.get(&in);

  CPPUNIT_ASSERT(in == t2);
};

void
SQLArchive_Basic::sublist_load_one_where() {
  dba::SQLOStream ostream(mSQLArchive->getOStream());
  ostream.open();
  ObjWithList t1("obj1",1);
  ostream.put(&t1);
  ObjWithList t2("obj2",2);
  ostream.put(&t2);
  ObjWithList t3("obj3",3);
  ostream.put(&t3);
  ostream.destroy();

  ObjWithList in;
  dba::SQLIStream istream = mSQLArchive->getIStream();
  istream.setWherePart("id = 1");
  istream.get(&in);

  CPPUNIT_ASSERT(in == t1);
};


void
SQLArchive_Basic::sublist_store_tree() {
  TreeObject t1(1,1,"root");
  TreeObject leaf(2,2,"2leaf");
  leaf.mLeafs.push_back(TreeObject(3,3,"3leaf"));
  t1.mLeafs.push_back(leaf);
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  ostream.put(&t1);
  ostream.destroy();

  dba::SQLIStream istream = mSQLArchive->getIStream();
  //every foreign key should have one reference
  std::string query("select fk_owner,count(*) from test_objects where fk_owner is not null group by fk_owner");
  std::auto_ptr<dba::DbResult> res(istream.sendQuery(query.c_str()));
  int count = 0;
  while (res->fetchRow()) {
    CPPUNIT_ASSERT(res->getInt(1) == 1);
    count++;
  };
  CPPUNIT_ASSERT(count == 2);
};

void
SQLArchive_Basic::sublist_load_tree() {
  TreeObject t1(1,1,"root");
  TreeObject leaf(2,2,"2leaf");
  leaf.mLeafs.push_back(TreeObject(3,3,"3leaf"));
  t1.mLeafs.push_back(leaf);
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  ostream.put(&t1);
  ostream.destroy();

  TreeObject result;
  dba::SQLIStream istream = mSQLArchive->getIStream();
  istream.setWhereId(1);
  istream.get(&result);
  
  CPPUNIT_ASSERT(t1 == result);
};

void
SQLArchive_Basic::sublist_custom_table_store() {
  ObjWithCustomTableList t1("obj",1);
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  ostream.put(&t1);
  ostream.destroy();

  dba::SQLIStream istream = mSQLArchive->getIStream();
  std::string query("SELECT count(*) FROM test_objects_only WHERE fk_owner=" + dba::toStr(t1.getId()));
  std::auto_ptr<dba::DbResult> res(istream.sendQuery(query.c_str()));
  res->fetchRow();

  CPPUNIT_ASSERT(res->getInt(0) == 1);
};

void
SQLArchive_Basic::sublist_custom_table_load() {
  ObjWithCustomTableList t1("obj",1);
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  ostream.put(&t1);
  ostream.destroy();

  ObjWithCustomTableList result;
  dba::SQLIStream istream = mSQLArchive->getIStream();
  istream.get(&result);

  CPPUNIT_ASSERT(t1 == result);
};

void
SQLArchive_Basic::sublist_coll_id_store() {
  TwoSubobjects t1("obj");
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  ostream.put(&t1);
  ostream.destroy();

  dba::SQLIStream istream = mSQLArchive->getIStream();
  std::string query("SELECT dba_coll_id FROM test_objects ORDER BY dba_coll_id");
  std::auto_ptr<dba::DbResult> res(istream.sendQuery(query.c_str()));
  if (!res->fetchRow()) {
    CPPUNIT_FAIL("subobjects not stored");  
  };
  CPPUNIT_ASSERT_MESSAGE("collection id wrong for first subobject",res->getInt(0) == 1);
  
  if (!res->fetchRow()) {
    CPPUNIT_FAIL("second subobject not stored");  
  };
  CPPUNIT_ASSERT_MESSAGE("collection id wrong for second subobject",res->getInt(0) == 2);
};

void
SQLArchive_Basic::sublist_coll_id_load() {
  TwoSubobjects t1("obj");
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  ostream.put(&t1);
  ostream.destroy();

  TwoSubobjects result;
  dba::SQLIStream istream = mSQLArchive->getIStream();
  istream.get(&result);

  CPPUNIT_ASSERT(t1 == result);
};

void
SQLArchive_Basic::stdset_store() {
  ObjWithSet t1("obj",1);
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  ostream.put(&t1);
  ostream.destroy();

  dba::SQLIStream istream = mSQLArchive->getIStream();
  std::string query("SELECT count(*) FROM test_objects WHERE fk_owner=" + dba::toStr(t1.getId()));
  std::auto_ptr<dba::DbResult> res(istream.sendQuery(query.c_str()));
  res->fetchRow();

  CPPUNIT_ASSERT(res->getInt(0) == 1);
};

void
SQLArchive_Basic::stdvector_store() {
  ObjWithVector t1("obj",1);
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  ostream.put(&t1);
  ostream.destroy();

  dba::SQLIStream istream = mSQLArchive->getIStream();
  std::string query("SELECT count(*) FROM test_objects WHERE fk_owner=" + dba::toStr(t1.getId()));
  std::auto_ptr<dba::DbResult> res(istream.sendQuery(query.c_str()));
  res->fetchRow();

  CPPUNIT_ASSERT(res->getInt(0) == 1);
};

void
SQLArchive_Basic::stddeque_store() {
  ObjWithDeque t1("obj",1);
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  ostream.put(&t1);
  ostream.destroy();

  dba::SQLIStream istream = mSQLArchive->getIStream();
  std::string query("SELECT count(*) FROM test_objects WHERE fk_owner=" + dba::toStr(t1.getId()));
  std::auto_ptr<dba::DbResult> res(istream.sendQuery(query.c_str()));
  res->fetchRow();

  CPPUNIT_ASSERT(res->getInt(0) == 1);
};

void
SQLArchive_Basic::stdmultiset_store() {
  ObjWithMultiset t1("obj",1);
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  ostream.put(&t1);
  ostream.destroy();

  dba::SQLIStream istream = mSQLArchive->getIStream();
  std::string query("SELECT count(*) FROM test_objects WHERE fk_owner=" + dba::toStr(t1.getId()));
  std::auto_ptr<dba::DbResult> res(istream.sendQuery(query.c_str()));
  res->fetchRow();

  CPPUNIT_ASSERT(res->getInt(0) == 1);
};

// class STLArgsObject : public dba::Storeable {
//     DECLARE_STORE_TABLE()
//   public:
//     std::set<char,std::greater<char>,std::allocator<char> > mSet;
// };
// 
// BEGIN_STORE_TABLE(STLArgsObject,dba::Storeable,"STLArgsObject")
//   BIND_COL(STLArgsObject::mSet, stdSet<char,std::greater<char>,std::allocator<char> >, "set")
// END_STORE_TABLE()
// 
// void
// SQLArchive_Basic::stl_args() {
//   STLArgsObject o;
// };


void 
SQLArchive_Basic::convspec_change() {
  //one connection is created already in setUp for recreating SQL Schema.
  //This connection has default encoding and here we want to change it to UTF7
  dba::ConvSpec specs(mSQLArchive->getConversionSpecs());
  specs.mDbCharset = dba::ConvSpec::UTF7;
  mSQLArchive->setConversionSpecs(specs);  
  
  //created stream will use already existing connection
  //created earlier than change above
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  CPPUNIT_ASSERT(ostream.getConversionSpecs().mDbCharset == dba::ConvSpec::UTF7);
};

void 
SQLArchive_Basic::nullMainTable_insert() {
  NullTestObject obj;
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open("null_objects");
  ostream.put(&obj);
  ostream.destroy();

  dba::SQLIStream istream = mSQLArchive->getIStream();
  std::string query("SELECT count(*) FROM null_objects");
  std::auto_ptr<dba::DbResult> res(istream.sendQuery(query.c_str()));
  CPPUNIT_ASSERT(res->fetchRow());
  CPPUNIT_ASSERT_MESSAGE("TestObject was not added",res->getInt(0) == 1);

  query = ("SELECT count(*) FROM test_objects");
  res.reset(istream.sendQuery(query.c_str()));
  res->fetchRow();
  CPPUNIT_ASSERT_MESSAGE("DoubleNullTestObject was not fully added",res->getInt(0) == 1);
};

void 
SQLArchive_Basic::nullMainTable_temp_insert() {
  NullTestObject obj;
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open("null_objects");
  ostream.put(&obj);

  TestObject t;
  ostream.open();
  ostream.put(&t);

  ostream.destroy();

  dba::SQLIStream istream = mSQLArchive->getIStream();
  std::string query("SELECT count(*) FROM test_objects");
  std::auto_ptr<dba::DbResult> res(istream.sendQuery(query.c_str()));
  CPPUNIT_ASSERT(res->fetchRow());
  CPPUNIT_ASSERT_MESSAGE("TestObject was not added",res->getInt(0) == 2);
};


void 
SQLArchive_Basic::nullMainTable_change() {
  NullTestObject obj;
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open("null_objects");
  ostream.put(&obj);

  obj.val = 2;
  obj.i = 20;
  obj.setChanged();
  ostream.open("null_objects");
  ostream.put(&obj);

  ostream.destroy();

  dba::SQLIStream istream = mSQLArchive->getIStream();
  std::string query("SELECT val FROM null_objects");
  std::auto_ptr<dba::DbResult> res(istream.sendQuery(query.c_str()));
  CPPUNIT_ASSERT(res->fetchRow());
  CPPUNIT_ASSERT_MESSAGE("NullTestObject was not updated",res->getInt(0) == 2);

  query = ("SELECT i_value FROM test_objects");
  res.reset(istream.sendQuery(query.c_str()));
  res->fetchRow();
  CPPUNIT_ASSERT_MESSAGE("NullTestObject base class was not fully updated",res->getInt(0) == 20);
};

void 
SQLArchive_Basic::nullMainTable_erase() {
  NullTestObject obj;
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open("null_objects");
  ostream.put(&obj);

  obj.setDeleted();
  ostream.put(&obj);

  ostream.destroy();

  dba::SQLIStream istream = mSQLArchive->getIStream();
  std::string query("SELECT count(*) FROM null_objects");
  std::auto_ptr<dba::DbResult> res(istream.sendQuery(query.c_str()));
  CPPUNIT_ASSERT(res->fetchRow());
  CPPUNIT_ASSERT_MESSAGE("TestObject was not erased",res->getInt(0) == 0);

  query = ("SELECT count(*) FROM test_objects");
  res.reset(istream.sendQuery(query.c_str()));
  res->fetchRow();
  CPPUNIT_ASSERT_MESSAGE("DoubleNullTestObject was not fully erased",res->getInt(0) == 0);
};


void 
SQLArchive_Basic::customMainTable_insert() {
  TestObject obj;
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open("test_objects_only");
  ostream.put(&obj);
  ostream.destroy();

  dba::SQLIStream istream = mSQLArchive->getIStream();
  std::string query("SELECT count(*) FROM test_objects_only");
  std::auto_ptr<dba::DbResult> res(istream.sendQuery(query.c_str()));
  CPPUNIT_ASSERT(res->fetchRow());
  CPPUNIT_ASSERT_MESSAGE("TestObject was not added",res->getInt(0) == 1);
};

void 
SQLArchive_Basic::customMainTable_temp_insert() {
  TestObject obj;
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open("test_objects_only");
  ostream.put(&obj);

  TestObject t;
  ostream.open();
  ostream.put(&t);

  ostream.destroy();

  dba::SQLIStream istream = mSQLArchive->getIStream();
  std::string query("SELECT count(*) FROM test_objects");
  std::auto_ptr<dba::DbResult> res(istream.sendQuery(query.c_str()));
  CPPUNIT_ASSERT(res->fetchRow());
  CPPUNIT_ASSERT_MESSAGE("TestObject was not added",res->getInt(0) == 1);
};


void 
SQLArchive_Basic::customMainTable_erase() {
  TestObject obj;
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open("test_objects_only");
  ostream.put(&obj);

  obj.setDeleted();
  ostream.put(&obj);

  ostream.destroy();

  dba::SQLIStream istream = mSQLArchive->getIStream();
  std::string query("SELECT count(*) FROM test_objects_only");
  std::auto_ptr<dba::DbResult> res(istream.sendQuery(query.c_str()));
  CPPUNIT_ASSERT(res->fetchRow());
  CPPUNIT_ASSERT_MESSAGE("TestObject was not erased",res->getInt(0) == 0);
};

void 
SQLArchive_Basic::rootTable_insert() {
  NullTestObject obj1, obj2;
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  ostream.setRootTable("null_objects");
  ostream.put(&obj1);
  ostream.put(&obj2);
  ostream.destroy();

  dba::SQLIStream istream = mSQLArchive->getIStream();
  std::string query("SELECT count(*) FROM null_objects");
  std::auto_ptr<dba::DbResult> res(istream.sendQuery(query.c_str()));
  CPPUNIT_ASSERT(res->fetchRow());
  CPPUNIT_ASSERT_MESSAGE("TestObject was not added",res->getInt(0) == 2);

  query = ("SELECT count(*) FROM test_objects");
  res.reset(istream.sendQuery(query.c_str()));
  res->fetchRow();
  CPPUNIT_ASSERT_MESSAGE("DoubleNullTestObject was not fully added",res->getInt(0) == 2);
};

void 
SQLArchive_Basic::rootTable_change() {
  NullTestObject obj1;
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  ostream.setRootTable("null_objects");
  ostream.put(&obj1);

  obj1.val = 2;
  obj1.setChanged();
  ostream.put(&obj1);

  ostream.destroy();

  dba::SQLIStream istream = mSQLArchive->getIStream();
  std::string query("SELECT val FROM null_objects");
  std::auto_ptr<dba::DbResult> res(istream.sendQuery(query.c_str()));
  CPPUNIT_ASSERT(res->fetchRow());
  CPPUNIT_ASSERT_MESSAGE("NullTestObject was not updated",res->getInt(0) == 2);
};

void 
SQLArchive_Basic::rootTable_erase() {
  NullTestObject obj1, obj2;
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  ostream.setRootTable("null_objects");
  ostream.put(&obj1);

  obj2.setId(obj1.getId());
  obj2.setDeleted();
  ostream.put(&obj2);

  ostream.destroy();

  dba::SQLIStream istream = mSQLArchive->getIStream();
  std::string query("SELECT count(*) FROM null_objects");
  std::auto_ptr<dba::DbResult> res(istream.sendQuery(query.c_str()));
  CPPUNIT_ASSERT(res->fetchRow());
  CPPUNIT_ASSERT_MESSAGE("NullTestObject was not erased",res->getInt(0) == 0);

  query = ("SELECT count(*) FROM test_objects");
  res.reset(istream.sendQuery(query.c_str()));
  res->fetchRow();
  CPPUNIT_ASSERT_MESSAGE("DoubleNullTestObject was not fully erased",res->getInt(0) == 0);
};


void 
SQLArchive_Basic::rootAndCustomTable_insert() {
  TestObject obj1, obj2;
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  ostream.setRootTable("test_objects_only");
  ostream.put(&obj1);
  ostream.put(&obj2);
  ostream.destroy();

  dba::SQLIStream istream = mSQLArchive->getIStream();
  std::string query("SELECT count(*) FROM test_objects_only");
  std::auto_ptr<dba::DbResult> res(istream.sendQuery(query.c_str()));
  CPPUNIT_ASSERT(res->fetchRow());
  CPPUNIT_ASSERT_MESSAGE("TestObject was not added",res->getInt(0) == 2);
};

void 
SQLArchive_Basic::rootAndCustomTable_change() {
  TestObject obj1;
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  ostream.setRootTable("test_objects_only");
  ostream.put(&obj1);

  obj1.i = 2;
  obj1.setChanged();
  ostream.put(&obj1);

  ostream.destroy();

  dba::SQLIStream istream = mSQLArchive->getIStream();
  std::string query("SELECT i_value FROM test_objects_only");
  std::auto_ptr<dba::DbResult> res(istream.sendQuery(query.c_str()));
  CPPUNIT_ASSERT(res->fetchRow());
  CPPUNIT_ASSERT_MESSAGE("NullTestObject was not added",res->getInt(0) == 2);
};

void 
SQLArchive_Basic::rootAndCustomTable_erase() {
  TestObject obj1, obj2;
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  ostream.setRootTable("test_objects_only");
  ostream.put(&obj1);

  obj2.setId(obj1.getId());
  obj2.setDeleted();
  ostream.put(&obj2);

  ostream.destroy();

  dba::SQLIStream istream = mSQLArchive->getIStream();
  std::string query("SELECT count(*) FROM test_objects_only");
  std::auto_ptr<dba::DbResult> res(istream.sendQuery(query.c_str()));
  CPPUNIT_ASSERT(res->fetchRow());
  CPPUNIT_ASSERT_MESSAGE("NullTestObject was not erased",res->getInt(0) == 0);
};

void 
SQLArchive_Basic::doubleNullObject_insert() {
  DoubleNullTestObject obj1;
  obj1.i = 1;
  obj1.val = 10;
  obj1.topint = 100;
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open("double_null_objects");
  ostream.put(&obj1);
  ostream.destroy();

  dba::SQLIStream istream = mSQLArchive->getIStream();
  std::string query("SELECT dnto_int FROM double_null_objects");
  std::auto_ptr<dba::DbResult> res(istream.sendQuery(query.c_str()));
  CPPUNIT_ASSERT(res->fetchRow());
  CPPUNIT_ASSERT_MESSAGE("DoubleNullTestObject was not fully stored",res->getInt(0) == 100);

  query = ("SELECT i_value FROM test_objects");
  res.reset(istream.sendQuery(query.c_str()));
  res->fetchRow();
  CPPUNIT_ASSERT_MESSAGE("DoubleNullTestObject was not fully added",res->getInt(0) == 1);

};

void 
SQLArchive_Basic::doubleNullObject_update() {
  DoubleNullTestObject obj1;
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open("double_null_objects");
  ostream.put(&obj1);
  obj1.topint = 10;
  obj1.setChanged();
  ostream.put(&obj1);
  ostream.destroy();

  dba::SQLIStream istream = mSQLArchive->getIStream();
  std::string query("SELECT dnto_int FROM double_null_objects");
  std::auto_ptr<dba::DbResult> res(istream.sendQuery(query.c_str()));
  CPPUNIT_ASSERT(res->fetchRow());
  CPPUNIT_ASSERT_MESSAGE("DoubleNullTestObject was not fully stored",res->getInt(0) == 10);
};

void 
SQLArchive_Basic::doubleNullObject_erase() {
  DoubleNullTestObject obj1;
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open("double_null_objects");
  ostream.put(&obj1);
  obj1.setDeleted();
  ostream.put(&obj1);
  ostream.destroy();

  dba::SQLIStream istream = mSQLArchive->getIStream();
  std::string query("SELECT count(*) FROM double_null_objects");
  std::auto_ptr<dba::DbResult> res(istream.sendQuery(query.c_str()));
  CPPUNIT_ASSERT(res->fetchRow());
  CPPUNIT_ASSERT_MESSAGE("DoubleNullTestObject was not erased",res->getInt(0) == 0);

  query = ("SELECT count(*) FROM test_objects");
  res.reset(istream.sendQuery(query.c_str()));
  res->fetchRow();
  CPPUNIT_ASSERT_MESSAGE("DoubleNullTestObject was not fully erased",res->getInt(0) == 0);
};

void 
SQLArchive_Basic::middleNullObject_insert() {
  MiddleNullObject obj1;
  obj1.val = 10;
  obj1.mno_val = 100;
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  ostream.put(&obj1);
  ostream.destroy();

  dba::SQLIStream istream = mSQLArchive->getIStream();
  std::string query("SELECT val,mno_val FROM middle_null_objects");
  std::auto_ptr<dba::DbResult> res(istream.sendQuery(query.c_str()));
  CPPUNIT_ASSERT(res->fetchRow());
  CPPUNIT_ASSERT_MESSAGE("DoubleNullTestObject was not fully stored",res->getInt(0) == 10);
  CPPUNIT_ASSERT_MESSAGE("DoubleNullTestObject was not fully stored",res->getInt(1) == 100);

  query = ("SELECT count(*) FROM test_objects");
  res.reset(istream.sendQuery(query.c_str()));
  res->fetchRow();
  CPPUNIT_ASSERT_MESSAGE("DoubleNullTestObject was not fully added",res->getInt(0) == 1);
};

void 
SQLArchive_Basic::middleNullObject_update() {
  MiddleNullObject obj1;
  obj1.val = 1;
  obj1.mno_val = 10;
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  ostream.put(&obj1);

  obj1.val = 10;
  obj1.mno_val = 100;
  obj1.setChanged();
  ostream.put(&obj1);
  ostream.destroy();

  dba::SQLIStream istream = mSQLArchive->getIStream();
  std::string query("SELECT val,mno_val FROM middle_null_objects");
  std::auto_ptr<dba::DbResult> res(istream.sendQuery(query.c_str()));
  CPPUNIT_ASSERT(res->fetchRow());
  CPPUNIT_ASSERT_MESSAGE("MiddleNullObject was not fully stored",res->getInt(0) == 10);
  CPPUNIT_ASSERT_MESSAGE("MiddleNullObject was not fully stored",res->getInt(1) == 100);
};

void 
SQLArchive_Basic::middleNullObject_erase() {
  MiddleNullObject obj1;
  obj1.val = 1;
  obj1.mno_val = 10;
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  ostream.put(&obj1);

  obj1.setDeleted();
  ostream.put(&obj1);
  ostream.destroy();

  dba::SQLIStream istream = mSQLArchive->getIStream();
  std::string query("SELECT count(*) FROM test_objects");
  std::auto_ptr<dba::DbResult> res(istream.sendQuery(query.c_str()));
  res->fetchRow();
  CPPUNIT_ASSERT_MESSAGE("MiddleNullObject was not fully erased",res->getInt(0) == 0);

  query = ("SELECT count(*) FROM middle_null_objects");
  res.reset(istream.sendQuery(query.c_str()));
  res->fetchRow();
  CPPUNIT_ASSERT_MESSAGE("MiddleNullObject was not fully erased",res->getInt(0) == 0);
};

void 
SQLArchive_Basic::inheritedObject_insert() {
  InheritedObject obj1(10,10,"inheritedObject_insert",dba::DbResult::sInvalidTm);
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  ostream.put(&obj1);
  ostream.destroy();

  dba::SQLIStream istream = mSQLArchive->getIStream();
  InheritedObject obj2;
  istream.open(obj2);
  istream.getNext(&obj2);

  CPPUNIT_ASSERT_MESSAGE("DoubleNullTestObject was not fully added",obj1 == obj2);
};

void 
SQLArchive_Basic::inheritedObject_update() {
  InheritedObject obj1;
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  ostream.put(&obj1);
  InheritedObject obj2(10,10,"inheritedObject_insert",dba::DbResult::sInvalidTm);
  InheritedObject obj3(obj2);
  obj2.setId(obj1.getId());
  ostream.put(&obj2);
  ostream.destroy();

  dba::SQLIStream istream = mSQLArchive->getIStream();
  istream.open(obj3);
  istream.getNext(&obj3);

  CPPUNIT_ASSERT_MESSAGE("DoubleNullTestObject was not fully updated",obj2 == obj3);
};

void 
SQLArchive_Basic::inheritedObject_erase() {
  InheritedObject obj1(10,10,"inheritedObject_insert",dba::DbResult::sInvalidTm);
  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  ostream.put(&obj1);
  obj1.setDeleted();
  ostream.put(&obj1);
  ostream.destroy();

  dba::SQLIStream istream = mSQLArchive->getIStream();
  std::string query("SELECT count(*) FROM test_objects");
  std::auto_ptr<dba::DbResult> res(istream.sendQuery(query.c_str()));
  res->fetchRow();
  CPPUNIT_ASSERT_MESSAGE("DoubleNullTestObject was not fully erased",res->getInt(0) == 0);

  query = ("SELECT count(*) FROM test_inherited");
  res.reset(istream.sendQuery(query.c_str()));
  res->fetchRow();
  CPPUNIT_ASSERT_MESSAGE("DoubleNullTestObject was not fully erased",res->getInt(0) == 0);
};

class SQLTestClass {
  public:
    int a1;
    int a2;
};

class SQLTestClassFilter : public dba::StoreableFilter<SQLTestClass> {
  public:
    SQLTestClassFilter(SQLTestClass& pData) : dba::StoreableFilter<SQLTestClass> (pData) {};
    virtual std::string toString(const dba::ConvSpec& pSpec) const throw (dba::StoreableFilterException) { return std::string(); }
    virtual bool isNull() const { return false; }
    
    virtual void fromInt(const dba::ConvSpec& pSpec, int pData) throw (dba::StoreableFilterException) {};
    virtual void fromDouble(const dba::ConvSpec& pSpec, double pData) throw (dba::StoreableFilterException) {};
    virtual void fromString(const dba::ConvSpec& pSpec, const std::string& pData) throw (dba::StoreableFilterException) {};
    virtual void fromDate(const dba::ConvSpec& pSpec, const tm& pDate) throw (dba::StoreableFilterException) {};
    virtual void fromNull() throw (dba::StoreableFilterException) {};
    virtual ~SQLTestClassFilter() {};
};

class SQLTestIntFilter : public dba::StoreableFilter<int> {
  public:
    SQLTestIntFilter(int& pData) : dba::StoreableFilter<int> (pData) {};
    virtual std::string toString(const dba::ConvSpec& pSpec) const throw (dba::StoreableFilterException) { return std::string("3"); }
    virtual bool isNull() const { return false; }
    
    virtual void fromInt(const dba::ConvSpec& pSpec, int pData) throw (dba::StoreableFilterException) {
      *mMember = 3;
    }
    virtual void fromDouble(const dba::ConvSpec& pSpec, double pData) throw (dba::StoreableFilterException) { *mMember = 3; }
    virtual void fromString(const dba::ConvSpec& pSpec, const std::string& pData) throw (dba::StoreableFilterException) { *mMember = 3; }
    virtual void fromDate(const dba::ConvSpec& pSpec, const tm& pDate) throw (dba::StoreableFilterException) { *mMember = 3; }
    virtual void fromNull() throw (dba::StoreableFilterException) { *mMember = 0; };
    virtual ~SQLTestIntFilter() {};
};


void 
SQLArchive_Basic::sql_single_int() {
  int intval = 2;
  mSQLArchive->getOStream().sendUpdate(dba::SQL("INSERT INTO sql_basics(i_val) VALUES (:d)") << intval);

  int retval = 0;
  std::auto_ptr<dba::DbResult> res(mSQLArchive->getIStream().sendQuery(dba::SQL("SELECT i_val FROM sql_basics").into(retval)));
  res->fetchRow();

  CPPUNIT_ASSERT(intval == retval);
};

void 
SQLArchive_Basic::sql_where() {
  for (int i = 0; i < 5; i++)
    mSQLArchive->getOStream().sendUpdate(dba::SQL("INSERT INTO sql_basics(i_val) VALUES (:d)") << i);

  int retval = 0;
  std::auto_ptr<dba::DbResult> res(mSQLArchive->getIStream().sendQuery(
    dba::SQL("SELECT i_val FROM sql_basics WHERE i_val = :d").into(retval) << 3)
  );
  res->fetchRow();

  CPPUNIT_ASSERT(3 == retval);
};


void 
SQLArchive_Basic::sql_single_negative_int() {
  int intval = -2;
  mSQLArchive->getOStream().sendUpdate(dba::SQL("INSERT INTO sql_basics(i_val) VALUES (:d)") << intval);

  int retval = 0;
  std::auto_ptr<dba::DbResult> res(mSQLArchive->getIStream().sendQuery(dba::SQL("SELECT i_val FROM sql_basics").into(retval)));
  res->fetchRow();

  CPPUNIT_ASSERT(intval == retval);
};

void 
SQLArchive_Basic::sql_string_noquotes() {
  std::string strval = "test";

  mSQLArchive->getOStream().sendUpdate(dba::SQL("INSERT INTO sql_basics(s_val) VALUES (:s)") << strval);

  std::string retval = "";
  std::auto_ptr<dba::DbResult> res(mSQLArchive->getIStream().sendQuery(dba::SQL("SELECT s_val FROM sql_basics").into(retval)));
  res->fetchRow();

  CPPUNIT_ASSERT(strval == retval);
};

void 
SQLArchive_Basic::sql_char() {
  mSQLArchive->getOStream().sendUpdate(dba::SQL("INSERT INTO sql_basics(s_val) VALUES (:s)") << "test");

  std::string retval = "";
  std::auto_ptr<dba::DbResult> res(mSQLArchive->getIStream().sendQuery(dba::SQL("SELECT s_val FROM sql_basics").into(retval)));
  res->fetchRow();

  CPPUNIT_ASSERT("test" == retval);
};

void 
SQLArchive_Basic::sql_int_as_string() {
  int intval = 2;

  mSQLArchive->getOStream().sendUpdate(dba::SQL("INSERT INTO sql_basics(s_val) VALUES (':s')") << intval);

  int retval = 0;
  std::auto_ptr<dba::DbResult> res(mSQLArchive->getIStream().sendQuery(dba::SQL("SELECT s_val FROM sql_basics").into(retval)));
  res->fetchRow();

  CPPUNIT_ASSERT(intval == retval);
};

void 
SQLArchive_Basic::sql_string_quotes() {
  std::string strval = "test";

  mSQLArchive->getOStream().sendUpdate(dba::SQL("INSERT INTO sql_basics(s_val) VALUES (':s')") << strval);

  std::string retval = "";
  std::auto_ptr<dba::DbResult> res(mSQLArchive->getIStream().sendQuery(dba::SQL("SELECT s_val FROM sql_basics").into(retval)));
  res->fetchRow();

  CPPUNIT_ASSERT(strval == retval);
};

void 
SQLArchive_Basic::sql_date() {
  tm date(Utils::getNow());

  mSQLArchive->getOStream().sendUpdate(dba::SQL("INSERT INTO sql_basics(d_val) VALUES (:s)") << date);

  tm retval = dba::DbResult::sInvalidTm;

  std::auto_ptr<dba::DbResult> res(mSQLArchive->getIStream().sendQuery(dba::SQL("SELECT d_val FROM sql_basics").into(retval)));
  res->fetchRow();

  //FIXME add coparsion operator to debea for tm
  //CPPUNIT_ASSERT(date == retval);
}

void 
SQLArchive_Basic::sql_float() {
  float f = (float)0.12345;
  mSQLArchive->getOStream().sendUpdate(dba::SQL("INSERT INTO sql_basics(f_val) VALUES (:f)") << f);

  float retval = 0;
  std::auto_ptr<dba::DbResult> res(mSQLArchive->getIStream().sendQuery(dba::SQL("SELECT f_val FROM sql_basics").into(retval)));
  res->fetchRow();

  CPPUNIT_ASSERT(f == retval);
}

void 
SQLArchive_Basic::sql_two() {
  int intval = 2;
  std::string strval("test");
  mSQLArchive->getOStream().sendUpdate(dba::SQL("INSERT INTO sql_basics(i_val,s_val) VALUES (:d,:s)") << intval << strval);

  int retint = 0;
  std::string retstr = "";

  std::auto_ptr<dba::DbResult> res(mSQLArchive->getIStream().sendQuery(dba::SQL("SELECT i_val,s_val FROM sql_basics").into(retint).into(retstr)));
  res->fetchRow();

  CPPUNIT_ASSERT(intval == retint);
  CPPUNIT_ASSERT(strval == retstr);
}

void 
SQLArchive_Basic::sql_sql() {
  dba::SQL rest("(5)");
  mSQLArchive->getOStream().sendUpdate(dba::SQL("INSERT INTO sql_basics(f_val) VALUES ") + rest);
}

void 
SQLArchive_Basic::sql_sql_param() {
  dba::SQL rest("(:d)");
  rest << 5;
  mSQLArchive->getOStream().sendUpdate(dba::SQL("INSERT INTO sql_basics(i_val) VALUES ") + rest);
}

void 
SQLArchive_Basic::sql_sql_paramtoparam() {
  dba::SQL rest(":d)");
  rest << 5;
  mSQLArchive->getOStream().sendUpdate((dba::SQL("INSERT INTO sql_basics(s_val,i_val) VALUES (:s,") << std::string("test")) + rest);
}

void 
SQLArchive_Basic::sql_custom_filter() {
  int intval = 2;
  mSQLArchive->getOStream().sendUpdate(dba::SQL("INSERT INTO sql_basics(i_val) VALUES (:d)") << new SQLTestIntFilter(intval));

  int retval = 0;
  std::auto_ptr<dba::DbResult> res(mSQLArchive->getIStream().sendQuery(dba::SQL("SELECT i_val FROM sql_basics").into(new SQLTestIntFilter(retval))));
  res->fetchRow();

  CPPUNIT_ASSERT(3 == retval);
};

void 
SQLArchive_Basic::sql_custom_filter_in_nodata() {
  try {
    mSQLArchive->getOStream().sendUpdate(dba::SQL("INSERT INTO sql_basics(i_val) VALUES (:d)") << new dba::Int());
    CPPUNIT_ASSERT_MESSAGE("APIException not thrown",false);
  } catch (const dba::APIException&) {};
};

void 
SQLArchive_Basic::sql_custom_filter_out_nodata() {
  int intval = 2;
  mSQLArchive->getOStream().sendUpdate(dba::SQL("INSERT INTO sql_basics(i_val) VALUES (:d)") << new SQLTestIntFilter(intval));
  std::auto_ptr<dba::DbResult> res(mSQLArchive->getIStream().sendQuery(dba::SQL("SELECT i_val FROM sql_basics").into(new dba::Int())));
  try {
    res->fetchRow();
    CPPUNIT_ASSERT_MESSAGE("APIException not thrown",false);
  } catch (const dba::APIException&) {};
};


void 
SQLArchive_Basic::sql_escape_string() {
  std::string strval = "O'c";

  mSQLArchive->getOStream().sendUpdate(dba::SQL("INSERT INTO sql_basics(s_val) VALUES (:s)") << strval);

  std::string retval = "";
  std::auto_ptr<dba::DbResult> res(mSQLArchive->getIStream().sendQuery(dba::SQL("SELECT s_val FROM sql_basics").into(retval)));
  res->fetchRow();

  CPPUNIT_ASSERT(strval == retval);
};

void 
SQLArchive_Basic::sql_escape_param() {
  mSQLArchive->getOStream().sendUpdate(dba::SQL("INSERT INTO sql_basics(s_val,s_val2) VALUES (:s,'::')") << "test");

  std::string retval1 = "";
  std::string retval2 = "";
  std::auto_ptr<dba::DbResult> res(mSQLArchive->getIStream().sendQuery(dba::SQL("SELECT s_val,s_val2 FROM sql_basics").into(retval1).into(retval2)));
  res->fetchRow();

  CPPUNIT_ASSERT("test" == retval1);
  CPPUNIT_ASSERT(":" == retval2);
};

void 
SQLArchive_Basic::sql_escape_like_at_end() {
  mSQLArchive->getOStream().sendUpdate(dba::SQL("INSERT INTO sql_basics(s_val) VALUES (:s)") << "%");

  std::string retval = "";
  std::auto_ptr<dba::DbResult> res(mSQLArchive->getIStream().sendQuery(dba::SQL("SELECT s_val FROM sql_basics WHERE s_val LIKE '%:s%'").into(retval) << "%"));
  CPPUNIT_ASSERT_MESSAGE("Parser failed to escape LIKE char in SELECT",res->fetchRow());
  CPPUNIT_ASSERT("%" == retval);
};

void 
SQLArchive_Basic::sql_escape_like_escape() {
  mSQLArchive->getOStream().sendUpdate(dba::SQL("INSERT INTO sql_basics(s_val) VALUES (:s)") << "!");

  std::string retval = "";
  std::auto_ptr<dba::DbResult> res(mSQLArchive->getIStream().sendQuery(dba::SQL("SELECT s_val FROM sql_basics WHERE s_val LIKE '%:s%'").into(retval) << "!"));
  CPPUNIT_ASSERT_MESSAGE("Parser failed to escape LIKE char in SELECT",res->fetchRow());
  CPPUNIT_ASSERT("!" == retval);
};


void 
SQLArchive_Basic::sql_noparams_novars() {
  mSQLArchive->getOStream().sendUpdate(dba::SQL("INSERT INTO sql_basics(s_val) VALUES (':s')"));

  std::string retval = "";
  std::auto_ptr<dba::DbResult> res(mSQLArchive->getIStream().sendQuery(dba::SQL("SELECT s_val FROM sql_basics").into(retval)));
  res->fetchRow();

  CPPUNIT_ASSERT(":s" == retval);
};

class ColBugListObj : public dba::Storeable {
  public:
    DECLARE_STORE_TABLE()
  public:
    ColBugListObj() : s("unset") {};
    std::string s;
    bool operator==(const ColBugListObj& pObj) const {
      return s == pObj.s;
    }
    virtual ~ColBugListObj() {};
};

BEGIN_STORE_TABLE(ColBugListObj, dba::Storeable, "test_objects_only")
  BIND_STR(ColBugListObj::s, dba::String, "s_value")
END_STORE_TABLE()

class ColBugObj : public MIBase, public dba::Storeable {
  public:
    DECLARE_STORE_TABLE();
  public:
    ColBugObj(){}
    bool operator==(const ColBugObj& pObj) const {
      return mObjects == pObj.mObjects;
    }
    virtual ~ColBugObj() {}
    std::list<ColBugListObj> mObjects;
};

BEGIN_STORE_TABLE(ColBugObj, dba::Storeable, "obj_with_list")
  BIND_COL(ColBugObj::mObjects, dba::stdList<ColBugListObj>, "fk_owner")
END_STORE_TABLE()

void
SQLArchive_Basic::colbug_load_store() {
  ColBugListObj obj;
  ColBugObj stored;

  stored.mObjects.push_back(obj);

  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  void* mptr = &(stored.mObjects);
  ostream.put(&stored);
  ostream.destroy();

  ColBugObj loaded;
  dba::SQLIStream istream = mSQLArchive->getIStream();
  istream.get(&loaded);

  CPPUNIT_ASSERT(stored == loaded);
};

void
SQLArchive_Basic::colbug_sublist_erase() {
  ColBugListObj obj;
  ColBugObj stored;

  stored.mObjects.push_back(obj);

  dba::SQLOStream ostream = mSQLArchive->getOStream();
  ostream.open();
  ostream.put(&stored);

  stored.mObjects.clear();
  ostream.put(&stored);
  ostream.destroy();

  ColBugObj loaded;
  dba::SQLIStream istream = mSQLArchive->getIStream();
  istream.get(&loaded);

  CPPUNIT_ASSERT(loaded.mObjects.size() == 0);
};


} //namespace


