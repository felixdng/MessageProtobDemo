#include "messageserver.h"

MessageServer::MessageServer() : MessageBase()
{
    _MsgType = 0;
    _SN = 0;
    _UpdGroup = 0;
    _RespCode = 0;
    _Length = 0;
    _Adler32 = 0;
    _EncodeBuffer = nullptr;
    _EncodeLength = 0;
    _DecodeData = nullptr;
    _DecodeCount = 0;
    _EncodeBufferSize = 0;
}

MessageServer::~MessageServer()
{
    if (_EncodeBuffer != nullptr) {
        delete[] _EncodeBuffer;
        _EncodeBuffer = nullptr;
    }
}

bool MessageServer::encode(unsigned int msgType, unsigned int sn, unsigned int updGroup, unsigned short respCode, const void *buff)
{
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

    //msgtype
    _MsgType = msgType;
    unsigned char msgTypeBytes[MsgTypeByteSize];
    int2bytes(_MsgType, msgTypeBytes);
    for (int i = 0; i < MsgTypeByteSize; ++i)
        _EncodeBuffer[i] = msgTypeBytes[i];

    //sn
    _SN = sn;
    unsigned char snBytes[SNByteSize];
    int2bytes(_SN, snBytes);
    for (int i = 0; i < SNByteSize; ++i)
        _EncodeBuffer[MsgTypeByteSize+i] = snBytes[i];

    //updgroup
    _UpdGroup = updGroup;
    unsigned char updGroupBytes[UpdGroupByteSize];
    int2bytes(_UpdGroup, updGroupBytes);
    for (int i = 0; i < UpdGroupByteSize; ++i)
        _EncodeBuffer[MsgTypeByteSize+SNByteSize+i] = updGroupBytes[i];

    //respcode
    _RespCode = respCode;
    unsigned char respCodeBytes[RespCodeByteSize];
    short2bytes(_RespCode, respCodeBytes);
    for (int i = 0; i < RespCodeByteSize; ++i)
        _EncodeBuffer[MsgTypeByteSize+SNByteSize+UpdGroupByteSize+i] = respCodeBytes[i];

    //length
    _Length = msgBuf.length();
    unsigned char lengthBytes[LengthByteSize];
    short2bytes(msgBuf.length(), lengthBytes);
    for (int i = 0; i < LengthByteSize; ++i)
        _EncodeBuffer[MsgTypeByteSize+SNByteSize+UpdGroupByteSize+RespCodeByteSize+i] = lengthBytes[i];

    if (msgBuf.length() > 0) {
        //body
        for (size_t i = 0; i < msgBuf.length(); ++i)
            _EncodeBuffer[MsgTypeByteSize+SNByteSize+UpdGroupByteSize+RespCodeByteSize+LengthByteSize+i] = msgBuf[i];

        //adler32
        unsigned char adler32Bytes[Adler32ByteSize];
        _Adler32 = makeAdler32((const unsigned char*)msgBuf.data(), msgBuf.length());
        long2bytes(_Adler32, adler32Bytes);
        for (int i = 0; i < Adler32ByteSize; ++i)
            _EncodeBuffer[MsgTypeByteSize+SNByteSize+UpdGroupByteSize+RespCodeByteSize+LengthByteSize+msgBuf.length()+i] = adler32Bytes[i];

        //message end
        for (int i = 0; i < MsgTypeByteSize; ++i)
            _EncodeBuffer[MsgTypeByteSize+SNByteSize+UpdGroupByteSize+RespCodeByteSize+LengthByteSize+msgBuf.length()+Adler32ByteSize+i] = msgTypeBytes[i];
    }
    else {
        //adler32
        unsigned char adler32Bytes[Adler32ByteSize];
        _Adler32 = 0;
        long2bytes(0, adler32Bytes);
        for (int i = 0; i < Adler32ByteSize; ++i)
            _EncodeBuffer[MsgTypeByteSize+SNByteSize+UpdGroupByteSize+RespCodeByteSize+LengthByteSize+i] = adler32Bytes[i];

        //message end
        for (int i = 0; i < MsgTypeByteSize; ++i)
            _EncodeBuffer[MsgTypeByteSize+SNByteSize+UpdGroupByteSize+RespCodeByteSize+LengthByteSize+Adler32ByteSize+i] = msgTypeBytes[i];
    }

    return true;
}

bool MessageServer::decode(const unsigned char *buff)
{
    //msgtype
    _MsgType = bytes2int(buff, 0);

    //sn
    _SN = bytes2int(buff, MsgTypeByteSize);

    //updgroup
    _UpdGroup = bytes2int(buff, MsgTypeByteSize+SNByteSize);

    //respcode
    _RespCode = bytes2short(buff, MsgTypeByteSize+SNByteSize+UpdGroupByteSize);

    //length
    _Length = bytes2short(buff, MsgTypeByteSize+SNByteSize+UpdGroupByteSize+RespCodeByteSize);

    if (_Length > 0) {
        unsigned char msgBuf[_Length];
        for (int i = 0; i < _Length; ++i)
            msgBuf[i] = buff[MsgTypeByteSize+SNByteSize+UpdGroupByteSize+RespCodeByteSize+LengthByteSize+i];

        //adler32
        _Adler32 = bytes2long(buff, MsgTypeByteSize+SNByteSize+UpdGroupByteSize+RespCodeByteSize+LengthByteSize+_Length);

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

bool MessageServer::isEnoughDecode(const unsigned char *buff, size_t size)
{
    if (size < HeadByteSize)
        return false;

    unsigned short len = bytes2short(buff, MsgTypeByteSize+SNByteSize+UpdGroupByteSize+RespCodeByteSize);
    for (int i = 0; i < MsgTypeByteSize; ++i) {
        if (buff[i] !=
                buff[MsgTypeByteSize+SNByteSize+UpdGroupByteSize+RespCodeByteSize+LengthByteSize+len+Adler32ByteSize+i])
            return false;
    }
    return true;
}

unsigned int MessageServer::getMsgType() const
{
    return _MsgType;
}

unsigned int MessageServer::getSN() const
{
    return _SN;
}

unsigned int MessageServer::getUpdGroup() const
{
    return _UpdGroup;
}

unsigned short MessageServer::getRespCode() const
{
    return _RespCode;
}

unsigned short MessageServer::getLength() const
{
    return _Length;
}

unsigned long long MessageServer::getAdler32() const
{
    return _Adler32;
}

const unsigned char *MessageServer::getEncodeBuffer() const
{
    return _EncodeBuffer;
}

unsigned short MessageServer::getEncodeLength() const
{
    return _EncodeLength;
}

const void *MessageServer::getDecodeData() const
{
    return _DecodeData;
}

int MessageServer::getDecodeCount() const
{
    return _DecodeCount;
}
