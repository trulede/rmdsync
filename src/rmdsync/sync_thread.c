#define REDISMODULE_EXPERIMENTAL_API
#include "redismodule.h"
#include "sds.h"

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

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

static int _get_sync_request(RedisModuleCtx *ctx, RedisModuleString *requests_key_name)
{
    RedisModuleKey *key = NULL;
    RedisModuleString *buffer = NULL;
    double score = 0;

    key = RedisModule_OpenKey(ctx, requests_key_name, REDISMODULE_READ);
    if (key == NULL)
        goto release_sync_request_resources;
    if (RedisModule_ZsetFirstInScoreRange(key,
            1, REDISMODULE_POSITIVE_INFINITE, 0, 0) == REDISMODULE_ERR)
        goto release_sync_request_resources;
    buffer = RedisModule_ZsetRangeCurrentElement(key, &score);
    RedisModule_ZsetRangeStop(key);

release_sync_request_resources:
    if (key)    RedisModule_CloseKey(key);
    if (buffer) RedisModule_FreeString(ctx, buffer);

    return (int)score;
}

static int _get_timebase(RedisModuleCtx *ctx, RedisModuleString *config_key_name)
{
    RedisModuleKey *key = NULL;
    RedisModuleString *value = NULL;
    int rc = SYNC_CONFIG_DEFAULT__TIMEBASE_US_INT;
    const char *s = NULL;

    key = RedisModule_OpenKey(ctx, config_key_name, REDISMODULE_READ);
    if (key) {
        RedisModule_HashGet(key, REDISMODULE_HASH_CFIELDS, SYNC_CONFIG_VALUE__TIMEBASE_US, &value, NULL);
        s = RedisModule_StringPtrLen(value, NULL);
        rc = atoi(s);
        RedisModule_FreeString(ctx, value);
        RedisModule_CloseKey(key);
    }

    return rc;
}

static int _get_sample_freq(RedisModuleCtx *ctx, RedisModuleString *config_key_name)
{
    RedisModuleKey *key = NULL;
    RedisModuleString *value = NULL;
    int rc = SYNC_CONFIG_DEFAULT__SAMPLE_FREQ_US_INT;
    const char *s = NULL;

    key = RedisModule_OpenKey(ctx, config_key_name, REDISMODULE_READ);
    if (key) {
        RedisModule_HashGet(key, REDISMODULE_HASH_CFIELDS, SYNC_CONFIG_VALUE__SAMPLE_FREQ_US, &value, NULL);
        s = RedisModule_StringPtrLen(value, NULL);
        rc = atoi(s);
        RedisModule_FreeString(ctx, value);
        RedisModule_CloseKey(key);
    }

    return rc;
}


void *sync_thread_main(void *arg)
{
    RedisModuleCtx* ctx = RedisModule_GetThreadSafeContext(NULL);
    RedisModuleString *sync_name;
    RedisModuleString *config_key_name = NULL;
    RedisModuleString *clients_key_name = NULL;
    RedisModuleString *requests_key_name = NULL;
    const char *s = NULL;

    /* Setup the various key names which will be used by the Sync Thread. */
    sync_name = RedisModule_CreateStringFromString(ctx, (const RedisModuleString *)arg);
    s = RedisModule_StringPtrLen(sync_name, NULL);
    config_key_name = RedisModule_CreateStringPrintf(ctx, SYNC_CONFIG_KEY_FMT, s);
    clients_key_name = RedisModule_CreateStringPrintf(ctx, SYNC_CLIENTS_KEY_FMT, s);
    requests_key_name = RedisModule_CreateStringPrintf(ctx, SYNC_REQUESTS_KEY_FMT, s);

    while (1) {
        int timebase_us = _get_timebase(ctx, config_key_name);
        int sample_freq_us = _get_sample_freq(ctx, config_key_name);

        /* Wait for all connected clients to call Sync. */
        // TODO Implement a timeout when waiting for connected clients to call
        //  sync, if they don't make it, disconnect them.
        int sample_count = timebase_us / sample_freq_us;
        while (sample_count--) {
            __sleep_us__(sample_freq_us);

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

            if (client_count == sync_count) break;
        }

        /* Read the sync time request, if specified by a clients sync command. */
        int sync_request_us = _get_sync_request(ctx, requests_key_name);
        if (sync_request_us == 0) sync_request_us = timebase_us;

        /* Burn down the rest of the timebase, drift adjust. */
        __sleep_us__(sync_request_us);

        /* Release the blocked clients Sync. */
        // first delete the requests key.

    }


sync_thread_exit:
    if (sync_name)              RedisModule_FreeString(ctx, sync_name);
    if (config_key_name)        RedisModule_FreeString(ctx, config_key_name);
    if (clients_key_name)       RedisModule_FreeString(ctx, clients_key_name);
    if (requests_key_name)      RedisModule_FreeString(ctx, requests_key_name);

    RedisModule_FreeThreadSafeContext(ctx);
    pthread_exit(NULL);
}
