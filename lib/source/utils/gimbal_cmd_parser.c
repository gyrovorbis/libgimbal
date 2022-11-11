#include <gimbal/utils/gimbal_cmd_parser.h>
#include <gimbal/containers/gimbal_array_map.h>
#include <gimbal/containers/gimbal_array_list.h>

GBL_DECLARE_STRUCT_PRIVATE(GblCmdParser) {
    GblArrayMap* pContextMap;
    GblArrayMap* pOptionalMap;
    GblArrayList positionalList;
    GblArrayMap* pUnknownOptionaMap;
    GblArrayList extraPositionalList;
};

static GBL_RESULT GblCmdParser_init_(GblInstance* pInstance, GblContext* pCtx) {

}

static GBL_RESULT GblCmdParser_final_(GblBox* pInstance) {

}

static GBL_RESULT GblCmdParserClass_init_(GblClass* pClass, const void* pUd, GblContext* pCtx) {
    GblCmdParserClass* pParserClass = GBL_CMD_PARSER_CLASS(pClass);

    GBL_BOX_CLASS(pClass)->pFnDestructor = GblCmdParser_final_;
}

GBL_EXPORT GblType GblCmdParser_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if(type == GBL_INVALID_TYPE) {
        GBL_CTX_BEGIN(NULL);

        type = GblType_registerStatic(GblQuark_internStringStatic("GblCmdParser"),
                                      GBL_OBJECT_TYPE,
                                      &(GblTypeInfo) {
                                          .pFnClassInit        = GblCmdParserClass_init_,
                                          .classSize           = sizeof(GblCmdParserClass),
                                          .pFnInstanceInit     = GblCmdParser_init_,
                                          .instanceSize        = sizeof(GblCmdParser),
                                          .instancePrivateSize = sizeof(GblCmdParser_)
                                      },
                                      GBL_TYPE_FLAGS_NONE);

        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();

    }

    return type;
}
