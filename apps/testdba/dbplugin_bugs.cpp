// File: dbplugin_bugs.cpp
// Purpose: Regression test class for bug testing
// Author: Lukasz Michalski <lm at zork.pl>, Copyright 2007
// License: See COPYING file that comes with this distribution
//
// This file is a part of debea library (http://debea.net)
//
//
#include "dbplugin_bugs.h"

namespace dba_tests {

DbPlugin_Bugs::DbPlugin_Bugs(const char* pPluginName, const char* pDbParams)
  : SharedSQLArchive_Tests(pPluginName, pDbParams)
{
}

DbPlugin_Bugs::~DbPlugin_Bugs()
{
}


}
