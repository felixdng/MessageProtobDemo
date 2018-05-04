#ifndef PROTOBUFBASE_H
#define PROTOBUFBASE_H

#include <map>

using namespace std;

class ProtoBufBase
{
public:
    ProtoBufBase();
    ~ProtoBufBase();

    //message types
    enum {
        MT_Connection = 0x6000F001,
        //...
    };

    map<unsigned int, bool (*)(const void *, string &)> protobEncodeMap;
    map<unsigned int, bool (*)(const char *, size_t, void **)> protobDecodeMap;
};

#endif // PROTOBUFBASE_H
