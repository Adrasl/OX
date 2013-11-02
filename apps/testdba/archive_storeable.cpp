// File: archive_storeable.cpp
// Purpose: 
// Author: Lukasz Michalski <lm at zork.pl>, Copyright 2007
// License: See COPYING file that comes with this distribution
//
// This file is a part of debea library (http://debea.net)
//
//
#include "archive_storeable.h"

namespace dba_tests {

Archive_Storeable::Archive_Storeable(const char* pPluginName, const char* pDbParams)
  : ArchiveTestSuite(pPluginName,pDbParams)
{
}

}
