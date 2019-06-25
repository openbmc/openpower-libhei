#include "chip_data_stream.hpp"

//Constructors

ChipDataStream::ChipDataStream(void* localInputBuffer, size_t localInputBufferSize)
    {
        inputBuffer = localInputBuffer;
        inputBufferSize = localInputBufferSize;
    }

ChipDataStream::ChipDataStream(){};

//Deconstructor

ChipDataStream::~ChipDataStream(){};

//Setters

int8_t ChipDataStream::SetOffset(uint32_t offset)
{
    if (offset < inputBufferSize)
    {
        asyncOffset =  offset;
        //cout << "Offset set to: " << asyncOffset << endl;
        return 0;
    }
    else
    {
        return -1;
    }
}

//Getters
//Get data at a given offset:

bool ChipDataStream::GetBool(uint32_t offset)
{
    bool tempData = 0;
    if ((offset + sizeof(tempData)) <= inputBufferSize)
    { 
        memcpy(&tempData,(int8_t*)inputBuffer+offset,sizeof(tempData));
        asyncOffset = offset;
        return tempData;
    }
    return 0;
}

bool ChipDataStream::GetNextBool()
{
    bool tempData = 0;
    if ((asyncOffset + sizeof(tempData)) <= inputBufferSize)
    {
        memcpy(&tempData,(int8_t*)inputBuffer+asyncOffset,sizeof(tempData));
        asyncOffset = asyncOffset + sizeof(tempData);
        return tempData;
    }
    return 0;
}

char ChipDataStream::GetChar(uint32_t offset)
{
    char tempData = 0;
    if ((offset + sizeof(tempData)) <= inputBufferSize)
    {
        memcpy(&tempData,(int8_t*)inputBuffer+offset,sizeof(tempData));
        asyncOffset = offset;
        return tempData;
    }
    return 0;
}

char ChipDataStream::GetNextChar()
{
    char tempData = 0;
    if ((asyncOffset + sizeof(tempData)) <= inputBufferSize)
    {
        memcpy(&tempData,(int8_t*)inputBuffer+asyncOffset,sizeof(tempData));
        asyncOffset = asyncOffset + sizeof(tempData);
        return tempData;
    }
    return 0;
}

int8_t ChipDataStream::GetInt8(uint32_t offset)
{
    int8_t tempData = 0;
    if ((offset + sizeof(tempData)) <= inputBufferSize)
    {
        memcpy(&tempData,(int8_t*)inputBuffer+offset,sizeof(tempData));
        asyncOffset = offset;
        return tempData;
    }
    return 0;
}

uint8_t ChipDataStream::GetUInt8(uint32_t offset)
{
    uint8_t tempData = 0;
    if ((offset + sizeof(tempData)) <= inputBufferSize)
    {
        memcpy(&tempData,(int8_t*)inputBuffer+offset,sizeof(tempData));
        asyncOffset = offset;
        return tempData;
    }
    return 0;
}
int8_t ChipDataStream::GetNextInt8()
{
    int8_t tempData = 0;
    if ((asyncOffset + sizeof(tempData)) <= inputBufferSize)
    {
        memcpy(&tempData,(int8_t*)inputBuffer+asyncOffset,sizeof(tempData));
        asyncOffset = asyncOffset + sizeof(tempData);
        return tempData;
    }
    return 0;
}

uint8_t ChipDataStream::GetNextUInt8()
{
    uint8_t tempData = 0;
    if ((asyncOffset + sizeof(tempData)) <= inputBufferSize)
    {
        memcpy(&tempData,(int8_t*)inputBuffer+asyncOffset,sizeof(tempData));
        asyncOffset = asyncOffset + sizeof(tempData);
        return tempData;
    }
    return 0;
}

int16_t ChipDataStream::GetInt16(uint32_t offset)
{
    int16_t tempData = 0;
    if ((offset + sizeof(tempData)) <= inputBufferSize)
    {
        memcpy(&tempData,(int8_t*)inputBuffer+offset,sizeof(tempData));
        tempData = ntohs(tempData);
        asyncOffset = offset;
        return tempData;
    }
    return 0;
}

uint16_t ChipDataStream::GetUInt16(uint32_t offset)
{
    uint16_t tempData = 0;
    if ((offset + sizeof(tempData)) <= inputBufferSize)
    {
        memcpy(&tempData,(int8_t*)inputBuffer+offset,sizeof(tempData));
        tempData = ntohs(tempData);
        asyncOffset = offset;
        return tempData;
    }
    return 0;
}

int16_t ChipDataStream::GetNextInt16()
{
    int16_t tempData = 0;
    if ((asyncOffset + sizeof(tempData)) <= inputBufferSize)
    {
        memcpy(&tempData,(int8_t*)inputBuffer+asyncOffset,sizeof(tempData));
        tempData = ntohs(tempData);
        asyncOffset = asyncOffset + sizeof(tempData);
        return tempData;
    }
    return 0;
}

uint16_t ChipDataStream::GetNextUInt16()
{
    uint16_t tempData = 0;
    if ((asyncOffset + sizeof(tempData)) <= inputBufferSize)
    {
        memcpy(&tempData,(int8_t*)inputBuffer+asyncOffset,sizeof(tempData));
        tempData = ntohs(tempData);
        asyncOffset = asyncOffset + sizeof(tempData);
        return tempData;
    }
    return 0;
}

int32_t ChipDataStream::GetInt32(uint32_t offset)
{
    int32_t tempData = 0;
    if ((offset + sizeof(tempData)) <= inputBufferSize)
    {
        memcpy(&tempData,(int8_t*)inputBuffer+offset,sizeof(tempData));
        tempData = ntohl(tempData);
        asyncOffset = offset;
        return tempData;
    }
    return 0;
}

uint32_t ChipDataStream::GetUInt32(uint32_t offset)
{
    uint32_t tempData = 0;
    if ((offset + sizeof(tempData)) <= inputBufferSize)
    {
        memcpy(&tempData,(int8_t*)inputBuffer+offset,sizeof(tempData));
        tempData = ntohl(tempData);
        asyncOffset = offset;
        return tempData;
    }
    return 0;
}

int32_t ChipDataStream::GetNextInt32()
{
    int32_t tempData = 0;
    if ((asyncOffset + sizeof(tempData)) <= inputBufferSize)
    {
        memcpy(&tempData,(int8_t*)inputBuffer+asyncOffset,sizeof(tempData));
        tempData = ntohl(tempData);
        asyncOffset = asyncOffset + sizeof(tempData);
        return tempData;
    }
    return 0;
}

uint32_t ChipDataStream::GetNextUInt32()
{
    uint32_t tempData = 0;
    if ((asyncOffset + sizeof(tempData)) <= inputBufferSize)
    {
        memcpy(&tempData,(int8_t*)inputBuffer+asyncOffset,sizeof(tempData));
        tempData = ntohl(tempData);
        asyncOffset = asyncOffset + sizeof(tempData);
        return tempData;
    }
    return 0;
}

//Get data at the next location.  The offset is
//taken care of but you must set it before using
//one of the following commands or start from the
//default "0" location.
int64_t ChipDataStream::GetInt64(uint32_t offset)
{
    int64_t tempData = 0;
    if ((offset + sizeof(tempData)) <= inputBufferSize)
    {
        memcpy(&tempData,(int8_t*)inputBuffer+offset,sizeof(tempData));
        tempData = ntohll(tempData);
        asyncOffset = offset;
        return tempData;
    }
    return 0;
}

uint64_t ChipDataStream::GetUInt64(uint32_t offset)
{
    uint64_t tempData = 0;
    if ((offset + sizeof(tempData)) <= inputBufferSize)
    {
        memcpy(&tempData,(int8_t*)inputBuffer+offset,sizeof(tempData));
        tempData = ntohll(tempData);
        asyncOffset = offset;
        return tempData;
    }
    return 0;
}

int64_t ChipDataStream::GetNextInt64()
{
    int64_t tempData = 0;
    if ((asyncOffset + sizeof(tempData)) <= inputBufferSize)
    {
        memcpy(&tempData,(int8_t*)inputBuffer+asyncOffset,sizeof(tempData));
        tempData = ntohll(tempData);
        asyncOffset = asyncOffset + sizeof(tempData);
        return tempData;
    }
    return 0;
}

uint64_t ChipDataStream::GetNextUInt64()
{
    uint64_t tempData = 0;
    if ((asyncOffset + sizeof(tempData)) <= inputBufferSize)
    {
        memcpy(&tempData,(int8_t*)inputBuffer+asyncOffset,sizeof(tempData));
        tempData = ntohll(tempData);
        asyncOffset = asyncOffset + sizeof(tempData);
        return tempData;
    }
    return 0;
}

//Overload the stream operator ">>" for every type so
//it's possible to write a command like:  cds >> x >> y >> z;
//Where "cds" is the instantiation of the ChipDataStream object.
ChipDataStream& ChipDataStream::operator>> (bool& in)
{
    in = GetNextBool();
    return *this;
}

ChipDataStream& ChipDataStream::operator>> (char& in)
{
    in = GetNextChar();
    return *this;
}

ChipDataStream& ChipDataStream::operator>> (int8_t& in)
{
    in = GetNextInt8();
    return *this;
}

ChipDataStream& ChipDataStream::operator>> (uint8_t& in)
{
    in = GetNextUInt8();
    return *this;
}

ChipDataStream& ChipDataStream::operator>> (int16_t& in)
{
    in = GetNextInt16();
    return *this;
}

ChipDataStream& ChipDataStream::operator>> (uint16_t& in)
{
    in = GetNextUInt16();
    return *this;
}

ChipDataStream& ChipDataStream::operator>> (int32_t& in)
{
    in = GetNextInt32();
    return *this;
}

ChipDataStream& ChipDataStream::operator>> (uint32_t& in)
{
    in = GetNextUInt32();
    return *this;
}

ChipDataStream& ChipDataStream::operator>> (int64_t& in)
{
    in = GetNextInt64();
    return *this;
}

ChipDataStream& ChipDataStream::operator>> (uint64_t& in)
{
    in = GetNextUInt64();
    return *this;
}


//Utility functions

//ntohll takes care of 64 bit endian.  This had to be used because a 64 bit
//endian fix isn't available in netinet/in.h library.
uint64_t ChipDataStream::ntohll(uint64_t hostLongLong)
{
    int x = 1;

    // little endian
    if (*(char*)&x == 1)
    {
        return ((((uint64_t)ntohl(hostLongLong)) << 32) +
            ntohl(hostLongLong >> 32));
    }
    
    // big endian
    else
    {
        return hostLongLong;
    }
}
