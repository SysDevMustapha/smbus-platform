/*
** SMBus, The Communication Platform,
** Client Handle Slot Manager Layer (API).
*/

#ifndef MSLOT_H
#define MSLOT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#define CONST_NUM -1
#define MS_SET 0xa
#define MS_GET 0xb
#define MS_FREE 0xc

// @brief This API is main API of this layer for managing client handle slots.
int* mslot(short cmd, int* slot, int n, int fd);

#ifdef __cplusplus
}
#endif
#endif // MSLOT_H 
