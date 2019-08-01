#include "hei_chip_data_stream.hpp"

using namespace libhei;

//Constructor
ChipDataStream::ChipDataStream(uint8_t* i_buffer, size_t i_bufferSize)
{
    iv_buffer = i_buffer;
    iv_bufferSize = i_bufferSize;
    iv_asyncOffset =  0;
}

//Destructor
ChipDataStream::~ChipDataStream(){};

//Setters

/**
@brief Sets the offset into iv_buffer where read will begin to extract the next piece of data.
*/
void ChipDataStream::setOffset(size_t i_offset)
{
    HEI_ASSERT(i_offset < iv_bufferSize);
    iv_asyncOffset =  i_offset;
}

//Getters

/*
* The templates below extract the common data types, bool, char, uint8,
* int8, uint16, etc... from the data buffer.
*/

///@brief Reads a boolean type
template <>
    ChipDataStream & ChipDataStream::operator>>( bool & o_right )
    {
        read( &o_right, sizeof(o_right) );
        return *this;
    }

///@brief Reads a char type
template <>
    ChipDataStream & ChipDataStream::operator>>( char & o_right )
    {
        read( &o_right, sizeof(o_right) );
        return *this;
    }

///@brief Reads a uint8_t type
template <>
    ChipDataStream & ChipDataStream::operator>>( uint8_t & o_right )
    {
        read( &o_right, sizeof(o_right) );
        return *this;
    }

///@brief Reads an int8_t type
template <>
    ChipDataStream & ChipDataStream::operator>>( int8_t & o_right )
    {
        read( &o_right, sizeof(o_right) );
        return *this;
    }

///@brief Reads a uint16_t type
template <>
    ChipDataStream & ChipDataStream::operator>>( uint16_t & o_right )
    {
        read( &o_right, 2);
        be16toh( o_right );
        return *this;
    }

///@brief Reads an int16_t type
template <>
    ChipDataStream & ChipDataStream::operator>>( int16_t & o_right )
    {
        read( &o_right, sizeof(o_right) );
        be16toh( o_right );
        return *this;
    }

///@brief Reads a uint32_t type
template <>
    ChipDataStream & ChipDataStream::operator>>( uint32_t & o_right )
    {
        read( &o_right, sizeof(o_right) );
        be32toh( o_right );
        return *this;
    }

///@brief Reads a int32_t type
template <>
    ChipDataStream & ChipDataStream::operator>>( int32_t & o_right )
    {
        read( &o_right, sizeof(o_right) );
        be32toh( o_right );
        return *this;
    }

///@brief Reads a uint64_t type
template <>
    ChipDataStream & ChipDataStream::operator>>( uint64_t & o_right )
    {
        read( &o_right, sizeof(o_right) );
        be64toh( o_right );
        return *this;
    }

///@brief Reads an int64_t type
template <>
    ChipDataStream & ChipDataStream::operator>>( int64_t & o_right )
    {
        read( &o_right, sizeof(o_right) );
        be64toh( o_right );
        return *this;
    }
