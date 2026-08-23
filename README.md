# SMBus Platform

A compact, frame‑based communication system written in C on Linux.  
Built to explore how real network systems behave under load, concurrency, and failure.

---

## Purpose

To practice true system‑level development:
- non‑blocking TCP sockets  
- epoll/select multiplexing  
- binary message framing  
- resource and error handling  
- multi‑client architecture  
- capacity limits and controlled rejection (mutex‑client)

---

## Components

### Server
- epoll‑based, non‑blocking TCP server  
- supports up to 512 concurrent clients  
- fixed‑size binary message frames  
- state machine: `HELLO`, `MSG`, `LEAVE`, `FULL`  
- broadcast messaging to all active clients  
- lightweight logging with timestamps  
- **mutex‑client handler:**  
  when the server reaches MAXCLIENT, it accepts one temporary client, sends `STATE_FULL`, and disconnects cleanly—preventing overload and preserving stability

### Client
- select‑based interactive terminal client  
- username handshake (`STATE_HELLO`)  
- frame‑based messaging  
- colored terminal output  
- handles `STATE_FULL` gracefully  
- simple, responsive UI

---

## Content
  
    bin/
 
       server/
              SMBus-Server
 
       client/
              SMBus-Client

    client/

        include/
              smbus.h

        src/
              client.c
              main.c

    common/
    
        protocol-frame.c

    server/

        include/
              core.h
              log.h
              mslot.h
              sock.h
 
        src/
              core.c
              log.c
              main.c
              mslot.c
              sock.c
              
---

## Protocol

```c
typedef struct {
    char msg[BUFSIZ];
    uint32_t len;
    proto_state_t state;
} smbus_msgframe_t;
```

## States
- STATE_HELLO,
- STATE_MSG,
- STATE_LEAVE,
- STATE_FULL,

## Build
cd server && make
cd client && make

## Run
./smbus-server
./smbus-client `<ip> <port> <username>`

## Author
- Mustapha — System‑Level Develope,
- Mashhad, Iran.
