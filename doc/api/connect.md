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

* String reply: 'OK' - The client is connected to the Sync Instance.



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
```

__Redis CLI:__
```cli
redis> rmdsync.connect foo bar
(string) 'OK'
redis>
```
