#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

using namespace std;

class ChipDataStream
{

    private:

        uint32_t asyncOffset;
        void* inputBuffer;
        uint32_t inputBufferSize;

        //Home-made endian fix - naming convention 
        //matches similar C library functions.
        uint64_t ntohll(uint64_t host_longlong);

    public:

        //Functions to Get any size data given on
        //offset into the passed buffer
        bool GetBool(uint32_t offset);
        char GetChar(uint32_t offset);
        int8_t GetInt8(uint32_t offset);
        uint8_t GetUInt8(uint32_t offset);
        int16_t GetInt16(uint32_t offset);
        uint16_t GetUInt16(uint32_t offset);
        int32_t GetInt32(uint32_t offset);
        uint32_t GetUInt32(uint32_t offset);
        int64_t GetInt64(uint32_t offset);
        uint64_t GetUInt64(uint32_t offset);

        //Functions to get any size data.  The
        //offset is automatically taken care of
        //after it's been set once with SetOffset().
        bool GetNextBool();
        char GetNextChar();
        int8_t GetNextInt8();
        uint8_t GetNextUInt8();
        int16_t GetNextInt16();
        uint16_t GetNextUInt16();
        int32_t GetNextInt32();
        uint32_t GetNextUInt32();
        int64_t GetNextInt64();
        uint64_t GetNextUInt64();
    
        //Use with GetNext... functions above.
        //SetOffset needs to be called once
        //before calling GetNext... functions.
        int8_t SetOffset(uint32_t offset);

        //These overload the ">>" operators so it's
        //possible to gather data using a command
        //like:  cds >> x >> y >> z; where "cds" is
        //the instantiation of a ChipDataStream object.
        ChipDataStream& operator>> (bool& in);
        ChipDataStream& operator>> (char& in);

        ChipDataStream& operator>> (int8_t& in);
        ChipDataStream& operator>> (uint8_t& in);
    
        ChipDataStream& operator>> (int16_t& in);
        ChipDataStream& operator>> (uint16_t& in);

        ChipDataStream& operator>> (int32_t& in);
        ChipDataStream& operator>> (uint32_t& in);

        ChipDataStream& operator>> (int64_t& in);
        ChipDataStream& operator>> (uint64_t& in);

        //Constructors
        ChipDataStream();
        //When instantiating the ChipDataStream object
        //a pointer to a data buffer containing all
        //the data and its size is passed into the class.
        ChipDataStream(void* localInputBuffer, size_t localInputBufferSize); 

        //Deconstructor
        ~ChipDataStream();

};
