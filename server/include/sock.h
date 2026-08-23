/*
** SMBus, The Communication Platform,
** Socket Management Layer (API).
*/

#ifndef SOCK_H
#define SOCK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include "mslot.h"
#include "log.h"

#define SOCKET_CREAT 0xCF
#define SOCKET_REMOVE 0xFC
#define PORT 8080
#define CLIENT_ADD 0xBA
#define CLIENT_DEL 0xAB
#define ALL_CLIENT_DEL 0xBC

#define MAXCLIENT 512
typedef struct 
{
    int server, epoll, mutex_client, client[MAXCLIENT];
}handle_t;

typedef struct 
{
    struct epoll_event sev, cev, qev[MAXCLIENT];
}event_t;

typedef struct 
{
    handle_t handle;
    event_t event;
    int saved_errno;
}sock_ctx_t;

typedef enum
{
    STATE_HELLO,
    STATE_MSG,
    STATE_LEAVE,
    STATE_FULL,
}proto_state_t;

typedef struct 
{
    char msg[BUFSIZ];
    uint32_t len;
    proto_state_t state;
}smbus_msgframe_t;

#define EP_INSTANCE_CREAT 0xab
#define EP_INSTANCE_DESTROY 0xba
#define EP_SERVER_ADD 0xac
#define EP_SERVER_REMOVE 0xca
#define EP_CLIENT_ADD 0xad
#define EP_CLIENT_REMOVE 0xda
#define EP_ALL_CLIENT_REMOVE 0xcf

// @brief This API manages server listening socket,
// @note The error code of this API are:
// @note error code -1: socket() failed,
// @note error code -2: bind() failed,
// @note error code -3: listen() failed,
// @note On success 0 is returned.
int manage_socket(short cmd);

// @brief This API manages epoll & can create/remove epoll instance, server epoll, clients epoll.
// @note On success 0 & on error -1 is returned.
int manage_epoll(short cmd, int fd);

// @brief This API can accept/disconnect a client to server,
// @note The error code of this API are:
// @note error code -1: mslot() failed, because fd poll is full,
// @note error code -2: accept() failed,
// @note On success fd of new accepted client or 0 (on cmd == CLIENT_DEL) is returned.
int manage_client(short cmd, int fd);

// @brief This API receives message from a client, and broadcastes that message to all available clients,
// than clears the message frame buffer.
void recv_msg(int fd);

// This API designed for handle mutex client, when MAXCLIENT limit reached,
// this API is used to accept only one client as mutex and sends it STATE_FULL,
// STATE_FULL is meaning, server cannot accept you as normal client so try again later.
void handle_mutex_client(void);

// This API broadcasts the received message to all clients that are connected to server.
void broadcast_msg(void);

// This API sets a client-attend message & broadcasts to all available clients. 
void set_attend_msg(void);

#ifdef __cplusplus
}
#endif

#endif // SOCK_H
