#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<sys/msg.h>
#include<sys/ipc.h>
#include "broadcast.h"
#include "cJSON.h"

#define DEBUG 1
#define KEY_PATH "/home/tym/workspace/run/brodcast"
#define KEY_PRO_ID 66
#define IPC_WAITFORMESSAGE 0
#define MESSAGE_DATA_LEN 1024

#define RECEIVERS_MIN 0
#define RECEIVERS_MAX 50

#define BMSG_FROM "f"
#define BMSG_TARGET "t"
#define BMSG_TYPE "p"
#define BMSG_DATALEN "l"
#define BMSG_DATA "m"

enum {
    BROADCAST_MESSAGE = 0x101,
};

struct BroadcastMessage{
	long int broadcastType;
	char broadcastData[MESSAGE_DATA_LEN];
};

static int msgID = 0;
static key_t msgKey;
static int receivers[RECEIVERS_MAX] = {0};

int getMessage(struct Message *message, char *json);
char *getMsgJson(struct Message *message);
int handleSystem(struct Message *message);
int handleQPlay(struct Message *message);
int handleAirplay(struct Message *message);
int handleSpotify(struct Message *message);

int sendBroadcast(struct Message *message);

int main()
{
	struct BroadcastMessage broadcast;
	msgKey = ftok(KEY_PATH,KEY_PRO_ID);
	msgID = msgget(msgKey,IPC_CREAT | 0666);
	if(msgID == -1)
	{
		printf("create msg error \n");
		return 0;
	}
    //if(msgctl(id,IPC_RMID,NULL) < 0);
    while(1)
	{
		if(msgrcv(msgID,(void *)&broadcast, MESSAGE_DATA_LEN, BROADCAST_MESSAGE, IPC_WAITFORMESSAGE) < 0)
		{
			printf("error:receive broadcast error \n");
			return 0;
		}
		//解析广播
        char *json = broadcast.broadcastData;
        if(DEBUG)printf("%s\n", json);
        struct Message message;
        getMessage(&message, json);

        //组织返回信息
        switch (message.target){
        case APPUID_SYSTEM:
            handleSystem(&message);
            break;
        case APPUID_QPLAY:
            handleQPlay(&message);
            break;
        case APPUID_AIRPLAY:
            handleAirplay(&message);
            break;
        case APPUID_SPOTIFY:
            handleSpotify(&message);
            break;
        default:
            break;
        }
	}
sys_exit:
    if(msgctl(msgID,IPC_RMID,NULL) < 0)
	{
		printf("error:del msg error \n");
		return 0;
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

int sendBroadcast(struct Message *message)
{
    if(message->target < RECEIVERS_MIN || message->target >= RECEIVERS_MAX)
        return -1;
    if(receivers[message->target] != message->target)
        return -1;
    struct BroadcastMessage broadcast;
    broadcast.broadcastType = message->target;
    strcpy(broadcast.broadcastData, getMsgJson(message));

    printf("info:data : %s\n", broadcast.broadcastData);
    if(msgsnd(msgID,(void *)&broadcast,MESSAGE_DATA_LEN,0) < 0)
    {
		printf("send msg error \n");
		return -1;
	}
    return 0;
}

int handleSystem(struct Message *message)
{
    struct Message returnMsg;

    switch (message->type)
    {
    case MESSAGE_TYPE_REGISTER:
        receivers[message->from] = message->from;

        returnMsg.from = APPUID_SYSTEM;
        returnMsg.target = message->from;
        returnMsg.type = MESSAGE_TYPE_REGISTER;
        returnMsg.data = "register success";
        sendBroadcast(&returnMsg);
        break;
    
    default:
        break;
    }
    return 0;
}

int handleQPlay(struct Message *message)
{
    sendBroadcast(message);
    return 0;
}

int handleAirplay(struct Message *message)
{
    sendBroadcast(message);
    return 0;
}

int handleSpotify(struct Message *message)
{
    sendBroadcast(message);
    return 0;
}