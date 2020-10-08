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

enum MessageTypeHi{
    MESSAGE_TYPE_REGISTER = 0x100,
	MESSAGE_TYPE_UNREGISTER = 0x199
};

struct BroadcastMessage{
	long int broadcastType;
	char broadcastData[MESSAGE_DATA_LEN];
};

static int CurrentUID = -1;

int getMessage(struct Message *message, char *json);
char *getMsgJson(struct Message *message);

int onReceiver(int applicationUID, void (*callback)(void))
{
	CurrentUID = applicationUID;
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

int unReceiver()
{
	if(CurrentUID == -1)return -1;
	struct Message message;
	message.from = CurrentUID;
	message.target = APPUID_SYSTEM;
	message.type = MESSAGE_TYPE_UNREGISTER;
	message.data = "";
	sendBroadcastMessage(&message);
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

	strcpy(broadcast.broadcastData, getMsgJson(message));
	if(msgsnd(id,(void *)&broadcast,MESSAGE_DATA_LEN,0) < 0)
    {
		printf("send msg error \n");
		return -1;
	}
    return 0;
}

int getMessage(struct Message *message, char *json)
{
    if(!(json && *json))return -1;
    cJSON *root = cJSON_Parse(json);
    message->from = cJSON_GetObjectItem(root, BMSG_FROM)->valueint;
    message->target = cJSON_GetObjectItem(root, BMSG_TARGET)->valueint;
    message->type = cJSON_GetObjectItem(root, BMSG_TYPE)->valueint;
    message->dataLen = cJSON_GetObjectItem(root, BMSG_DATALEN)->valueint;
    strcpy(message->data, cJSON_GetObjectItem(root, BMSG_DATA)->valuestring);
    cJSON_Delete(root);
    return 0;
}

char *getMsgJson(struct Message *message)
{
    cJSON *root = cJSON_CreateObject();
	cJSON_AddItemToObject(root, "f", cJSON_CreateNumber(message->from));
	cJSON_AddItemToObject(root, "t", cJSON_CreateNumber(message->target));
	cJSON_AddItemToObject(root, "p", cJSON_CreateNumber(message->type));
	cJSON_AddItemToObject(root, "l", cJSON_CreateNumber(MESSAGE_DATA_LEN));
	cJSON_AddItemToObject(root, "m", cJSON_CreateString(message->data));
    char *json = strdup(cJSON_Print(root));
    cJSON_Delete(root);
    return json;
}

int main()
{
	CurrentUID = APPUID_QPLAY;
	unReceiver();
	return 0;
}