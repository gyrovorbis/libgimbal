#include <gimbal/utils/gimbal_cmd_parser.h>
#include <gimbal/strings/gimbal_string_buffer.h>
#include <gimbal/strings/gimbal_string_view.h>
#include <gimbal/meta/types/gimbal_variant.h>

#define GBL_CMD_PARSER_(self) ((GblCmdParser_*)GBL_INSTANCE_PRIVATE(self, GBL_CMD_PARSER_TYPE))

GBL_DECLARE_STRUCT_PRIVATE(GblCmdParser) {
    int              argc;
    const char**     pArgv;
    const GblCmdArg* pArgInfo;
    GblSize          argInfoCount;
    GblBool          extraArgsDisabled;
    GblBool          executableArgDisabled;
    GblBool          unknownOptionsDisabled;

    GBL_CMD_STATUS   status;
    GblStringRef*    pErrorMsg;

    GblStringRef*    pExecutable;
    GblStringList*   pKnownArgs;
    GblStringList*   pExtraArgs;
    GblStringList*   pUnknownOptions;
};

GblType argToGblType_(GBL_ARG_TYPE type);

static void GblCmdParser_setError_(GblCmdParser_* pSelf_, GBL_CMD_STATUS status, const char* pFmt, ...) {
    struct {
        GblStringBuffer str;
        char            stackBytes[255];
    } errorMsg;

    pSelf_->status = status;

    if(pFmt) {
        va_list varArgs;
        va_start(varArgs, pFmt);
        GblStringBuffer_construct(&errorMsg.str, GBL_STRV(""), sizeof(errorMsg.stackBytes));
        GblStringBuffer_appendVPrintf(&errorMsg.str, pFmt, varArgs);
        pSelf_->pErrorMsg = GblStringRef_create(GblStringBuffer_cString(&errorMsg.str));
        va_end(varArgs);
    }
}

static GBL_RESULT GblCmdParser_parseArg_(GblCmdParser_* pSelf_, GblSize index, GblStringRef* pValue) {
    GBL_CTX_BEGIN(NULL);
    GblVariant v1 = GBL_VARIANT_INIT;
    GblVariant v2 = GBL_VARIANT_INIT;
#if 1
    GBL_ASSERT(GBL_FALSE);
#else
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueMove(&v1,
                                                      GBL_STRING_TYPE,
                                                      GblStringRef_acquire(pValue)));
    GBL_CTX_VERIFY_CALL(GblVariant_constructDefault(&v2,
                                                    GblCmdParser_toGblType_(pSelf_->pArgInfo[index].type)));

    GBL_CTX_VERIFY_CALL(GblVariant_convert(&v1, &v2));

    GBL_CTX_VERIFY_CALL(GblVariant_getValuePeek(&v2, pSelf_->pArgInfo[index].pData));


    GBL_CTX_END_BLOCK();
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v1));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v2));
#endif
    return GBL_CTX_RESULT();
}

static GblStringList* GblCmdParser_takeArg_(GblCmdParser_* pSelf_, GblStringList* pNode) {
    GblStringList* pNext = pNode->ringNode.pNext;
    GblDoublyLinkedList_remove(&pNode->listNode);

    if(!pSelf_->executableArgDisabled && !pSelf_->pExecutable) {
        pSelf_->pExecutable = GblStringRef_acquire(pNode->pData);
    } else if(GblStringList_size(pSelf_->pKnownArgs) < pSelf_->argInfoCount) {
        GblDoublyLinkedList_pushBack(&pSelf_->pKnownArgs->listNode, &pNode->listNode);
    } else if(pSelf_->extraArgsDisabled) {
        GblCmdParser_setError_(pSelf_,
                               GBL_CMD_STATUS_EXTRA_POSITIONAL,
                               "Too many positional arguments were provided! [%s]",
                               pNode->pData);
    } else {
        GblDoublyLinkedList_pushBack(&pSelf_->pExtraArgs->listNode, &pNode->listNode);
    }

    return pNext;
}

static GblStringList* GblCmdParser_takeOption_(GblCmdParser_* pSelf_, GblStringList* pNode) {
    GblStringList* pNext = pNode->ringNode.pNext;
    GBL_CTX_BEGIN(NULL);

    if(pSelf_->unknownOptionsDisabled) {
        GblCmdParser_setError_(pSelf_,
                               GBL_CMD_STATUS_UNKNOWN_OPTION,
                               "Unkown option was provided! [%s]",
                               pNode->pData);
    } else {

        GblDoublyLinkedList_remove(&pNode->listNode);
        GblDoublyLinkedList_pushBack(&pSelf_->pUnknownOptions->listNode, &pNode->listNode);
    }

    GBL_CTX_END_BLOCK();
    return pNext;
}

GBL_EXPORT GblBool GblCmdParser_parse(GblCmdParser* pSelf, int argc, const char** pArgv) {
    GblVariant v1 = GBL_VARIANT_INIT;
    GblVariant v2 = GBL_VARIANT_INIT;

    GBL_CTX_BEGIN(NULL);
    GblCmdParser_* pSelf_ = GBL_CMD_PARSER_(pSelf);

    pSelf_->argc  = argc;
    pSelf_->pArgv = pArgv;

    GblStringList* pList = GblStringList_createFromArray(pArgv, argc);

    pSelf_->status = GBL_CMD_STATUS_PARSING;

    GblStringList* pIt = pList->ringNode.pNext;
    while(pIt != pList) {
        GblStringRef* pString = pIt->pData;
        GblStringView view = GblStringRef_view(pString);

        if(GblStringView_startsWith(view, GBL_STRV("-"))) {
            pIt = GblCmdParser_takeOption_(pSelf_, pIt);
        } else {
            pIt = GblCmdParser_takeArg_(pSelf_, pIt);
        }

        if(GBL_CMD_ERROR(pSelf_->status)) GBL_CTX_DONE();
    }

    // emit pre-parse

    // Check argument requirement
    if(GblStringList_size(pSelf_->pKnownArgs) < pSelf_->argInfoCount) {
        GblCmdParser_setError_(pSelf_,
                               GBL_CMD_STATUS_MISSING_POSITIONAL,
                               "Not enough positional arguments were provided!");
        GBL_CTX_DONE();
    }

    // Check missing options
    // @todo

    GblSize a = 0;
    for(pIt = pSelf_->pKnownArgs->ringNode.pNext;
        pIt != pSelf_->pKnownArgs;
        pIt = pIt->ringNode.pNext)
    {
        if(pSelf_->pArgInfo[a].pData) {
            GBL_CTX_VERIFY_CALL(GblCmdParser_parseArg_(pSelf_, a, pIt->pData));
        }
        ++a;
    }

    // emit post-parse
    pSelf_->status = GBL_CMD_STATUS_SUCCESS;

    GBL_CTX_END_BLOCK();
    GblStringList_destroy(pList);
    GblVariant_destruct(&v1);
    GblVariant_destruct(&v2);

    if(GBL_RESULT_ERROR(GBL_CTX_RESULT()))
        GblCmdParser_setError_(pSelf_,
                               GBL_CMD_STATUS_ERROR,
                               "Internal Error: [%s]", GBL_CTX_RECORD().message);
    return !GBL_CMD_ERROR(pSelf_->status);
}

void GblCmdParser_setArgs(GblCmdParser* pSelf, const GblCmdArg* pArgs) {
    GblCmdParser_* pSelf_ = GBL_CMD_PARSER_(pSelf);
    pSelf_->pArgInfo = pArgs;

    // count args
    for(const GblCmdArg* pIt = pArgs; pIt->pName; ++pIt)
        ++pSelf_->argInfoCount;
}

GBL_EXPORT GblSize GblCmdParser_argCount(const GblCmdParser* pSelf) {
    GblCmdParser_* pSelf_ = GBL_CMD_PARSER_(pSelf);
    return GblStringList_size(pSelf_->pKnownArgs) +
           GblStringList_size(pSelf_->pExtraArgs);
}

GBL_EXPORT GBL_RESULT GblCmdParser_arg(const GblCmdParser* pSelf,
                                       GblSize             index,
                                       GBL_ARG_TYPE        toType,
                                       void*               pData)
{
    GblCmdParser_* pSelf_ = GBL_CMD_PARSER_(pSelf);
    return GblCmdParser_parseArg_(pSelf_,
                                  index,
                                  GblStringList_at(pSelf_->pKnownArgs, index));
}

static GBL_RESULT GblCmdParser_init_(GblInstance* pInstance, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblCmdParser_* pSelf_ = GBL_CMD_PARSER_(pInstance);

    pSelf_->pKnownArgs      = GblStringList_createEmpty();
    pSelf_->pExtraArgs      = GblStringList_createEmpty();
    pSelf_->pUnknownOptions = GblStringList_createEmpty();

    GBL_CTX_END();
}

static GBL_RESULT GblCmdParser_final_(GblBox* pInstance) {
    GBL_CTX_BEGIN(NULL);
    GblCmdParser_* pSelf_ = GBL_CMD_PARSER_(pInstance);

    GblStringRef_release(pSelf_->pExecutable);
    GblStringRef_release(pSelf_->pErrorMsg);

    GblStringList_destroy(pSelf_->pKnownArgs);
    GblStringList_destroy(pSelf_->pExtraArgs);
    GblStringList_destroy(pSelf_->pUnknownOptions);

    GBL_CTX_END();
}

static GBL_RESULT GblCmdParserClass_init_(GblClass* pClass, const void* pUd, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GBL_UNUSED(pUd);

    GBL_BOX_CLASS(pClass)->pFnDestructor = GblCmdParser_final_;
    GBL_CTX_END();
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
