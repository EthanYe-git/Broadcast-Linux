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
static int receivers[50] = {0};

int getMessage(struct Message *message, char *json);
int handleSystem(struct Message *message);
int handleQPlay(struct Message *message);
int handleAirplay(struct Message *message);
int handleSpotify(struct Message *message);

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
        /*struct BroadcastMessage sendMessage;
		sendMessage.broadcastType = APPUID_SYSTEM;
		strcpy(sendMessage.broadcastData, broadcast.broadcastData);
		if(msgsnd(id,(void *)&sendMessage,MESSAGE_DATA_LEN,0) < 0)
	    {
		    printf("error:system send broadcast error \n");
		    return 0;
		}*/
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

int handleSystem(struct Message *message)
{
    switch (message->type)
    {
    case MESSAGE_TYPE_REGISTER:
        if(message->from > 0 && message->from < 50)
            receivers[message->from] = message->from;
        struct BroadcastMessage sendMessage;
		sendMessage.broadcastType = message->from;

		strcpy(sendMessage.broadcastData, "system:register success");
		if(msgsnd(msgID,(void *)&sendMessage,MESSAGE_DATA_LEN,0) < 0)
	    {
		    printf("error:system send broadcast error \n");
		    return 0;
		}
        break;
    
    default:
        break;
    }
    return 0;
}

int handleQPlay(struct Message *message)
{
    return 0;
}

int handleAirplay(struct Message *message)
{
    return 0;
}

int handleSpotify(struct Message *message)
{
    return 0;
}