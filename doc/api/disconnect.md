![DSync API - Disconnect](../images/dsync_logo_api.png)


## RMDSYNC.DISCONNECT sync_name client_name

### Synopsis

<strong><samp>rmdsync.disconnect sync_name client_name</samp></strong>


#### Description

TODO


### Parameters

| Parameter | Type | Description
| --- | --- | ---
| sync_name | String | Name of the Sync to disconnect from.
| client_name | String | Unique name of the client which is disconnecting from the Sync Instance.


### Return Value

* String reply: 'OK' - The client is disconnected from the Sync Instance.


### Example

__Python:__
```python
import redis

r = redis.from_url("redis://localhost:6379")
rc = r.execute_command("rmdsync.connect", "foo", "bar")
print(rc) # 'OK'

rc = r.execute_command("rmdsync.disconnect", "foo", "bar")
print(rc) # 'OK'
```

__Redis CLI:__
```cli
redis> rmdsync.connect foo bar
(string) 'OK'
redis> rmdsync.disconnect foo bar
(string) 'OK'
```
