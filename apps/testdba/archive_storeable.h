// File: archive_storeable.h
// Purpose: 
// Author: Lukasz Michalski <lm at zork.pl>, Copyright 2007
// License: See COPYING file that comes with this distribution
//
// This file is a part of debea library (http://debea.net)
//
//
#ifndef DBA_TESTSARCHIVE_STOREABLE_H
#define DBA_TESTSARCHIVE_STOREABLE_H

#include "db_basic.h"
#include "utils.h"

namespace dba_tests {

/**
Tests for simple Storeable derived objects */
class Archive_Storeable : public ArchiveTestSuite {
    CPPUNIT_TEST_SUITE(Archive_Storeable);
    CPPUNIT_TEST_SUITE_END();
  public:
    Archive_Storeable() {};
    Archive_Storeable(const char* pPluginName, const char* pDbParams);
};

}

#endif
