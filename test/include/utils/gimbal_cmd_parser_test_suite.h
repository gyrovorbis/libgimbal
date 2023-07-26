#ifndef GIMBAL_CMD_PARSER_TEST_SUITE_H
#define GIMBAL_CMD_PARSER_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_CMD_PARSER_TEST_SUITE_TYPE             (GBL_TYPEID(GblCmdParserTestSuite))

#define GBL_CMD_PARSER_TEST_SUITE(inst)            (GBL_CAST(inst, GBL_CMD_PARSER_TEST_SUITE_TYPE, GblCmdParserTestSuite))
#define GBL_CMD_PARSER_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST(klass, GBL_CMD_PARSER_TEST_SUITE_TYPE, GblCmdParserTestSuiteClass))
#define GBL_CMD_PARSER_TEST_SUITE_GET_CLASS(inst)  (GBL_INSTANCE_GET_CLASS_CAST(inst, GBL_CMD_PARSER_TEST_SUITE_TYPE, GblCmdParserTestSuiteClass))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblCmdParserTestSuite, GblTestSuite)

GBL_INSTANCE_DERIVE_EMPTY(GblCmdParserTestSuite, GblTestSuite)

GBL_EXPORT GblType GblCmdParserTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_CMD_PARSER_TEST_SUITE_H
