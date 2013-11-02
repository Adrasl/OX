// File: test_object.h
// Purpose: Class for testing
// Author: Lukasz Michalski <lm at zork.pl>, Copyright 2007
// License: See COPYING file that comes with this distribution
//
// This file is a part of debea library (http://debea.net)
//
//
#ifndef DBA_TESTSTESTOBJECT_H
#define DBA_TESTSTESTOBJECT_H

#include "utils.h"
#include "dba/conversion.h"
#include "dba/storeable.h"
#include <algorithm>
#include <iostream>
#include <deque>
#include <list>

namespace dba_tests {

class TestStoreable : public dba::Storeable {
  public:
    virtual const dba::StoreTable* st_getTable() { return dba::Storeable::st_getTable(); }
    virtual const dba::ColTable* st_getColTable() { return dba::Storeable::st_getColTable(); }
};

/**
  Base class for multiple inheritance tests
*/
class MIBase {
  public:
    MIBase()
      : a1(-1),
        a2("mibase_empty")
    {}
    MIBase(int pa1, const std::string& pa2) 
      : a1(pa1),
        a2(pa2) 
    {};
    int a1;
    std::string a2;
    virtual ~MIBase() {};
};

/**
  Used by MIAdvObject
*/
class MIData {
  public:
    MIData()
      : c1(-3),
        c2("midata_empty")
    {};
    MIData(int pc1, const std::string& pc2) 
      : c1(pc1),c2(pc2) 
    {};
    ~MIData() {}
    int c1;
    std::string c2;
};

/**
  Object for multiple inheritance tests
*/
class MIObject : public MIBase, public TestStoreable {
    DECLARE_STORE_TABLE();
  public:
    MIObject() 
      : b1(-2),
        b2("miobject_empty")
    {};
    MIObject(int pa1, const std::string& pa2, int pb1, const std::string& pb2) 
      : MIBase(pa1,pa2),
        b1(pb1),
        b2(pb2) 
    {};
    int b1;
    std::string b2;
    bool operator== (const MIObject& pArg) {
      if (a1 != pArg.a1)
        return false;
      if (a2 != pArg.a2)
        return false;
      if (b1 != pArg.b1)
        return false;
      if (b2 != pArg.b2)
        return false;
      return true;
    };
};

/**
  Advanced object for multiple inheritance tests
*/
class MIAdvObject : public MIObject, public MIData {
    DECLARE_STORE_TABLE();
  public:
    MIAdvObject() 
      : d1(-4),
        d2("miadv_empty")
    {};
    MIAdvObject(int pa1, const std::string& pa2, int pb1, const std::string& pb2,int pc1, const std::string& pc2, int pd1, const std::string& pd2) 
      : MIObject(pa1,pa2,pb1,pb2),
        MIData(pc1,pc2),
        d1(pd1),
        d2(pd2)
    {};
    bool operator== (const MIAdvObject& pArg) {
      if (!MIObject::operator ==(pArg))
        return false;
      if (c1 != pArg.c1)
        return false;
      if (c2 != pArg.c2)
        return false;
      if (d1 != pArg.d1)
        return false;
      if (d2 != pArg.d2)
        return false;
      return true;
    };
    int d1;
    std::string d2;
};

/**
  Simple Object derived from Storeable with all types of fields
*/
class TestObject : public TestStoreable {
    DECLARE_STORE_TABLE();
  public:
    TestObject() 
      : i(-1),
        d(-1),
        s(""),
        date(dba::DbResult::sInvalidTm) 
    {};
    TestObject(int pInt, double pDouble, const std::string& pStr, const tm& pTm) 
      : i(pInt),
        d(pDouble),
        s(pStr),
        date(pTm) 
    {};
    virtual bool operator==(const TestObject& pObj) const {
      if (i != pObj.i) {
        std::cerr << i << " != " << pObj.i << std::endl;
        return false;
      };
      if (d != pObj.d) {
        std::cerr << d << " != " << pObj.d << std::endl;
        return false;
      };
      if (s != pObj.s) {
        std::cerr << s << " != " << pObj.s << std::endl;
        return false;
      };
      if (!(compareTm(pObj.date))) {
        std::cerr << "date mismatch" << std::endl;
        return false;
      };
      return true;
    };
    bool operator< (const TestObject& pObj) const {
      return i < pObj.i;
    };
    int i;
    double d;
    std::string s;
    ::tm date;
  private:
    bool compareTm(const tm& pTm) const {
      if (date.tm_year != pTm.tm_year) return false;
      if (date.tm_mon != pTm.tm_mon) return false;
      if (date.tm_mday != pTm.tm_mday) return false;
      if (date.tm_hour != pTm.tm_hour) return false;
      if (date.tm_min != pTm.tm_min) return false;
      if (date.tm_sec != pTm.tm_sec) return false;
      return true;
    };
};

class InheritedObject : public TestObject {
    DECLARE_STORE_TABLE();
  public:
    InheritedObject() {};
    InheritedObject(int pInt, double pDouble, const std::string& pStr, const tm& pTm) 
      : TestObject(pInt, pDouble, pStr, pTm)
    {
      mName = pStr + "_inherited";
    };
    virtual bool operator==(const InheritedObject& pObj) const {
      if (mName != pObj.mName)
        return false;
      return TestObject::operator==(pObj);
    };
    std::string mName;
};

/**
  TestObject with NULL root storeable table name
*/
class NullEmptyTestObject : public TestObject {
    DECLARE_STORE_TABLE();
  public:
    NullEmptyTestObject() 
      : TestObject()
    {};
    NullEmptyTestObject(int pInt, double pDouble, const std::string& pStr, const tm& pTm)
      : TestObject(pInt, pDouble, pStr, pTm)
    {};
};

/**
  TestObject with NULL root storeable table name
*/
class NullTestObject : public TestObject {
    DECLARE_STORE_TABLE();
  public:
    NullTestObject() 
      : TestObject(),
        val(-1)
    {};
    NullTestObject(int pInt, double pDouble, const std::string& pStr, const tm& pTm)
      : TestObject(pInt, pDouble, pStr, pTm),
        val(-1)
    {};
    virtual bool operator==(const NullTestObject& pObj) const {
      if (val == pObj.val) return false;
      return TestObject::operator==(pObj);
    };
    int val;
};

class MiddleNullObject : public NullTestObject {
    DECLARE_STORE_TABLE();
  public:
    MiddleNullObject() 
      : NullTestObject(),
        mno_val(-1)
    {};
    virtual bool operator==(const MiddleNullObject& pObj) const {
      if (mno_val == pObj.mno_val) return false;
      return NullTestObject::operator==(pObj);
    };
    int mno_val;
};

/**
  two top tables does not have name.
  can be used with test_objects table
*/
class DoubleNullTestObject : public NullTestObject {
    DECLARE_STORE_TABLE();
  public:
    DoubleNullTestObject() 
      : NullTestObject()
    {};
    int topint;
};

/**
  Object used to test BIND_STB agregation
*/
class AgregatedObject : public TestStoreable {
    DECLARE_STORE_TABLE();
  public:
    /**
      Arg allwes to create two different objects
    */
    AgregatedObject(bool pVersion = false)
      : mObject(7,7.7,"7",Utils::getDate(2008,1,1,0,0,0))
    {
      if (pVersion) {
        mInt = 77;
      } else {
        mObject = TestObject(3,3.3,"333",Utils::getDate(2008,1,33,0,0,0));
        mInt = 33;
      };
    };
    bool operator==(const AgregatedObject& pObj) const {
      return (mInt == pObj.mInt) && (mObject == pObj.mObject);
    };
    ~AgregatedObject() {};
  private:
    int mInt;    
    TestObject mObject;
};

/**
  Object for testing multiple BIND_STB agregation
*/
class DoubleAgregatedObject : public TestStoreable {
    DECLARE_STORE_TABLE();
  public:
    DoubleAgregatedObject(bool pVersion = false) 
      : mObject(pVersion)
    {
      mString = pVersion ? "7-7-7" : "3-3-3";
    };
    bool operator==(const DoubleAgregatedObject& pObj) const {
      return (mString == pObj.mString) && (mObject == pObj.mObject);
    };
  private:
    std::string mString;
    AgregatedObject mObject;
};

/**
  Test data contains TestObject and two variables that are binded to stream in tests
*/
class TestData {
  public:
    TestData() {};
    TestData(const TestObject& pObj, int pBi, const std::string& pBs) 
      : o(pObj),bi(pBi),bs(pBs)
    {};
    bool operator==(const TestData& pData) const {
      return (bi == pData.bi) && (bs == pData.bs) && (o == pData.o); 
    };
    TestObject o;
    int bi;
    std::string bs;
};

/**
  Object that contains list of other storeable objects
*/
class ObjWithListBase : public TestStoreable {
  public:
    ObjWithListBase() {};
    ObjWithListBase(const std::string& pName, int pChildrenCount) 
      : mName(pName)
    {
      for (int i=0; i < pChildrenCount; i++) {
        mList.push_back(TestObject(i,i,"test_object",Utils::getDate(2008,1,i+1,0,0,0)));
      };
    };
    bool operator==(const ObjWithListBase& pObj) const {
      if (mName != pObj.mName) {
        std::cerr << "Name mismatch. (mine:[" << mName << "], pObj:[" << pObj.mName << "])" << std::endl;
        return false;
      };
      if (mList.size() != pObj.mList.size()) {
        std::cerr << "lists have different size (mine: " << mList.size() << ", pObj:" << pObj.mList.size() << ")" << std::endl;
        return false;
      };
      for(std::list<TestObject>::const_iterator it = mList.begin(); it != mList.end(); it++) {
        std::list<TestObject>::const_iterator ot = std::find(pObj.mList.begin(), pObj.mList.end(), *it);
        if (ot == pObj.mList.end()) {
          std::cerr << "subobject " << it->i << " not found" << std::endl;
          return false;
        };
      };
      return true;
    };
    std::string mName;
    std::list<TestObject> mList;
};

class ObjWithList : public ObjWithListBase {
    DECLARE_STORE_TABLE();
  public:
    ObjWithList() {};
    ObjWithList(const std::string& pName, int pChildrenCount) 
      : ObjWithListBase(pName, pChildrenCount) {};
};

class ObjWithMultiset : public TestStoreable {
    DECLARE_STORE_TABLE();
  public:
    ObjWithMultiset() {};
    ObjWithMultiset(const std::string& pName, int pChildrenCount) 
      : mName(pName)
    {
      for (int i=0; i < pChildrenCount; i++) {
        mList.insert(TestObject(i,i,"test_object",Utils::getDate(2008,1,i+1,0,0,0)));
      };
    };
    bool operator==(const ObjWithMultiset& pObj) const {
      if (mName != pObj.mName) return false;
      if (mList.size() != pObj.mList.size())
        return false;
      for(std::multiset<TestObject>::const_iterator it = mList.begin(); it != mList.end(); it++) {
        std::multiset<TestObject>::const_iterator ot = std::find(pObj.mList.begin(), pObj.mList.end(), *it);
        if (ot == pObj.mList.end()) {
          std::cerr << "subobject " << it->i << " not found" << std::endl;
          return false;
        };
      };
      return true;
    };
    std::string mName;
    std::multiset<TestObject> mList;    
};

class ObjWithSet : public TestStoreable {
    DECLARE_STORE_TABLE();
  public:
    ObjWithSet() {};
    ObjWithSet(const std::string& pName, int pChildrenCount) 
      : mName(pName)
    {
      for (int i=0; i < pChildrenCount; i++) {
        mList.insert(TestObject(i,i,"test_object",Utils::getDate(2008,1,i+1,0,0,0)));
      };
    };
    bool operator==(const ObjWithSet& pObj) const {
      if (mName != pObj.mName) return false;
      if (mList.size() != pObj.mList.size())
        return false;
      for(std::set<TestObject>::const_iterator it = mList.begin(); it != mList.end(); it++) {
        std::set<TestObject>::const_iterator ot = std::find(pObj.mList.begin(), pObj.mList.end(), *it);
        if (ot == pObj.mList.end()) {
          std::cerr << "subobject " << it->i << " not found" << std::endl;
          return false;
        };
      };
      return true;
    };
    std::string mName;
    std::set<TestObject> mList;    
};

class ObjWithVector : public TestStoreable {
    DECLARE_STORE_TABLE();
  public:
    ObjWithVector() {};
    ObjWithVector(const std::string& pName, int pChildrenCount) 
      : mName(pName)
    {
      for (int i=0; i < pChildrenCount; i++) {
        mList.push_back(TestObject(i,i,"test_object",Utils::getDate(2008,1,i+1,0,0,0)));
      };
    };
    bool operator==(const ObjWithVector& pObj) const {
      if (mName != pObj.mName) return false;
      if (mList.size() != pObj.mList.size())
        return false;
      for(std::vector<TestObject>::const_iterator it = mList.begin(); it != mList.end(); it++) {
        std::vector<TestObject>::const_iterator ot = std::find(pObj.mList.begin(), pObj.mList.end(), *it);
        if (ot == pObj.mList.end()) {
          std::cerr << "subobject " << it->i << " not found" << std::endl;
          return false;
        };
      };
      return true;
    };
    std::string mName;
    std::vector<TestObject> mList;    
};

class ObjWithDeque : public TestStoreable {
    DECLARE_STORE_TABLE();
  public:
    ObjWithDeque() {};
    ObjWithDeque(const std::string& pName, int pChildrenCount) 
      : mName(pName)
    {
      for (int i=0; i < pChildrenCount; i++) {
        mList.push_back(TestObject(i,i,"test_object",Utils::getDate(2008,1,i+1,0,0,0)));
      };
    };
    bool operator==(const ObjWithDeque& pObj) const {
      if (mName != pObj.mName) return false;
      if (mList.size() != pObj.mList.size())
        return false;
      for(std::deque<TestObject>::const_iterator it = mList.begin(); it != mList.end(); it++) {
        std::deque<TestObject>::const_iterator ot = std::find(pObj.mList.begin(), pObj.mList.end(), *it);
        if (ot == pObj.mList.end()) {
          std::cerr << "subobject " << it->i << " not found" << std::endl;
          return false;
        };
      };
      return true;
    };
    std::string mName;
    std::deque<TestObject> mList;    
};

class ObjWithInheritedList : public TestStoreable {
    DECLARE_STORE_TABLE();
  public:
    ObjWithInheritedList() {};
    ObjWithInheritedList(const std::string& pName, int pChildrenCount) 
      : mName(pName)
    {
      for (int i=0; i < pChildrenCount; i++) {
        mList.push_back(InheritedObject(i,i,"ObjWithInheritedList child",Utils::getDate(2008,1,i+1,0,0,0)));
      };
    };
    bool operator==(const ObjWithInheritedList& pObj) const {
      if (mName != pObj.mName) return false;
      if (mList.size() != pObj.mList.size())
        return false;
      for(std::list<InheritedObject>::const_iterator it = mList.begin(); it != mList.end(); it++) {
        std::list<InheritedObject>::const_iterator ot = std::find(pObj.mList.begin(), pObj.mList.end(), *it);
        if (ot == pObj.mList.end()) {
          std::cerr << "subobject " << it->i << " not found" << std::endl;
          return false;
        };
      };
      return true;
    };
    std::string mName;
    std::list<InheritedObject> mList;
};


class ThreeLists : public TestStoreable {
    DECLARE_STORE_TABLE();
  public:
    ThreeLists() {};
    ThreeLists(const std::string& pName, int pChildrenCount1, int pChildrenCount2) 
      : mName(pName)
    {
      for (int i=0; i < pChildrenCount1; i++) {
        std::string name(mName);
        name += "_";
        name += dba::toStr(i);
        mList.push_back(ObjWithList(name,pChildrenCount2));
      };
    };
    bool operator==(const ThreeLists& pObj) const {
      if (mName != pObj.mName)
        return false;
      if (mList.size() != pObj.mList.size())
        return false;
      for(std::list<ObjWithList>::const_iterator it = mList.begin(); it != mList.end(); it++) {
        std::list<ObjWithList>::const_iterator ot = std::find(pObj.mList.begin(), pObj.mList.end(), *it);
        if (ot == pObj.mList.end()) {
          std::cerr << "ObjWithList " << it->mName << " not found" << std::endl;
          return false;
        };
      };
      return true;
    };
    std::string mName;
    std::list<ObjWithList> mList;
};

class TreeObject : public TestObject {
    DECLARE_STORE_TABLE();
  public:
    TreeObject() {};
    TreeObject(int pInt, double pDouble, const std::string& pStr)
      : TestObject(pInt, pDouble, pStr, Utils::getDate(2008,1,1,0,0,0))
    {};
    bool operator==(const TreeObject& pObj) const {
      if (!TestObject::operator ==(pObj))
        return false;
      for(std::list<TreeObject>::const_iterator it = mLeafs.begin(); it != mLeafs.end(); it++) {
        std::list<TreeObject>::const_iterator ot = std::find(pObj.mLeafs.begin(), pObj.mLeafs.end(), *it);
        if (ot == pObj.mLeafs.end()) {
          std::cerr << "TreeObject " << it->i << " not found as leaf of " << i << std::endl;
          return false;
        };
      };
      return true;
    };
    void dump(int pLevel = 0) const {
      std::cerr << "level:" << pLevel << " i_val: " << i << std::endl;
      for(std::list<TreeObject>::const_iterator it = mLeafs.begin(); it != mLeafs.end(); it++)
        it->dump(pLevel+1);
    };
    std::list<TreeObject> mLeafs;
};

class ObjWithCustomTableList : public TestStoreable {
    DECLARE_STORE_TABLE();
  public:
    ObjWithCustomTableList() {};
    ObjWithCustomTableList(const std::string& pName, int pChildrenCount) 
      : mName(pName)
    {
      for (int i=0; i < pChildrenCount; i++) {
        mList.push_back(NullEmptyTestObject(i,i,"null_test_object",Utils::getDate(2008,1,i+1,0,0,0)));
      };
    };
    bool operator==(const ObjWithCustomTableList& pObj) const {
      if (mName != pObj.mName) return false;
      if (mList.size() != pObj.mList.size())
        return false;
      for(std::list<NullEmptyTestObject>::const_iterator it = mList.begin(); it != mList.end(); it++) {
        std::list<NullEmptyTestObject>::const_iterator ot = std::find(pObj.mList.begin(), pObj.mList.end(), *it);
        if (ot == pObj.mList.end()) {
          std::cerr << "subobject " << it->i << " not found" << std::endl;
          return false;
        };
      };
      return true;
    };
    std::string mName;
    std::list<NullEmptyTestObject> mList;    
};

class TwoSubobjects : public TestStoreable {
    DECLARE_STORE_TABLE();
  public:
    TwoSubobjects() {};
    TwoSubobjects(const char* pName) 
      : mName(pName),
        mObj1(TestObject(1,1,"1",Utils::getDate(2008,1,1,0,0,0))),
        mObj2(TestObject(2,2,"2",Utils::getDate(2008,1,2,0,0,0)))
    {};
    bool operator==(const TwoSubobjects& pObj) {
      if (mName != pObj.mName)
        return false;
      return mObj1 == pObj.mObj1 && mObj2 == pObj.mObj2;
    };
    std::string mName;
    TestObject mObj1;
    TestObject mObj2;  
};

class EnumObject : public TestStoreable {
    DECLARE_STORE_TABLE();
  public:
    typedef enum {
      ENUM_ONE = 1,
      ENUM_TWO,
      ENUM_THREE
    } enum_type_t;
    
    EnumObject(enum_type_t pVal = ENUM_ONE) : mVal(pVal) {}
    bool operator==(const EnumObject& pObj) {
       return mVal == pObj.mVal;
    };
    enum_type_t mVal;
};

struct NoFieldsObject : public TestStoreable {
  DECLARE_STORE_TABLE();
};

struct StoreTest4Middle : public TestStoreable {
  StoreTest4Middle() {
    for (size_t i=0; i<1; ++i)
      mAList.push_back(NoFieldsObject());
  };
  std::list<NoFieldsObject> mAList;
  DECLARE_STORE_TABLE();
};

struct StoreTest4Top : public TestStoreable {
  StoreTest4Top() {
    for (size_t i=0; i<1 ; ++i)
      mBList.push_back(StoreTest4Middle());
  };
  std::list<StoreTest4Middle> mBList;
  DECLARE_STORE_TABLE();
};

} //namespace

#endif
