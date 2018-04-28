#include <iostream>
#include "messageclient.h"
using namespace std;

void print(const string& tag, const unsigned char *buf, int size)
{
    printf("\n%s %d\n", tag.data(), size);
    for (int i = 0; i < size; ++i)
        printf("%u ", buf[i]);
    printf("\n");
}

class TestClient : public MessageClient
{
public:
    TestClient()
    {
        _Buffer = nullptr;
        _BufferSize = 0;
    }

    ~TestClient()
    {
        if (_Buffer != nullptr) {
            delete[] _Buffer;
            _Buffer = nullptr;
        }
    }

    bool protobEncode(const void *data, string &str) override
    {
        str = (char *)data;
        return true;
    }

    bool protobDecode(const unsigned char *buf, size_t bufSize) override
    {
        if (_Buffer == nullptr) {
            _Buffer = new unsigned char[bufSize];
            _BufferSize = bufSize;
            for (int i = 0; i < bufSize; ++i)
                _Buffer[i] = buf[i];
            _DecodeData = _Buffer;
            _DecodeCount = 1;
        }
        else {
            if (bufSize > _BufferSize) {
                delete[] _Buffer;
                _Buffer = new unsigned char[bufSize];
                _BufferSize = bufSize;
                for (int i = 0; i < bufSize; ++i)
                    _Buffer[i] = buf[i];
                _DecodeData = _Buffer;
                _DecodeCount = 1;
            }
            else {
                for (int i = 0; i < bufSize; ++i)
                    _Buffer[i] = buf[i];
                _DecodeData = _Buffer;
                _DecodeCount = 1;
            }
        }
        return true;
    }
private:
    unsigned char *_Buffer;
    size_t _BufferSize;
};

int main()
{
    string sessid{"57D99D89caab482aa0e9a0a803eed3ba"};
    TestClient cli;
    cli.encode(sessid, 0xf0019004, "how are you?");
    cout << "encode: -----------------------" << endl;
    cout << "sessionid: " << cli.getSessionId() << endl;
    cout << "msgtype: " << hex << cli.getMsgType() << dec << endl;
    cout << "sn: " << cli.getSN() << endl;
    cout << "length: " << cli.getLength() << endl;
    cout << "adler32: " << cli.getAdler32() << endl;
    print("encodebuffer: ", cli.getEncodeBuffer(), cli.getEncodeLength());

    cli.decode(cli.getEncodeBuffer());
    cout << "decode: -----------------------" << endl;
    cout << "sessionid: " << cli.getSessionId() << endl;
    cout << "msgtype: " << hex << cli.getMsgType() << dec << endl;
    cout << "sn: " << cli.getSN() << endl;
    cout << "length: " << cli.getLength() << endl;
    cout << "adler32: " << cli.getAdler32() << endl;
    cout << "decodecount: " << cli.getDecodeCount() << endl;
    cout << "decodedata: " << (char*)cli.getDecodeData() << endl;

    cout << "=========================================================" << endl;
    cli.encode(sessid, 0xf0e18045, "how old are you?");
    cout << "encode: -----------------------" << endl;
    cout << "sessionid: " << cli.getSessionId() << endl;
    cout << "msgtype: " << hex << cli.getMsgType() << dec << endl;
    cout << "sn: " << cli.getSN() << endl;
    cout << "length: " << cli.getLength() << endl;
    cout << "adler32: " << cli.getAdler32() << endl;
    print("encodebuffer: ", cli.getEncodeBuffer(), cli.getEncodeLength());

    cli.decode(cli.getEncodeBuffer());
    cout << "decode: -----------------------" << endl;
    cout << "sessionid: " << cli.getSessionId() << endl;
    cout << "msgtype: " << hex << cli.getMsgType() << dec << endl;
    cout << "sn: " << cli.getSN() << endl;
    cout << "length: " << cli.getLength() << endl;
    cout << "adler32: " << cli.getAdler32() << endl;
    cout << "decodecount: " << cli.getDecodeCount() << endl;
    cout << "decodedata: " << (char*)cli.getDecodeData() << endl;
    return 0;
}
