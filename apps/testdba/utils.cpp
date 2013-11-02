// File: utils.cpp
// Purpose: Utilities for testing
// Author: Lukasz Michalski <lm at zork.pl>, Copyright 2007
// License: See COPYING file that comes with this distribution
//
// This file is a part of debea library (http://debea.net)
//
//
#include "utils.h"
#include "sql_queries.h"
#include "dba/genericfetcher.h"
#include "dba/conversion.h"
#include "dbplugintestsuite.h" //for TestSQLException
#include <time.h>

namespace dba_tests {

tm 
Utils::getNow(int pDays) {
  time_t ti = time(NULL);
  ti += pDays*3600*24;
  tm val = *localtime(&ti);
  return val;
};

tm 
Utils::getDate(int pYear, int pMonth, int pDay, int pHour, int pMin, int pSec) {
  struct tm ret;
  ret.tm_year = pYear - 1900;
  ret.tm_mon = pMonth -1;
  ret.tm_mday = pDay;
  ret.tm_hour = pHour;
  ret.tm_min = pMin;
  ret.tm_sec = pSec;
  ret.tm_isdst = -1;
  //normalize fields
  mktime(&ret);
  return ret;
};


dba::SharedSQLArchive*
Utils::initSharedSQLArchive(dba::DbPlugin& pPlugin, const char* pDbParams) {
  dba::Database* db = pPlugin.createHandle();
  dba::SharedSQLArchive* ar = new dba::SharedSQLArchive(db);
  ar->setIdFetcher(new dba::GenericFetcher);
  ar->open(pDbParams);
  return ar;
};

dba::SharedSQLArchive*
Utils::initSharedSQLArchive(const char* pPlugin, const char* pDbParams) {
  dba::SharedSQLArchive* ar = new dba::SharedSQLArchive();
  ar->open(pPlugin,pDbParams);
  ar->setIdFetcher(new dba::GenericFetcher);
  return ar;
};

void
Utils::cleanupSharedSQLArchive(dba::DbPlugin& pPlugin, dba::SQLArchive* pArchive) {
  dba::Database* db = pArchive->getDatabase();
  delete pArchive;
  pPlugin.destroyHandle(db);
};

Utils::dbtype
Utils::getBackendType(const char* pConnParams) {
  std::string params(pConnParams);
  if (std::string::size_type pos = params.find("mssql",0) != std::string::npos)
    return DB_MSSQL;
  return DB_UNKNOWN;
};

std::string
Utils::alterTypes(const char* pTableDefinition, const char* pConnParams) {
  std::string ret(pTableDefinition);
  if (getBackendType(pConnParams) == DB_MSSQL)
    dba::replaceAll(ret,"timestamp","datetime");        
  return ret;
};

void 
Utils::prepareDatabaseSchema(dba::DbConnection& pConn, const char* pConnParams) {
  dropTable(pConn,"db_strdata");
  dropTable(pConn,"db_bigstrdata");
  dropTable(pConn,"db_norows");
  dropTable(pConn,"db_onerow");  
  dropTable(pConn,"db_fiverows");  
  dropTable(pConn,"db_namedcols");  
  dropTable(pConn,"db_nulls");  
  dropTable(pConn,"db_affectedrows");  
  dropTable(pConn,"db_numcols");  
  dropTable(pConn,"db_conv");  
  
  pConn.sendUpdate(alterTypes(db_strdata,pConnParams).c_str());
  pConn.sendUpdate(alterTypes(db_bigstrdata,pConnParams).c_str());
  pConn.sendUpdate(alterTypes(db_bigstrdata1,pConnParams).c_str());
  pConn.sendUpdate(alterTypes(db_norows,pConnParams).c_str());
  pConn.sendUpdate(alterTypes(db_onerow0,pConnParams).c_str());
  pConn.sendUpdate(alterTypes(db_onerow1,pConnParams).c_str());
  pConn.sendUpdate(alterTypes(db_fiverows0,pConnParams).c_str());
  pConn.sendUpdate(alterTypes(db_fiverows1,pConnParams).c_str());
  pConn.sendUpdate(alterTypes(db_fiverows2,pConnParams).c_str());
  pConn.sendUpdate(alterTypes(db_fiverows3,pConnParams).c_str());
  pConn.sendUpdate(alterTypes(db_fiverows4,pConnParams).c_str());
  pConn.sendUpdate(alterTypes(db_fiverows5,pConnParams).c_str());
  pConn.sendUpdate(alterTypes(db_namedcols0,pConnParams).c_str());
  pConn.sendUpdate(alterTypes(db_namedcols1,pConnParams).c_str());
  pConn.sendUpdate(alterTypes(db_namedcols2,pConnParams).c_str());
  pConn.sendUpdate(alterTypes(db_nulls0,pConnParams).c_str());
  pConn.sendUpdate(alterTypes(db_nulls1,pConnParams).c_str());
  pConn.sendUpdate(alterTypes(db_affectedrows,pConnParams).c_str());
  pConn.sendUpdate(alterTypes(db_numcols,pConnParams).c_str());
  pConn.sendUpdate(alterTypes(db_conv,pConnParams).c_str());
  pConn.sendUpdate(alterTypes(db_conv1,pConnParams).c_str());
};

void 
Utils::prepareSharedSQLArchiveSchema(dba::SQLOStream& pStream, const char* pConnParams) {
  dropTable(pStream,"db_transactions");  
  dropTable(pStream,"debea_object_count");
  dropTable(pStream,"test_objects");
  dropTable(pStream,"null_objects");
  dropTable(pStream,"double_null_objects");
  dropTable(pStream,"middle_null_objects");
  dropTable(pStream,"test_inherited");
  dropTable(pStream,"test_objects_only");
  dropTable(pStream,"test_agregated");
  dropTable(pStream,"test_advagregated");
  dropTable(pStream,"mi_object");
  dropTable(pStream,"mi_advobject");
  dropTable(pStream,"obj_with_list");
  dropTable(pStream,"three_lists");
  dropTable(pStream,"no_fields");
  dropTable(pStream,"storetest4_middle");
  dropTable(pStream,"storetest4_top");
  dropTable(pStream,"sql_basics");

  pStream.begin();
  pStream.sendUpdate(alterTypes(db_transactions,pConnParams).c_str());
  pStream.sendUpdate(alterTypes(ar_fetcher0,pConnParams).c_str());
  pStream.sendUpdate(alterTypes(ar_fetcher1,pConnParams).c_str());
  pStream.sendUpdate(alterTypes(ar_test_objects,pConnParams).c_str());
  pStream.sendUpdate(alterTypes(ar_null_objects,pConnParams).c_str());
  pStream.sendUpdate(alterTypes(ar_double_null_objects,pConnParams).c_str());
  pStream.sendUpdate(alterTypes(ar_middle_null_objects,pConnParams).c_str());
  pStream.sendUpdate(alterTypes(ar_test_inherited,pConnParams).c_str());
  pStream.sendUpdate(alterTypes(ar_test_objects_only,pConnParams).c_str());
  pStream.sendUpdate(alterTypes(ar_agregation,pConnParams).c_str());
  pStream.sendUpdate(alterTypes(ar_advagregation,pConnParams).c_str());
  pStream.sendUpdate(alterTypes(ar_mi_object,pConnParams).c_str());
  pStream.sendUpdate(alterTypes(ar_mi_advobject,pConnParams).c_str());
  pStream.sendUpdate(alterTypes(ar_obj_with_list,pConnParams).c_str());
  pStream.sendUpdate(alterTypes(ar_three_lists,pConnParams).c_str());
  pStream.sendUpdate(alterTypes(ar_no_fields,pConnParams).c_str());
  pStream.sendUpdate(alterTypes(ar_storetest4_middle,pConnParams).c_str());
  pStream.sendUpdate(alterTypes(ar_storetest4_top,pConnParams).c_str());
  pStream.sendUpdate(alterTypes(ar_sql_basics,pConnParams).c_str());
  pStream.commit();
};

void 
Utils::dropTable(dba::SQLOStream& pStream, const char* pTableName) {
  try {
    pStream.sendUpdate((std::string("DROP TABLE ") + pTableName).c_str());
  } catch (const dba::SQLException&) {
  } catch (const TestSQLException&) {
    //ignore "table does not exists" errors
  };
};

void 
Utils::dropTable(dba::DbConnection& pConn, const char* pTableName) {
  try {
    pConn.sendUpdate((std::string("DROP TABLE ") + pTableName).c_str());
  } catch (const dba::SQLException&) {
  } catch (const TestSQLException&) {
    //ignore "table does not exists" errors
  };
};


}
