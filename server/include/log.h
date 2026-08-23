/*
** SMBus, The Communication Platform,
** Logger Layer (API).
*/

#ifndef LOG_H
#define LOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>

typedef short code;
#define RS_CREAT 0x1
#define RS_CLOSE 0x2

#define PATHSIZ 256
typedef struct 
{
    char log_buf[BUFSIZ], log_path[PATHSIZ], log_dir[PATHSIZ];
    int log_fd, saved_errno;
}log_ctx_t;

// @brief This API manages log file, opens log file & closes log file.
void manage_resource(short mode);

// @brief This API writes the log txt in log file.
void write_log(const char* log);

#ifdef __cplusplus
}
#endif
#endif // LOG_H
