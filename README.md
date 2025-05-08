# Overview

A Concurrent In-memory Redis Application that supports Database key-value storage, transactions, replication, and  concurrent client connections: https://app.codecrafters.io/courses/redis/overview

# Local testing :

  * Running CMake for local testing : 

```
mkdir build

cd build 

cmake ..

make 

./server

```

Refer to [List of testing stage](https://app.codecrafters.io/courses/redis/introduction) for detail testings of each functionality of Redis in-memory database.

After calling ./server, we can start sending commands to the Redis in-memory DB server. Note that the format for this Redis implementation is [RESP Bulk Format](https://redis.io/docs/latest/develop/reference/protocol-spec/)



# Supporting commands and features of In-memory Redis Database :

## Basic RDB Persistent for Database Storage

  ✅ Store RDB Config file 
  
  ✅ SET/GET Command with expiry

## Streaming

  ✅ TYPE the command for checking the key-value storage type
  
  ✅ XADD command for creating streams of storage with a partially and fully automated ID generator
  
  ✅ XRANGE for range streaming query for timestamp
  
  ✅ Support blocking the data streaming 

## Transactions 

  ✅  INCR command to modify the key-value pair
  
  ✅  MULT command for supporting multiple concurrent transactions 
  
  ✅  EXEC command for execution of queued transactions

## Replication

 ✅ Slave replication will be on port 6380
 
 ✅ Info command for checking the information for the slave replication
 
 ✅ Send persistent ping communication
 
 ✅ ACK and WAIT commands for communications between the  main thread and replication

## Concurrent user connection 

 ✅ Support multiple clients executing the Redis CLI command at once.

 
