#ifndef GIMBAL_GENERATOR_H
#define GIMBAL_GENERATOR_H

CLIENT_DECLARE_ENUM(CLIENT_RESULT) {
    CLIENT_RESULT_START = GIM_RESULT_COUNT,
    CLIENT_RESULT_COUNT = CLIENT_RESULT_START+1
}

typedef struct ClientContextCreateInfo {
    GimContextCreateInfo base_;
    //Extra create params
};

typedef struct ClientContext {
    GimContext base_;
    //Extra state data
} ClientContext;

CLIENT_API CLIENT_RESULT clientContextInit(ClientContext* pCtx,
                                           ClientContextCreateInfo* pInfo)
{
    gimClientContextInit(pCtx->base_, pInfo->base_);
    //Your contructor logic here, sunshine
}

CLIENT_API CLIENT_RESULT clientContextDeinit(ClientContext* pCtx)
{
    gimClientContextDeinit(pCtx->base_);
    //Your destructor logic here, sunshine

}

#define clientContextProcessArgs    gimContextProcessArgs
#define clientContextUserdata       gimContextUserdata
#define clientContextFromUserdata   gimContextUserdata
#define clientContextApiCookieTop   gimContextApiCookieTop
#define clientContextApiCookiePush  gimContextApiCookiePush
#define clientContextApiCookiePop   gimContextApiCookiePop
#define clientContextEventEmit      gimContextEventEmit
#define clientContextLastError      glmContextLastError



GIM_API CLIENT_RESULT clientResultString(CLIENT_RESULT result, const char** ppPtr);




#endif // GIMBAL_GENERATOR_H
