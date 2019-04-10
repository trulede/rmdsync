#ifndef RMDSYNC_PRIVATE_H
#define RMDSYNC_PRIVATE_H

#define REDISMODULE_EXPERIMENTAL_API
#include "redismodule.h"

#define RMDSYNC_MODULE_NAME         "rmdsync"
#define RMDSYNC_MODULE_VERSION      1

#define SYNC_CONFIG_KEY_FMT                                 "%s.config"
#define SYNC_CONFIG_VALUE__NAME                             "name"
#define SYNC_CONFIG_VALUE__TIMEBASE_US                      "timebase_us"
#define SYNC_CONFIG_DEFAULT__TIMEBASE_US                    "5000"
#define SYNC_CONFIG_DEFAULT__TIMEBASE_US_INT                5000
#define SYNC_CONFIG_VALUE__SAMPLE_FREQ_US                   "sample_freq_us"
#define SYNC_CONFIG_DEFAULT__SAMPLE_FREQ_US                 "50"
#define SYNC_CONFIG_DEFAULT__SAMPLE_FREQ_US_INT             50

#define SYNC_CLIENTS_KEY_FMT                                "%s.clients"

#define SYNC_THREAD_KEY_FMT                                 "%s.thread"

#define SYNC_REQUESTS_KEY_FMT                               "%s.requests"



/* sync_thread.c */
extern void *sync_thread_main(void *arg);


/* trace.c */
extern void __log__(RedisModuleCtx *ctx, const char* fmt, ...);
extern void __monitor__(RedisModuleCtx *ctx, const char* fmt, ...);


/* clock.c */
extern void __sleep_us__(int sleep_us);


/* redis.c */
extern int redis_hash_lookup_int(RedisModuleCtx *ctx, RedisModuleString *hash, const char *field, int _default);
extern int redis_set_count(RedisModuleCtx *ctx, RedisModuleString *set, const char *command);


#endif /* RMDSYNC_PRIVATE_H */
