/*
** SMBus, The Communication Platform,
** Core Layer (API).
*/

#ifndef CORE_H
#define CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include "mslot.h"
#include "sock.h"
#include "log.h"


typedef struct 
{
    bool terminate;
    int check_stat, saved_errno, tmp_fd;
}core_ctx_t;

// This designed API is main loop of smbus server.
void server_loop(void);

#ifdef __cplusplus
}
#endif
#endif // CORE_H
