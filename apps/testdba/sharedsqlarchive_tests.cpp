// File: sharedsqlarchive_tests.cpp
// Purpose: Regression tests for transactions
// Author: Lukasz Michalski <lm at zork.pl>, Copyright 2007
// License: See COPYING file that comes with this distribution
//
// This file is a part of debea library (http://debea.net)
//
//
#include "sharedsqlarchive_tests.h"

namespace dba_tests {

SharedSQLArchive_Tests::SharedSQLArchive_Tests(const char* pPluginName, const char* pDbParams)
  : SQLArchive_Lists(pPluginName, pDbParams)
{
}


void
SharedSQLArchive_Tests::bigListTransactionsTest() {
  tm date(Utils::getNow());
  TestObject obj;
  obj = TestObject(1,1.1,"str1",date);
  for(int i = 0; i < 300; i++) {
    for(int j = 0; j < 100; j++) {
      dba::Transaction t(mSQLArchive->createTransaction());
      dba::SQLOStream ostream = t.getOStream();
      ostream.open();
      ostream.put(&obj);      
      obj = TestObject(1,1.1,"1234567890123456789023456789234567892345678923456789wertyuisdfghjkcvbnmxcvbnmdfghjwertyuio",date);
    };
  };  
  dba::SQLIStream istream = mSQLArchive->getIStream();
  std::auto_ptr<dba::DbResult> res(istream.sendQuery("select count(*) from test_objects"));
  CPPUNIT_ASSERT(res->fetchRow());
  CPPUNIT_ASSERT(res->getInt(0) == 30000);
};

void
SharedSQLArchive_Tests::loadStoreTest() {
  tm date(Utils::getNow());
  TestObject obj1(1,1.1,"str1",date);
  {
    dba::Transaction t1(mSQLArchive->createTransaction());
    dba::SQLOStream ostream = mSQLArchive->getOStream();
    ostream.open();
    ostream.put(&obj1);      
  }
  TestObject obj2;
  {
    dba::Transaction t(mSQLArchive->createTransaction());
    dba::SQLIStream is = t.getIStream();
    {
      dba::Transaction t2(mSQLArchive->createTransaction());
      dba::SQLIStream istream = t2.getIStream();
      istream.open(obj2);
      CPPUNIT_ASSERT(istream.getNext(&obj2));
    };
    {
      dba::Transaction t3(mSQLArchive->createTransaction());
      dba::SQLOStream ostream = t3.getOStream();
      ostream.open();
      ostream.put(&obj2);      
    };
    CPPUNIT_ASSERT(obj1 == obj2);
  };
};

void 
SharedSQLArchive_Tests::rollback() {
  {
    dba::Transaction t(mSQLArchive->createTransaction());  
    dba::SQLOStream stream = t.getOStream();
    stream.sendUpdate("INSERT INTO db_transactions(val1,val2,val3) VALUES (1,1,1)");  
    t.rollback();
  }  
  {
    dba::SQLIStream stream = mSQLArchive->getIStream();
    std::auto_ptr<dba::DbResult> res(stream.sendQuery("SELECT count(*) FROM db_transactions"));
    CPPUNIT_ASSERT(res->fetchRow());
    CPPUNIT_ASSERT(!res->isNull(0));
    CPPUNIT_ASSERT(res->getInt(0) == 0);
  };
};

void 
SharedSQLArchive_Tests::commit() {
  {
    dba::Transaction t(mSQLArchive->createTransaction());  
    dba::SQLOStream stream = t.getOStream();
    stream.sendUpdate("INSERT INTO db_transactions(val1,val2,val3) VALUES (1,1,1)");  
  }  
  {
    dba::SQLIStream stream = mSQLArchive->getIStream();
    std::auto_ptr<dba::DbResult> res2(stream.sendQuery("SELECT count(*) FROM db_transactions"));
    CPPUNIT_ASSERT(res2->fetchRow());
    CPPUNIT_ASSERT(!res2->isNull(0));
    CPPUNIT_ASSERT(res2->getInt(0) == 1);    
  }
};

void 
SharedSQLArchive_Tests::commit2() {
  dba::Transaction t1(mSQLArchive->createTransaction());  
  {
    dba::Transaction t2(mSQLArchive->createTransaction());  
    dba::SQLOStream stream = t2.getOStream();
    stream.sendUpdate("INSERT INTO db_transactions(val1,val2,val3) VALUES (1,1,1)");  
  }  
  {
    dba::SQLIStream stream = t1.getIStream();
    std::auto_ptr<dba::DbResult> res2(stream.sendQuery("SELECT count(*) FROM db_transactions"));
    CPPUNIT_ASSERT(res2->fetchRow());
    CPPUNIT_ASSERT(!res2->isNull(0));
    CPPUNIT_ASSERT(res2->getInt(0) == 1);    
  }
};

void 
SharedSQLArchive_Tests::connmgmt_getConnection() {
  CPPUNIT_ASSERT(mSQLArchive->getUsedConnections() == 0);
  {
    std::auto_ptr<dba::DbConnection> conn(mSQLArchive->getConnection());
    CPPUNIT_ASSERT(mSQLArchive->getUsedConnections() == 0);
  }
};

void 
SharedSQLArchive_Tests::connmgmt_singleTransaction() {
  CPPUNIT_ASSERT(mSQLArchive->getUsedConnections() == 0);
  {
    dba::Transaction t(mSQLArchive->createTransaction());
    CPPUNIT_ASSERT(mSQLArchive->getAvailableConnections() == 0);
    CPPUNIT_ASSERT(mSQLArchive->getUsedConnections() == 1);
  }
  CPPUNIT_ASSERT(mSQLArchive->getAvailableConnections() == 1);
  CPPUNIT_ASSERT(mSQLArchive->getUsedConnections() == 0);
  {
    dba::Transaction t(mSQLArchive->createTransaction());
    CPPUNIT_ASSERT(mSQLArchive->getAvailableConnections() == 0);
    CPPUNIT_ASSERT(mSQLArchive->getUsedConnections() == 1);
  }
  CPPUNIT_ASSERT(mSQLArchive->getAvailableConnections() == 1);
  CPPUNIT_ASSERT(mSQLArchive->getUsedConnections() == 0);
};

void 
SharedSQLArchive_Tests::connmgmt_TransactionAfterTransaction() {
  CPPUNIT_ASSERT(mSQLArchive->getUsedConnections() == 0);
  {
    dba::Transaction t1(mSQLArchive->createTransaction());
    CPPUNIT_ASSERT(mSQLArchive->getAvailableConnections() == 0);
    CPPUNIT_ASSERT(mSQLArchive->getUsedConnections() == 1);
    {
      dba::Transaction t2(mSQLArchive->createTransaction());
      CPPUNIT_ASSERT(mSQLArchive->getAvailableConnections() == 0);
      CPPUNIT_ASSERT(mSQLArchive->getUsedConnections() == 1);
    };
    CPPUNIT_ASSERT(mSQLArchive->getAvailableConnections() == 0);
    CPPUNIT_ASSERT(mSQLArchive->getUsedConnections() == 1);
  }
  CPPUNIT_ASSERT(mSQLArchive->getAvailableConnections() == 1);
  CPPUNIT_ASSERT(mSQLArchive->getUsedConnections() == 0);
};

void 
SharedSQLArchive_Tests::transactions_nested_rollback() {
  {
    dba::Transaction t(mSQLArchive->createTransaction());  
    { 
      dba::Transaction nested(mSQLArchive->createTransaction());  
  
      dba::SQLOStream stream = nested.getOStream();
      stream.sendUpdate("INSERT INTO db_transactions(val1,val2,val3) VALUES (1,1,1)");  
      nested.rollback();
    }
  };
  dba::SQLIStream stream = mSQLArchive->getIStream();
  std::auto_ptr<dba::DbResult> res(stream.sendQuery("SELECT count(*) FROM db_transactions"));
  CPPUNIT_ASSERT(res->fetchRow());
  CPPUNIT_ASSERT(!res->isNull(0));
  CPPUNIT_ASSERT(res->getInt(0) == 0);    
};

void 
SharedSQLArchive_Tests::transactions_live_time() {
  dba::Transaction* nested;
  {
    dba::Transaction t(mSQLArchive->createTransaction());  
    { 
      nested = new dba::Transaction(mSQLArchive->createTransaction());  
      dba::SQLOStream stream = nested->getOStream();
      stream.sendUpdate("INSERT INTO db_transactions(val1,val2,val3) VALUES (1,1,1)");  
    }
  };
  nested->commit();
  delete nested;
  dba::SQLIStream stream = mSQLArchive->getIStream();
  std::auto_ptr<dba::DbResult> res(stream.sendQuery("SELECT count(*) FROM db_transactions"));
  CPPUNIT_ASSERT(res->fetchRow());
  CPPUNIT_ASSERT(!res->isNull(0));
  CPPUNIT_ASSERT(res->getInt(0) == 1);    
};

void 
SharedSQLArchive_Tests::transactions_two_transactions() {
  dba::Transaction* t = new dba::Transaction(mSQLArchive->createTransaction());  
  dba::Transaction* t2 = new dba::Transaction(mSQLArchive->createTransaction(dba::Transaction::TRANS_NEW));  
  {
    dba::SQLOStream stream = t->getOStream();
    stream.sendUpdate("INSERT INTO db_transactions(val1,val2,val3) VALUES (1,1,1)");  
    stream.destroy();
    t->rollback();
    delete t;
  }  

  {
    dba::SQLOStream stream = t2->getOStream();
    stream.sendUpdate("INSERT INTO db_transactions(val1,val2,val3) VALUES (1,1,1)");  
    stream.destroy();
    delete t2;
  }  
  {
    dba::SQLIStream stream = mSQLArchive->getIStream();
    std::auto_ptr<dba::DbResult> res2(stream.sendQuery("SELECT count(*) FROM db_transactions"));
    CPPUNIT_ASSERT(res2->fetchRow());
    CPPUNIT_ASSERT(!res2->isNull(0));
    CPPUNIT_ASSERT(res2->getInt(0) == 1);    
  }
};

//create two transaction t and t2 that share one connection.
//modifications done on t2 should not be visible until t is destroyed.
void 
SharedSQLArchive_Tests::transactions_shared_rollback() {
  dba::Transaction* t = new dba::Transaction(mSQLArchive->createTransaction());  
  {
    dba::Transaction t2(mSQLArchive->createTransaction());  
    dba::SQLOStream stream = t2.getOStream();
    stream.sendUpdate("INSERT INTO db_transactions(val1,val2,val3) VALUES (1,1,1)");  

    dba::Transaction t3(mSQLArchive->createTransaction(dba::Transaction::TRANS_NEW));  
  };
  t->rollback();
  delete t;
  //t and t2 should be not commited 
  dba::SQLIStream stream = mSQLArchive->getIStream();
  std::auto_ptr<dba::DbResult> res2(stream.sendQuery("SELECT count(*) FROM db_transactions"));
  CPPUNIT_ASSERT(res2->fetchRow());
  CPPUNIT_ASSERT(!res2->isNull(0));
  CPPUNIT_ASSERT(res2->getInt(0) == 0);    
};

void 
SharedSQLArchive_Tests::transactions_load_and_store() {
  tm date(Utils::getNow());
  TestObject obj1(1,1.1,"str1",date);
  {
    dba::Transaction t1(mSQLArchive->createTransaction());
    dba::SQLOStream ostream = mSQLArchive->getOStream();
    ostream.open();
    ostream.put(&obj1);      
  }
  TestObject obj2;
  {
    dba::Transaction t2(mSQLArchive->createTransaction());
    dba::SQLIStream istream = t2.getIStream();
    istream.open(obj2);
    CPPUNIT_ASSERT(istream.getNext(&obj2));
  };
  {
    dba::Transaction t3(mSQLArchive->createTransaction());
    dba::SQLOStream ostream = t3.getOStream();
    ostream.open();
    ostream.put(&obj2);      
  };
  CPPUNIT_ASSERT(obj1 == obj2);
};

void 
SharedSQLArchive_Tests::transactions_store_after_store() {
  tm date(Utils::getNow());
  TestObject out1(1,1.1,"str1",date);
  {
    dba::Transaction t1(mSQLArchive->createTransaction());
    dba::SQLOStream ostream = t1.getOStream();
    ostream.open();
    ostream.put(&out1);      
  }
  TestObject out2(2,2.2,"str2",date);
  {
    dba::Transaction t2(mSQLArchive->createTransaction());
    dba::SQLOStream ostream = t2.getOStream();
    ostream.open();
    ostream.put(&out2);      
  }
  TestObject in;
  {
    dba::Transaction t3(mSQLArchive->createTransaction());
    dba::SQLIStream istream = t3.getIStream();
    istream.open(in);
    CPPUNIT_ASSERT(istream.getNext(&in));
    CPPUNIT_ASSERT(out1 == in);
    CPPUNIT_ASSERT(istream.getNext(&in));
    CPPUNIT_ASSERT(out2 == in);
  };
};

void 
SharedSQLArchive_Tests::transactions_rollback() {
  tm date(Utils::getNow());
  TestObject out1(1,1.1,"str1",date);
  {
    dba::Transaction t1(mSQLArchive->createTransaction());
    dba::SQLOStream ostream = t1.getOStream();
    ostream.open();
    ostream.put(&out1);
    t1.rollback();
  }
  TestObject out2(2,2.2,"str2",date);
  {
    dba::Transaction t2(mSQLArchive->createTransaction());
    dba::SQLOStream ostream = t2.getOStream();
    ostream.open();
    ostream.put(&out2);
  }
  TestObject in;
  {
    dba::Transaction t3(mSQLArchive->createTransaction());
    dba::SQLIStream istream = t3.getIStream();
    istream.open(in);
    CPPUNIT_ASSERT(istream.getNext(&in));
    CPPUNIT_ASSERT(out2 == in);
    CPPUNIT_ASSERT(istream.getNext(&in) == false);
  };
};

} //namespace
