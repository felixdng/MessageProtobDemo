#ifndef MESSAGECLIENT_H
#define MESSAGECLIENT_H

#include <string>
#include "messagebase.h"

using namespace std;

class MessageClient : public MessageBase
{
public:
    bool encode(const string &session_id, unsigned int msg_type, const void *buff);
    bool decode(const unsigned char *buff) override;
    bool isEnoughDecode(const unsigned char *buff, size_t size) override;

    const string& getSessionId() const;
    unsigned int getMsgType() const;
    unsigned int getSN() const;
    unsigned short getLength() const;
    unsigned long long getAdler32() const;
    const unsigned char *getEncodeBuffer() const;
    unsigned short getEncodeLength() const;
    const void *getDecodeData() const;
    int getDecodeCount() const;

protected:
    MessageClient();
    ~MessageClient();

    enum {
        HeadByteSize = SessionIDByteSize + MsgTypeByteSize + SNByteSize + LengthByteSize + Adler32ByteSize + MsgTypeByteSize,
    };

    unsigned int makeSN();

    string _SessionId;
    unsigned int _MsgType;
    unsigned int _SN;
    unsigned short _Length;
    unsigned long long _Adler32;
    unsigned char *_EncodeBuffer;
    unsigned short _EncodeLength;
    void *_DecodeData;
    int _DecodeCount;

    size_t _EncodeBufferSize;
};

#endif // MESSAGECLIENT_H
