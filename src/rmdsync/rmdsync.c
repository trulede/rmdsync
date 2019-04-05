#define REDISMODULE_EXPERIMENTAL_API
#include "redismodule.h"
#include "sds.h"

#include "private.h"


/**
 *  RMDSYNC.CONNECT sync_name client_name
 */

static void _init_sync_config(RedisModuleCtx *ctx, RedisModuleString *sync_name)
{
    // TODO RedisModule_AutoMemory(ctx); // really need that? in caller
    RedisModuleKey *key = NULL;
    RedisModuleString *name = NULL;
    RedisModuleString *rms = NULL;
    const char *s = NULL;

    // TODO consider if sync name is valid ... space, special characters, length.
    s = RedisModule_StringPtrLen(sync_name, NULL);
    name = RedisModule_CreateStringPrintf(ctx, SYNC_CONFIG_KEY_FMT, s);
    key = RedisModule_OpenKey(ctx, name, REDISMODULE_READ|REDISMODULE_WRITE);

    // name
    RedisModule_HashSet(key, REDISMODULE_HASH_NX|REDISMODULE_HASH_CFIELDS, SYNC_CONFIG_VALUE__NAME, sync_name, NULL);

    // timebase_us
    rms = RedisModule_CreateStringPrintf(ctx, SYNC_CONFIG_DEFAULT__TIMEBASE_US);
    RedisModule_HashSet(key, REDISMODULE_HASH_NX|REDISMODULE_HASH_CFIELDS, SYNC_CONFIG_VALUE__TIMEBASE_US, rms, NULL);
}

static void _connect_sync(RedisModuleCtx *ctx, RedisModuleString *sync_name, RedisModuleString *client_name)
{
    RedisModuleString *key_name = NULL;
    const char *s = NULL;

    // TODO consider if sync name is valid ... space, special characters, length.
    s = RedisModule_StringPtrLen(sync_name, NULL);
    key_name = RedisModule_CreateStringPrintf(ctx, SYNC_CLIENTS_KEY_FMT, s);
    RedisModule_Call(ctx, "SADD", "ss", key_name, client_name);
}

static void _start_sync_thread(RedisModuleCtx *ctx, RedisModuleString *sync_name)
{
    // Find it, probably put the address in the config hash ... dirty ... but
    //  perhaps better than a linked list. at least faster. perhaps do both.

    // Or start it.
}

 int rmdsync_connect_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc)
 {
     RedisModule_AutoMemory(ctx);

     RedisModuleString *sync_name = NULL;
     RedisModuleString *client_name = NULL;

     if (argc != 3) return RedisModule_WrongArity(ctx);
     sync_name = argv[1];
     client_name = argv[2];

     _init_sync_config(ctx, sync_name);
     _connect_sync(ctx, sync_name, client_name);
     _start_sync_thread(ctx, sync_name);

     return RedisModule_ReplyWithSimpleString(ctx, "OK");
 }


/**
 *  RMDSYNC.DISCONNECT sync_name client_name
 */
static void _disconnect_sync(RedisModuleCtx *ctx, RedisModuleString *sync_name, RedisModuleString *client_name)
{
    RedisModuleString *key_name = NULL;
    const char *s = NULL;

    // TODO consider if sync name is valid ... space, special characters, length.
    s = RedisModule_StringPtrLen(sync_name, NULL);
    key_name = RedisModule_CreateStringPrintf(ctx, SYNC_CLIENTS_KEY_FMT, s);
    RedisModule_Call(ctx, "SREM", "ss", key_name, client_name);
}

static void _stop_sync_thread(RedisModuleCtx *ctx, RedisModuleString *sync_name)
{
    // If the client list/set is empty, stop the thread, othwerwise ... NOP
}

int rmdsync_disconnect_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc)
{
    RedisModule_AutoMemory(ctx);

    RedisModuleString *sync_name = NULL;
    RedisModuleString *client_name = NULL;

    if (argc != 3) return RedisModule_WrongArity(ctx);
    sync_name = argv[1];
    client_name = argv[2];

    _disconnect_sync(ctx, sync_name, client_name);
    _stop_sync_thread(ctx, sync_name);

    return RedisModule_ReplyWithSimpleString(ctx, "OK");
}


/**
 *  RMDSYNC.RMDSYNC.SYNC sync_name client_name wakeup
 *
 *  Blocking Command.
 */
int rmdsync_sync_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc)
{
    RedisModule_AutoMemory(ctx);
    if (argc != 4) return RedisModule_WrongArity(ctx);

    // TODO Check that the Sync is valid. And the client.

    // TODO Calculate the next wakeup time.


    // TODO Block until the sync thread signals the release.

    // TODO ... OR ... return the curent sync instance time.


    RedisModule_ReplyWithArray(ctx, 2);
    RedisModule_ReplyWithLongLong(ctx, 0);
    RedisModule_ReplyWithLongLong(ctx, 0);

    return REDISMODULE_OK;
}


/**
 *  RMDSYNC.SIGNAL sync_name
 */
int rmdsync_signal_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc)
{
    RedisModule_AutoMemory(ctx);
    if (argc != 1) return RedisModule_WrongArity(ctx);

    // TODO Check that the Sync is valid.

    // TODO Signal the sync thread so that it wakeups up and releases any blocked clients.


    RedisModule_ReplyWithArray(ctx, 2);
    RedisModule_ReplyWithLongLong(ctx, 0);
    RedisModule_ReplyWithLongLong(ctx, 0);

    return REDISMODULE_OK;
}





int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc)
{
    if (RedisModule_Init(ctx, RMDSYNC_MODULE_NAME, RMDSYNC_MODULE_VERSION, REDISMODULE_APIVER_1)
            == REDISMODULE_ERR) return REDISMODULE_ERR;


    if (RedisModule_CreateCommand(ctx, "rmdsync.connect",
            rmdsync_connect_RedisCommand, "", 0, 0, 0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;
    if (RedisModule_CreateCommand(ctx, "rmdsync.disconnect",
            rmdsync_disconnect_RedisCommand, "", 0, 0, 0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;
    if (RedisModule_CreateCommand(ctx, "rmdsync.sync",
            rmdsync_sync_RedisCommand, "", 0, 0, 0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;
    if (RedisModule_CreateCommand(ctx, "rmdsync.signal",
            rmdsync_signal_RedisCommand, "", 0, 0, 0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;


    return REDISMODULE_OK;
}
