![DSync API - Sync](../images/dsync_logo_api.png)


## RMDSYNC.SYNC sync_name wakeup

### Synopsis

<strong><samp>rmdsync.sync sync_name wakeup</samp></strong>


#### Description

A blocking command which returns when the Sync Instance is released. The exact
time that a Sync Instance is released will depend on the wakeup time of _all_
Clients with are interacting with the Sync Instance - there a Sync Instance may
be released _before_ the requested wakeup time if another Client requests an
earlier wakeup.


### Parameters

| Parameter | Type | Description
| --- | --- | ---
| sync_name | String | Name of the Sync Instance to wait on (caller is blocked).
| wakeup | Integer | [-1, 0, uS] Wakeup time in uS. The wakeup time is relative to the current time of the Sync Instance, effectively a wakeup in X uS. A value of -1 return the current Sync Instance time and a value of 0 indicates to wake up at the next default wakeup time of the Sync Instance - see Redis Keyspace for more details.

### Return Value

* Array reply:
    * [0] - Integer reply: The current Sync Instance time (relative to 0, the start time).
    * [1] - Integer reply: The number of uS since the previous Sync Release. Use this value to determine if the Sync Instance was released earlier than expected.


### Example

__Python:__
```python
import redis

r = redis.from_url("redis://localhost:6379")
rc = r.execute_command("rmdsync.connect", "foo", "bar")
print(rc) # 'OK'

# Get the timeinfo of the Sync Instance.
timeinfo = r.execute_command("rmdsync.sync", "foo", -1)
print(timeinfo) # [b'0', b'0']

# Block for 5000 uS.
timeinfo = r.execute_command("rmdsync.sync", "foo", 5000)
print(timeinfo) # [b'5000', b'5000']

# Block at the default cadence of the Sync Instance (in this case 10000 uS).
timeinfo = r.execute_command("rmdsync.sync", "foo", 0)
print(timeinfo) # [b'15000', b'10000']

rc = r.execute_command("rmdsync.disconnect", "foo", "bar")
print(rc) # 'OK'
```

__Redis CLI:__
```cli
redis> rmdsync.connect foo bar
(string) 'OK'
redis> rmdsync.sync foo -1
1) (integer) 0
2) (integer) 0
redis> rmdsync.sync foo 5000
1) (integer) 5000
2) (integer) 5000
redis> rmdsync.sync foo 0
1) (integer) 15000
2) (integer) 10000
redis> rmdsync.disconnect foo bar
```
