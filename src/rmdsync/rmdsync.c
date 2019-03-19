#define REDISMODULE_EXPERIMENTAL_API
#include "redismodule.h"
#include "sds.h"

#include "private.h"


/**
 *  RMDSYNC.CONNECT sync_name client_name
 */
 int rmdsync_connect_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc)
 {
     RedisModule_AutoMemory(ctx);
     if (argc != 3) return RedisModule_WrongArity(ctx);


     // TODO Create the Sync if it does not exist.

     // TODO Star the sync thread.


     // TODO Connect the client (if not already connected).



     return RedisModule_ReplyWithSimpleString(ctx, "OK");
 }


/**
 *  RMDSYNC.DISCONNECT sync_name client_name
 */
int rmdsync_disconnect_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc)
{
    RedisModule_AutoMemory(ctx);
    if (argc != 3) return RedisModule_WrongArity(ctx);

    // TODO Delete the client from the Sync.

    // TODO Stop the sync thread.


    // TODO Delete the Sync??? Or keep the configuration and have clients in a separate hash/list?
    //    Dict for config with: name = sync
    //    List or Dict for clients, when empty, not active: name.clients
    //        Benfit of Dict, place to store wakeup time (relative to 0).
    //        But could also use a sorted list for the next wakeup ... which could be faster/smarter.



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
