#ifndef GIMBAL_CMDLINE_PARSER_H
#define GIMBAL_CMDLINE_PARSER_H

#include "gimbal_api.h"
#include "gimbal_container.h"

#define GBL_CMDLINE_PARSER_POSITIONAL_ARG_VECTOR_INITIAL_CAPACITY   5
#define GBL_CMDLINE_PARSER_OPTION_VECTOR_INITIAL_CAPACITY           5
#define GBL_CMDLINE_PARSER_UNKNOWN_OPTION_VECTOR_INITIAL_CAPACITY   3

#ifdef __cplusplus
extern "C" {
#endif

#define GBL_META_CMDLINE_PARSER_STATUS_TABLE (                                                                          \
        ( GBL_CMDLINE_PARSER_STATUS, CmdLineParserStatus, "Command-Line Parser Status", gblCmdLineParserStatusString),  \
        (                                                                                                               \
            (GBL_CMDLINE_PARSER_STATUS_PENDING,     0x0, Pending,       "Pending"),                                     \
            (GBL_CMDLINE_PARSER_STATUS_COMPLETE,    0x1, Complete,      "Complete"),                                    \
            (GBL_CMDLINE_PARSER_STATUS_UNEXPECTED,  0x2, Unexpected,    "Unexpected"),                                  \
            (GBL_CMDLINE_PARSER_STATUS_ERROR,       0x3, Error,         "Error"),                                       \
            (GBL_CMDLINE_PARSER_STATUS_HELP,        0x4, Help,          "Help"),                                        \
            (GBL_CMDLINE_PARSER_STATUS_VERSION,     0x5, Version,       "Version"),                                     \
            (GBL_CMDLINE_PARSER_STATUS_COUNT,       0x6, Count,         "Count")                                        \
        )                                                                                                               \
    )

GBL_ENUM_TABLE_DECLARE(GBL_META_CMDLINE_PARSER_STATUS_TABLE);

typedef struct GblCmdLinePositionalArg {
    const char* pName;
    const char* pDesc;
    const char* pValue;
} GblCmdLinePositionalArg;

typedef struct GblCmdLineOption {
    const char* pName;
    const char* pDesc;
    const char* pOptionShort;
    const char* pOptionLong;
    const char* pDefaultValue;
    const char* pValue;
} GblCmdLineOption;

typedef struct GblCmdLineParser {
    GblContext                  hCtx;
    int                         argc;
    const char**                ppArgv;
    GBL_CMDLINE_PARSER_STATUS   status;
    GblVector                   positionalArgs;
    GblVector                   options;
    GblVector                   unknownOptions;
    struct {
        const char*             pName;
        //          name, desc, version
    }                           appInfo;
} GblCmdLineParser;


GBL_INLINE GBL_API gblCmdLineParserConstruct(GblCmdLineParser* pParser, GblContext hCtx) {
    GBL_API_BEGIN(hCtx);
    GBL_API_VERIFY_POINTER(pParser);
    memset(pParser, 0, sizeof(GblCmdLineParser));
    pParser->hCtx = hCtx;
    GBL_API_CALL(gblVectorConstruct(&pParser->positionalArgs, hCtx, sizeof(GblCmdLinePositionalArg),    NULL, 0, GBL_CMDLINE_PARSER_POSITIONAL_ARG_VECTOR_INITIAL_CAPACITY));
    GBL_API_CALL(gblVectorConstruct(&pParser->options,        hCtx, sizeof(GblCmdLineOption),           NULL, 0, GBL_CMDLINE_PARSER_OPTION_VECTOR_INITIAL_CAPACITY));
    GBL_API_CALL(gblVectorConstruct(&pParser->unknownOptions, hCtx, sizeof(GblCmdLineOption),           NULL, 0, GBL_CMDLINE_PARSER_UNKNOWN_OPTION_VECTOR_INITIAL_CAPACITY));
    GBL_API_END();
}

GBL_INLINE GBL_API gblCmdLineParserDestruct(GblCmdLineParser* pParser) {
    GBL_ASSERT(pParser);
    GBL_API_BEGIN(pParser->hCtx);
    GBL_API_CALL(gblVectorDestruct(&pParser->positionalArgs));
    GBL_API_CALL(gblVectorDestruct(&pParser->options));
    GBL_API_CALL(gblVectorDestruct(&pParser->unknownOptions));
    GBL_API_END();
}

GBL_INLINE GBL_API gblCmdLineParserPositionalArgAdd(GblCmdLineParser* pParser, const char* pName, const char* pDesc) {
    const GblCmdLinePositionalArg arg = {
        pName, pDesc, NULL
    };
    GBL_ASSERT(pParser);
    GBL_API_BEGIN(pParser->hCtx);
    GBL_API_VERIFY_POINTER(pName);
    GBL_API_VERIFY_POINTER(pDesc);
    GBL_API_CALL(gblVectorPushBack(&pParser->positionalArgs, &arg));
    GBL_API_END();
}

GBL_INLINE GBL_API gblCmdLineParserOptionAdd(GblCmdLineParser* pParser, const char* pName, const char* pOptionShort, const char* pOptionLong, const char* pDesc, const char* pDefaultValue) {
    const GblCmdLineOption option = {
        pName, pDesc, pOptionShort, pOptionLong, pDefaultValue, NULL
    };
    GBL_ASSERT(pParser);
    GBL_API_BEGIN(pParser->hCtx);
    GBL_API_VERIFY_POINTER(pName);
    GBL_API_VERIFY_POINTER(pDesc);
    GBL_API_VERIFY_POINTER(pOptionShort);
    GBL_API_VERIFY_POINTER(pOptionLong);
    //GBL_API_VERIFY_POINTER(pDefaultValue);
    GBL_API_CALL(gblVectorPushBack(&pParser->options, &option));
    GBL_API_END();
}

GBL_INLINE GBL_API gblCmdLineParserOptionAddHelp(GblCmdLineParser* pParser) {
    return gblCmdLineParserOptionAdd(pParser, "help", "h", "help", "Help Information", NULL);
}

GBL_INLINE GBL_API gblCmdLineParserOptionAddVersion(GblCmdLineParser* pParser) {
    return gblCmdLineParserOptionAdd(pParser, "version", "v", "version", "Version Info", NULL);
}

GBL_INLINE GBL_API gblCmdLineParserOptionValue(const GblCmdLineParser* pParser, const char* pOptionName, const char** ppValue) {
    GblSize size = 0;
    GblCmdLineOption* pOption = NULL;
    *ppValue = NULL;
    GBL_ASSERT(pParser);
    GBL_API_BEGIN(pParser->hCtx);
    GBL_API_VERIFY_POINTER(pOptionName);
    GBL_API_VERIFY_POINTER(ppValue);
    GBL_API_CALL(gblVectorSize(&pParser->options, &size));
    for(GblSize o = 0; o < size; ++o) {
        GBL_API_CALL(gblVectorAt(&pParser->options, &pOption));
        GBL_API_VERIFY_POINTER(pOption);
        if(strcmp(pOption->pName, pOptionName) == 0) {
            *ppValue = pOption->pValue? pOption->pValue : pOption->pDefaultValue;
            GBL_API_DONE();
        }
    }
    GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_CMDLINE_ARG);
    GBL_API_END();
}

GBL_INLINE GBL_API gblCmdLineParserPositionalArgValue(const GblCmdLineParser* pParser, uint32_t argPos, const char** ppValue) {
    GblSize size = 0;
    GblCmdLinePositionalArg* pArg = NULL;
    *ppValue = NULL;
    GBL_ASSERT(pParser);
    GBL_API_BEGIN(pParser->hCtx);
    GBL_API_VERIFY_POINTER(ppValue);
    GBL_API_CALL(gblVectorSize(&pParser->positionalArgs, &size));
    GBL_API_VERIFY(argPos < size, GBL_RESULT_ERROR_INVALID_CMDLINE_ARG, "Index %u is out of range!", argPos);
    GBL_API_CALL(gblVectorAt(&pParser->positionalArgs, &pArg));
    GBL_API_VERIFY_POINTER(pArg);
    *ppValue = pArg->pValue;
    GBL_API_END();
}

//pass NULL ppOptionName to get # of unknown options/size! (or iterate until NULL?)
GBL_INLINE GBL_API gblCmdLineParserUnknownOption(const GblCmdLineParser* pParser, GblSize index, const char** ppOptionName) {
    GblSize size = 0;
    GblCmdLineOption* pOption = NULL;
    GBL_ASSERT(pParser);
    GBL_API_BEGIN(pParser->hCtx);
    GBL_API_VERIFY_POINTER(ppOptionName);
    *ppOptionName = NULL;
    GBL_API_CALL(gblVectorSize(&pParser->unknownOptions));
    GBL_API_VERIFY(index < size, GBL_RESULT_ERROR_INVALID_CMDLINE_ARG, "Index %u is out of range!", index);
    GBL_API_CALL(gblVectorAt(&pParser->unknownOptions, index));
    *ppOptionName = pOption->pName;
    GBL_API_END();
}

GBL_INLINE GBL_API gblCmdLineParserParse(GblCmdLineParser* pParser, int argc, const char** ppArgv) {
    GBL_ASSERT(pParser);
    GBL_API_BEGIN(pParser->hCtx);
    GBL_API_VERIFY_ARG(argc > 0);
    GBL_API_VERIFY_POINTER(ppArgv);
    pParser->argc   = argc;
    pParser->ppArgv = ppArgv;

    uint32_t positionalArgCount = 0;
    uint32_t newPositionalArgCount = 0;
    uint32_t unknownOptionCount = 0;

    // ========= PARSE THE SHIT ===============
    for(int a = 0; a < argc; ++a) {
        const char* pArgv = ppArgv[a];
        GBL_API_VERIFY_POINTER(pArgv);

        if(pArgv[0] == '-') {
            GblBool found = GBL_FALSE;
            GblBool shortOption = GBL_TRUE;
            const char* pOptionBuff = &pArgv[1];

            GBL_API_VERIFY_EXPRESSION(a+1 < argc);

            if(pOptionBuff[0] == '-') { // long
                shortOption = GBL_FALSE;
                pOptionBuff = &pOptionBuff[1];
            }

            GblSize optionSize = 0;
            GblCmdLineOption* pOption = NULL;
            GBL_API_CALL(gblVectorSize(&pParser->options, &optionSize));

            for(GblSize o = 0; o < optionSize; ++o) {
                GBL_API_CALL(gblVectorAt(&pParser->options, &pOption));
                GBL_API_VERIFY_POINTER(pOption);

                if((!shortOption && strcmp(pOption->pOptionLong, pOptionBuff) == 0)
                    || (shortOption && strcmp(pOption->pOptionShort, pOptionBuff) == 0)) {
                    pOption->pValue = ppArgv[a+1];
                    found = GBL_TRUE;
                    break;
                }
            }

            if(!found) {
                GblCmdLineOption option = { pOptionBuff, NULL, pOptionBuff, pOptionBuff, NULL, ppArgv[a+1] };
                GBL_API_CALL(gblVectorPushBack(&pParser->unknownOptions, &option));
                ++unknownOptionCount;
            } else {
                if(strcmp(pOption->pName, "help") == 0) {
                    pParser->status = GBL_CMDLINE_PARSER_STATUS_HELP;
                } else if(strcmp(pOption->pName, "version")) {
                    pParser->status = GBL_CMDLINE_PARSER_STATUS_VERSION;
                }
            }
            ++a; // consume its value

        } else { // positional arguments
            GblCmdLinePositionalArg* pArg = NULL;
            GblSize size = 0;
            GBL_API_CALL(gblVectorSize(&pParser->positionalArgs));
            if(positionalArgCount < size) {
                GBL_API_CALL(gblVectorAt(&pParser->positionalArgs, positionalArgCount, &pArg));
                pArg->pValue = pArgv;
            } else {
                GblCmdLinePositionalArg arg = {NULL, NULL, pArgv};
                GBL_API_CALL(gblVectorPushBack(&pParser->positionalArgs, &arg));
                ++newPositionalArgCount;
            }
            ++positionalArgCount;
        }
    }
    // ========= / PARSE THE SHIT ===============
    GBL_API_END_BLOCK();
    if(GBL_RESULT_SUCCESS(GBL_API_RESULT())) {
        // nothing special happened previously
        if(pParser->status == GBL_CMDLINE_PARSER_STATUS_PENDING) {
            if(!newPositionalArgCount && !unknownOptionCount) {
                // nothing sketchy to report!
                pParser->status = GBL_CMDLINE_PARSER_STATUS_COMPLETE;
            } else {
                // found some unsupported shit!
                pParser->status = GBL_CMDLINE_PARSER_STATUS_UNEXPECTED;
            }
        }
    } else {
        pParser->status = GBL_CMDLINE_PARSER_STATUS_ERROR;
    }
    return GBL_API_RESULT();
}

GBL_INLINE GBL_API gblCmdLineParserShowHelp(GblCmdLineParser* pParser) {
    GBL_ASSERT(pParser);
    GBL_API_BEGIN(pParser->hCtx);
    GBL_API_VERBOSE("REQUESTED HELP INFO!");
    GBL_API_END();
}
GBL_INLINE GBL_API gblCmdLineParserShowVersion(GblCmdLineParser* pParser) {
    GBL_ASSERT(pParser);
    GBL_API_BEGIN(pParser->hCtx);
    GBL_API_VERBOSE("REQUESTED VERSION INFO!");
    GBL_API_END();
}
GBL_INLINE GBL_API gblCmdLineParserShowError(GblCmdLineParser* pParser) {
    GBL_ASSERT(pParser);
    GBL_API_BEGIN(pParser->hCtx);
    GBL_API_ERROR("ERRROR WITH COMMANDLINEPARSER PARSAGE!");
    GBL_API_END();
}

GBL_INLINE GBL_API gblCmdLineParserShowWarning(GblCmdLineParser* pParser) {
    GBL_ASSERT(pParser);
    GBL_API_BEGIN(pParser->hCtx);
    GBL_API_WARN("WARNINGZ: COMMANDLINEPARSER PARSAGE!");
    GBL_API_END();
}

GBL_INLINE GBL_API gblCmdLineParserProcess(GblCmdLineParser* pParser, int argc, const char** ppArgv) {
    GBL_ASSERT(pParser);
    GBL_API_BEGIN(pParser);
    GBL_API_CALL(gblCmdLineParserParse(pParser, argc, ppArgv));
    switch(pParser->status) {
    case GBL_CMDLINE_PARSER_STATUS_ERROR:       GBL_API_CALL(gblCmdLineParserShowError(pParser));   break;
    case GBL_CMDLINE_PARSER_STATUS_UNEXPECTED:  GBL_API_CALL(gblCmdLineParserShowWarning(pParser)); break;
    case GBL_CMDLINE_PARSER_STATUS_HELP:        GBL_API_CALL(gblCmdLineParserShowHelp(pParser));    break;
    case GBL_CMDLINE_PARSER_STATUS_VERSION:     GBL_API_CALL(gblCmdLineParserShowVersion(pParser)); break;
    default: break;
    }
    GBL_API_END();
}


#ifdef __cpluspus
}
#endif

#endif // GIMBAL_CMDLINE_PARSER_H
