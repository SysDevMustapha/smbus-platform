# SMBus Server

A lightweight, epoll‑based TCP server designed for high‑concurrency message handling using fixed‑size binary frames.  
This module is responsible for managing client connections, broadcasting messages, enforcing capacity limits, and maintaining protocol state integrity.

---

## Features

- Non‑blocking TCP server using epoll  
- Supports up to 512 concurrent clients  
- Fixed‑size binary message frames  
- Protocol states: HELLO, MSG, LEAVE, FULL  
- Broadcast messaging to all active clients  
- Mutex‑client handler for controlled overload protection  
- Timestamped logging for all server events  

---

## Important Notice

### ⚠️ Root Access Required  
The server **must be executed with root privileges**.  
This is necessary because the server binds to a privileged port and uses low‑level socket operations that require elevated permissions.

---

## source structure

    server/

         src/
            core.c
            log.c
            main.c
            mslot.c
            sock.c
        
        include/
            core.h
            log.h
            mslot.h
            sock.h

        common/
            protocol-frame.c

--- 

## Log File
All server logs are written to:
`/SMBus-Server/SMBus.log`

## Run
`sudo ./smbus-server`

## Author
- Mustapha — System‑Level Developer,
- Mashhad, Iran.
