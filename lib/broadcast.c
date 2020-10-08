#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<sys/msg.h>
#include<sys/ipc.h>

#include "broadcast.h"
#include "cJSON.h"

#define KEY_PRO_ID 66
#define KEY_PATH "/home/tym/workspace/run/brodcast"
#define IPC_WAITFORMESSAGE 0
#define MESSAGE_DATA_LEN 1024

#define BMSG_FROM "f"
#define BMSG_TARGET "t"
#define BMSG_TYPE "p"
#define BMSG_DATALEN "l"
#define BMSG_DATA "m"

enum {
    SEND_BROADCAST = 0x101,
	SYSTEM_BROADCAST = 0x102
};

struct BroadcastMessage{
	long int broadcastType;
	char broadcastData[MESSAGE_DATA_LEN];
};

int onReceiver(int applicationUID, void (*callback)(void))
{
    int id = 0;
	key_t key = ftok(KEY_PATH,KEY_PRO_ID);
	id = msgget(key,IPC_CREAT | 0666);
	if(id == -1){
		printf("System broadcast server error!\n");
		return -1;
	}

	//1 : register receiver
	struct Message broadcast;
	broadcast.from = applicationUID;
	broadcast.target = APPUID_SYSTEM;
	broadcast.type = MESSAGE_TYPE_REGISTER;
	broadcast.data = "";
	if(sendBroadcastMessage(&broadcast) != 0){
		printf("send broadcast faild!");
	}

	//2 : receiver broadcast
	struct BroadcastMessage broadcastMsg;
    while(1){
		if(msgrcv(id,(void *)&broadcastMsg, MESSAGE_DATA_LEN, applicationUID, IPC_WAITFORMESSAGE) < 0){
			printf("Broadcast receiver error \n");
			return 0;
		}
        if(callback != NULL)callback();
		printf("System Broadcast : %s\n", broadcastMsg.broadcastData);
	}
    return 0;
}

int sendBroadcastMessage(struct Message *message)
{
	int id = 0;
	struct BroadcastMessage broadcast;
	key_t key = ftok(KEY_PATH,KEY_PRO_ID);
	id = msgget(key,IPC_CREAT | 0666);
	if(id == -1)
	{
		printf("create msg error \n");
		return 0;
	}
	broadcast.broadcastType = SEND_BROADCAST;
	//create json
	cJSON *root = cJSON_CreateObject();
	cJSON_AddItemToObject(root, "f", cJSON_CreateNumber(message->from));
	cJSON_AddItemToObject(root, "t", cJSON_CreateNumber(message->target));
	cJSON_AddItemToObject(root, "p", cJSON_CreateNumber(message->type));
	cJSON_AddItemToObject(root, "l", cJSON_CreateNumber(MESSAGE_DATA_LEN));
	cJSON_AddItemToObject(root, "m", cJSON_CreateString(message->data));
	printf("%s\n", cJSON_Print(root));

	strcpy(broadcast.broadcastData, cJSON_Print(root));
	if(msgsnd(id,(void *)&broadcast,MESSAGE_DATA_LEN,0) < 0)
    {
		printf("send msg error \n");
		return -1;
	}
	cJSON_Delete(root);
    return 0;
}