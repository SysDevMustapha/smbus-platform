# SMBus Protocol Frame

The protocol frame defines the shared binary message format used by both the server and client.  
It ensures that all messages follow a unified structure, making communication predictable, efficient, and easy to parse in non‑blocking I/O environments.

---

## Structure

```c
typedef enum
{
    STATE_HELLO,
    STATE_MSG,
    STATE_LEAVE,
    STATE_FULL,
} proto_state_t;

typedef struct
{
    char msg[BUFSIZ];
    uint32_t len;
    proto_state_t state;
} smbus_msgframe_t;
```

---

## Description
- msg — message buffer
- len — actual message length
- state — protocol state controlling message flow
  
The frame is fixed‑size, binary‑safe, and optimized for epoll/select‑based communication.
Both server and client use this exact structure to encode and decode messages.

---

## Purpose
- Unified message format
- Shared protocol states
- Simple parsing
- Reliable communication between server ↔ client

---

## Author
Mustapha — System‑Level Developer,
Mashhad, Iran.
