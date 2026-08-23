/*
** SMBus, The Communication Platform,
** Socket Management Layer (Source).
*/

#include "sock.h"
sock_ctx_t sock_ctx;
smbus_msgframe_t msg_frame;

static void make_non_block(int fd)
{
    int flg = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, (flg | O_NONBLOCK));
}

static void send_msg(int fd)
{
    ssize_t count = sizeof(msg_frame);
    ssize_t sent = 0L;

    while (sent < count)
    {
        ssize_t n = send(fd, ((char*)&msg_frame + sent), (count - sent), MSG_NOSIGNAL);
        if (n < 0)
        {
            sock_ctx.saved_errno = errno;
            
            if (sock_ctx.saved_errno == EINTR)
            {
                n = 0;
                continue;
            }
            else if (sock_ctx.saved_errno == EPIPE)
            {
                manage_epoll(EP_CLIENT_REMOVE, fd);
                manage_client(CLIENT_DEL, fd);
                return;
            }
            else if (sock_ctx.saved_errno == EAGAIN || sock_ctx.saved_errno == EWOULDBLOCK)
            {
                return;
            }
            else
            {
                write_log("an error occurred while sending message to a client");

                n = 0;
                return;
            }
        }
        sent += n;        
    }
}

int manage_socket(short cmd)
{
    if (cmd == SOCKET_CREAT)
    {
        if ((sock_ctx.handle.server = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            return -1;           
        }
        make_non_block(sock_ctx.handle.server);
        write_log("server listening socket created");

        struct sockaddr_in addr = {0};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(PORT);
        addr.sin_addr.s_addr = INADDR_ANY;

        if (bind(sock_ctx.handle.server, (struct sockaddr*)&addr, sizeof(addr)) < 0)
        {
            return -2;
        }
        write_log("server listening socket binded successfully");

        if (listen(sock_ctx.handle.server, MAXCLIENT) < 0)
        {
            return -3;
        }
        write_log("server is listening to clients...");
        
        return 0;
    }
    else if (cmd == SOCKET_REMOVE)
    {
        close(sock_ctx.handle.server);
        write_log("server listening socket freed successfully");

        return 0;
    }
}

int manage_epoll(short cmd, int fd)
{
    if (cmd == EP_INSTANCE_CREAT)
    {
        if ((sock_ctx.handle.epoll = epoll_create1(0)) < 0)
        {
            return -1;
        }
        
        write_log("epoll instance created successfully");
        return 0;  
    }
    else if (cmd == EP_INSTANCE_DESTROY)
    {
        close(sock_ctx.handle.epoll);
        
        write_log("epoll instance freed successfully");
        return 0;
    }
    else if (cmd == EP_SERVER_ADD)
    {
        sock_ctx.event.sev.data.fd = sock_ctx.handle.server;
        sock_ctx.event.sev.events = EPOLLIN;

        if (epoll_ctl(sock_ctx.handle.epoll, EPOLL_CTL_ADD, sock_ctx.handle.server, &sock_ctx.event.sev) < 0)
        {
            return -1;
        }
        
        write_log("server listening socket added to epoll instance");
        return 0;    
    }
    else if (cmd == EP_SERVER_REMOVE)
    {
        epoll_ctl(sock_ctx.handle.epoll, EPOLL_CTL_DEL, sock_ctx.handle.server, NULL);
        
        write_log("server listening socket removed from epoll instance");
        return 0;
    }
    else if (cmd == EP_CLIENT_ADD)
    {
        sock_ctx.event.cev.data.fd = fd;
        sock_ctx.event.cev.events = EPOLLIN | EPOLLRDHUP;

        if (epoll_ctl(sock_ctx.handle.epoll, EPOLL_CTL_ADD, fd, &sock_ctx.event.cev) < 0)
        {
            return -1;
        }
        
        write_log("client added to epoll instance successfully");
        return 0;
    }
    else if (cmd == EP_CLIENT_REMOVE)
    {
        epoll_ctl(sock_ctx.handle.epoll, EPOLL_CTL_DEL, fd, NULL);
        
        write_log("a client removed from epoll instance successfully");
        return 0;
    }
    else if (cmd == EP_ALL_CLIENT_REMOVE)
    {
        for (int i = 0; i < MAXCLIENT; ++i)
        {
            if (!(sock_ctx.handle.client[i] == CONST_NUM))
            {
                epoll_ctl(sock_ctx.handle.epoll, EPOLL_CTL_DEL, sock_ctx.handle.client[i], NULL);
            }
        }

        write_log("all clients epoll freed successfully");
        return 0;
    }
}

int manage_client(short cmd, int fd)
{
    if (cmd == CLIENT_ADD)
    {
        int* new_fd = mslot(MS_GET, sock_ctx.handle.client, MAXCLIENT, -1);
        if (!new_fd)
        {
            return -1; // slot poll is full.
        }
        
        struct sockaddr_in addr = {0};
        socklen_t len = sizeof(addr);
        char ip[32] = {0}, tmp_buf[80] = {0};

        if ((*new_fd = accept(sock_ctx.handle.server, (struct sockaddr*)&addr, &len)) < 0)
        {
            mslot(MS_FREE, sock_ctx.handle.client, MAXCLIENT, *new_fd);
            return -2;
        }
        make_non_block(*new_fd);

        if (inet_ntop(AF_INET, (struct in_addr*)&addr.sin_addr, ip, sizeof(ip)))
        {
            snprintf(tmp_buf, (sizeof(tmp_buf) -1), "a client connected, IP address: %s", ip);
            write_log(tmp_buf);
        }
        else
        {
            write_log("a client connected to server, but getting ip address failed");
        }

        return *new_fd;
    }
    else if (cmd == CLIENT_DEL)
    {
        close(fd);
        mslot(MS_FREE, sock_ctx.handle.client, MAXCLIENT, fd);

        write_log("a client connected socket freed successfully");
        return 0;
    }
    else if (cmd == ALL_CLIENT_DEL)
    {
        for (int i = 0; i < MAXCLIENT; ++i)
        {
            if (!(sock_ctx.handle.client[i] == CONST_NUM))
            {
                close(sock_ctx.handle.client[i]); 
            }
        }

        mslot(MS_SET, sock_ctx.handle.client, MAXCLIENT, -1);
        write_log("all clients connected socket freed successfully");
        
        return 0;
    }
}

void broadcast_msg(void)
{
    for (int i = 0; i < MAXCLIENT; ++i)
    {
        if (!(sock_ctx.handle.client[i] == CONST_NUM))
        {
            send_msg(sock_ctx.handle.client[i]);
        }
    }
    memset(&msg_frame, 0, sizeof(msg_frame));
}

void recv_msg(int fd)
{
    ssize_t count = sizeof(msg_frame);
    ssize_t recvd = 0L;

    while (recvd < count)
    {
        ssize_t n = recv(fd, ((char*)&msg_frame + recvd), (count - recvd), MSG_DONTWAIT);
        if (n == 0)
        {
            manage_epoll(EP_CLIENT_REMOVE, fd);
            manage_client(CLIENT_DEL, fd);
            write_log("a client suddenly disconnected from server");
            
            return;
        }
        else if (n < 0)
        {
            sock_ctx.saved_errno = errno;

            if (sock_ctx.saved_errno == EINTR)
            {
                n = 0;
                continue;
            }
            else if (sock_ctx.saved_errno == EAGAIN || sock_ctx.saved_errno == EWOULDBLOCK)
            {
                return;
            }
            else
            {
                write_log("an unexpected error occurred while receiving message from a client");
                
                n = 0;
                return;
            }
        }
        recvd += n; 
    }
}

void handle_mutex_client(void)
{
    if ((sock_ctx.handle.mutex_client = accept(sock_ctx.handle.server, NULL, NULL)) < 0)
    {
        write_log("an unexpected error occurred while accepting mutex client");
        return;
    }
    make_non_block(sock_ctx.handle.mutex_client);

    msg_frame.state = STATE_FULL;
    send_msg(sock_ctx.handle.mutex_client);
    memset(&msg_frame, 0, sizeof(msg_frame));
    manage_client(CLIENT_DEL, sock_ctx.handle.mutex_client);
}

void set_attend_msg(void)
{
    char tmp_buf[256] = {0};
    strncpy(tmp_buf, msg_frame.msg, 255);
    memset(&msg_frame, 0, sizeof(msg_frame));

    msg_frame.state = STATE_MSG;
    snprintf(msg_frame.msg, (BUFSIZ -1), "\033[33m%s\033[0m \033[97mattended to smbus chat room\033[0m\n", tmp_buf);
    msg_frame.len = strlen(msg_frame.msg);
}
