// File: archive_lists.h
// Purpose: 
// Author: Lukasz Michalski <lm at zork.pl>, Copyright 2007
// License: See COPYING file that comes with this distribution
//
// This file is a part of debea library (http://debea.net)
//
//
#ifndef DBA_TESTSARCHIVE_LISTS_H
#define DBA_TESTSARCHIVE_LISTS_H

#include "archive_storeable.h"
#include <string>

namespace dba_tests {

/**
Tests for storing and retrieving lists of Storeable objects*/
class Archive_Lists : public Archive_Storeable {
  public:
    Archive_Lists(const char* pPluginName, const char* pDbParams);
};

class Archive_SQLPlugin : public Archive_Lists {
  public:
    Archive_SQLPlugin(const char* pPluginName, const char* pDbParams);
    virtual void setUp();
  private:
    dba::SQLArchive* mSQLArchive;
};

}

#endif
