#define REDISMODULE_EXPERIMENTAL_API
#include "redismodule.h"

#include <stdlib.h>

#include "private.h"


int redis_hash_lookup_int(RedisModuleCtx *ctx, RedisModuleString *hash, const char *field, int _default)
{
    RedisModuleKey *key = NULL;
    RedisModuleString *value = NULL;
    int rc = _default;
    const char *s = NULL;

    key = RedisModule_OpenKey(ctx, hash, REDISMODULE_READ);
    if (key) {
        RedisModule_HashGet(key, REDISMODULE_HASH_CFIELDS, field, &value, NULL);
        s = RedisModule_StringPtrLen(value, NULL);
        rc = atoi(s);
        RedisModule_FreeString(ctx, value);
        RedisModule_CloseKey(key);
    }

    return rc;
}

int redis_set_count(RedisModuleCtx *ctx, RedisModuleString *set, const char *command)
{
    RedisModuleCallReply *reply;
    int rc = -1;    // Indicate an error condition by default.

    /**
     * command can be:
     *      SCARD   - Redis Set
     *      ZCARD   - Redis Sorted Set
     */

    reply = RedisModule_Call(ctx, command, "s", set);
    if (RedisModule_CallReplyType(reply) == REDISMODULE_REPLY_INTEGER) {
        rc = RedisModule_CallReplyInteger(reply);
    }

    RedisModule_FreeCallReply(reply);
    return rc;
}
