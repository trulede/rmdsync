#ifndef RMDSYNC_PRIVATE_H
#define RMDSYNC_PRIVATE_H

#define RMDSYNC_MODULE_NAME         "rmdsync"
#define RMDSYNC_MODULE_VERSION      1

#define SYNC_CONFIG_KEY_FMT                                 "%s.config"
#define SYNC_CONFIG_VALUE__NAME                             "name"
#define SYNC_CONFIG_VALUE__TIMEBASE_US                      "timebase_us"
#define SYNC_CONFIG_DEFAULT__TIMEBASE_US                    "5000"

#define SYNC_CLIENTS_KEY_FMT                                 "%s.clients"



/* sync_thread.c */
extern void *sync_thread_main(void *arg);


/* trace.c */
extern void __log__(RedisModuleCtx *ctx, const char* fmt, ...);
extern void __monitor__(RedisModuleCtx *ctx, const char* fmt, ...);


#endif /* RMDSYNC_PRIVATE_H */
