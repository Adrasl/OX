//
// C++ Implementation: sqlparamparsertestcase
//
// Description: 
//
//
// Author: Lukasz Michalski <lm@zork.pl>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "sqlparamparsertestcase.h"

#define private public //for access to tested dba::SQL::SQLParamParser
#include "dba/sql.h"

namespace dba_tests {

CPPUNIT_TEST_SUITE_REGISTRATION(SQLParamParserTestCase);

void
SQLParamParserTestCase::noParams() {
  dba::SQL::Placeholders h;
  std::string q("n");

  dba::SQL::SQLParamParser parser(q,h);
  parser.parse();

  CPPUNIT_ASSERT(h.size() == 0);
};

void
SQLParamParserTestCase::singleParamAlone() {
  dba::SQL::Placeholders h;
  std::string q(":d");

  dba::SQL::SQLParamParser parser(q,h);
  parser.parse();

  CPPUNIT_ASSERT(h.size() == 1);
  CPPUNIT_ASSERT(h[0] == dba::SQL::ParamPlaceholder(0,dba::SQL::PARAM_INT));
};

void
SQLParamParserTestCase::singleParamInBrackets() {
  dba::SQL::Placeholders h;
  std::string q("(:d)");

  dba::SQL::SQLParamParser parser(q,h);
  parser.parse();

  CPPUNIT_ASSERT(h.size() == 1);
  CPPUNIT_ASSERT(h[0] == dba::SQL::ParamPlaceholder(1,dba::SQL::PARAM_INT));
};

void
SQLParamParserTestCase::singleEscaped() {
  dba::SQL::Placeholders h;
  std::string q("::");

  dba::SQL::SQLParamParser parser(q,h);
  parser.parse();

  CPPUNIT_ASSERT(h.size() == 1);
  CPPUNIT_ASSERT(h[0] == dba::SQL::ParamPlaceholder(0,dba::SQL::PARAM_ESCAPED));
};

void
SQLParamParserTestCase::singleStringInQuotes() {
  dba::SQL::Placeholders h;
  std::string q("':s'");

  dba::SQL::SQLParamParser parser(q,h);
  parser.parse();

  CPPUNIT_ASSERT(h.size() == 1);
  CPPUNIT_ASSERT(h[0] == dba::SQL::ParamPlaceholder(1,dba::SQL::PARAM_STR_VALUE));
};

void
SQLParamParserTestCase::singleStringNoQuotes() {
  dba::SQL::Placeholders h;
  std::string q(":s");

  dba::SQL::SQLParamParser parser(q,h);
  parser.parse();

  CPPUNIT_ASSERT(h.size() == 1);
  CPPUNIT_ASSERT(h[0] == dba::SQL::ParamPlaceholder(0,dba::SQL::PARAM_STR_VALUE,true));
};

void 
SQLParamParserTestCase::likeAlone() {
  dba::SQL::Placeholders h;
  std::string q("like");

  dba::SQL::SQLParamParser parser(q,h);
  parser.parse();

  CPPUNIT_ASSERT(h.size() == 0);
}

void 
SQLParamParserTestCase::likeInQuotes() {
  dba::SQL::Placeholders h;
  std::string q("'like'");

  dba::SQL::SQLParamParser parser(q,h);
  parser.parse();

  CPPUNIT_ASSERT(h.size() == 0);
}

void 
SQLParamParserTestCase::likeOneParam() {
  dba::SQL::Placeholders h;
  std::string q("like ':s'");

  dba::SQL::SQLParamParser parser(q,h);
  parser.parse();

  CPPUNIT_ASSERT(h.size() == 2);
  CPPUNIT_ASSERT(h[0] == dba::SQL::ParamPlaceholder(6,dba::SQL::PARAM_STR_LIKE,false));
  CPPUNIT_ASSERT(h[1] == dba::SQL::ParamPlaceholder(9,dba::SQL::PARAM_ESCAPE_CHAR,false));
}

void 
SQLParamParserTestCase::likeOneParamNoQuotes() {
  dba::SQL::Placeholders h;
  std::string q("like :s");

  dba::SQL::SQLParamParser parser(q,h);
  parser.parse();

  CPPUNIT_ASSERT(h.size() == 2);
  CPPUNIT_ASSERT(h[0] == dba::SQL::ParamPlaceholder(6,dba::SQL::PARAM_STR_LIKE,true));
  CPPUNIT_ASSERT(h[1] == dba::SQL::ParamPlaceholder(7,dba::SQL::PARAM_ESCAPE_CHAR,false));
}

void 
SQLParamParserTestCase::likeTwoParams() {
  dba::SQL::Placeholders h;
  std::string q("like ':s :s'");

  dba::SQL::SQLParamParser parser(q,h);
  parser.parse();

  CPPUNIT_ASSERT(h.size() == 3);
  CPPUNIT_ASSERT(h[0] == dba::SQL::ParamPlaceholder(6,dba::SQL::PARAM_STR_LIKE,false));
  CPPUNIT_ASSERT(h[1] == dba::SQL::ParamPlaceholder(9,dba::SQL::PARAM_STR_LIKE,false));
  CPPUNIT_ASSERT(h[2] == dba::SQL::ParamPlaceholder(12,dba::SQL::PARAM_ESCAPE_CHAR,false));
}

void 
SQLParamParserTestCase::likeTwoParamsLikeBetween() {
  dba::SQL::Placeholders h;
  std::string q("like ':s like :s'");

  dba::SQL::SQLParamParser parser(q,h);
  parser.parse();

  CPPUNIT_ASSERT(h.size() == 3);
  CPPUNIT_ASSERT(h[0] == dba::SQL::ParamPlaceholder(6,dba::SQL::PARAM_STR_LIKE,false));
  CPPUNIT_ASSERT(h[1] == dba::SQL::ParamPlaceholder(14,dba::SQL::PARAM_STR_LIKE,false));
  CPPUNIT_ASSERT(h[2] == dba::SQL::ParamPlaceholder(17,dba::SQL::PARAM_ESCAPE_CHAR,false));
}

void 
SQLParamParserTestCase::likeOneParamWildcardPrepended() {
  dba::SQL::Placeholders h;
  std::string q("like '%:s'");

  dba::SQL::SQLParamParser parser(q,h);
  parser.parse();

  CPPUNIT_ASSERT(h.size() == 2);
  CPPUNIT_ASSERT(h[0] == dba::SQL::ParamPlaceholder(7,dba::SQL::PARAM_STR_LIKE,false));
  CPPUNIT_ASSERT(h[1] == dba::SQL::ParamPlaceholder(10,dba::SQL::PARAM_ESCAPE_CHAR,false));
}

void 
SQLParamParserTestCase::likeOneParamWildcardSPrepended() {
  dba::SQL::Placeholders h;
  std::string q("like '::s:s'");

  dba::SQL::SQLParamParser parser(q,h);
  parser.parse();

  CPPUNIT_ASSERT(h.size() == 3);
  CPPUNIT_ASSERT(h[0] == dba::SQL::ParamPlaceholder(6,dba::SQL::PARAM_ESCAPED,false));
  CPPUNIT_ASSERT(h[1] == dba::SQL::ParamPlaceholder(9,dba::SQL::PARAM_STR_LIKE,false));
  CPPUNIT_ASSERT(h[2] == dba::SQL::ParamPlaceholder(12,dba::SQL::PARAM_ESCAPE_CHAR,false));
}

void 
SQLParamParserTestCase::likeOneParamWildcardAppended() {
  dba::SQL::Placeholders h;
  std::string q("like ':s%'");

  dba::SQL::SQLParamParser parser(q,h);
  parser.parse();

  CPPUNIT_ASSERT(h.size() == 2);
  CPPUNIT_ASSERT(h[0] == dba::SQL::ParamPlaceholder(6,dba::SQL::PARAM_STR_LIKE,false));
  CPPUNIT_ASSERT(h[1] == dba::SQL::ParamPlaceholder(10,dba::SQL::PARAM_ESCAPE_CHAR,false));
}


void 
SQLParamParserTestCase::likeLike() {
  dba::SQL::Placeholders h;
  std::string q("like 'like :s'");

  dba::SQL::SQLParamParser parser(q,h);
  parser.parse();

  CPPUNIT_ASSERT(h.size() == 2);
  CPPUNIT_ASSERT(h[0] == dba::SQL::ParamPlaceholder(11,dba::SQL::PARAM_STR_LIKE,false));
  CPPUNIT_ASSERT(h[1] == dba::SQL::ParamPlaceholder(14,dba::SQL::PARAM_ESCAPE_CHAR,false));
}

void 
SQLParamParserTestCase::likeNoLike() {
  dba::SQL::Placeholders h;
  std::string q("like 'a' ':s'");

  dba::SQL::SQLParamParser parser(q,h);
  parser.parse();

  CPPUNIT_ASSERT(h.size() == 2);
  CPPUNIT_ASSERT(h[0] == dba::SQL::ParamPlaceholder(10,dba::SQL::PARAM_STR_VALUE,false));
  CPPUNIT_ASSERT(h[1] == dba::SQL::ParamPlaceholder(13,dba::SQL::PARAM_ESCAPE_CHAR,false));
}

void 
SQLParamParserTestCase::likeEscape1() {
  dba::SQL::Placeholders h;
  std::string q("like '%'");

  dba::SQL::SQLParamParser parser(q,h);
  parser.parse();

  CPPUNIT_ASSERT(h.size() == 1);
  CPPUNIT_ASSERT(h[0] == dba::SQL::ParamPlaceholder(8,dba::SQL::PARAM_ESCAPE_CHAR,false));
}

void 
SQLParamParserTestCase::likeEscape2() {
  dba::SQL::Placeholders h;
  std::string q("like '%:s_'");

  dba::SQL::SQLParamParser parser(q,h);
  parser.parse();

  CPPUNIT_ASSERT(h.size() == 2);
  CPPUNIT_ASSERT(h[0] == dba::SQL::ParamPlaceholder(7,dba::SQL::PARAM_STR_LIKE,false));
  CPPUNIT_ASSERT(h[1] == dba::SQL::ParamPlaceholder(11,dba::SQL::PARAM_ESCAPE_CHAR,false));
}

void 
SQLParamParserTestCase::paramAndEscape() {
  dba::SQL::Placeholders h;
  std::string q(":s,'::'");

  dba::SQL::SQLParamParser parser(q,h);
  parser.parse();

  CPPUNIT_ASSERT(h.size() == 2);
  CPPUNIT_ASSERT(h[0] == dba::SQL::ParamPlaceholder(0,dba::SQL::PARAM_STR_VALUE,true));
  CPPUNIT_ASSERT(h[1] == dba::SQL::ParamPlaceholder(4,dba::SQL::PARAM_ESCAPED,false));
}


void 
SQLParamParserTestCase::quotes() {
  dba::SQL::Placeholders h;
  dba::SQL::SQLParamParser parser("'0'",h);
  parser.parse();

  CPPUNIT_ASSERT(h.size() == 0);
}

} //namespace
