#ifndef GIMBAL_CMD_PARSER_TEST_SUITE_H
#define GIMBAL_CMD_PARSER_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_CMD_PARSER_TEST_SUITE_TYPE             (GBL_TYPEID(GblCmdParserTestSuite))

#define GBL_CMD_PARSER_TEST_SUITE(inst)          (GBL_CAST(GblCmdParserTestSuite, inst))
#define GBL_CMD_PARSER_TEST_SUITE_CLASS(klass)   (GBL_CLASS_CAST(GblCmdParserTestSuite, klass))
#define GBL_CMD_PARSER_TEST_SUITE_CLASSOF(inst)  (GBL_CLASSOF(GblCmdParserTestSuite, inst))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblCmdParserTestSuite, GblTestSuite)

GBL_INSTANCE_DERIVE_EMPTY(GblCmdParserTestSuite, GblTestSuite)

GBL_EXPORT GblType GblCmdParserTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_CMD_PARSER_TEST_SUITE_H
