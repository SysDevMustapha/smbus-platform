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
