// File: sqlarchive_basic.h
// Purpose: Regression tests for SQLArchive
// Author: Lukasz Michalski <lm at zork.pl>, Copyright 2007
// License: See COPYING file that comes with this distribution
//
// This file is a part of debea library (http://debea.net)
//
//
#ifndef DBA_TESTSSQLARCHIVE_BASIC_H
#define DBA_TESTSSQLARCHIVE_BASIC_H

#include "dbplugintestsuite.h"

namespace dba_tests {

/**
Tests for SQLArchive features
*/
class SQLArchive_Basic : public SQLArchiveTestSuite {
    CPPUNIT_TEST_SUITE(SQLArchive_Basic);  
      CPPUNIT_TEST(sqlError);  
      CPPUNIT_TEST(noTables);  
      CPPUNIT_TEST(advAgregationTest);  
      CPPUNIT_TEST(agregationTest);  
      CPPUNIT_TEST(unbindField);  
      CPPUNIT_TEST(unbindTable);  
      CPPUNIT_TEST(MIObjectTest);  
      CPPUNIT_TEST(MIAdvObjectTest);  
      CPPUNIT_TEST(bindVar);  
      CPPUNIT_TEST(storeEnum);  
      CPPUNIT_TEST(bind_var_with_custom_table);  
      CPPUNIT_TEST(multifetcher_inheritance);
      CPPUNIT_TEST(multifetcher_forced_root_table);
      CPPUNIT_TEST(sublist_store);  
      CPPUNIT_TEST(sublist_erase);  
      CPPUNIT_TEST(sublist_store_3);  
      CPPUNIT_TEST(sublist_store_4);  
      CPPUNIT_TEST(sublist_erase_3);  
      CPPUNIT_TEST(sublist_erase_3_sec_deleted);  
      CPPUNIT_TEST(sublist_erase_one_from_two);  
      CPPUNIT_TEST(sublist_store_inherited);  
      CPPUNIT_TEST(sublist_erase_inherited);  
      CPPUNIT_TEST(sublist_load);  
      CPPUNIT_TEST(sublist_load_bug3024);  
      CPPUNIT_TEST(sublist_load_3);  
      CPPUNIT_TEST(sublist_load_dirty);  
      CPPUNIT_TEST(sublist_load_two_on_sublist);  
      CPPUNIT_TEST(sublist_load_3_with_two);  
      CPPUNIT_TEST(sublist_load_list);  
      CPPUNIT_TEST(sublist_load_one);  
      CPPUNIT_TEST(sublist_load_one_where);  
      CPPUNIT_TEST(sublist_store_tree);  
      CPPUNIT_TEST(sublist_load_tree);  
      CPPUNIT_TEST(sublist_custom_table_store);  
      CPPUNIT_TEST(sublist_custom_table_load);  
      CPPUNIT_TEST(sublist_coll_id_store);  
      CPPUNIT_TEST(sublist_coll_id_load);
      CPPUNIT_TEST(stdset_store);  
      CPPUNIT_TEST(stdvector_store);
      CPPUNIT_TEST(stddeque_store);
      CPPUNIT_TEST(stdmultiset_store);
      //see http://debea.net/trac/ticket/29
      //CPPUNIT_TEST(stl_args); 
      CPPUNIT_TEST(convspec_change);
      CPPUNIT_TEST(nullMainTable_insert);
      CPPUNIT_TEST(nullMainTable_change);
      CPPUNIT_TEST(nullMainTable_erase);
      CPPUNIT_TEST(nullMainTable_temp_insert);
      CPPUNIT_TEST(customMainTable_insert);
      CPPUNIT_TEST(customMainTable_erase);
      CPPUNIT_TEST(customMainTable_temp_insert);
      CPPUNIT_TEST(rootTable_insert);
      CPPUNIT_TEST(rootTable_change);
      CPPUNIT_TEST(rootTable_erase);
      CPPUNIT_TEST(rootAndCustomTable_insert);
      CPPUNIT_TEST(rootAndCustomTable_change);
      CPPUNIT_TEST(rootAndCustomTable_erase);
      CPPUNIT_TEST(doubleNullObject_insert);
      CPPUNIT_TEST(doubleNullObject_update);
      CPPUNIT_TEST(doubleNullObject_erase);
      CPPUNIT_TEST(middleNullObject_insert);
      CPPUNIT_TEST(middleNullObject_update);
      CPPUNIT_TEST(middleNullObject_erase);
      CPPUNIT_TEST(inheritedObject_insert);
      CPPUNIT_TEST(inheritedObject_update);
      CPPUNIT_TEST(inheritedObject_erase);

      CPPUNIT_TEST(sql_single_int);
      CPPUNIT_TEST(sql_where);
      CPPUNIT_TEST(sql_single_negative_int);
      CPPUNIT_TEST(sql_string_noquotes);
      CPPUNIT_TEST(sql_char);
      CPPUNIT_TEST(sql_int_as_string);
      CPPUNIT_TEST(sql_string_quotes);
      CPPUNIT_TEST(sql_date);
      CPPUNIT_TEST(sql_float);
      CPPUNIT_TEST(sql_two);
      CPPUNIT_TEST(sql_sql);
      CPPUNIT_TEST(sql_sql_param);
      CPPUNIT_TEST(sql_sql_paramtoparam);
      CPPUNIT_TEST(sql_custom_filter);
      CPPUNIT_TEST(sql_custom_filter_in_nodata);
      CPPUNIT_TEST(sql_custom_filter_out_nodata);
      CPPUNIT_TEST(sql_escape_string);
      CPPUNIT_TEST(sql_escape_param);
      CPPUNIT_TEST(sql_escape_like_at_end);
      CPPUNIT_TEST(sql_escape_like_escape);
      CPPUNIT_TEST(sql_noparams_novars);
      CPPUNIT_TEST(colbug_load_store);
      CPPUNIT_TEST(colbug_sublist_erase);
    CPPUNIT_TEST_SUITE_END();  
  public:
    SQLArchive_Basic() {};
    SQLArchive_Basic(const char* pPluginName, const char* pDbParams);
    
    void sqlError();
    void noTables();
    void noFields();
    void advAgregationTest();
    void agregationTest();
    void unbindField();
    void unbindTable();
    void MIObjectTest();
    void MIAdvObjectTest();
    void storeEnum();
    void bindVar();
    void bind_var_with_custom_table();
    void multifetcher_inheritance();
    void multifetcher_forced_root_table();
    void sublist_store();
    void sublist_erase();
    void sublist_store_3();
    void sublist_store_4();
    void sublist_erase_3();
    void sublist_erase_3_sec_deleted();
    void sublist_erase_one_from_two();
    void sublist_store_inherited();
    void sublist_erase_inherited();
    void sublist_load();
    void sublist_load_bug3024();
    void sublist_load_3();
    void sublist_load_dirty();
    void sublist_load_two_on_sublist();
    void sublist_load_3_with_two();
    void sublist_load_list();
    void sublist_load_one();
    void sublist_load_one_where();
    void sublist_store_tree();
    void sublist_load_tree();
    void sublist_custom_table_store();
    void sublist_custom_table_load();
    void sublist_coll_id_store();
    void sublist_coll_id_load();
    void stdset_store();
    void stdvector_store();
    void stddeque_store();
    void stdmultiset_store();
    void stl_args();

    void convspec_change();

    void nullMainTable_insert();
    void nullMainTable_change();
    void nullMainTable_erase();
    void nullMainTable_temp_insert();

    void customMainTable_insert();
    void customMainTable_erase();
    void customMainTable_temp_insert();

    void rootTable_insert();
    void rootTable_change();
    void rootTable_erase();
    void rootAndCustomTable_insert();
    void rootAndCustomTable_change();
    void rootAndCustomTable_erase();

    void doubleNullObject_insert();
    void doubleNullObject_update();
    void doubleNullObject_erase();

    void middleNullObject_insert();
    void middleNullObject_update();
    void middleNullObject_erase();

    void inheritedObject_insert();
    void inheritedObject_update();
    void inheritedObject_erase();

    //query construction tests
    void sql_single_int();
    void sql_where();
    void sql_single_negative_int();
    void sql_string_noquotes();
    void sql_char();
    void sql_int_as_string();
    void sql_string_quotes();
    void sql_date();
    void sql_float();
    void sql_two();
    void sql_sql();
    void sql_sql_param();
    void sql_sql_paramtoparam();
    void sql_custom_filter();
    void sql_custom_filter_in_nodata();
    void sql_custom_filter_out_nodata();
    void sql_escape_string();
    void sql_escape_param();
    void sql_escape_like_at_end();
    void sql_escape_like_escape();
    void sql_noparams_novars();
    void colbug_load_store();
    void colbug_sublist_erase();

    time_t mTimeForConv;
};

}

#endif
