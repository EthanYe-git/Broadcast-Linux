#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<sys/msg.h>
#include<sys/ipc.h>

#include "broadcast.h"

#define KEY_PATH "/home/tym/workspace/run/brodcast"
#define IPC_WAITFORMESSAGE 0
#define TEST_APPLICATIONS_COUNT 2
#define MESSAGE_DATA_LEN 1024

enum {
    SEND_BROADCAST = 0x101,
	SYSTEM_BROADCAST = 0x102
};

struct BroadcastMessage{
	long int messageType;
	char messageData[MESSAGE_DATA_LEN];
};

int getAppUID();

int onReceiver(struct APP *data, void (*callback)(void))
{
    int id = 0;
	struct BroadcastMessage message;
    message.messageType = SYSTEM_BROADCAST;
	key_t key = ftok(KEY_PATH,66);
	id = msgget(key,IPC_CREAT | 0666);
	if(id == -1){
		printf("System broadcast server error!\n");
		return -1;
	}
    while(1){
		if(msgrcv(id,(void *)&message, MESSAGE_DATA_LEN, SYSTEM_BROADCAST, IPC_WAITFORMESSAGE) < 0){
			printf("Broadcast receiver error \n");
			return 0;
		}
        if(callback != NULL)callback();
		printf("System Broadcast : %s\n", message.messageData);
	}
    return 0;
}

int sendBroadcastMessage(struct Message *message)
{
    return 0;
}
