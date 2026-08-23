/*
** SMBus Client
** Client Core Layer(Source)
*/

#include "smbus.h"
bool first_msg = true;

static int connect_server(const char *ip, int port)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port   = htons((uint16_t)port);

    if (inet_pton(AF_INET, ip, &addr.sin_addr) < 1)
    {
        fprintf(stderr, "invalid ip\n");
        close(sockfd);
        return -1;
    }

    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("connect");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

static int send_frame(int fd, smbus_msgframe_t *frame)
{
    ssize_t count = sizeof(*frame);
    ssize_t sent = 0L;

    while (sent < count)
    {
        ssize_t n = send(fd, ((char*)frame + sent), (count - sent), MSG_NOSIGNAL);
        if (n < 0)
        {
            perror("send");
            return -1;
        }
        sent += n;
    }
    return 0;
}

static int recv_frame(int fd, smbus_msgframe_t *frame)
{
    ssize_t count = sizeof(*frame);
    ssize_t recvd = 0L;

    while (recvd < count)
    {
        ssize_t n = recv(fd, ((char*)frame + recvd), (count - recvd), 0);
        if (n == 0)
        {
            return 0;
        }
        else if (n < 0)
        {
            perror("recv");
            return -1;
        }
        recvd += n;
    }
    return 1;
}

static int handle_msg(smbus_msgframe_t* in, const char* username)
{
    switch (in->state)
    {
        case STATE_MSG:
            if (in->len > 0 && in->len < BUFSIZ)
            {
                in->msg[in->len] = '\0';

                if (first_msg)
                {
                    printf("\n%s", in->msg);
                    first_msg = false;               
                }
                else
                {
                    printf("\n\033[33m%s:\033[0m \033[97m%s\033[0m", username, in->msg);
                }
            }
            return 0;
        break; 

        case STATE_FULL:
            fprintf(stderr, "\n\033[33mErr:\033[0m \033[97mSMBus server is full, please try at another time\033[0m");
            fflush(stderr);
            return -1;
        break;
    }
}

static void show_welcome(const char* ip, int port)
{
    printf("\033[33mConnected to\033[0m \033[97m%s:%d\033[0m\n", ip, port);
    printf("\033[33mPlease wait a moment...\033[0m");
    fflush(stdout); sleep(2);

    system("clear");
    printf("\033[33m---------------------------\033[0m");
    printf("\033[33mWelcome To\033[0m \033[97mSMBus\033[0m \033[33mChat Room\033[0m");
    printf("\033[33m---------------------------\033[0m\n");
    fflush(stdout); 
}

void client_core(char** argv)
{
    int  port = atoi(argv[2]);
    const char* ip = argv[1];
    const char* username = argv[3];
    signal(SIGPIPE, SIG_IGN);

    int sockfd = connect_server(ip, port);
    if (sockfd < 0)
    {
        exit(EXIT_FAILURE);
    }
    
    show_welcome(ip, port);

    /* send HELLO with username */
    smbus_msgframe_t frame = {0};
    frame.state = STATE_HELLO;
    strncpy(frame.msg, username, BUFSIZ - 1);
    frame.len = strlen(frame.msg);

    if (send_frame(sockfd, &frame) < 0)
    {
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(sockfd, &rfds);
        FD_SET(STDIN_FILENO, &rfds);

        int maxfd = (sockfd > STDIN_FILENO ? sockfd : STDIN_FILENO) + 1;
        int ret = select(maxfd, &rfds, NULL, NULL, NULL);
        if (ret < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            else
            {
                perror("select");
                break;
            }
        }

        if (FD_ISSET(sockfd, &rfds))
        {
            smbus_msgframe_t in = {0};
            int r = recv_frame(sockfd, &in);
            if (r == 0)
            {
                printf("Server closed.\n");
                break;
            }
            else if (r < 0)
            {
                break;
            }

            if (handle_msg(&in, username) < 0)
            {
                break;
            }

            memset(&in, 0, sizeof(smbus_msgframe_t));
            printf("\033[33mWrite a message: \033[0m");
            fflush(stdout);
        }

        if (FD_ISSET(STDIN_FILENO, &rfds))
        {
            char buf[BUFSIZ] = {0};

            if (!fgets(buf, sizeof(buf), stdin))
            {
                break;
            }

            smbus_msgframe_t out = {0};
            out.state = STATE_MSG;
            strncpy(out.msg, buf, BUFSIZ - 1);
            out.len = strlen(out.msg);

            if (send_frame(sockfd, &out) < 0)
            {
                break;
            }
            
            memset(&out, 0, sizeof(smbus_msgframe_t));
        }
    }

    close(sockfd);
}
