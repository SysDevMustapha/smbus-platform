/*
** SMBus, The Communication Platform,
** Client Handle Slot Manager Layer (Source).
*/

#include "mslot.h"

int* mslot(short cmd, int* slot, int n, int fd)
{
    if (cmd == MS_SET)
    {
        for (int i = 0; i < n; ++i)
        {
            slot[i] = CONST_NUM;
        }
    }
    else if (cmd == MS_GET)
    {
        for (int i = 0; i < n; ++i)
        {
            if (slot[i] == CONST_NUM)
            {
                return &slot[i];
            }
        }

        return NULL;
    }
    else if (cmd == MS_FREE)
    {
        for (int i = 0; i < n; ++i)
        {
            if (slot[i] == fd)
            {
                slot[i] = CONST_NUM;
            }
        }
    }
    
}
