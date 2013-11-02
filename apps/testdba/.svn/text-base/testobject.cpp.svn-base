// File: test_object.cpp
// Purpose: Class for testing
// Author: Lukasz Michalski <lm at zork.pl>, Copyright 2007
// License: See COPYING file that comes with this distribution
//
// This file is a part of debea library (http://debea.net)
//
//
#include "testobject.h"
#include "dba/stdlist.h"
#include "dba/stdset.h"
#include "dba/stdvector.h"
#include "dba/stddeque.h"
#include "dba/stdmultiset.h"
#include "dba/stdfilters.h"
#include "dba/single.h"

namespace dba_tests {

BEGIN_STORE_TABLE(MIObject, TestStoreable, "mi_object")
  BIND_INT(MIObject::a1,dba::Int,"a1")
  BIND_INT(MIObject::b1,dba::Int,"b1")
  BIND_STR(MIObject::a2,dba::String,"a2")
  BIND_STR(MIObject::b2,dba::String,"b2")
END_STORE_TABLE()

BEGIN_STORE_TABLE(MIAdvObject,MIObject, "mi_advobject")
  BIND_INT(MIAdvObject::d1,dba::Int,"d1")
  BIND_INT(MIAdvObject::c1,dba::Int,"c1")
  BIND_STR(MIAdvObject::d2,dba::String,"d2")
  BIND_STR(MIAdvObject::c2,dba::String,"c2")
END_STORE_TABLE()

BEGIN_STORE_TABLE(DoubleAgregatedObject,TestStoreable,"test_advagregated")
  BIND_STR(DoubleAgregatedObject::mString,dba::String,"agre_str")
  BIND_STB(DoubleAgregatedObject::mObject)
END_STORE_TABLE()

BEGIN_STORE_TABLE(AgregatedObject,TestStoreable,"test_agregated")
  BIND_INT(AgregatedObject::mInt,dba::Int,"agre_int")
  BIND_STB(AgregatedObject::mObject)
END_STORE_TABLE()

BEGIN_STORE_TABLE(NullEmptyTestObject,TestObject,NULL)
END_STORE_TABLE()

BEGIN_STORE_TABLE(NullTestObject,TestObject,NULL)
  BIND_INT(NullTestObject::val,dba::Int,"val")
END_STORE_TABLE()

BEGIN_STORE_TABLE(MiddleNullObject,NullTestObject,"middle_null_objects")
  BIND_INT(MiddleNullObject::mno_val,dba::Int,"mno_val")
END_STORE_TABLE()

BEGIN_STORE_TABLE(DoubleNullTestObject,NullTestObject,NULL)
  BIND_INT(DoubleNullTestObject::topint,dba::Int,"dnto_int")
END_STORE_TABLE()

BEGIN_STORE_TABLE(TestObject,TestStoreable,"test_objects")
  BIND_INT(TestObject::i,dba::Int,"i_value")
  BIND_FLT(TestObject::d,dba::Double,"f_value")
  BIND_STR(TestObject::s,dba::String,"s_value")
  BIND_DAT(TestObject::date,dba::DateTime,"d_value")
END_STORE_TABLE()

BEGIN_STORE_TABLE(InheritedObject,TestObject,"test_inherited")
  BIND_STR(InheritedObject::mName,dba::String,"name")
END_STORE_TABLE()

BEGIN_STORE_TABLE(ObjWithList,ObjWithListBase,"obj_with_list")
  BIND_STR(ObjWithListBase::mName, dba::String, "name")
  BIND_COL(ObjWithListBase::mList, dba::stdList<TestObject>, "fk_owner")
END_STORE_TABLE()

BEGIN_STORE_TABLE(ObjWithMultiset,TestStoreable,"obj_with_list")
  BIND_STR(ObjWithMultiset::mName, dba::String, "name")
  BIND_COL(ObjWithMultiset::mList, dba::stdMultiset<TestObject>, "fk_owner")
END_STORE_TABLE()

BEGIN_STORE_TABLE(ObjWithSet,TestStoreable,"obj_with_list")
  BIND_STR(ObjWithSet::mName, dba::String, "name")
  BIND_COL(ObjWithSet::mList, dba::stdSet<TestObject>, "fk_owner")
END_STORE_TABLE()

BEGIN_STORE_TABLE(ObjWithVector,TestStoreable,"obj_with_list")
  BIND_STR(ObjWithVector::mName, dba::String, "name")
  BIND_COL(ObjWithVector::mList, dba::stdVector<TestObject>, "fk_owner")
END_STORE_TABLE()

BEGIN_STORE_TABLE(ObjWithDeque,TestStoreable,"obj_with_list")
  BIND_STR(ObjWithDeque::mName, dba::String, "name")
  BIND_COL(ObjWithDeque::mList, dba::stdDeque<TestObject>, "fk_owner")
END_STORE_TABLE()

BEGIN_STORE_TABLE(ObjWithInheritedList,TestStoreable,"obj_with_list")
  BIND_STR(ObjWithInheritedList::mName, dba::String, "name")
  BIND_COL(ObjWithInheritedList::mList, dba::stdList<InheritedObject>, "fk_owner")
END_STORE_TABLE()

BEGIN_STORE_TABLE(ThreeLists,TestStoreable,"three_lists")
  BIND_STR(ThreeLists::mName, dba::String, "three_name")
  BIND_COL(ThreeLists::mList, dba::stdList<ObjWithList>, "fk_three_owner")
END_STORE_TABLE()

BEGIN_STORE_TABLE(TreeObject,TestObject,"test_objects")
  BIND_COL(TreeObject::mLeafs, dba::stdList<TreeObject>, "fk_owner")
END_STORE_TABLE()

BEGIN_STORE_TABLE(ObjWithCustomTableList,TestStoreable,"obj_with_list")
  BIND_STR(ObjWithCustomTableList::mName, dba::String, "name")
  BIND_CLA(ObjWithCustomTableList::mList, dba::stdList<NullEmptyTestObject>, "fk_owner", 0, "test_objects_only")
END_STORE_TABLE()

BEGIN_STORE_TABLE(TwoSubobjects,TestStoreable,"obj_with_list")
  BIND_STR(TwoSubobjects::mName, dba::String, "name")
  BIND_CLA(TwoSubobjects::mObj1, dba::Single, "fk_owner",1,NULL)
  BIND_CLA(TwoSubobjects::mObj2, dba::Single, "fk_owner",2,NULL)
END_STORE_TABLE()

BEGIN_STORE_TABLE(EnumObject, TestStoreable, "test_objects")
  BIND_INT(EnumObject::mVal, dba::Int,"i_value")
END_STORE_TABLE()

BEGIN_STORE_TABLE(NoFieldsObject, TestStoreable, "no_fields")
END_STORE_TABLE()

BEGIN_STORE_TABLE(StoreTest4Middle, TestStoreable, "storetest4_middle")
  BIND_COL(StoreTest4Middle::mAList, dba::stdList<NoFieldsObject>, "b_id")
END_STORE_TABLE()


BEGIN_STORE_TABLE(StoreTest4Top, TestStoreable, "storetest4_top")
  BIND_COL(StoreTest4Top::mBList, dba::stdList<StoreTest4Middle>, "c_id")
END_STORE_TABLE()

} //namespace

