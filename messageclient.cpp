#include "messageclient.h"
#include <iostream>

MessageClient::MessageClient() : MessageBase()
{
    _SessionId.clear();
    _MsgType = 0;
    _SN = 0;
    _Length = 0;
    _Adler32 = 0;
    _EncodeBuffer = nullptr;
    _EncodeLength = 0;
    _DecodeData = nullptr;
    _DecodeCount = 0;
    _EncodeBufferSize = 0;
}

MessageClient::~MessageClient()
{
    if (_EncodeBuffer != nullptr) {
        delete[] _EncodeBuffer;
        _EncodeBuffer = nullptr;
    }
}

bool MessageClient::encode(const string &session_id, unsigned int msg_type, const void *buff)
{
    _SessionId.clear();
    _SessionId = session_id;

    string msgBuf{};
    protobEncode(buff, msgBuf);

    _EncodeLength = msgBuf.length() + HeadByteSize;
    if (_EncodeBuffer == nullptr) {
        _EncodeBuffer = new unsigned char[_EncodeLength];
        _EncodeBufferSize = _EncodeLength;
    }
    else {
        if (_EncodeLength > _EncodeBufferSize) {
            delete[] _EncodeBuffer;
            _EncodeBuffer = new unsigned char[_EncodeLength];
            _EncodeBufferSize = _EncodeLength;
        }
        else {
            for (size_t i = 0; i < _EncodeBufferSize; ++i)
                _EncodeBuffer[i] = 0;
        }
    }

    //sessionid
    unsigned char sessionIdBytes[SessionIDByteSize];
    sessionId2Bytes(_SessionId, sessionIdBytes);
    for (int i = 0; i < SessionIDByteSize; ++i)
        _EncodeBuffer[i] = sessionIdBytes[i];

    //msgtype
    _MsgType = msg_type;
    unsigned char msgTypeBytes[MsgTypeByteSize];
    int2bytes(_MsgType, msgTypeBytes);
    for (int i = 0; i < MsgTypeByteSize; ++i)
        _EncodeBuffer[SessionIDByteSize+i] = msgTypeBytes[i];

    //sn
    _SN = makeSN();
    unsigned char snBytes[SNByteSize];
    int2bytes(_SN, snBytes);
    for (int i = 0; i < SNByteSize; ++i)
        _EncodeBuffer[SessionIDByteSize+MsgTypeByteSize+i] = snBytes[i];

    //length
    _Length = msgBuf.length();
    unsigned char lengthBytes[LengthByteSize];
    short2bytes(msgBuf.length(), lengthBytes);
    for (int i = 0; i < LengthByteSize; ++i)
        _EncodeBuffer[SessionIDByteSize+MsgTypeByteSize+SNByteSize+i] = lengthBytes[i];

    if (msgBuf.length() > 0) {
        //body
        for (size_t i = 0; i < msgBuf.length(); ++i)
            _EncodeBuffer[SessionIDByteSize+MsgTypeByteSize+SNByteSize+LengthByteSize+i] = msgBuf[i];

        //adler32
        unsigned char adler32Bytes[Adler32ByteSize];
        _Adler32 = makeAdler32((const unsigned char*)msgBuf.data(), msgBuf.length());
        long2bytes(_Adler32, adler32Bytes);
        for (int i = 0; i < Adler32ByteSize; ++i)
            _EncodeBuffer[SessionIDByteSize+MsgTypeByteSize+SNByteSize+LengthByteSize+msgBuf.length()+i] = adler32Bytes[i];

        //message end
        for (int i = 0; i < MsgTypeByteSize; ++i)
            _EncodeBuffer[SessionIDByteSize+MsgTypeByteSize+SNByteSize+LengthByteSize+msgBuf.length()+Adler32ByteSize+i] = msgTypeBytes[i];
    }
    else {
        //adler32
        unsigned char adler32Bytes[Adler32ByteSize];
        _Adler32 = 0;
        long2bytes(0, adler32Bytes);
        for (int i = 0; i < Adler32ByteSize; ++i)
            _EncodeBuffer[SessionIDByteSize+MsgTypeByteSize+SNByteSize+LengthByteSize+i] = adler32Bytes[i];

        //message end
        for (int i = 0; i < MsgTypeByteSize; ++i)
            _EncodeBuffer[SessionIDByteSize+MsgTypeByteSize+SNByteSize+LengthByteSize+Adler32ByteSize+i] = msgTypeBytes[i];
    }

    return true;
}

bool MessageClient::decode(const unsigned char *buff)
{
    _SessionId.clear();
    //sessionid
    unsigned char sessionIdBytes[SessionIDByteSize];
    for (int i = 0; i < SessionIDByteSize; ++i)
        sessionIdBytes[i] = buff[i];
    sessionId2String(sessionIdBytes, _SessionId);

    //msgtype
    _MsgType = bytes2int(buff, SessionIDByteSize);

    //sn
    _SN = bytes2int(buff, SessionIDByteSize+MsgTypeByteSize);

    //length
    _Length = bytes2short(buff, SessionIDByteSize+MsgTypeByteSize+SNByteSize);

    if (_Length > 0) {
        unsigned char msgBuf[_Length];
        for (int i = 0; i < _Length; ++i)
            msgBuf[i] = buff[SessionIDByteSize+MsgTypeByteSize+SNByteSize+LengthByteSize+i];

        //adler32
        _Adler32 = bytes2long(buff, SessionIDByteSize+MsgTypeByteSize+SNByteSize+LengthByteSize+_Length);

        //check
        if (_Adler32 == makeAdler32(msgBuf, _Length)) {
            return protobDecode(msgBuf, _Length);
        }
        else {
            _DecodeCount = 0;
            return false;
        }
    }
    else {
        _Adler32 = 0;
        _DecodeCount = 0;
        return true;
    }
}

bool MessageClient::isEnoughDecode(const unsigned char *buff, size_t size)
{
    if (size < HeadByteSize)
        return false;

    unsigned short len = bytes2short(buff, SessionIDByteSize+MsgTypeByteSize+SNByteSize);
    for (int i = 0; i < MsgTypeByteSize; ++i) {
        if (buff[SessionIDByteSize+i] !=
                buff[SessionIDByteSize+MsgTypeByteSize+SNByteSize+LengthByteSize+len+Adler32ByteSize+i])
            return false;
    }
    return true;
}

unsigned int  MessageClient::makeSN()
{
    static unsigned int sn = 1;
    return sn++;
}

const string&  MessageClient::getSessionId() const
{
    return _SessionId;
}

unsigned int MessageClient::getMsgType() const
{
    return _MsgType;
}

unsigned int MessageClient::getSN() const
{
    return _SN;
}

unsigned short MessageClient::getLength() const
{
    return _Length;
}

unsigned long long MessageClient::getAdler32() const
{
    return _Adler32;
}

const unsigned char *MessageClient::getEncodeBuffer() const
{
    return _EncodeBuffer;
}

unsigned short MessageClient::getEncodeLength() const
{
    return _EncodeLength;
}

const void *MessageClient::getDecodeData() const
{
    return _DecodeData;
}

int MessageClient::getDecodeCount() const
{
    return _DecodeCount;
}
