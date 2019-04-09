#define REDISMODULE_EXPERIMENTAL_API
#include "redismodule.h"
#include "sds.h"

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>

#include "private.h"


static int _get_client_count(RedisModuleCtx *ctx, RedisModuleString *clients_key_name)
{
    RedisModuleCallReply *reply;
    int rc = -1;    // Indicate an error condition by default.

    reply = RedisModule_Call(ctx, "SCARD", "s", clients_key_name);
    if (RedisModule_CallReplyType(reply) == REDISMODULE_REPLY_INTEGER) {
        rc = RedisModule_CallReplyInteger(reply);
    }

    RedisModule_FreeCallReply(reply);
    return rc;
}

static int _get_sync_count(RedisModuleCtx *ctx, RedisModuleString *requests_key_name)
{
    RedisModuleCallReply *reply;
    int rc = -1;    // Indicate an error condition by default.

    reply = RedisModule_Call(ctx, "ZCARD", "s", requests_key_name);
    if (RedisModule_CallReplyType(reply) == REDISMODULE_REPLY_INTEGER) {
        rc = RedisModule_CallReplyInteger(reply);
    }

    RedisModule_FreeCallReply(reply);
    return rc;
}


void *sync_thread_main(void *arg)
{
    RedisModuleCtx* ctx = RedisModule_GetThreadSafeContext(NULL);
    RedisModuleString *sync_name;
    RedisModuleString *clients_key_name = NULL;
    RedisModuleString *requests_key_name = NULL;
    const char *s = NULL;

    /* Setup the various key names which will be used by the Sync Thread. */
    sync_name = RedisModule_CreateStringFromString(ctx, (const RedisModuleString *)arg);
    s = RedisModule_StringPtrLen(sync_name, NULL);
    clients_key_name = RedisModule_CreateStringPrintf(ctx, SYNC_CLIENTS_KEY_FMT, s);
    requests_key_name = RedisModule_CreateStringPrintf(ctx, SYNC_REQUESTS_KEY_FMT, s);

    while (1) {
        __sleep_us__(5000); // 5 mS TODO read from redis key.
                            // FIXME this will become sample time, so 50-100 uS or so
                            //  still configure it in a key.


        /**
         *  Clients can connect at any time, and shortly after would issue
         *  a SYNC command. Therefore we should sample both values continuously
         *  to catch the case where the client count and sync counts are actually
         *  incremented but only the sync count is being sampled, which could
         *  cause an early release of the Blocking Sync as the newly sampled
         *  sync count could match the old sample for client count.
         */
        int client_count = _get_client_count(ctx, clients_key_name);
        int sync_count = _get_sync_count(ctx, requests_key_name);

        if (client_count == -1) goto sync_thread_exit;
        if (client_count == 0) goto sync_thread_exit;

        if (client_count == sync_count) {
            // Get sync time. From ZLIST, lowest value, SYNC command should
            // put in default if provieded with 0.
            // or put in 0 and select from range 1...INF, if no return then
            // take/read the default.

            // TODO Drift - Calculate remaining sleep time. Request - samples = sleep time.

            __sleep_us__(50); // TODO update with calculated sleep.

            // Update sync time (0 + sync time) = internal timebase.

            // Release the Blocked Sync calls.

        }
    }


sync_thread_exit:
    if (sync_name)          RedisModule_FreeString(ctx, sync_name);
    if (clients_key_name)   RedisModule_FreeString(ctx, clients_key_name);
    if (requests_key_name)      RedisModule_FreeString(ctx, requests_key_name);

    RedisModule_FreeThreadSafeContext(ctx);
    pthread_exit(NULL);
}
