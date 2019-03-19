#define REDISMODULE_EXPERIMENTAL_API
#include "redismodule.h"
#include "sds.h"

#include <string.h>
#include <stdarg.h>

#include "private.h"


/**
 *  LOG Functionality
 */
const char* LOG_LVL = "warning";

void __log__(RedisModuleCtx *ctx, const char* fmt, ...)
{
    sds s = sdsempty();
    va_list ap;
    va_start(ap, fmt);
    s = sdscatvprintf(s, fmt, ap);
    va_end(ap);

    RedisModule_Log(ctx, LOG_LVL, s);
}


/**
 *  MONITOR Functionality
 *
 *  Push monitor stream to a PUBSUB.
 *
 *  Enabled per Sync Instance via Redis keyspace (or a more efficient way).
 */
void __monitor__(RedisModuleCtx *ctx, const char* fmt, ...)
{

}
