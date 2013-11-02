// File: double_filter.cpp
// Purpose: StoreableFilter for double
// Author: Lukasz Michalski <lm at zork.pl>, Copyright 2007
// License: See COPYING file that comes with this distribution
//
// This file is a part of debea library (http://debea.net)


#include "dba/conversion.h"
#include <ipersistence/vdouble_filter.h>
#include <string.h>
#include <locale.h>
#include <limits.h>

namespace dba {

vDouble::vDouble(double& pDouble)
 : StoreableFilter<double>(pDouble)
{
}

std::string 
vDouble::toString(const ConvSpec& pSpec) const throw (StoreableFilterException) {
  try {
    std::string dest;
    convert(*mMember,dest);
    struct lconv* l = localeconv();
    //locale hack
    char* lptr = l->thousands_sep;
    while((*lptr != '\0')&&(*lptr != CHAR_MAX)) {
      std::string::size_type pos = 0;
      while( (pos = dest.find(*lptr, pos)) != std::string::npos ) {
        dest.erase(pos,1);
      };
      lptr++;
    };		
	lptr = l->decimal_point;
    while((*lptr != '\0')&&(*lptr != CHAR_MAX)) {
      if (*lptr != pSpec.mDecimalPoint) {
        std::string::size_type pos = 0;
        while( (pos = dest.find(*lptr, pos)) != std::string::npos ) {
          dest[pos] = pSpec.mDecimalPoint;
        };
      };
      lptr++;
    };
    return dest;
  } catch (ConversionException& pEx) {
    throw StoreableFilterException(pEx.what());
  };
};

bool 
vDouble::isNull() const {
  return false;
};

void 
vDouble::fromInt(const ConvSpec& pSpec, int pData) throw (StoreableFilterException) {
  *mMember = (int)pData;
};

void
vDouble::fromDouble(const ConvSpec& pSpec, double pData) throw (StoreableFilterException) {
  *mMember = pData;
};

void 
vDouble::fromString(const ConvSpec& pSpec, const std::string& pData) throw (StoreableFilterException) {
  try {
    convert(pData,*mMember,&pSpec.mDecimalPoint);
  } catch (ConversionException& pEx) {
    throw StoreableFilterException(pEx.what());
  };
};

void 
vDouble::fromDate(const ConvSpec& pSpec, const tm& pDate) throw (StoreableFilterException) {
  time_t val = mktime((tm*)&pDate);
  *mMember = static_cast<double>(val);
};

void 
vDouble::fromNull() throw (StoreableFilterException) {
  *mMember = 0;
};


vDouble::~vDouble()
{
}

vFloat::vFloat(float& pFloat)
 : StoreableFilter<float>(pFloat)
{
}

std::string 
vFloat::toString(const ConvSpec& pSpec) const throw (StoreableFilterException) {
  double d = *mMember;
  vDouble f(d);
  std::string result = f.toString(pSpec);
  return result;
};

bool 
vFloat::isNull() const {
  return false;
};

void 
vFloat::fromInt(const ConvSpec& pSpec, int pData) throw (StoreableFilterException) {
  *mMember = static_cast<float>(pData);
};

void
vFloat::fromDouble(const ConvSpec& pSpec, double pData) throw (StoreableFilterException) {
  *mMember = static_cast<float>(pData);
};

void 
vFloat::fromString(const ConvSpec& pSpec, const std::string& pData) throw (StoreableFilterException) {
  double d;
  vDouble f(d);
  f.fromString(pSpec, pData);
  *mMember = d;
};

void 
vFloat::fromDate(const ConvSpec& pSpec, const tm& pDate) throw (StoreableFilterException) {
  double d;
  vDouble f(d);
  f.fromDate(pSpec, pDate);
  *mMember = static_cast<float>(d);
};

void 
vFloat::fromNull() throw (StoreableFilterException) {
  *mMember = 0;
};

vFloat::~vFloat()
{
}


}
