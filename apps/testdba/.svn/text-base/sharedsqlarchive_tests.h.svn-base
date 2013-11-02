// File: sharedsqlarchive_tests.cpp
// Purpose: Regression tests for transactions
// Author: Lukasz Michalski <lm at zork.pl>, Copyright 2007
// License: See COPYING file that comes with this distribution
//
// This file is a part of debea library (http://debea.net)
//
//
#ifndef DBA_TESTSSHAREDSQLARCHIVE_TESTS_H
#define DBA_TESTSSHAREDSQLARCHIVE_TESTS_H

#include "sqlarchive_lists.h"

namespace dba_tests {

/**
Tests for %SQL transactions management*/
class SharedSQLArchive_Tests : public SQLArchive_Lists {
    CPPUNIT_TEST_SUB_SUITE(SharedSQLArchive_Tests, SQLArchive_Lists);  
      //CPPUNIT_TEST(archiveBigTransactionsData);  
      //CPPUNIT_TEST(bigListTransactionsTest);  
      CPPUNIT_TEST(connmgmt_getConnection);  
      CPPUNIT_TEST(connmgmt_singleTransaction);  
      CPPUNIT_TEST(connmgmt_TransactionAfterTransaction);  
      CPPUNIT_TEST(loadStoreTest);  
      CPPUNIT_TEST(rollback);  
      CPPUNIT_TEST(commit);  
      CPPUNIT_TEST(commit2);  
      CPPUNIT_TEST(transactions_live_time);  
      CPPUNIT_TEST(transactions_nested_rollback);  
      CPPUNIT_TEST(transactions_two_transactions);  
      CPPUNIT_TEST(transactions_shared_rollback);  
      CPPUNIT_TEST(transactions_store_after_store);  
      CPPUNIT_TEST(transactions_rollback);  
    CPPUNIT_TEST_SUITE_END();  
  public:
    SharedSQLArchive_Tests() {};
    SharedSQLArchive_Tests(const char* pPluginName, const char* pDbParams);
    void rollback();
    void commit();
    void commit2();
    void transactions_nested_rollback();
    void transactions_live_time();
    void transactions_two_transactions();
    void transactions_shared_rollback();
    void archiveBigTransactionsData();
    void connmgmt_getConnection();
    void connmgmt_singleTransaction();
    void connmgmt_TransactionAfterTransaction();
    void bigListTransactionsTest();
    void loadStoreTest();
    void transactions_load_and_store();
    void transactions_bad_begin();
    void transactions_store_after_store();
    void transactions_rollback();
};

}

#endif
