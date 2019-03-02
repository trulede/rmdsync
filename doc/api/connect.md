![DSync API - Connect](../images/dsync_logo_api.png)


## RMDSYNC.CONNECT sync_name client_name

### Synopsis

<strong><samp>rmdsync.connect sync_name client_name</samp></strong>


#### Description

TODO


### Parameters

| Parameter | Type | Description
| --- | --- | ---
| sync_name | String | Name of the Sync to establish a connection with. If the Sync Name does not exist it will be created - see Redis Keyspace for more details.
| client_name | String | Unique name of the client which is connecting to the Sync Instance.


### Return Value

* Integer reply: **sync_id** - a unique identifier for this Sync Name and Client Name. This ID is used for all subsequent DSync API commands. The Sync ID is unique within a Redis DB - the value is a hash derived from both parameters of the RMDSYNC.CONNECT command.


### Example

__Python:__
```python
import redis

r = redis.from_url("redis://localhost:6379")
id = r.execute_command("rmdsync.connect", "foo", "bar")
print(id) # 0

# Connect a second time ... id will be unchanged.
id = r.execute_command("rmdsync.connect", "foo", "bar")
print(id) # 0

# Get the timeinfo of the Sync Instance.
timeinfo = r.execute_command("rmdsync.sync", id, -1)
print(timeinfo) # [b'41342452345', [b'0']]
```

__Redis CLI:__
```cli
redis> rmdsync.connect foo bar
(integer) 0
redis[2]>
```
