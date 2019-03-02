# Redis Distributed Synchronisation Primitives

## Overview

This Redis Module implements a number of Synchronisation Primitives which can
be used to build Distributed Synchronised Systems. The operation is similar in
concept to POSIX Thread Synchronisation Primitives, however this Redis Module
allows for synchronisation across system boundaries.


## Documentation

  * [Module Commands](doc/api.md)
  * [Redis Key Space](doc/keyspace.md)


### Source Repository

git clone https://github.com/trulede/rmdsync


## Build and Test

Linux (Bash Shell)::

  git clone https://github.com/trulede/rmdsync
  cd rmdsync
  make
  make docker
  make start
  make list
  make test


## Example Configuration

### Redis Configuration

todo


### Dockerfile

todo


### Docker Stack

todo
