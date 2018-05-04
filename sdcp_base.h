#ifndef SDCP_BASE_H
#define SDCP_BASE_H

#include "protobufbase.h"
#include <string>

using namespace std;

class SDCP_Base : public ProtoBufBase
{
public:
    virtual bool decode(const char *, size_t) = 0;
    //virtual bool protobEncode(const void *, string &) = 0;
    //virtual bool protobDecode(const char *, size_t) = 0;

protected:
    SDCP_Base();
    ~SDCP_Base();

    //head info byte size
    enum {
        SessionIDByteSize = 16,
        MsgTypeByteSize   = 4,
        SNByteSize        = 4,
        LengthByteSize    = 2,
        Adler32ByteSize   = 8,
        UpdGroupByteSize  = 4,
        RespCodeByteSize  = 2,

        //client head size
        CHeadByteSize = SessionIDByteSize + MsgTypeByteSize + SNByteSize + LengthByteSize + Adler32ByteSize,

        //server head size
        SHeadByteSize = MsgTypeByteSize + SNByteSize + UpdGroupByteSize + RespCodeByteSize + LengthByteSize + Adler32ByteSize
    };

    enum {
        ShortByteSize = 2,
        IntByteSize   = 4,
        LongByteSize  = 8,
        BitsPerByte   = 8,
    };

    bool sessionId2Bytes(const string& id , char *bytes);
    bool sessionId2String(const char *bytes, string &id);
    unsigned long long  makeAdler32(const char *buf, int length);

    void short2bytes(unsigned short num, char *bytes);
    void int2bytes(unsigned int num, char *bytes);
    void long2bytes(unsigned long long num, char *bytes);

    unsigned short bytes2short(const char *bytes, int startIndex);
    unsigned int bytes2int(const char *bytes, int startIndex);
    unsigned long long bytes2long(const char *bytes, int startIndex);

private:
    // = Disallow copying and assignment
    SDCP_Base(const SDCP_Base &);
    void operator=(const SDCP_Base &);
};

#endif // SDCP_BASE_H
