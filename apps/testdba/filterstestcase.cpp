// File: filtertestcase.cpp
// Purpose: Base class for plugin regression tests
// Author: Lukasz Michalski <lm at zork.pl>, Copyright 2004
// License: See COPYING file that comes with this distribution
//
// This file is a part of debea library (http://debea.net)
//
//
#include "filterstestcase.h"

namespace dba_tests {

bool
operator==(const tm& d1, const tm& d2) {
  if (d1.tm_year != d2.tm_year)
    return false;
  if (d1.tm_mon != d2.tm_mon)
    return false;
  if (d1.tm_mday != d2.tm_mday)
    return false;
  if (d1.tm_hour != d2.tm_hour)
    return false;
  if (d1.tm_year != d2.tm_year)
    return false;
  if (d1.tm_min != d2.tm_min)
    return false;
  if (d1.tm_sec != d2.tm_sec)
    return false;
  return true;
};


};
