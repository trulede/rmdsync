FROM gcc:4.9 as builder
WORKDIR /rmdsync
COPY build/rmdsync .
RUN make

FROM redis:5.0-alpine
RUN apk add --no-cache bash
COPY --from=builder /rmdsync/rmdsync.so /mods/rmdsync.so
COPY redis.conf /usr/local/etc/redis/redis.conf
CMD [ "redis-server", "/usr/local/etc/redis/redis.conf" ]
