/*
** SMBus, The Communication Platform,
** Logger Layer (Source).
*/

#include "log.h"
log_ctx_t log_ctx;

static void set_path(void)
{
    strncpy(log_ctx.log_path, "/SMBus-Server/SMBus.log", (PATHSIZ -1));
    strncpy(log_ctx.log_dir, "/SMBus-Server/",           (PATHSIZ -1));
}

static void set_time(void)
{
    time_t now = time(NULL);
    struct tm* time_info = localtime(&now);
    strftime(log_ctx.log_buf, (BUFSIZ -1), "\033[33m%Y,%m,%d - %H:%M:%S\033[0m \033[97m", time_info);
}

static void set_log(const char* log)
{
    memset(log_ctx.log_buf, 0, BUFSIZ);

    set_time();
    strncat(log_ctx.log_buf, log,         (BUFSIZ -(strlen(log_ctx.log_buf) +1)));
    strncat(log_ctx.log_buf, "\033[0m\n", (BUFSIZ -(strlen(log_ctx.log_buf) +1)));
}

void manage_resource(short mode)
{
    if (mode == RS_CREAT)
    {   
        set_path();
        mkdir(log_ctx.log_dir, 0644);

        if ((log_ctx.log_fd = open(log_ctx.log_path, O_CREAT | O_EXCL | O_TRUNC | O_WRONLY, 0644)) < 0)
        {
            log_ctx.saved_errno = errno;
            
            if (log_ctx.saved_errno == EACCES || log_ctx.saved_errno == EPERM)
            {
                printf("\033[33mHey! dear admin,\033[0m \033[97mSMBus Server\033[0m\033[33m need to root access to work :)\033[0m\n");
                exit(EXIT_SUCCESS);
            }
            else if (log_ctx.saved_errno == EEXIST)
            {
                if ((log_ctx.log_fd = open(log_ctx.log_path, O_TRUNC | O_WRONLY)) < 0)
                {
                    fprintf(stderr, "\033[33mError:\033[0m \033[97mlog file opening failed because ->\033[0m\033[33m%s\033[0m",
                        strerror(log_ctx.saved_errno));
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                fprintf(stderr, "\033[33mError:\033[0m \033[97mlog file creation failed because ->\033[0m \033[33m%s\033[0m",
                    strerror(log_ctx.saved_errno));
                exit(EXIT_FAILURE);
            }
        }
        
    }
    else if (mode == RS_CLOSE)
    {
        close(log_ctx.log_fd);
    }
}

void write_log(const char* log)
{
    set_log(log);

    ssize_t count = strlen(log_ctx.log_buf);
    ssize_t wrote = 0L;

    while (wrote < count)
    {
        ssize_t n = write(log_ctx.log_fd, (log_ctx.log_buf + wrote), (count - wrote));
        if (n < 0)
        {
            log_ctx.saved_errno = errno;

            if (log_ctx.saved_errno == EBADF)
            {
                perror("The log file handle is invalid");
                manage_resource(RS_CLOSE);
                exit(EXIT_FAILURE);
            }
            else
            {
                perror("a suddenly-error occurred while logging");
                n = 0;
                return;
            }
        }
        
        wrote += n;
    }
}
