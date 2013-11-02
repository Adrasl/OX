// File: double_filter.h
// Purpose: StoreableFilter for double
// Author: Lukasz Michalski <lm at zork.pl>, Copyright 2007
// License: See COPYING file that comes with this distribution
//
// This file is a part of debea library (http://debea.net)

#ifndef V_DBADOUBLE_FILTER_H
#define V_DBADOUBLE_FILTER_H

#include "dba/storeablefilter.h"

namespace dba {

/**
Class for converting double to and from database types
@ingroup filters
*/
class dbaDLLEXPORT vDouble : public StoreableFilter<double> {
  public:
    vDouble() {}
    /**
      Constructor
      @param pDouble reference to object or class member that filer will modify
    */
    vDouble(double& pDouble);
    virtual std::string toString(const ConvSpec& pSpec) const throw (StoreableFilterException);
    virtual bool isNull() const;
    
    virtual void fromInt(const ConvSpec& pSpec, int pData) throw (StoreableFilterException);
    virtual void fromDouble(const ConvSpec& pSpec, double pData) throw (StoreableFilterException);
    virtual void fromString(const ConvSpec& pSpec, const std::string& pData) throw (StoreableFilterException);
    virtual void fromDate(const ConvSpec& pSpec, const tm& pDate) throw (StoreableFilterException);
    virtual void fromNull() throw (StoreableFilterException);
    virtual Database::StoreType getPrefferedStoreType() const { return Database::FLOAT; }
    virtual ~vDouble();
};

/**
Class for converting float to and from database types. Uses double filter internally.
@ingroup filters
*/
class dbaDLLEXPORT vFloat : public StoreableFilter<float> {
  public:
    vFloat() {}
    /**
      Constructor
      @param pFloat reference to object or class member that filer will modify
    */
    vFloat(float& pFloat);
    virtual std::string toString(const ConvSpec& pSpec) const throw (StoreableFilterException);
    virtual bool isNull() const;
    
    virtual void fromInt(const ConvSpec& pSpec, int pData) throw (StoreableFilterException);
    virtual void fromDouble(const ConvSpec& pSpec, double pData) throw (StoreableFilterException);
    virtual void fromString(const ConvSpec& pSpec, const std::string& pData) throw (StoreableFilterException);
    virtual void fromDate(const ConvSpec& pSpec, const tm& pDate) throw (StoreableFilterException);
    virtual void fromNull() throw (StoreableFilterException);
    virtual Database::StoreType getPrefferedStoreType() const { return Database::FLOAT; }
    virtual ~vFloat();
};

}

#endif
