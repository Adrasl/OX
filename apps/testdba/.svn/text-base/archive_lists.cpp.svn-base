// File: archive_lists.
// Purpose: Database plugin for PostgresSQL RDBMS
// Author: Lukasz Michalski <lm at zork.pl>, Copyright 2007
// License: See COPYING file that comes with this distribution
//
// This file is a part of debea library (http://debea.net)
//
//
#include "archive_lists.h"
#include "utils.h"

namespace dba_tests {

Archive_Lists::Archive_Lists(const char* pPluginName, const char* pDbParams)
  : Archive_Storeable(pPluginName, pDbParams)
{
}


Archive_SQLPlugin::Archive_SQLPlugin(const char* pPluginName, const char* pDbParams)
  : Archive_Lists(pPluginName,pDbParams)
{};

void 
Archive_SQLPlugin::setUp() {
  mSQLArchive = Utils::initSharedSQLArchive(mPluginName,mDbParams);
  dba::SQLOStream stream(mSQLArchive->getOStream());
  Utils::prepareSharedSQLArchiveSchema(stream,mDbParams);
  mArchive = mSQLArchive;
};

} //namespace
