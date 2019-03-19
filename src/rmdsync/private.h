#ifndef RMDSYNC_PRIVATE_H
#define RMDSYNC_PRIVATE_H

#define RMDSYNC_MODULE_NAME        "rmdsync"
#define RMDSYNC_MODULE_VERSION     1


/* sync_thread.c */
extern void *sync_thread_main(void *arg);


/* trace.c */
extern void __log__(RedisModuleCtx *ctx, const char* fmt, ...);
extern void __monitor__(RedisModuleCtx *ctx, const char* fmt, ...);


#endif /* RMDSYNC_PRIVATE_H */
