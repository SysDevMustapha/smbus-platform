/*
** SMBus, The Communication Platform,
** Main Layer.
*/

#include "smbus.h"

static void check_arg(int argc, char** argv)
{
    if (argc != 4)
    {
        fprintf(stderr, "\033[97mUsage:\033[0m \033[33mSMBus <server-ip> <port> <username>\033[0m\n");
        exit(EXIT_FAILURE);
    }
}
    
int main(int argc, char** argv)
{
    check_arg(argc, argv);
    client_core(argv);
    return 0;
}
