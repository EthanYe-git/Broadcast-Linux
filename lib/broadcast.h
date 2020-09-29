/*!
 * Broadcast interface
 */

enum AppUID{
    APPUID_SELF = 0x0,
    APPUID_SYSTEM = 0x5000,
    APPUID_QPLAY = 0x5001,
    APPUID_AIRPLAY = 0x5002,
    APPUID_SPOTIFY = 0x5003
};

enum MessageType{
    MESSAGE_TYPE_DATA = 0x500,
    MESSAGE_TYPE_VOLUME = 0x501,
    MESSAGE_TYPE_STATE = 0x502
};

struct APP{
    int AppUID;
    int AppType;
    int AppNameLen;
    char *AppName;
};


struct Message
{
    long int from;
    long int target;
    int type;
    int dataLen;
    char *data;
};

int onReceiver(struct APP *data, void (*callback)(void));

int sendBroadcastMessage(struct Message *message);