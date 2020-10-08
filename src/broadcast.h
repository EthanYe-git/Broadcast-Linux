/*!
 * Broadcast interface
 */

enum AppUID{
    APPUID_SYSTEM = 0x6,
    APPUID_QPLAY = 0x7,
    APPUID_AIRPLAY = 0x8,
    APPUID_SPOTIFY = 0x9
};

enum MessageType{
    MESSAGE_TYPE_REGISTER = 0x100,
    MESSAGE_TYPE_STRING = 0x101,
    MESSAGE_TYPE_VOLUME = 0x102,
    MESSAGE_TYPE_STATE = 0x103
};

struct Message
{
    int from;        //sender AppUID
    int target;      //receiver AppUID
    int type;             //MessageType
    int dataLen;          //broadcast message len
    char *data;           //broadcast message
};