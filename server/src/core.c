/*
** SMBus, The Communication Platform,
** Core Layer (Source).
*/

#include "core.h"
core_ctx_t core_ctx;
extern sock_ctx_t sock_ctx;
extern smbus_msgframe_t msg_frame;

static void sig_handler(int sig)
{
    if (sig == SIGINT || sig == SIGTSTP)
    {
        core_ctx.terminate = true;
    }
}

static void init_settings(void)
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, sig_handler);
    signal(SIGTSTP, sig_handler);
    core_ctx.terminate = false;

    manage_resource(RS_CREAT);
    write_log("SMBus - The Communication Platform, Server started...");

    if ((core_ctx.check_stat = manage_socket(SOCKET_CREAT)) < 0)
    {
        if (core_ctx.check_stat == -1)
        {
            write_log("an error occurred while creating server listening socket");
            manage_resource(RS_CLOSE);
            exit(EXIT_FAILURE);
        }
        else if (core_ctx.check_stat == -2)
        {
            write_log("binding server listening socket failed, an error occurred");
            manage_socket(SOCKET_REMOVE);
            manage_resource(RS_CLOSE);
            exit(EXIT_FAILURE);
        }
        else if (core_ctx.check_stat == -3)
        {
            write_log("failed to start listening on server socket");
            manage_socket(SOCKET_REMOVE);
            manage_resource(RS_CLOSE);
            exit(EXIT_FAILURE);
        }
    }
    
    if ((core_ctx.check_stat = manage_epoll(EP_INSTANCE_CREAT, -1)) < 0)
    {
        write_log("failed to creating epoll instance");
        manage_socket(SOCKET_REMOVE);
        manage_resource(RS_CLOSE);
        exit(EXIT_FAILURE);
    }

    if ((core_ctx.check_stat = manage_epoll(EP_SERVER_ADD, -1)) < 0)
    {
        write_log("adding server to epoll instance failed");
        manage_epoll(EP_INSTANCE_DESTROY, -1);
        manage_socket(SOCKET_REMOVE);
        manage_resource(RS_CLOSE);
        exit(EXIT_FAILURE);
    }
    
    mslot(MS_SET, sock_ctx.handle.client, MAXCLIENT, -1);
    write_log("server initial settings set successfully");
}

static void shutdown_server(void)
{
    manage_epoll(EP_ALL_CLIENT_REMOVE, -1);
    manage_epoll(EP_SERVER_REMOVE,     -1);
    manage_epoll(EP_INSTANCE_DESTROY,  -1);
    write_log("epoll resources freed successfully");

    manage_client(ALL_CLIENT_DEL,      -1);
    manage_socket(SOCKET_REMOVE);
    write_log("socket resources freed successfully");

    write_log("SMBus Server was shut down successfully");
    manage_resource(RS_CLOSE);
}

static void handle_msg(int fd)
{
    /*
        message states for handling with state-machine are:

        STATE_HELLO,
        STATE_MSG,
        STATE_LEAVE,
        STATE_FULL,    Note: client cannot send this type!
    */

    switch (msg_frame.state)
    {
        case STATE_HELLO:
                set_attend_msg();
                broadcast_msg();
            break;
    
        case STATE_MSG:
                broadcast_msg();
            break;

        case STATE_LEAVE:
                manage_epoll(EP_CLIENT_REMOVE, fd);
                manage_client(CLIENT_DEL, fd);
            break;
    }
}

void server_loop(void)
{
    init_settings();

    do
    {
        int n = epoll_wait(sock_ctx.handle.epoll, sock_ctx.event.qev, MAXCLIENT, -1);
        if (n < 0)
        {
            core_ctx.saved_errno = errno;
            if (core_ctx.saved_errno == EINTR)
            {
                continue;
            }
            
            write_log("epoll waiting failed unexpectedly");
            continue;
        }
        
        for (int i = 0; i < n; ++i)
        {
            if (sock_ctx.event.qev[i].data.fd == sock_ctx.handle.server) // in server.
            {
                if ((core_ctx.tmp_fd = manage_client(CLIENT_ADD, -1)) < 0)
                {
                    if (core_ctx.tmp_fd == -1)
                    {
                        handle_mutex_client();
                    }
                    else if (core_ctx.tmp_fd == -2)
                    {
                        write_log("failed to accept new client, an unexpected error occurred");
                    }
                    continue;
                }

                if ((core_ctx.check_stat = manage_epoll(EP_CLIENT_ADD, core_ctx.tmp_fd)) < 0)
                {
                    write_log("an error occurred while adding a client to epoll system");
                    manage_client(CLIENT_DEL, core_ctx.tmp_fd);
                    continue;
                }

                write_log("a client added successfully");
            }
            else if (!(sock_ctx.event.qev[i].data.fd == sock_ctx.handle.server)) // in client.
            {
                core_ctx.tmp_fd = sock_ctx.event.qev[i].data.fd;

                if (sock_ctx.event.qev[i].events == EPOLLIN)
                {
                    recv_msg(core_ctx.tmp_fd);
                    handle_msg(core_ctx.tmp_fd);
                    continue;
                }
                else if (sock_ctx.event.qev[i].events == EPOLLRDHUP)
                {
                    manage_epoll(EP_CLIENT_REMOVE, core_ctx.tmp_fd);
                    manage_client(CLIENT_DEL, core_ctx.tmp_fd);
                    continue;
                }
            }
        }

    } while (!(core_ctx.terminate));
    
    shutdown_server();
}
