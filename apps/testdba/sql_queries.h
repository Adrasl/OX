// File: sql_quieries.h
// Purpose: Database schema for regression tests
// Author: Lukasz Michalski <lm at zork.pl>, Copyright 2007
// License: See COPYING file that comes with this distribution
//
// This file is a part of debea library (http://debea.net)
//
//
namespace dba_tests {

/////////////////// Queries for Database tests ///////////////////////

static const char* db_strdata = 
"CREATE TABLE db_strdata ("
"  strval VARCHAR(128)"
");"
;

static const char* db_bigstrdata = 
"CREATE TABLE db_bigstrdata ("
"  strval VARCHAR(1000)"
");"
;

static const char* db_bigstrdata1 = 
"INSERT INTO db_bigstrdata(strval) VALUES ('"
"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
"');";

static const char* db_norows = 
"CREATE TABLE db_norows ("
"  intval INT"
");"
;
  
static const char* db_onerow0 = 
"CREATE TABLE db_onerow ("
"  intval INT"
");";
static const char* db_onerow1 = 
"INSERT INTO db_onerow(intval) VALUES (777);";
  

static const char* db_fiverows0 =
"CREATE TABLE db_fiverows ("
"  intval INT"
");";
static const char* db_fiverows1 = "INSERT INTO db_fiverows(intval) VALUES (0);";
static const char* db_fiverows2 = "INSERT INTO db_fiverows(intval) VALUES (1);";
static const char* db_fiverows3 = "INSERT INTO db_fiverows(intval) VALUES (2);";
static const char* db_fiverows4 = "INSERT INTO db_fiverows(intval) VALUES (3);";
static const char* db_fiverows5 = "INSERT INTO db_fiverows(intval) VALUES (4);";

  //namedColsQuery
static const char* db_namedcols0 =
"CREATE TABLE db_namedcols ("
"  intval INT,"
"  intval2 INT"
");"
;
static const char* db_namedcols1 = "INSERT INTO db_namedcols(intval,intval2) VALUES (777,777);";
static const char* db_namedcols2 = "INSERT INTO db_namedcols(intval,intval2) VALUES (NULL,777);";
  
  //nulls
static const char* db_nulls0 =
"CREATE TABLE db_nulls ("
  "intval INT,"
  "intval2 INT"
");";
static const char* db_nulls1 = "INSERT INTO db_nulls(intval,intval2) VALUES (NULL,777);";
  
  //numCols
static const char* db_numcols =
"CREATE TABLE db_numcols ("
  "col1 INT,"
  "col2 INT,"
  "col3 INT,"
  "col4 INT,"
  "col5 INT,"
  "col6 INT,"
  "col7 INT"
");";

static const char* db_affectedrows =
"CREATE TABLE db_affectedrows ("
  "val1 INT,"
  "val2 INT,"
  "val3 INT"
");";
  
  //transactions
static const char* db_transactions =
"CREATE TABLE db_transactions ("
  "val1 INT,"
  "val2 INT,"
  "val3 INT"
");";

  //table for conversions
static const char* db_conv =
"CREATE TABLE db_conv ("
  "i_val int,"
  "f_val float,"
  "s_val varchar(128),"
  "d_val timestamp"
");";
static const char* db_conv1 = "INSERT INTO db_conv(i_val,f_val,s_val,d_val) VALUES (1,2.2,'3.3','2007-12-12 14:15:16');";  

/////////////////// Queries for Archive tests ///////////////////////
    //generic fetcher
static const char* ar_fetcher0 = "create table debea_object_count(id int, root_table varchar(128))";
static const char* ar_fetcher1 = "insert into debea_object_count values (1,NULL)";

static const char* ar_test_objects = 
"create table test_objects ("
"id int,"
"i_value int,"
"f_value float,"
"s_value varchar(128),"
"d_value timestamp,"
"bindedint int,"
"bindedstring varchar(128),"
"dba_coll_id int,"
"fk_owner int"
")";

static const char* ar_null_objects = 
"create table null_objects ("
"id int,"
"val int"
")";

static const char* ar_middle_null_objects = 
"create table middle_null_objects ("
"id int,"
"val int,"
"mno_val int"
")";

static const char* ar_double_null_objects = 
"create table double_null_objects ("
"id int,"
"val int,"
"dnto_int int"
")";

static const char* ar_test_inherited = 
"create table test_inherited ("
"id int,"
"name varchar(128),"
"fk_owner int"
")";

//test object without binded vars
static const char* ar_test_objects_only = 
"create table test_objects_only ("
"id int,"
"i_value int,"
"f_value float,"
"s_value varchar(128),"
"d_value timestamp,"
"fk_owner int"
")";

//agregation test
static const char* ar_agregation = 
"create table test_agregated ("
"id int,"
"agre_int int,"
"i_value int,"
"f_value float,"
"s_value varchar(128),"
"d_value timestamp"
")";

//advagregation test
static const char* ar_advagregation = 
"create table test_advagregated ("
"id int,"
"agre_str varchar(128),"
"agre_int int,"
"i_value int,"
"f_value float,"
"s_value varchar(128),"
"d_value timestamp"
")";

static const char* ar_mi_object = 
"CREATE TABLE mi_object ("
"  id int,"
"  a1 INT,"
"  a2 VARCHAR(128),"
"  b1 INT,"
"  b2 VARCHAR(128)"
");"
;

static const char* ar_mi_advobject = 
"CREATE TABLE mi_advobject ("
"  id int,"
"  c1 INT,"
"  c2 VARCHAR(128),"
"  d1 INT,"
"  d2 VARCHAR(128)"
");"
;

static const char* ar_obj_with_list = 
"CREATE TABLE obj_with_list ("
"  id int,"
"  name VARCHAR(128),"
"  fk_three_owner INT"
");"
;

static const char* ar_three_lists = 
"CREATE TABLE three_lists ("
"  id int,"
"  three_name VARCHAR(128)"
");"
;

static const char* ar_no_fields = 
"CREATE TABLE no_fields ("
"  id int,"
"  b_id int"
");"
;

static const char* ar_storetest4_middle = 
"CREATE TABLE storetest4_middle ("
"  id int,"
"  c_id int"
");"
;

static const char* ar_storetest4_top = 
"CREATE TABLE storetest4_top ("
"  id int"
");"
;

static const char* ar_sql_basics =
"CREATE TABLE sql_basics ("
  "i_val int,"
  "f_val float,"
  "s_val varchar(128),"
  "s_val2 varchar(128),"
  "d_val timestamp"
");";

};//namespace
