# IPC-through-Shared-Memory
## Basic implementation of IPC through shared memory in C
### How to use
Start server by compiling server.c
Start client by compiling client.c
Client has 3 options 

Register -> server returns with a unique key

Unregister -> server deletes the pid from the list of known pids

Calculate -> client can send numbers to the server which is calculated using threads and then returned back to the client
