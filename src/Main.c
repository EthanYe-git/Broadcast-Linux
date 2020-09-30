#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<sys/msg.h>
#include<sys/ipc.h>

#define KEY_PATH "/home/tym/workspace/run/brodcast"
#define IPC_WAITFORMESSAGE 0
#define MESSAGE_DATA_LEN 1024

enum {
    SEND_BROADCAST = 0x101,
	BROADCAST_MESSAGE = 0x401
};

