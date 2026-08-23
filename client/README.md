# SMBus Client

A minimal, lightweight terminal client built for the SMBus Platform.  
Designed to be fast, responsive, and easy to use — with a clean colored UI and a simple interaction model.  
The client communicates using fixed-size binary frames and follows the shared SMBus protocol.

---

## Features

- Minimal and lightweight design  
- Select-based non-blocking I/O  
- Colored terminal UI for better readability  
- Username handshake (`STATE_HELLO`)  
- Graceful handling of `STATE_FULL`  
- Fixed-size binary message frames  
- Fully compatible with SMBus protocol (shared with server)  
- Zero external dependencies — pure C, pure terminal

---

## Why This Client Is Great

### ⚡ Extremely Lightweight  
No heavy libraries, no complex UI layers — just pure C and select().  
Runs fast even on low-resource systems.

### 🎨 Clean Colored UI  
Messages, states, and events are color-coded for clarity.  
This makes the client pleasant to use and easy to follow during high message traffic.

### 🧩 Minimal Yet Complete  
Despite being small, it supports:
- handshake  
- messaging  
- leaving  
- full-state rejection  
- frame-based communication  

Everything needed for a real system-level test environment.

---

## Source Structure

    client/
  
       src/
         client.c
         main.c
         
    include/
        smbus.h

---

## Run
./smbus-client `<ip> <port> <username>`

### Example:
./smbus-client `127.0.0.1 8080 mustapha`

## Author
Mustapha — System-Level Developer,
Mashhad, Iran.
