#ifndef MESSAGEBASE_H
#define MESSAGEBASE_H

#include <string>

using namespace std;

class MessageBase
{
public:
    //virtual bool encode(const string&, unsigned int, const void *) = 0;
    virtual bool decode(const unsigned char *) = 0;
    virtual bool isEnoughDecode(const unsigned char *, size_t) = 0;
    virtual bool protobEncode(const void *, string &) = 0;
    virtual bool protobDecode(const unsigned char *, size_t) = 0;

    //message types
    enum {
        TestToolConnection = 0x6000F001,
        //...
    };

protected:
    MessageBase();
    ~MessageBase();

    //head byte size
    enum {
        SessionIDByteSize = 16,
        MsgTypeByteSize   = 4,
        SNByteSize        = 4,
        LengthByteSize    = 2,
        Adler32ByteSize   = 8,
        UpdGroupByteSize  = 4,
        RespCodeByteSize  = 2,
    };

    enum {
        ShortByteSize = 2,
        IntByteSize   = 4,
        LongByteSize  = 8,
        BitsPerByte   = 8,
    };

    bool sessionId2Bytes(const string& id , unsigned char *bytes);
    bool sessionId2String(const unsigned char *bytes, string &id);
    unsigned long long  makeAdler32(const unsigned char *buf, int length);

    void short2bytes(unsigned short num, unsigned char *bytes);
    void int2bytes(unsigned int num, unsigned char *bytes);
    void long2bytes(unsigned long long num, unsigned char *bytes);

    unsigned short bytes2short(const unsigned char *bytes, int startIndex);
    unsigned int bytes2int(const unsigned char *bytes, int startIndex);
    unsigned long long bytes2long(const unsigned char *bytes, int startIndex);
};

#endif // MESSAGEBASE_H
