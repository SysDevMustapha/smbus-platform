/*
** SMBus Client
** Client Layer(API)
*/

#ifndef SMBUS_H
#define SMBUS_H

#ifdef __cplusplus
extern "C" {
#endif

#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <sys/socket.h>

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
}smbus_msgframe_t;

// This API is SMBus client's main core.
void client_core(char** argv);

#ifdef __cplusplus
}
#endif
#endif // SMBUS_H
