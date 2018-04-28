#include "messagebase.h"

MessageBase::MessageBase()
{

}

MessageBase::~MessageBase()
{

}

bool MessageBase::sessionId2Bytes(const string &id , unsigned char *bytes)
{
    if (id.length() != 32) {
        for (int i = 0; i < SessionIDByteSize; ++i)
            bytes[i] = 0;
        return false;
    }

    for (int i = 0, j = 0; j < SessionIDByteSize; i+=2, ++j) {
        unsigned char ch1 = 0;
        if (id[i] >= '0' && id[i] <= '9')
            ch1 = id[i] - '0';
        else if (id[i] >= 'a' && id[i] <= 'z')
            ch1 = id[i] - 'a' + 10;
        else if (id[i] >= 'A' && id[i] <= 'Z')
            ch1 = id[i] - 'A' + 10;

        unsigned char ch2 = 0;
        if (id[i+1] >= '0' && id[i+1] <= '9')
            ch2 = id[i+1] - '0';
        else if (id[i+1] >= 'a' && id[i+1] <= 'z')
            ch2 = id[i+1] - 'a' + 10;
        else if (id[i+1] >= 'A' && id[i+1] <= 'Z')
            ch2 = id[i+1] - 'A' + 10;

        bytes[j] = (ch1 << 4) + ch2;
    }
    return true;
}

bool MessageBase::sessionId2String(const unsigned char *bytes, string &id)
{
    for (int i = 0; i < SessionIDByteSize; ++i) {
        char ch1 = (bytes[i] >> 4) & 0xf;
        char ch2 = bytes[i] & 0xf;

        if (ch1 >=0 && ch1 <=9)
            ch1 += '0';
        else if (ch1 >=10 && ch1 <=15)
            ch1 += 'A' - 10;
        id.append(string{ch1});

        if (ch2 >=0 && ch2 <=9)
            ch2 += '0';
        else if (ch2 >=10 && ch2 <=15)
            ch2 += 'A' - 10;
        id.append(string{ch2});
    }
    return true;
}

unsigned long long MessageBase::makeAdler32(const unsigned char *buf, int length)
{
    const unsigned int ADLER32_BASE = 65521;

    unsigned long long adler = 1;
    unsigned long long s1 = adler & 0xffff;
    unsigned long long s2 = (adler >> 16) & 0xffff;

    for (int i = 0; i < length; i++) {
        s1 = (s1 + buf[i]) % ADLER32_BASE;
        s2 = (s2 + s1) % ADLER32_BASE;
    }
    return (s2 << 16) + s1;
}

void MessageBase::short2bytes(unsigned short num, unsigned char *bytes)
{
    for (int i = 0; i < ShortByteSize; ++i) {
        bytes[i] = (num >> (BitsPerByte * (ShortByteSize - i - 1))) & 0xff;
    }
}

void MessageBase::int2bytes(unsigned int num, unsigned char *bytes)
{
    for (int i = 0; i < IntByteSize; ++i) {
        bytes[i] = (num >> (BitsPerByte * (IntByteSize - i - 1))) & 0xff;
    }
}

void MessageBase::long2bytes(unsigned long long num, unsigned char *bytes)
{
    for (int i = 0; i < LongByteSize; i++) {
        bytes[i] = (num >> (BitsPerByte * (LongByteSize - i - 1))) & 0xff;
    }
}

unsigned short MessageBase::bytes2short(const unsigned char *bytes, int startIndex)
{
    unsigned short sourceNumber = 0;
    for (int i = startIndex; i < startIndex + ShortByteSize; i++) {
        sourceNumber <<= BitsPerByte;
        sourceNumber |= (bytes[i] & 0xff);
    }
    return sourceNumber;
}

unsigned int MessageBase::bytes2int(const unsigned char *bytes, int startIndex)
{
    unsigned int sourceNumber = 0;
    for (int i = startIndex; i < startIndex + IntByteSize; i++) {
        sourceNumber <<= BitsPerByte;
        sourceNumber |= (bytes[i] & 0xff);
    }
    return sourceNumber;
}

unsigned long long MessageBase::bytes2long(const unsigned char *bytes, int startIndex)
{
    unsigned long long sourceNumber = 0;
    for (int i = startIndex; i < startIndex + LongByteSize; i++) {
        sourceNumber <<= BitsPerByte;
        sourceNumber |= bytes[i] & 0xff;
    }
    return sourceNumber;
}
