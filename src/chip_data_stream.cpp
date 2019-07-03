#include "chip_data_stream.hpp"

//Constructor
ChipDataStream::ChipDataStream(uint8_t* i_buffer, size_t i_bufferSize)
{
    iv_buffer = i_buffer;
    iv_bufferSize = i_bufferSize;
}

//Destructor
ChipDataStream::~ChipDataStream(){};

//Setters

void ChipDataStream::setOffset(size_t i_offset)
{
    HEI_ASSERT(i_offset < iv_bufferSize);
    iv_asyncOffset =  i_offset;
}

template <>
    ChipDataStream & ChipDataStream::operator>>( bool & o_right )
    {
        read( &o_right, sizeof(o_right) );
        return *this;
    }

template <>
    ChipDataStream & ChipDataStream::operator>>( char & o_right )
    {
        read( &o_right, sizeof(o_right) );
        return *this;
    }

template <>
    ChipDataStream & ChipDataStream::operator>>( uint8_t & o_right )
    {
        read( &o_right, sizeof(o_right) );
        return *this;
    }

template <>
    ChipDataStream & ChipDataStream::operator>>( int8_t & o_right )
    {
        read( &o_right, sizeof(o_right) );
        return *this;
    }

template <>
    ChipDataStream & ChipDataStream::operator>>( uint16_t & o_right )
    {
        read( &o_right, 2);
        be16toh( o_right );
        return *this;
    }

template <>
    ChipDataStream & ChipDataStream::operator>>( int16_t & o_right )
    {
        read( &o_right, sizeof(o_right) );
        be16toh( o_right );
        return *this;
    }

template <>
    ChipDataStream & ChipDataStream::operator>>( uint32_t & o_right )
    {
        read( &o_right, sizeof(o_right) );
        be32toh( o_right );
        return *this;
    }

template <>
    ChipDataStream & ChipDataStream::operator>>( int32_t & o_right )
    {
        read( &o_right, sizeof(o_right) );
        be32toh( o_right );
        return *this;
    }

template <>
    ChipDataStream & ChipDataStream::operator>>( uint64_t & o_right )
    {
        read( &o_right, sizeof(o_right) );
        be64toh( o_right );
        return *this;
    }

template <>
    ChipDataStream & ChipDataStream::operator>>( int64_t & o_right )
    {
        read( &o_right, sizeof(o_right) );
        be64toh( o_right );
        return *this;
    }
