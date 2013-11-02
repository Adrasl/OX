//
// C++ Interface: sqlparamparsertestcase
//
// Description: 
//
//
// Author: Lukasz Michalski <lm@zork.pl>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DBA_TESTSSQLPARAMPARSERTESTCASE_H
#define DBA_TESTSSQLPARAMPARSERTESTCASE_H

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

namespace dba_tests {

/**
	@author Lukasz Michalski <lm@zork.pl>
*/
class SQLParamParserTestCase : public CppUnit::TestCase {
  public:
    CPPUNIT_TEST_SUITE(SQLParamParserTestCase);
      CPPUNIT_TEST(noParams);
      CPPUNIT_TEST(singleParamAlone);
      CPPUNIT_TEST(singleParamInBrackets);
      CPPUNIT_TEST(singleEscaped);
      CPPUNIT_TEST(singleStringInQuotes);
      CPPUNIT_TEST(singleStringNoQuotes);
      CPPUNIT_TEST(likeAlone);
      CPPUNIT_TEST(likeInQuotes);
      CPPUNIT_TEST(likeOneParam);
      CPPUNIT_TEST(likeTwoParams);
      CPPUNIT_TEST(likeTwoParamsLikeBetween);
      CPPUNIT_TEST(likeOneParamWildcardPrepended);
      CPPUNIT_TEST(likeOneParamWildcardSPrepended);
      CPPUNIT_TEST(likeOneParamWildcardAppended);
      CPPUNIT_TEST(likeLike);
      CPPUNIT_TEST(likeEscape1);
      CPPUNIT_TEST(likeEscape2);
      CPPUNIT_TEST(quotes);
    CPPUNIT_TEST_SUITE_END();
  public:
    void noParams();
    void singleParamAlone();
    void singleParamInBrackets();
    void singleEscaped();
    void singleStringInQuotes();
    void singleStringNoQuotes();
    void likeAlone();
    void likeInQuotes();
    void likeOneParam();
    void likeOneParamNoQuotes();
    void likeTwoParams();
    void likeTwoParamsLikeBetween();
    void likeOneParamWildcardPrepended();
    void likeOneParamWildcardSPrepended();
    void likeOneParamWildcardAppended();
    void likeLike();
    void likeEscape1();
    void likeEscape2();
    void paramAndEscape();
    void likeNoLike();
    void quotes();
};

} //namespace

#endif
