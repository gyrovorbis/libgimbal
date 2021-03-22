#ifndef GIMBAL_BUILD_H
#define GIMBAL_BUILD_H

// give ifdefs for defaults
// give ifdefs for debug/release and local/official/ci builds

#include <gimbal_macros.h>

#ifndef GBL_VERSION_MAJOR
#   ifdef CMAKE_PROJECT_VERSION_MAJOR
#       define GBL_VERSION_MAJOR            CMAKE_PROJECT_VERSION_MAJOR
#   else
#       define GBL_VERSION_MAJOR            0
#   endif
#endif

#ifndef GBL_VERSION_MINOR
#   ifdef CMAKE_PROJECT_VERSION_MINOR
#       define GBL_VERSION_MINOR            CMAKE_PROJECT_VERSION_MINOR
#   else
#       define GBL_VERSION_MINOR            0
#   endif
#endif

#ifndef GBL_VERSION_PATCH
#   ifdef CMAKE_PROJECT_VERSION_PATCH
#       define GBL_VERSION_PATCH            CMAKE_PROJECT_VERSION_PATCH
#   else
#       define GBL_VERSION_PATCH            0
#   endif
#endif

#ifndef GBL_VERSION
#   ifdef PROJECT_VERSION
#       define GBL_VERSION                  GBL_STRINGIFY(PROJECT_VERSION)
#   else
#       define GBL_VERSION                  GBL_VERSION_MAKE(GBL_VERSION_MAJOR,
                                                         GBL_VERSION_MINOR,
                                                         GBL_VERSION_PATCH)
#   endif
#endif

#ifdef GBL_VERSION_STRING
#   ifdef PROJECT_VERSION
#       define GBL_VERSION_STRING           PROJECT_VERSION
#   else
#       define GBL_VERSION_STRING           GBL_VERSION_STRINGIFY(GBL_VERSION_MAJOR,
                                                                  GBL_VERSION_MINOR,
                                                                  GBL_VERSION_PATCH)
#endif

#ifndef GBL_PROJECT_NAME
#   ifdef CMAKE_PROJECT_NAME
#       define GBL_PROJECT_NAME             CMAKE_PROJECT_NAME
#   else
#       define GBL_PROJECT_NAME             "libGimbal"
#   endif
#endif

#ifndef GBL_PROJECT_DESCRIPTION
#   ifdef CMAKE_PROJECT_DESCRIPTION
#       define GBL_PROJECT_DESCRIPTION      CMAKE_PROJECT_DESCRIPTION
#   else
#       define GBL_PROJECT_DESCRIPTION      "?"
#   endif
#endif

#ifndef GBL_PROJECT_URL
#   ifdef CMAKE_PROJECT_HOMEPAGE_URL
#       define GBL_PROJECT_URL              CMAKE_PROJECT_HOMEPAGE_URL
#   else
#       define GBL_PROJECT_URL              "?"
#   endif
#endif

#ifndef GBL_COPYRIGHT_YEAR
#   define GBL_COPYRIGHT_YEAR               "2020-2021"
#endif

/* Use GBL_VERSION_STRING for compile-time version.
 * Use GBLVersionString() for run-time version.
 */
GBL_API const char* gimVersionString(void);



#define GBL_COMPILER_PATH      CMAKE_C_COMPILER
#define GBL_COMPILER_ID        CMAKE_C_COMPILER_ID
#define GBL_COMPILER_TARGET    CMAKE_C_COMPILER_TARGET
#define GBL_COMPILER_VERSION   CMAKE_C_COMPILER_VERSION

#define GBL_CI                   CI

#define GBL_CI_CHAT_CHANNEL      CHAT_CHANNEL //slack/discord server?
#define GBL_CI_CHAT_INPUT        CHAT_INPUT

#define GBL_CI_SERVER_URL        CI_SERVER_URL
#define GBL_CI_SERVER_NAME       CI_SERVER_NAME
#define GBL_CI_SERVER_REVISION   CI_SERVER_REVISION
#define GBL_CI_SERVER_VERSION    CI_SERVER_VERSION

#define GBL_CI_PROJECT_ID        CI_PROJECT_ID
#define GBL_CI_PROJECT_TITLE     CI_PROJECT_TITLE
#define GBL_CI_PROJECT_URL       CI_PROJECT_URL
#define GBL_CI_PROJECT_NAME      CI_PROJECT_NAME
#define GBL_CI_PROJECT_NAMESPACE CI_PROJECT_NAMESPACE
#define GBL_CI_PROJECT_DIR       CI_PROJECT_DIR
#define GBL_CI_PROJECT_VISIBILITY GIM_STRINGIFY(CI_PROJECT_VISIBILITY)

#define GBL_CI_RUNNER_ID         CI_RUNNER_ID
#define GBL_CI_RUNNER_DESC       CI_RUNNER_DESCRIPTION
#define GBL_CI_RUNNER_EXEC_ARCH  CI_RUNNER_EXECUTABLE_ARCH
#define GBL_CI_RUNNER_REVISION   CI_RUNNER_REVISION
#define GBL_CI_RUNNER_TAGS       CI_RUNNER_TAGS
#define GBL_CI_RUNNER_VERSION    CI_RUNNER_VERSION

#define GBL_CI_USER_ID         GITLAB_USER_ID
#define GBL_CI_USER_NAME       GITLAB_USER_NAME
#define GBL_CI_USER_LOGIN      GITLAB_USER_LOGIN
#define GBL_CI_USER_EMAIL      GITLAB_USER_EMAIL

#define GBL_CI_JOB_ID          CI_JOB_ID
#define GBL_CI_JOB_IMAGE       CI_JOB_IMAGE
#define GBL_CI_JOB_NAME        CI_JOB_NAME
#define GBL_CI_JOB_STAGE       CI_JOB_STAGE
#define GBL_CI_JOB_STATUS      CI_JOB_STATUS
#define GBL_CI_JOB_URL         CI_JOB_URL
#define GBL_CI_JOB_ENV_NAME    CI_JOB_ENV_SLUG
#define GBL_CI_JOB_ENV_URL     CI_JOB_ENV_URL
#define GBL_CI_JOB_PIPELINE_ID   CI_PIPELINE_ID
#define GBL_CI_JOB_PIPELINE_URL  CI_PIPELINE_URL
#define GBL_CI_JOB_PIPELINE_SRC  CI_PIPELINE_SOURCE

#define GBL_BUILD_CONFIG             //shared module vs whatever
#define GBL_BUILD_TYPE               CMAKE_BUILD_TYPE
#define GBL_BUILD_FLAGS              CMAKE_BUILD_FLAGS

#define GBL_REPO_COMMIT_TITLE        CI_COMMIT_TITLE
#define GBL_REPO_COMMIT_MESSAGE      CI_COMMIT_DESCRIPTION
#define GBL_REPO_LANGUAGES           CI_PROJECT_REPOSITORY_LANGUAGES
#define GBL_REPO_COMMIT_TIMESTAMP    CI_COMMIT_TIMESTAMP
#define GBL_REPO_REVISION            CI_COMMIT_SHA
#define GBL_REPO_PREV_REVISION       CI_COMMIT_BEFORE_SHA
#define GBL_REPO_REF_NAME            CI_COMMIT_REF_NAME //works with bith git branches and tags
#define GBL_REPO_URL                 CI_REPOSITORY_URL

#endif // GIMBAL_BUILD_H
