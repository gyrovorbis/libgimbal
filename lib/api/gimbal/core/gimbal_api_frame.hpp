#ifndef GIMBAL_API_FRAME_HPP
#define GIMBAL_API_FRAME_HPP

#include "gimbal_api_frame.h"

#define GBL_API_TRY_BLOCK \
    Exception::TryBlock({SRC_FILE, SRC_FN, SRC_LN, SRC_COL}) = [&]()


#define GBL_API_TRY \
    GBL_STMT_START {    \
        auto blk = GBL_API_TRY_BLOCK

#define GBL_API_VERIFY_CATCH() \
        ; \
        if(!blk) {  \
            GBL_API_RECORD() = blk.getRecord(); \
            GBL_API_RECORD_HANDLER(&blk.getRecord()); \
            GBL_API_DONE(); \
        } \
    } GBL_STMT_END




#endif // GIMBAL_API_HPP
