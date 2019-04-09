#define REDISMODULE_EXPERIMENTAL_API
#include "redismodule.h"
#include "sds.h"

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>

#include "private.h"


void *sync_thread_main(void *arg)
{
    RedisModuleCtx* ctx = RedisModule_GetThreadSafeContext(NULL);
    RedisModuleString *sync_name;
    RedisModuleKey *key = NULL;
    RedisModuleString *key_name = NULL;
    const char *s = NULL;

    sync_name = RedisModule_CreateStringFromString(ctx, (const RedisModuleString *)arg);
    s = RedisModule_StringPtrLen(sync_name, NULL);
    key_name = RedisModule_CreateStringPrintf(ctx, SYNC_CLIENTS_KEY_FMT, s);

    while (1) {
        __sleep_us__(5000); // 5 mS TODO read from redis key.

        key = RedisModule_OpenKey(ctx, key_name, REDISMODULE_READ);
        if (key == NULL) {
            goto sync_thread_exit;
        }
        RedisModule_CloseKey(key);
    }


sync_thread_exit:
    if (sync_name)      RedisModule_FreeString(ctx, sync_name);
    if (key_name)       RedisModule_FreeString(ctx, key_name);

    RedisModule_FreeThreadSafeContext(ctx);
    pthread_exit(NULL);
}
