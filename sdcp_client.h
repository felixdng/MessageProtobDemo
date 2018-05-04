#ifndef SDCP_CLIENT_H
#define SDCP_CLIENT_H

#include "sdcp_base.h"
#include <string>

using namespace std;

class SDCP_Client : public SDCP_Base
{
public:
    SDCP_Client();
    ~SDCP_Client();

    bool encode(const string &session_id, unsigned int msg_type, unsigned int sn, const void *buff);
    bool decode(const char *buff, size_t size) override;

private:
    string _SessionId{"0000000000000000"};
    unsigned int _MsgType = 0x0;
    unsigned int _SN = 0;
    unsigned int _UpdGroup = 0x0;
    unsigned short _RespCode = 0;
    unsigned short _Length = 0;
    unsigned long long _Adler32 = 0;
    char *_EncodeBuffer = nullptr;
    size_t _EncodeLength = 0;
    void *_DecodeData = nullptr;

    size_t _EncodeBufferSize = 0;
};

#endif // SDCP_CLIENT_H
