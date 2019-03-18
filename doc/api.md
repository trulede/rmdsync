![DSync API](images/dsync_logo_api.png)


# Redis Module Distributed Sync API

## Overview

![DSync API Overlay](images/dsync_overlay_api.png)


## Commands

Distributed Sync API Commands are provided by a Redis Module. The Command
Names have the following semantics:

> <strong><samp>rmdsync.\<Name\></samp></strong>

where:

* <strong><samp>\<Name\></samp></strong> - the Name of the Command, either
Camel-Case or with a dot (<samp> . </samp>) separator.

The Commands as interpreted by Redis are not case-sensitive. Therefore, in
most of the Code Base and Examples, lower-case Commands are used.


#### Variadic Commands

Some Distributed Sync API Commands support a Variadic list of Parameters
(indicated by <samp>[arg1 arg2 arg3 ...]</samp> in the parameter list of
the command).


### Distributed Sync API Commands

[RMDSYNC.CONNECT](api/connect.md)

[RMDSYNC.SYNC](api/sync.md)

[RMDSYNC.SIGNAL](api/signal.md)

[RMDSYNC.DISCONNECT](api/disconnect.md)
