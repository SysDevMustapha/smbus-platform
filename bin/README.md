# SMBus Binary Directory (bin)

This directory contains all compiled binary executables for the SMBus Platform.  
Both the server and client modules place their final build outputs here to keep the project structure clean, organized, and consistent.

---

## Contents
- bin/ server/ smbus-server  # compiled server binary
- client/ smbus-client       # compiled client binary

---

## Purpose

- Keeps compiled binaries separate from source code  
- Makes the repository cleaner and easier to navigate  
- Allows server and client builds to remain independent  
- Ensures Makefile outputs are predictable and well‑structured  

---

## Notes

- The **server binary must be executed with root privileges**  
- The **client binary is lightweight, minimal, and uses a colored terminal UI**  
- Both binaries are generated automatically through their respective Makefiles  

---

## Build Location

- Server build output:
`bin/server/smbus-server`

## Client build output:
`bin/client/smbus-client`

---

## Author

Mustapha — System‑Level Developer,
Mashhad, Iran.
