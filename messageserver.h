#ifndef MESSAGESERVER_H
#define MESSAGESERVER_H

#include <string>
#include "messagebase.h"

using namespace std;

class MessageServer : public MessageBase
{
public:
    bool encode(unsigned int msgType, unsigned int sn, unsigned int updGroup, unsigned short respCode, const void *buff);
    bool decode(const unsigned char *buff) override;
    bool isEnoughDecode(const unsigned char *buff, size_t size) override;

    unsigned int getMsgType() const;
    unsigned int getSN() const;
    unsigned int getUpdGroup() const;
    unsigned short getRespCode() const;
    unsigned short getLength() const;
    unsigned long long getAdler32() const;
    const unsigned char *getEncodeBuffer() const;
    unsigned short getEncodeLength() const;
    const void *getDecodeData() const;
    int getDecodeCount() const;

protected:
    MessageServer();
    ~MessageServer();

    enum {
        HeadByteSize = MsgTypeByteSize + SNByteSize + UpdGroupByteSize + RespCodeByteSize + LengthByteSize + Adler32ByteSize + MsgTypeByteSize,
    };

    unsigned int _MsgType;
    unsigned int _SN;
    unsigned int _UpdGroup;
    unsigned short _RespCode;
    unsigned short _Length;
    unsigned long long _Adler32;
    unsigned char *_EncodeBuffer;
    unsigned short _EncodeLength;
    void *_DecodeData;
    int _DecodeCount;

    size_t _EncodeBufferSize;
};

#endif // MESSAGESERVER_H
