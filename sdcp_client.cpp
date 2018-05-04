#include "sdcp_client.h"

SDCP_Client::SDCP_Client()
{

}

SDCP_Client::~SDCP_Client()
{
    if (_EncodeBuffer != nullptr) {
        delete[] _EncodeBuffer;
        _EncodeBuffer = nullptr;
    }
}

bool SDCP_Client::encode(const string &session_id, unsigned int msg_type, unsigned int sn, const void *buff)
{
    string pbBuf{};
    protobEncodeMap.at(msg_type);
    protobEncode(buff, pbBuf);

    _EncodeLength = pbBuf.length() + CHeadByteSize;
    if (_EncodeBuffer == nullptr) {
        _EncodeBuffer = new char[_EncodeLength];
        _EncodeBufferSize = _EncodeLength;
    }
    else {
        if (_EncodeLength > _EncodeBufferSize) {
            delete[] _EncodeBuffer;
            _EncodeBuffer = new char[_EncodeLength];
            _EncodeBufferSize = _EncodeLength;
        }
        else {
            for (size_t i = 0; i < _EncodeBufferSize; ++i)
                _EncodeBuffer[i] = 0;
        }
    }

    //sessionid
    _SessionId = session_id;
    char sessionIdBytes[SessionIDByteSize];
    sessionId2Bytes(_SessionId, sessionIdBytes);
    for (int i = 0; i < SessionIDByteSize; ++i)
        _EncodeBuffer[i] = sessionIdBytes[i];

    //msgtype
    _MsgType = msg_type;
    char msgTypeBytes[MsgTypeByteSize];
    int2bytes(_MsgType, msgTypeBytes);
    for (int i = 0; i < MsgTypeByteSize; ++i)
        _EncodeBuffer[SessionIDByteSize+i] = msgTypeBytes[i];

    //sn
    _SN = sn;
    char snBytes[SNByteSize];
    int2bytes(_SN, snBytes);
    for (int i = 0; i < SNByteSize; ++i)
        _EncodeBuffer[SessionIDByteSize+MsgTypeByteSize+i] = snBytes[i];

    //length
    _Length = pbBuf.length();
    char lengthBytes[LengthByteSize];
    short2bytes(pbBuf.length(), lengthBytes);
    for (int i = 0; i < LengthByteSize; ++i)
        _EncodeBuffer[SessionIDByteSize+MsgTypeByteSize+SNByteSize+i] = lengthBytes[i];

    //adler32
    char adler32Bytes[Adler32ByteSize];
    _Adler32 = makeAdler32(pbBuf.data(), _Length);
    long2bytes(_Adler32, adler32Bytes);
    for (int i = 0; i < Adler32ByteSize; ++i)
        _EncodeBuffer[SessionIDByteSize+MsgTypeByteSize+SNByteSize+LengthByteSize+i] = adler32Bytes[i];

    //protobuf
    if (_Length > 0) {
        for (int i = 0; i < _Length; ++i)
            _EncodeBuffer[SessionIDByteSize+MsgTypeByteSize+SNByteSize+LengthByteSize+Adler32ByteSize+i] = pbBuf[i];
    }

    return true;
}

bool SDCP_Client::decode(const char *buff, size_t size)
{
    if (size < SHeadByteSize)
        return false;

    //length
    _Length = bytes2short(buff, MsgTypeByteSize+SNByteSize+UpdGroupByteSize+RespCodeByteSize);
    if (size != (SHeadByteSize+_Length))
        return false;

    //msgtype
    _MsgType = bytes2int(buff, 0);

    //sn
    _SN = bytes2int(buff, MsgTypeByteSize);

    //updgroup
    _UpdGroup = bytes2int(buff, MsgTypeByteSize+SNByteSize);

    //respcode
    _RespCode = bytes2short(buff, MsgTypeByteSize+SNByteSize+UpdGroupByteSize);

    //adler32
    _Adler32 = bytes2long(buff, MsgTypeByteSize+SNByteSize+UpdGroupByteSize+RespCodeByteSize+LengthByteSize);

    if (_Length > 0) {
        char pbBuf[_Length];
        for (int i = 0; i < _Length; ++i)
            pbBuf[i] = buff[MsgTypeByteSize+SNByteSize+UpdGroupByteSize+RespCodeByteSize+LengthByteSize+Adler32ByteSize+i];

        //checksum
        if (_Adler32 == makeAdler32(pbBuf, _Length)) {
            return protobDecode(pbBuf, _Length);
        }
        else {
            return false;
        }
    }

    return true;
}
