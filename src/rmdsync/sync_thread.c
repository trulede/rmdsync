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


    RedisModule_FreeThreadSafeContext(ctx);
    pthread_exit(NULL);
}
