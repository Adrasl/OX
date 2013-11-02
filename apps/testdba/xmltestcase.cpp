//
// C++ Implementation: xmltestcase
//
// Description: 
//
//
// Author: Lukasz Michalski <lm@zork.pl>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifdef TEST_XML

#include <string.h>

#include "xmltestcase.h"
#include "testobject.h"
#include "dba/fileutils.h"
#include "dba/int_filter.h"
#include "dba/string_filter.h"
#include "dba/stdlist.h"
#include "dba/single.h"
#include "dba/bool_filter.h"

#include <fstream>


namespace dba_tests {

CPPUNIT_TEST_SUITE_REGISTRATION(XMLTestCase);

void 
XMLTestCase::setUp() {
};

void 
XMLTestCase::tearDown() {
};

XMLTestCase::~XMLTestCase() {
};

bool
XMLTestCase::compareXML(const char* pFilename, const char* pData) {
  ErrorContext c(this);
  xmlDocPtr doc = xmlParseFile(pFilename);
  if (doc == NULL) {
    if (c.wasError())
      throw c.createException();
    else
      throw dba::XMLException("Error reading test file");
  };
  int len;
  xmlChar* buf;
  xmlDocDumpFormatMemoryEnc(doc,&buf,&len,(const char*)doc->encoding,1);
  bool ret = strcmp((const char*)buf,pData) == 0;
  if (!ret) {
    std::cerr << "file: " << std::endl << buf << std::endl;
    std::cerr << "expected: " << std::endl << pData << std::endl;
  };
  xmlFree(buf);
  return ret;
};

//tests
void 
XMLTestCase::empty() {
  { 
    unlink("empty.xml");
    dba::XMLArchive ar;  
    ar.open("empty.xml");
  };
  CPPUNIT_ASSERT(dba::FileUtils::exists("empty.xml"));
};

void
XMLTestCase::encodingChange() {
  { 
    unlink("encoding.xml");
    dba::XMLArchive ar;
    ar.conversionSpecs().mDbCharset = dba::ConvSpec::ISO8859_2;
    ar.open("encoding.xml");
  };  
  dba::XMLArchive res;
  res.open("encoding.xml");
  CPPUNIT_ASSERT(res.getConversionSpecs().mDbCharset == dba::ConvSpec::ISO8859_2);
};

void
XMLTestCase::simpleStore() {
  const char* result = 
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<dba>\n"
"  <test_objects i_value=\"1\" f_value=\"1.1\" s_value=\"test\" d_value=\"2008-01-01T00:00:00\"/>\n"
"</dba>\n";
  {
    dba::XMLArchive ar;
    unlink("simplestore.xml");
    ar.open("simplestore.xml");
    TestObject obj1(1,1.1,"test",Utils::getDate(2008,1,1,0,0,0));
    dba::XMLOStream stream(ar.getOStream());
    stream.open();
    stream.put(&obj1);
  }
  CPPUNIT_ASSERT(compareXML("simplestore.xml",result));
};

void
XMLTestCase::simpleOneStore() {
  const char* result = 
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<test_objects i_value=\"1\" f_value=\"1.1\" s_value=\"test\" d_value=\"2008-01-01T00:00:00\"/>\n"
;
  {
    dba::XMLArchive ar;
    ar.setRootNodeName(NULL);
    unlink("simpleonestore.xml");
    ar.open("simpleonestore.xml");
    TestObject obj1(1,1.1,"test",Utils::getDate(2008,1,1,0,0,0));
    dba::XMLOStream stream(ar.getOStream());
    stream.open();
    stream.put(&obj1);
  }
  CPPUNIT_ASSERT(compareXML("simpleonestore.xml",result));
};

void
XMLTestCase::storeVar() {
  const char* result = 
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<dba>\n"
"  <test_objects i_value=\"1\" f_value=\"1.1\" s_value=\"test\" d_value=\"2008-01-01T00:00:00\" binded=\"4\"/>\n"
"</dba>\n";
  {
    dba::XMLArchive ar;
    unlink("simplestore.xml");
    ar.open("simplestore.xml");
    TestObject obj1(1,1.1,"test",Utils::getDate(2008,1,1,0,0,0));
    dba::XMLOStream stream(ar.getOStream());
    int binded = 4;
    stream.bind("test_objects","binded",new dba::Int(binded));
    stream.open();
    stream.put(&obj1);
  }
  CPPUNIT_ASSERT(compareXML("simplestore.xml",result));
};

void
XMLTestCase::emptyLoad() {
  dba::XMLArchive ar;
  unlink("emptyload.xml");
  ar.open("emptyload.xml");
  TestObject obj1;
  dba::XMLIStream stream(ar.getIStream());
  stream.open(obj1);
  CPPUNIT_ASSERT(!stream.getNext(&obj1));
};

void
XMLTestCase::simpleLoad() {
  const char* data = 
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<dba>\n"
"  <test_objects i_value=\"1\" f_value=\"1.1\" s_value=\"test\" d_value=\"2008-01-01T00:00:00\"/>\n"
"</dba>\n";
  { 
    std::ofstream file("simpleload.xml");
    file << data;
  };
  {
    dba::XMLArchive ar;
    ar.open("simpleload.xml");
    TestObject obj1;
    dba::XMLIStream stream(ar.getIStream());

    stream.open(obj1);
    CPPUNIT_ASSERT(stream.getNext(&obj1));

    TestObject expected(1,1.1,"test",Utils::getDate(2008,1,1,0,0,0));
    CPPUNIT_ASSERT(obj1 == expected);
  };
};

void
XMLTestCase::simpleOneLoad() {
  const char* data = 
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<test_objects i_value=\"1\" f_value=\"1.1\" s_value=\"test\" d_value=\"2008-01-01T00:00:00\"/>\n"
;
  { 
    std::ofstream file("simpleoneload.xml");
    file << data;
  };
  {
    dba::XMLArchive ar;
    ar.setRootNodeName(NULL);
    ar.open("simpleoneload.xml");
    TestObject obj1;
    dba::XMLIStream stream(ar.getIStream());

    stream.open(obj1);
    CPPUNIT_ASSERT(stream.getNext(&obj1));

    TestObject expected(1,1.1,"test",Utils::getDate(2008,1,1,0,0,0));
    CPPUNIT_ASSERT(obj1 == expected);
  };
};


void
XMLTestCase::varLoad() {
  const char* data = 
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<dba>\n"
"  <test_objects i_value=\"1\" f_value=\"1.1\" s_value=\"test\" d_value=\"2008-01-01T00:00:00\" binded=\"4\"/>\n"
"</dba>\n";
  { 
    std::ofstream file("simpleload.xml");
    file << data;
  };
  {
    dba::XMLArchive ar;
    ar.open("simpleload.xml");
    TestObject obj1;
    dba::XMLIStream stream(ar.getIStream());

    int binded = 0;
    stream.bind("test_objects","binded",new dba::Int(binded));
    stream.open(obj1);
    stream.getNext(&obj1);
    CPPUNIT_ASSERT(binded == 4);
  };
};

void 
XMLTestCase::sublist_one_store() {
  const char* result = 
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<dba>\n"
"  <obj_with_list name=\"sub\">\n"
"    <fk_owner>\n"
"      <test_objects i_value=\"0\" f_value=\"0\" s_value=\"test_object\" d_value=\"2008-01-01T00:00:00\"/>\n"
"    </fk_owner>\n"
"  </obj_with_list>\n"
"</dba>\n";
  {
    dba::XMLArchive ar;
    unlink("sublist_one.xml");
    ar.open("sublist_one.xml");
    ObjWithList obj1("sub",1);
    dba::XMLOStream stream(ar.getOStream());
    stream.open();
    stream.put(&obj1);
  }
  CPPUNIT_ASSERT(compareXML("sublist_one.xml",result));
};

void 
XMLTestCase::sublist_two_store() {
  const char* result = 
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<dba>\n"
"  <obj_with_list name=\"sub\">\n"
"    <fk_owner>\n"
"      <test_objects i_value=\"1\" f_value=\"1\" s_value=\"test_object\" d_value=\"2008-01-02T00:00:00\"/>\n"
"      <test_objects i_value=\"0\" f_value=\"0\" s_value=\"test_object\" d_value=\"2008-01-01T00:00:00\"/>\n"
"    </fk_owner>\n"
"  </obj_with_list>\n"
"</dba>\n";
  {
    dba::XMLArchive ar;
    unlink("sublist_two.xml");
    ar.open("sublist_two.xml");
    ObjWithList obj1("sub",2);
    dba::XMLOStream stream(ar.getOStream());
    stream.open();
    stream.put(&obj1);
  }
  CPPUNIT_ASSERT(compareXML("sublist_two.xml",result));
};


void 
XMLTestCase::sublist_one_load() {
  const char* data = 
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<dba>\n"
"  <obj_with_list name=\"sub\">\n"
"    <fk_owner>\n"
"      <test_objects i_value=\"0\" f_value=\"0\" s_value=\"test_object\" d_value=\"2008-01-01T00:00:00\"/>\n"
"    </fk_owner>\n"
"  </obj_with_list>\n"
"</dba>\n";
  { 
    std::ofstream file("sublist_one_load.xml");
    file << data;
  };
  {
    dba::XMLArchive ar;
    ar.open("sublist_one_load.xml");
    ObjWithList obj1;
    dba::XMLIStream stream(ar.getIStream());

/*    std::cerr << &(obj1) << std::endl;
    std::cerr << &(obj1.mList) << std::endl;*/
    stream.get(&obj1);

    ObjWithList expected("sub",1);

    CPPUNIT_ASSERT(obj1 == expected);
  };
};

void 
XMLTestCase::sublist_two_load() {
  const char* data = 
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<dba>\n"
"  <obj_with_list name=\"sub\">\n"
"    <fk_owner>\n"
"      <test_objects i_value=\"0\" f_value=\"0\" s_value=\"test_object\" d_value=\"2008-01-01T00:00:00\"/>\n"
"      <test_objects i_value=\"1\" f_value=\"1\" s_value=\"test_object\" d_value=\"2008-01-02T00:00:00\"/>\n"
"    </fk_owner>\n"
"  </obj_with_list>\n"
"</dba>\n";
  { 
    std::ofstream file("sublist_one_load.xml");
    file << data;
  };
  {
    dba::XMLArchive ar;
    ar.open("sublist_one_load.xml");
    ObjWithList obj1;
    dba::XMLIStream stream(ar.getIStream());

/*    std::cerr << &(obj1) << std::endl;
    std::cerr << &(obj1.mList) << std::endl;*/
    stream.get(&obj1);

    ObjWithList expected("sub",2);

    CPPUNIT_ASSERT(obj1 == expected);
  };
};

class XMLObjWithOneUnnamedList : public ObjWithListBase {
    DECLARE_STORE_TABLE();
  public:
    XMLObjWithOneUnnamedList() {}
    XMLObjWithOneUnnamedList(const char* pName, int pCount) 
      : ObjWithListBase(pName, pCount) {}
};

BEGIN_STORE_TABLE(XMLObjWithOneUnnamedList,ObjWithListBase,"obj_with_list")
  BIND_STR(ObjWithListBase::mName, dba::String, "name")
  BIND_COL(ObjWithListBase::mList, dba::stdList<TestObject>, NULL)
END_STORE_TABLE()

void 
XMLTestCase::sublist_one_store_nofk() {
  const char* result = 
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<dba>\n"
"  <obj_with_list name=\"sub\">\n"
"    <test_objects i_value=\"0\" f_value=\"0\" s_value=\"test_object\" d_value=\"2008-01-01T00:00:00\"/>\n"
"  </obj_with_list>\n"
"</dba>\n";
  {
    dba::XMLArchive ar;
    unlink("sublist_one_nofk.xml");
    ar.open("sublist_one_nofk.xml");
    XMLObjWithOneUnnamedList obj1("sub",1);
    dba::XMLOStream stream(ar.getOStream());
    stream.open();
    stream.put(&obj1);
  }
  CPPUNIT_ASSERT(compareXML("sublist_one_nofk.xml",result));
};

void 
XMLTestCase::sublist_one_load_nofk() {
  const char* data = 
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<dba>\n"
"  <obj_with_list name=\"sub\">\n"
"    <test_objects i_value=\"0\" f_value=\"0\" s_value=\"test_object\" d_value=\"2008-01-01T00:00:00\"/>\n"
"  </obj_with_list>\n"
"</dba>\n";
  { 
    std::ofstream file("sublist_one_load_nofk.xml");
    file << data;
  };
  {
    dba::XMLArchive ar;
    ar.open("sublist_one_load_nofk.xml");
    XMLObjWithOneUnnamedList obj1;
    dba::XMLIStream stream(ar.getIStream());

    stream.get(&obj1);

    XMLObjWithOneUnnamedList expected("sub",1);

    CPPUNIT_ASSERT(obj1 == expected);
  };
};

void 
XMLTestCase::sublist_tree_store() {
  const char* result = 
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<dba>\n"
"  <test_objects i_value=\"1\" f_value=\"1\" s_value=\"1\" d_value=\"2008-01-01T00:00:00\">\n"
"    <fk_owner>\n"
"      <test_objects i_value=\"2\" f_value=\"2\" s_value=\"2\" d_value=\"2008-01-01T00:00:00\">\n"
"        <fk_owner>\n"
"          <test_objects i_value=\"3\" f_value=\"3\" s_value=\"3\" d_value=\"2008-01-01T00:00:00\"/>\n"
"        </fk_owner>\n"
"      </test_objects>\n"
"    </fk_owner>\n"
"  </test_objects>\n"
"</dba>\n";
  {
    dba::XMLArchive ar;
    unlink("sublist_tree.xml");
    ar.open("sublist_tree.xml");
    TreeObject root(1,1,"1");
    TreeObject middle(2,2,"2");
    TreeObject last(3,3,"3");
    middle.mLeafs.push_back(last);
    root.mLeafs.push_back(middle);

    dba::XMLOStream stream(ar.getOStream());
    stream.open();
    stream.put(&root);
  }
  CPPUNIT_ASSERT(compareXML("sublist_tree.xml",result));
};

void 
XMLTestCase::sublist_tree_load() {
  const char* data = 
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<dba>\n"
"  <test_objects i_value=\"1\" f_value=\"1\" s_value=\"1\" d_value=\"2008-01-01T00:00:00\">\n"
"    <fk_owner>\n"
"      <test_objects i_value=\"2\" f_value=\"2\" s_value=\"2\" d_value=\"2008-01-01T00:00:00\">\n"
"        <fk_owner>\n"
"          <test_objects i_value=\"3\" f_value=\"3\" s_value=\"3\" d_value=\"2008-01-01T00:00:00\"/>\n"
"        </fk_owner>\n"
"      </test_objects>\n"
"    </fk_owner>\n"
"  </test_objects>\n"
"</dba>\n";
  { 
    std::ofstream file("sublist_tree_load.xml");
    file << data;
  };
  {
    dba::XMLArchive ar;
    ar.open("sublist_tree_load.xml");

    dba::XMLIStream stream(ar.getIStream());
    //stream.enableDebug();
    TreeObject obj1;
    stream.get(&obj1);

    //obj1.dump();

    TreeObject expected(1,1,"1");
    TreeObject middle(2,2,"2");
    TreeObject last(3,3,"3");
    middle.mLeafs.push_back(last);
    expected.mLeafs.push_back(middle);

    CPPUNIT_ASSERT(obj1 == expected);
  };
};


void 
XMLTestCase::sublist_three_simple_store() {
  const char* result = 
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<dba>\n"
"  <three_lists three_name=\"sub\">\n"
"    <fk_three_owner>\n"
"      <obj_with_list name=\"sub_0\">\n"
"        <fk_owner>\n"
"          <test_objects i_value=\"0\" f_value=\"0\" s_value=\"test_object\" d_value=\"2008-01-01T00:00:00\"/>\n"
"        </fk_owner>\n"
"      </obj_with_list>\n"
"    </fk_three_owner>\n"
"  </three_lists>\n"
"</dba>\n";
  {
    dba::XMLArchive ar;
    unlink("sublist_three_simple.xml");
    ar.open("sublist_three_simple.xml");
    ThreeLists root("sub",1,1);

    dba::XMLOStream stream(ar.getOStream());
    stream.open();
    stream.put(&root);
  }
  CPPUNIT_ASSERT(compareXML("sublist_three_simple.xml",result));
};

void 
XMLTestCase::sublist_three_simple_load() {
  const char* data = 
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<dba>\n"
"  <three_lists three_name=\"sub\">\n"
"    <fk_three_owner>\n"
"      <obj_with_list name=\"sub_0\">\n"
"        <fk_owner>\n"
"          <test_objects i_value=\"0\" f_value=\"0\" s_value=\"test_object\" d_value=\"2008-01-01T00:00:00\"/>\n"
"        </fk_owner>\n"
"      </obj_with_list>\n"
"    </fk_three_owner>\n"
"  </three_lists>\n"
"</dba>\n";
  { 
    std::ofstream file("sublist_three_simple_load.xml");
    file << data;
  };
  {
    dba::XMLArchive ar;
    ar.open("sublist_three_simple_load.xml");

    dba::XMLIStream stream(ar.getIStream());
    ThreeLists obj1;
    stream.get(&obj1);

    ThreeLists expected("sub",1,1);

    CPPUNIT_ASSERT(obj1 == expected);
  };
};

class NsObject: public dba::Storeable {
  DECLARE_STORE_TABLE();
public:
  NsObject() { }
  NsObject(const std::string& pNsName)
    : mNsName(pNsName)
  {
  }
  virtual bool operator ==(const NsObject& pNsObject) const {
    return (mNsName == pNsObject.mNsName);
  }
  std::string mNsName;
};

BEGIN_STORE_TABLE(NsObject, dba::Storeable, "nsobject")
  BIND_STR(NsObject::mNsName, dba::String , "ns:name")
END_STORE_TABLE();

void
XMLTestCase::load_missing_ns() {
  const char* nsdata = 
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
" <nsobject name=\"test\" ns:name=\"namespace\">\n"
" </nsobject>\n";
  {
    std::ofstream file("ns_missing.xml");
    file << nsdata;
  };
  {
    dba::XMLArchive xmlArch;
    xmlArch.setRootNodeName("nsobject");
    xmlArch.open("ns_missing.xml");
    NsObject loaded;
    dba::XMLIStream stream(xmlArch.getIStream());
    try {
      stream.get(&loaded);
    } catch (const dba::APIException& pEx) {
    };
  };
}

void
XMLTestCase::load_ns_single() {
  const char* nsdata = 
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
" <nsobject xmlns:ns='http://bogus_ns_namespace' ns:name=\"namespace\" />\n"
;
  {
    std::ofstream file("ns_load.xml");
    file << nsdata;
  };
  {
    NsObject expected("namespace");

    dba::XMLArchive xmlArch;
    xmlArch.setRootNodeName(NULL);
    xmlArch.open("ns_load.xml");
    NsObject loaded;
    dba::XMLIStream stream(xmlArch.getIStream());
    stream.get(&loaded);
    CPPUNIT_ASSERT(loaded == expected);
  };
}

void
XMLTestCase::store_ns_single() {
  const char* nsdata = 
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<nsobject xmlns:ns=\"http://bogus_ns_namespace\" ns:name=\"namespace\"/>\n"
;
  {
    dba::XMLArchive xmlArch;
    xmlArch.setRootNodeName(NULL);
    unlink("ns_store.xml");
    xmlArch.open("ns_store.xml");
    xmlArch.addNamespace("http://bogus_ns_namespace","ns");
    NsObject obj("namespace");
    dba::XMLOStream stream(xmlArch.getOStream());
    stream.put(&obj);
  };
  CPPUNIT_ASSERT(compareXML("ns_store.xml",nsdata));
}

void
XMLTestCase::load_ns() {
  const char* nsdata = 
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<dba xmlns:ns=\"http://bogus_ns_namespace\">\n"
"  <nsobject ns:name=\"namespace\"/>\n"
"</dba>\n"
;
  {
    std::ofstream file("ns_load.xml");
    file << nsdata;
  };
  {
    NsObject expected("namespace");

    dba::XMLArchive xmlArch;
    xmlArch.open("ns_load.xml");
    NsObject loaded;
    dba::XMLIStream stream(xmlArch.getIStream());
    stream.get(&loaded);
    CPPUNIT_ASSERT(loaded == expected);
  };
}

void
XMLTestCase::store_ns() {
  const char* nsdata = 
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<dba xmlns:ns=\"http://bogus_ns_namespace\">\n"
"  <nsobject ns:name=\"namespace\"/>\n"
"</dba>\n"
;
  {
    dba::XMLArchive xmlArch;
    unlink("ns_store.xml");
    xmlArch.open("ns_store.xml");
    xmlArch.addNamespace("http://bogus_ns_namespace","ns");
    NsObject obj("namespace");
    dba::XMLOStream stream(xmlArch.getOStream());
    stream.put(&obj);
  };
  CPPUNIT_ASSERT(compareXML("ns_store.xml",nsdata));
}

class SecondSingle : public TestObject {
    DECLARE_STORE_TABLE()
  public:
    SecondSingle() {};
    SecondSingle(int pInt, double pDouble, const std::string& pStr, const tm& pDate)
      : TestObject(pInt,pDouble,pStr,pDate) {};
};

BEGIN_STORE_TABLE(SecondSingle,TestObject,"secondsingle")
END_STORE_TABLE()

class TwoSingles : public dba::Storeable {
    DECLARE_STORE_TABLE();
  public:
    TwoSingles() {};
    TwoSingles(const char* pName) 
      : mName(pName),
        mObj1(TestObject(1,1,"1",Utils::getDate(2008,1,1,0,0,0))),
        mObj2(SecondSingle(20,20,"20",Utils::getDate(2008,2,2,0,0,0)))
    {};
    bool operator==(const TwoSingles& pObj) {
      if (mName != pObj.mName)
        return false;
      return mObj1 == pObj.mObj1 && mObj2 == pObj.mObj2;
    };
    std::string mName;
    TestObject mObj1;
    SecondSingle mObj2;  
};

BEGIN_STORE_TABLE(TwoSingles,dba::Storeable,"twosingles")
  BIND_STR(TwoSingles::mName, dba::String, "parent")
  BIND_COL(TwoSingles::mObj1, dba::Single, NULL)
  BIND_COL(TwoSingles::mObj2, dba::Single, NULL)
END_STORE_TABLE()

void
XMLTestCase::load_two() {
  const char* nsdata = 
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<dba>\n"
"  <twosingles parent=\"parent_name\">\n"
"    <test_objects i_value=\"1\" f_value=\"1\" s_value=\"1\" d_value=\"2008-01-01T00:00:00\"/>\n"
"    <secondsingle i_value=\"20\" f_value=\"20\" s_value=\"20\" d_value=\"2008-02-02T00:00:00\"/>\n"
"  </twosingles>\n"
"</dba>\n"
;
  {
    std::ofstream file("load_two.xml");
    file << nsdata;
  };
  {
    TwoSingles expected("parent_name");

    dba::XMLArchive xmlArch;
    xmlArch.open("load_two.xml");
    TwoSingles loaded;
    dba::XMLIStream stream(xmlArch.getIStream());
    //stream.enableDebug();
    stream.get(&loaded);
    CPPUNIT_ASSERT(loaded == expected);
  };
};

void
XMLTestCase::store_two() {
  const char* nsdata = 
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<dba>\n"
"  <twosingles parent=\"parent_name\">\n"
"    <test_objects i_value=\"1\" f_value=\"1\" s_value=\"1\" d_value=\"2008-01-01T00:00:00\"/>\n"
"    <test_objects i_value=\"20\" f_value=\"20\" s_value=\"20\" d_value=\"2008-02-02T00:00:00\"/>\n"
"  </twosingles>\n"
"</dba>\n"
;
  {
    dba::XMLArchive xmlArch;
    unlink("store_two.xml");
    xmlArch.open("store_two.xml");
    TwoSingles obj("parent_name");
    dba::XMLOStream stream(xmlArch.getOStream());
    stream.put(&obj);
  };
  CPPUNIT_ASSERT(compareXML("store_two.xml",nsdata));
}

void
XMLTestCase::load_two_inverted() {
  const char* nsdata = 
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<dba>\n"
"  <twosingles parent=\"parent_name\">\n"
"    <test_objects i_value=\"20\" f_value=\"20\" s_value=\"20\" d_value=\"2008-02-02T00:00:00\"/>\n"
"    <test_objects i_value=\"1\" f_value=\"1\" s_value=\"1\" d_value=\"2008-01-01T00:00:00\"/>\n"
"  </twosingles>\n"
"</dba>\n"
;
  {
    std::ofstream file("load_two_inverted.xml");
    file << nsdata;
  };
  {
    TwoSingles expected("parent_name");

    dba::XMLArchive xmlArch;
    xmlArch.open("load_two_inverted.xml");
    TwoSingles loaded;
    dba::XMLIStream stream(xmlArch.getIStream());
    stream.get(&loaded);
    CPPUNIT_ASSERT(loaded == expected);
  };
};

void
XMLTestCase::loadFromElements() {
  const char* data = 
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<test_objects>\n"
"  <i_value>1</i_value>\n"
"  <f_value>1.1</f_value>\n"
"  <s_value>test</s_value>\n"
"  <d_value>2008-01-01T00:00:00</d_value>\n"
"</test_objects>\n"
;
  { 
    std::ofstream file("loadfromelements.xml");
    file << data;
  };
  {
    dba::XMLArchive ar;
    ar.setRootNodeName(NULL);
    ar.useElements();
    ar.open("loadfromelements.xml");
    TestObject obj1;
    dba::XMLIStream stream(ar.getIStream());
    stream.enableDebug();
    stream.open(obj1);
    CPPUNIT_ASSERT(stream.getNext(&obj1));

    TestObject expected(1,1.1,"test",Utils::getDate(2008,1,1,0,0,0));
    CPPUNIT_ASSERT(obj1 == expected);
  };
};

void
XMLTestCase::storeToElements() {
  const char* result = 
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<test_objects>\n"
"  <i_value>1</i_value>\n"
"  <f_value>1.1</f_value>\n"
"  <s_value>test</s_value>\n"
"  <d_value>2008-01-01T00:00:00</d_value>\n"
"</test_objects>\n"
;
  {
    dba::XMLArchive ar;
    ar.setRootNodeName(NULL);
    ar.useElements();
    unlink("storetoelements.xml");
    ar.open("storetoelements.xml");
    TestObject obj1(1,1.1,"test",Utils::getDate(2008,1,1,0,0,0));
    dba::XMLOStream stream(ar.getOStream());
    stream.open();
    stream.put(&obj1);
  }
  CPPUNIT_ASSERT(compareXML("storetoelements.xml",result));
};

struct storeBug1Address : public dba::Storeable {
  DECLARE_STORE_TABLE();
  std::string mData;
};

BEGIN_STORE_TABLE(storeBug1Address,dba::Storeable,"address")
  BIND_STR(mData,dba::String,"data")
END_STORE_TABLE()

struct storeBug1Patient : public dba::Storeable {
  DECLARE_STORE_TABLE();
  storeBug1Address mAddress;
};

BEGIN_STORE_TABLE(storeBug1Patient,dba::Storeable,"patient")
  BIND_COL(mAddress,dba::Single,NULL)
END_STORE_TABLE()


struct storeBug1Report : public dba::Storeable {
  DECLARE_STORE_TABLE();
};

BEGIN_STORE_TABLE(storeBug1Report,dba::Storeable,"report")
END_STORE_TABLE()

struct storeBug1Request : public dba::Storeable {
  DECLARE_STORE_TABLE();
  storeBug1Patient mPatient;
  storeBug1Report mReport;
};

BEGIN_STORE_TABLE(storeBug1Request,dba::Storeable,"request")
  BIND_COL(mReport,dba::Single,NULL)
  BIND_COL(mPatient,dba::Single,NULL)
END_STORE_TABLE()

void
XMLTestCase::storeBug1() {
  const char* result = 
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<request>\n"
"  <report/>\n"
"  <patient>\n"
"    <address/>\n"
"  </patient>\n"
"</request>\n"
;
  {
    dba::XMLArchive ar;
    ar.setRootNodeName(NULL);
    //ar.useElements();
    unlink("storebug1.xml");
    ar.open("storebug1.xml");
    storeBug1Request obj1;
    dba::XMLOStream stream(ar.getOStream());
    stream.enableDebug();
    stream.open();
    stream.put(&obj1);
  }
  CPPUNIT_ASSERT(compareXML("storebug1.xml",result));
};

struct storeBug2Address : public dba::Storeable {
  storeBug2Address(const std::string& pData) : mData(pData) {};
  std::string mData;
  DECLARE_STORE_TABLE();
};

BEGIN_STORE_TABLE(storeBug2Address,dba::Storeable,"address")
  BIND_STR(mData,dba::String,"addressdata")
END_STORE_TABLE()

struct storeBug2Patient : public dba::Storeable {
  storeBug2Patient() : mAddress("patient address") {};
  storeBug2Address mAddress;
  DECLARE_STORE_TABLE();
};

BEGIN_STORE_TABLE(storeBug2Patient,dba::Storeable,"patient")
  BIND_COL(mAddress,dba::Single,NULL)
END_STORE_TABLE()

struct storeBug2Request : public dba::Storeable {
  storeBug2Request() : mData("request data"), mAddress("request address") {};
  storeBug2Patient mPatient;
  storeBug2Address mAddress;
  std::string mData;
  DECLARE_STORE_TABLE();
};

BEGIN_STORE_TABLE(storeBug2Request,dba::Storeable,"request")
  BIND_STB(mAddress)
  BIND_COL(mPatient,dba::Single,NULL)
  BIND_STR(mData,dba::String,"requestdata")
END_STORE_TABLE()

void
XMLTestCase::storeBug2() {
  const char* result = 
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<request>\n"
"  <addressdata>request address</addressdata>\n"
"  <patient>\n"
"    <address>\n"
"      <addressdata>patient address</addressdata>\n"
"    </address>\n"
"  </patient>\n"
"  <requestdata>request data</requestdata>\n"
"</request>\n"
;
  {
    dba::XMLArchive ar;
    ar.setRootNodeName(NULL);
    ar.useElements();
    unlink("storebug2.xml");
    ar.open("storebug2.xml");
    storeBug2Request obj1;
    dba::XMLOStream stream(ar.getOStream());
    stream.enableDebug();
    stream.open();
    stream.put(&obj1);
  }
  CPPUNIT_ASSERT(compareXML("storebug2.xml",result));
};

struct StoreBug3A : public dba::Storeable {
  DECLARE_STORE_TABLE();
}; 
BEGIN_STORE_TABLE(StoreBug3A, dba::Storeable, "ris_examparamval")
END_STORE_TABLE()

struct StoreBug3B : public dba::Storeable {
  StoreBug3B() : mIsRemoved(false) {};
  bool mIsRemoved;
  DECLARE_STORE_TABLE();
};
BEGIN_STORE_TABLE(StoreBug3B, dba::Storeable, NULL)
BIND_INT(mIsRemoved, dba::Bool, "is_removed")
END_STORE_TABLE()

struct StoreBug3C : public dba::Storeable {
  StoreBug3C() {
    for (int i=0; i<2; ++i)
      mVals.push_back(StoreBug3A());
  }
  DECLARE_STORE_TABLE();
  StoreBug3B mRemoveable;
  std::list<StoreBug3A> mVals;
};
BEGIN_STORE_TABLE(StoreBug3C, dba::Storeable, "ris_examparam")
BIND_STB(StoreBug3C::mRemoveable)
BIND_COL(StoreBug3C::mVals, dba::stdList<StoreBug3A>, "examparam_id")
END_STORE_TABLE()

void
XMLTestCase::storeBug3() {
  /* bug: the result is:
  <?xml version="1.0" encoding="utf-8"?>
  <ris_examparam>
    <is_removed>
      <ris_examparamval/>
      <ris_examparamval/>
      0
    </is_removed>
    <examparam_id/>
  </ris_examparam>
  */

  const char* result = 
    "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
    "<ris_examparam>\n"
    "  <is_removed>0</is_removed>\n"
    "  <examparam_id>\n"
    "    <ris_examparamval/>\n"
    "    <ris_examparamval/>\n"
    "  </examparam_id>\n"
    "</ris_examparam>\n"
    ;
  {
    dba::XMLArchive ar;
    ar.setRootNodeName(NULL);
    ar.useElements();
    unlink("storebug3.xml");
    ar.open("storebug3.xml");
    StoreBug3C obj;
    dba::XMLOStream stream(ar.getOStream());
    stream.enableDebug();
    stream.open();
    stream.put(&obj);
  }
  CPPUNIT_ASSERT(compareXML("storebug3.xml",result));
};


struct bug4_A : public dba::Storeable {
  DECLARE_STORE_TABLE();
}; 
BEGIN_STORE_TABLE(bug4_A, dba::Storeable, "A")
END_STORE_TABLE()

struct bug4_B : public dba::Storeable {
  DECLARE_STORE_TABLE();
};
BEGIN_STORE_TABLE(bug4_B, dba::Storeable, "B")
END_STORE_TABLE()

struct bug4_B_Full : public bug4_B {
  std::list<bug4_A> mAItems;
  DECLARE_STORE_TABLE();
};
BEGIN_STORE_TABLE(bug4_B_Full, bug4_B, "B")
BIND_COL(bug4_B_Full::mAItems, dba::stdList<bug4_A>, NULL)
END_STORE_TABLE()

struct bug4_C : public dba::Storeable {
  bug4_B_Full mB;
  DECLARE_STORE_TABLE();
};
BEGIN_STORE_TABLE(bug4_C, dba::Storeable, "C")
BIND_COL(mB, dba::Single, NULL)
END_STORE_TABLE()

struct bug4_D : public dba::Storeable {
  bug4_D() {
    bug4_C c1;
    bug4_A c1a1;
    bug4_A c1a2;
    c1.mB.mAItems.push_back(c1a1);
    c1.mB.mAItems.push_back(c1a2);
    mCItems.push_back(c1);
    bug4_C c2;
    mCItems.push_back(c2);
    bug4_C c3;
    mCItems.push_back(c3);
  }
  std::list<bug4_C> mCItems;
  DECLARE_STORE_TABLE();
}; 
BEGIN_STORE_TABLE(bug4_D, dba::Storeable, "D")
BIND_COL(mCItems, dba::stdList<bug4_C>, NULL)
END_STORE_TABLE()

void
XMLTestCase::storeBug4() {
  /* bug: the result is:
  <D>
    <C>
      <C>
        <B/>
      </C>
      <C>
        <B/>
      </C>
      <B>
        <A/>
        <A/>
      </B>
    </C>
  </D>
  */

  /**
    Note: the problem is caused by the fact that B is splitted (bug3_B & bug3_B_Full)
  */

  const char* result = 
    "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
    "<D>\n"
    "  <C>\n"
    "    <B/>\n"
    "  </C>\n"
    "  <C>\n"
    "    <B/>\n"
    "  </C>\n"
    "  <C>\n"
    "    <B>\n"
    "      <A/>\n"
    "      <A/>\n"
    "    </B>\n"
    "  </C>\n"
    "</D>\n"
    ;
  {
    dba::XMLArchive ar;
    ar.setRootNodeName(NULL);
    ar.useElements();
    bug4_D obj;
    unlink("storebug4.xml");
    ar.open("storebug4.xml");
    dba::XMLOStream stream(ar.getOStream());
    stream.enableDebug();
    stream.open();
    stream.put(&obj);
  }
  CPPUNIT_ASSERT(compareXML("storebug4.xml",result));
};


} //namespace

#endif //TEST_XML


