//
// C++ Interface: xmltestcase
//
// Description: 
//
//
// Author: Lukasz Michalski <lm@zork.pl>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef TESTXMLTESTCASE_H
#define TESTXMLTESTCASE_H

#ifdef TEST_XML

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>
#include "dba/xmlarchive.h"

namespace dba_tests {

/**
tests from XMLArchive
*/
class XMLTestCase : public CppUnit::TestCase, public dba::XMLErrorHandler {
    CPPUNIT_TEST_SUITE(XMLTestCase);
      CPPUNIT_TEST(empty);
      CPPUNIT_TEST(encodingChange);
      CPPUNIT_TEST(simpleStore);
      CPPUNIT_TEST(simpleOneStore);
      CPPUNIT_TEST(storeVar);
      CPPUNIT_TEST(emptyLoad);
      CPPUNIT_TEST(simpleLoad);
      CPPUNIT_TEST(simpleOneLoad);
      CPPUNIT_TEST(varLoad);
      CPPUNIT_TEST(sublist_one_store);
      CPPUNIT_TEST(sublist_two_store);
      CPPUNIT_TEST(sublist_one_load);
      CPPUNIT_TEST(sublist_two_load);
      CPPUNIT_TEST(sublist_one_store_nofk);
      CPPUNIT_TEST(sublist_one_load_nofk);
      CPPUNIT_TEST(sublist_tree_store);
      CPPUNIT_TEST(sublist_three_simple_store);
      CPPUNIT_TEST(sublist_tree_load);
      CPPUNIT_TEST(sublist_three_simple_load);
      CPPUNIT_TEST(load_missing_ns);
      CPPUNIT_TEST(load_ns_single);
      CPPUNIT_TEST(store_ns_single);
      CPPUNIT_TEST(load_ns);
      CPPUNIT_TEST(store_ns);
      CPPUNIT_TEST(load_two);
      CPPUNIT_TEST(loadFromElements);
      CPPUNIT_TEST(storeToElements);
      CPPUNIT_TEST(storeBug2);
      CPPUNIT_TEST(storeBug3);
      CPPUNIT_TEST(storeBug4);
    CPPUNIT_TEST_SUITE_END();
  public:
    virtual void setUp();
    virtual void tearDown();
    virtual ~XMLTestCase();

    //tests
    void empty();
    void encodingChange();
    void simpleStore();
    void simpleOneStore();
    void storeVar();
    void emptyLoad();
    void simpleLoad();
    void simpleOneLoad();
    void varLoad();
    void sublist_one_store();
    void sublist_two_store();
    void sublist_one_load();
    void sublist_two_load();
    void sublist_one_store_nofk();
    void sublist_one_load_nofk();
    void sublist_tree_store();
    void sublist_three_simple_store();
    void sublist_tree_load();
    void sublist_three_simple_load();
    void load_missing_ns();
    void load_ns_single();
    void store_ns_single();
    void load_ns();
    void store_ns();
    void load_two();
    void load_two_inverted();
    void store_two();
    void loadFromElements();
    void storeToElements();
    void storeBug1();
    void storeBug2();
    void storeBug3();
    void storeBug4();
  private:
    bool compareXML(const char* pFilename, const char* pData);
};

} //namespace

#endif //TEST_XML

#endif
