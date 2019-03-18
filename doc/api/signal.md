![DSync API - Connect](../images/dsync_logo_api.png)


## RMDSYNC.SIGNAL sync_name

### Synopsis

<strong><samp>rmdsync.signal sync_name</samp></strong>


#### Description

TODO


### Parameters

| Parameter | Type | Description
| --- | --- | ---
| sync_name | String | Name of the Sync Instance to signal. All Clients waiting
on that Sync Instance, via the [RMDSYNC.SYNC](./sync.md) command, will be released.


### Return Value

* Array reply:
    * [0] - Integer reply: The number of Clients which were waiting on the
    Sync Instance, and were subsequently released by the Signal.
    * [1] - Integer reply: the total number of Clients connected to the
    at the time the Signal was raised.


### Example

__Python (1st Client):__
```python
import redis

r = redis.from_url("redis://localhost:6379")
rc = r.execute_command("rmdsync.connect", "foo", "bar")
print(rc) # 'OK'

# Block on the Sync Instance until RMDSYNC.SIGNAL is called.
timeinfo = r.execute_command("rmdsync.sync", "foo", 0)
# ... run code in 2nd Client to release the Sync.
print(timeinfo) # [b'0', b'0']

rc = r.execute_command("rmdsync.disconnect", "foo", "bar")
print(rc) # 'OK'
```

__Python (2st Client):__
```python
import redis

r = redis.from_url("redis://localhost:6379")
rc = r.execute_command("rmdsync.connect", "foo", "lettuce")
print(rc) # 'OK'

# Signal any Clients waiting on a RMDSYNC.SYNC command.
rc = r.execute_command("rmdsync.signal", "foo", 0)
print(rc) # '[1, 1]'

rc = r.execute_command("rmdsync.disconnect", "foo", "bar")
print(rc) # 'OK'
```
