# Overview

A Concurrent In-memory Redis Application that supports Database key-value storage, transactions, replication, and  concurrent client connections: https://app.codecrafters.io/courses/redis/overview

# Local testing :

  * Running cmake for local testing : 

```
mkdir build

cd build 

cmake ..

make 

./server

```

After calling to ./server, we can start sending commands to Redis in-memory DB server, note that the format for this Redis implementation is [RESP Bulk Format](https://redis.io/docs/latest/develop/reference/protocol-spec/)



# Supporting commands and features of In-memory Redis Database :

## Basic RDB Persistant for Database Storage

  ✅ Store RDB Config file 
  
  ✅ SET/GET Command with expiry
