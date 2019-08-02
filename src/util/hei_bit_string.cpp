/** @file hei_bit_string.cpp
 *  @brief BitString and BitStringBuffer class definitions
 */

#include "hei_bit_string.hpp"

#include <hei_user_defines.hpp>

#include <algorithm>

#include <iostream>
#include <iomanip>
namespace libhei
{

//##############################################################################
//                             BitString class
//##############################################################################

// number of bits in a uint64_t
constexpr uint32_t BitString::UINT64_BIT_LEN = sizeof(uint64_t) * 8;
// number of bits in a uint8_t
constexpr uint32_t BitString::UINT8_BIT_LEN = sizeof(uint8_t) * 8;

//------------------------------------------------------------------------------

uint64_t BitString::getFieldRight( uint32_t i_pos, uint32_t i_len ) const
{
    HEI_ASSERT( nullptr != getBufAddr() );      // must to have a valid address
    HEI_ASSERT( 0 < i_len );                    // must have at least one bit
    HEI_ASSERT( i_len <= UINT64_BIT_LEN );       // i_len length must be valid
    HEI_ASSERT( i_pos + i_len <= getBitLen() ); // field must be within range

    // The returned value.
    uint64_t o_val = 0;

    // The desired bitstring may be larger than one byte so retrieve one
    // byte at a time. Each byte that is to be retrieved may cross a byte
    // boundary. In this case retrieve each byte as a low chunk and a high
    // chunk masking and shifting each chunk and combining them to form one.

    uint32_t bytes_remain = getMinBytes(i_len);
    uint32_t len0;
    uint32_t len1;

    while (0 != bytes_remain)
    {

    o_val <<= UINT8_BIT_LEN; // next byte

    // Get the relative address and position of the field.
    uint32_t relPos = 0;
    uint8_t * relAddr = (uint8_t *)getRelativePosition( relPos, i_pos );

    // Assume the low  chunk contains the entire value
    len0 = i_len;
    len1 = 0;

    // If value crosses byte boundary calculate low and high chunk lengths
    if ( UINT8_BIT_LEN < relPos + i_len )
    {
        len0 = UINT8_BIT_LEN - relPos; // low chunk size
        len1 = i_len - len0; // high chunk size

        // Limit value to fit in one byte - handle remaining bits in next pass
        if ( i_len >= UINT8_BIT_LEN )
        {
            len1 = UINT8_BIT_LEN - len0;
        }
    }

    // Masks are length of chunk
    uint8_t mask0 = UINT8_MAX << ( UINT8_BIT_LEN - len0 );
    uint8_t mask1 = UINT8_MAX << ( UINT8_BIT_LEN - len1 );

    uint32_t shift = UINT8_BIT_LEN - relPos;

    o_val |= ((*relAddr << relPos) & mask0);

    // Get the high chunk if needed
    if (0 != len1)
    {
        ++relAddr; // high chunk is in the next array element

        // Get the high chunk, mask the bits of interest and
        // align it to bit position 0 plus the length
        o_val |= ( (*relAddr & mask1) >> shift );
    }

    // Next field
    i_pos += (len0 + len1);
    i_len -= (len0 + len1);

    bytes_remain--;

    } // end while (0 != bytes_remain)

    // right-justify the return value
    o_val >>= (UINT8_BIT_LEN - (len0 +len1));

    return o_val;
}

//------------------------------------------------------------------------------

void BitString::setFieldRight( uint32_t i_pos, uint32_t i_len, uint64_t i_val )
{
    HEI_ASSERT( nullptr != getBufAddr() );      // must to have a valid address
    HEI_ASSERT( 0 < i_len );                    // must have at least one bit
    HEI_ASSERT( i_len <= UINT64_BIT_LEN );       // i_len length must be valid
    HEI_ASSERT( i_pos + i_len <= getBitLen() ); // field must be within range

    uint64_t set_val = 0; // the value to set

    // Swap byte order
    i_val <<= (UINT64_BIT_LEN - i_len);

    for(uint32_t i = 0; i < sizeof(i_val); i++ )
    {
        set_val <<= UINT8_BIT_LEN;
        set_val |= (uint8_t)(i_val >> (UINT8_BIT_LEN * i));
    }

    // The bitstring may be larger than one byte so set one byte at a time.
    // Each byte that is to be set may cross a byte boundary. In this case set
    // each byte as a low chunk and a high chunk masking and shifting each
    // chunk before writing them.

    // Assume the low  chunk contains the entire value
    uint32_t bytes_remain = getMinBytes(i_len);
    uint32_t len0;
    uint32_t len1;

    while (0 != bytes_remain)
    {

    // Get the relative address and position of the field.
    uint32_t relPos = 0;
    uint8_t * relAddr = (uint8_t *)getRelativePosition( relPos, i_pos);

    // Assume the low  chunk contains the entire value
    len0 = i_len;
    len1 = 0;

    // If value crosses byte boundary calculate low and high chunk lengths
    if ( UINT8_BIT_LEN < relPos + i_len )
    {
        len0 = UINT8_BIT_LEN - relPos; // low chunk size
        len1 = i_len - len0; // high chunk size

        // Limit value to fit in one byte - handle remaining bits in next pass
        if ( i_len >= UINT8_BIT_LEN )
        {
            len1 = UINT8_BIT_LEN - len0;
        }
    }

    // Mask is length of chunk
    uint8_t mask = UINT8_MAX << ( UINT8_BIT_LEN - len0 );
    uint8_t val = (uint8_t)set_val & mask;

    *relAddr &= ~(mask >> relPos);
    *relAddr |= (val >> relPos);

    // Get the hight chunk if needed
    if (0 != len1)
    {
        ++relAddr; //high chunk is in the next array element

        *relAddr &= ~(mask << (UINT8_BIT_LEN - relPos)); // clear field
        *relAddr |= (set_val << (UINT8_BIT_LEN - relPos)); // set field

        // Set destination bits.
        *relAddr |= (uint8_t)(((uint8_t)set_val & (mask << len0)) >> len0);
    }

    // Set next byte in next field
    set_val >>= UINT8_BIT_LEN;
    i_pos += (len0 + len1);
    i_len -= (len0 + len1);

    bytes_remain--;

    } // end while(0 != bytes_remain)

    return;
}

//------------------------------------------------------------------------------

void BitString::setPattern( uint32_t i_sPos, uint32_t i_sLen,
                            uint64_t i_pattern, uint32_t i_pLen )
{

    HEI_ASSERT(nullptr != getBufAddr());        // must to have a valid address
    HEI_ASSERT(0 < i_sLen);                     // must have at least one bit
    HEI_ASSERT(i_sPos + i_sLen <= getBitLen()); // field must be within range
    HEI_ASSERT(0 < i_pLen);                     // must have at least one bit
    HEI_ASSERT(i_pLen <= UINT64_BIT_LEN);        // i_pLen length must be valid

    // Get a bit string for the pattern subset (right justified).
    BitStringBuffer bso(UINT64_BIT_LEN);
    bso.setFieldRight(0, i_pLen, i_pattern);

    // Iterate the range in chunks the size of i_pLen.
    uint32_t endPos = i_sPos + i_sLen;
    for ( uint32_t pos = i_sPos; pos < endPos; pos += i_pLen )
    {
        // The true chunk size is either i_pLen or the leftovers at the end.
        uint32_t len = std::min( i_pLen, endPos - pos );

        // Get this chunk's pattern value, truncate (right justified) if needed.
        uint64_t pattern = bso.getFieldRight( 0, len );

        // Set the pattern in this string.
        setFieldRight( pos, len, pattern );
    }
}

//------------------------------------------------------------------------------

void BitString::setString( const BitString & i_sStr, uint32_t i_sPos,
                           uint32_t i_sLen, uint32_t i_dPos )
{
    // Ensure the source parameters are valid.
    HEI_ASSERT( nullptr != i_sStr.getBufAddr() );
    HEI_ASSERT( 0 < i_sLen ); // at least one bit to copy
    HEI_ASSERT( i_sPos + i_sLen <= i_sStr.getBitLen() );

    // Ensure the destination has at least one bit available to copy.
    HEI_ASSERT( nullptr != getBufAddr() );
    HEI_ASSERT( i_dPos < getBitLen() );

    // If the source length is greater than the destination length than the
    // extra source bits are ignored.
    uint32_t actLen = std::min( i_sLen, getBitLen() - i_dPos );

    // The bit strings may be in overlapping memory spaces. So we need to copy
    // the data in the correct direction to prevent overlapping.
    uint32_t sRelOffset = 0, dRelOffset = 0;
    uint8_t * sRelAddr = (uint8_t *) \
                            i_sStr.getRelativePosition( sRelOffset, i_sPos );
    uint8_t * dRelAddr = (uint8_t *) \
                                   getRelativePosition( dRelOffset, i_dPos );

    // Copy the data.
    if ( (dRelAddr == sRelAddr) && (dRelOffset == sRelOffset) )
    {
        // Do nothing. The source and destination are the same.
    }
    else if ( (dRelAddr < sRelAddr) ||
              ((dRelAddr == sRelAddr) && (dRelOffset < sRelOffset)) )
    {
        // Copy the data forward.
        for ( uint32_t pos = 0; pos < actLen; pos += UINT8_BIT_LEN )
        {
            uint32_t len = std::min( actLen - pos, UINT8_BIT_LEN );

            uint64_t value = i_sStr.getFieldRight( i_sPos + pos, len );
            setFieldRight( i_dPos + pos, len, value );
        }
    }
    else // Copy the data backwards.
    {
        // Get the first position of the last chunk (byte aligned).
        uint32_t lastPos = ((actLen-1) / UINT8_BIT_LEN) * UINT8_BIT_LEN;

        // Start with the last chunk and work backwards.
        for ( int32_t pos = lastPos; 0 <= pos; pos -= UINT8_BIT_LEN )
        {
            uint32_t len = std::min( actLen - pos, UINT8_BIT_LEN );
            uint64_t value = i_sStr.getFieldRight( i_sPos + pos, len );
            setFieldRight( i_dPos + pos, len, value );
        }
    }
}

//------------------------------------------------------------------------------

void BitString::maskString( const BitString & i_mask )
{
    // Get the length of the smallest string.
    uint32_t actLen = std::min( getBitLen(), i_mask.getBitLen() );

    for ( uint32_t pos = 0; pos < actLen; pos += UINT8_BIT_LEN )
    {
        uint32_t len = std::min( actLen - pos, UINT8_BIT_LEN );

        uint64_t dVal =        getFieldRight( pos, len );
        uint64_t sVal = i_mask.getFieldRight( pos, len );

        setFieldRight( pos, len, dVal & ~sVal );
    }
}

//------------------------------------------------------------------------------

bool BitString::isEqual( const BitString & i_str ) const
{
    if ( getBitLen() != i_str.getBitLen() )
        return false; // size not equal

    for ( uint32_t pos = 0; pos < getBitLen(); pos += UINT8_BIT_LEN )
    {
        uint32_t len = std::min( getBitLen() - pos, UINT8_BIT_LEN );

        if ( getFieldRight(pos, len) != i_str.getFieldRight(pos, len) )
            return false; // bit strings do not match
    }

    return true; // bit strings match
}

//------------------------------------------------------------------------------

bool BitString::isZero() const
{
    for ( uint32_t pos = 0; pos < getBitLen(); pos += UINT8_BIT_LEN )
    {
        uint32_t len = std::min( getBitLen() - pos, UINT8_BIT_LEN );

        if ( 0 != getFieldRight(pos, len) )
            return false; // something is non-zero
    }

    return true; // everything was zero
}

//------------------------------------------------------------------------------

uint32_t BitString::getSetCount( uint32_t i_pos, uint32_t i_len ) const
{
    uint32_t endPos = i_pos + i_len;

    HEI_ASSERT( endPos <= getBitLen() );

    uint32_t count = 0;

    for ( uint32_t i = i_pos; i < endPos; i++ )
    {
        if ( isBitSet(i) ) count++;
    }

    return count;
}

//------------------------------------------------------------------------------

BitStringBuffer BitString::operator~() const
{
    BitStringBuffer bsb( getBitLen() );

    for ( uint32_t pos = 0; pos < getBitLen(); pos += UINT8_BIT_LEN )
    {
        uint32_t len = std::min( getBitLen() - pos, UINT8_BIT_LEN );

        uint64_t dVal = getFieldRight( pos, len );

        bsb.setFieldRight( pos, len, ~dVal );
    }

    return bsb;
}

//------------------------------------------------------------------------------

BitStringBuffer BitString::operator&( const BitString & i_bs ) const
{
    // Get the length of the smallest string.
    uint32_t actLen = std::min( getBitLen(), i_bs.getBitLen() );

    BitStringBuffer bsb( actLen );

    for ( uint32_t pos = 0; pos < actLen; pos += UINT8_BIT_LEN )
    {
        uint32_t len = std::min( actLen - pos, UINT8_BIT_LEN );

        uint64_t dVal =      getFieldRight( pos, len );
        uint64_t sVal = i_bs.getFieldRight( pos, len );

        bsb.setFieldRight( pos, len, dVal & sVal );
    }

    return bsb;
}

//------------------------------------------------------------------------------

BitStringBuffer BitString::operator|( const BitString & i_bs ) const
{
    // Get the length of the smallest string.
    uint32_t actLen = std::min( getBitLen(), i_bs.getBitLen() );

    BitStringBuffer bsb( actLen );

    for ( uint32_t pos = 0; pos < actLen; pos += UINT8_BIT_LEN )
    {
        uint32_t len = std::min( actLen - pos, UINT8_BIT_LEN );

        uint64_t dVal =      getFieldRight( pos, len );
        uint64_t sVal = i_bs.getFieldRight( pos, len );

        bsb.setFieldRight( pos, len, dVal | sVal );
    }

    return bsb;
}

//------------------------------------------------------------------------------

BitStringBuffer BitString::operator>>( uint32_t i_shift ) const
{
    BitStringBuffer bsb( getBitLen() ); // default all zeros

    if ( i_shift < getBitLen() )
    {
        // bso overlays bsb, containing the shifted offset.
        BitString bso ( bsb.getBitLen() - i_shift, bsb.getBufAddr(), i_shift );

        // Copy this into bso.
        bso.setString( *this );
    }

    return bsb;
}

//------------------------------------------------------------------------------

BitStringBuffer BitString::operator<<( uint32_t i_shift ) const
{
    BitStringBuffer bsb( getBitLen() ); // default all zeros

    if ( i_shift < getBitLen() )
    {
        // bso overlays *this, containing the shifted offset.
        BitString bso ( this->getBitLen() - i_shift, this->getBufAddr(),
                        i_shift );

        // Copy bso into bsb.
        bsb.setString( bso );
    }

    return bsb;
}

//------------------------------------------------------------------------------

void * BitString::getRelativePosition( uint32_t & o_relPos,
                                           uint32_t   i_absPos ) const
{
    HEI_ASSERT( nullptr != getBufAddr() ); // must to have a valid address
    HEI_ASSERT( i_absPos < getBitLen() );  // must be a valid position

    o_relPos = (i_absPos + iv_offset) % UINT8_BIT_LEN;

    return ((uint8_t *)iv_bufAddr + ((i_absPos + iv_offset) / UINT8_BIT_LEN));
}

//##############################################################################
//                          BitStringBuffer class
//##############################################################################

BitStringBuffer::BitStringBuffer( uint32_t i_bitLen ) :
    BitString( i_bitLen, nullptr )
{
    initBuffer();
}

//------------------------------------------------------------------------------

BitStringBuffer::~BitStringBuffer()
{
    delete [] getBufAddr();
}

//------------------------------------------------------------------------------

BitStringBuffer::BitStringBuffer( const BitString & i_bs ) :
    BitString( i_bs.getBitLen(), nullptr )
{
    initBuffer();
    if ( !i_bs.isZero() ) setString( i_bs );
}

//------------------------------------------------------------------------------

BitStringBuffer::BitStringBuffer( const BitStringBuffer & i_bsb ) :
    BitString( i_bsb.getBitLen(), nullptr )
{
    initBuffer();
    if ( !i_bsb.isZero() ) setString( i_bsb );
}

//------------------------------------------------------------------------------

BitStringBuffer & BitStringBuffer::operator=( const BitString & i_bs )
{
    // The initBuffer() function will deallocate the buffer as well, however we
    // also need to deallocate the buffer here before we set the length.
    delete [] getBufAddr();
    setBufAddr( nullptr );

    setBitLen( i_bs.getBitLen() );
    initBuffer();
    if ( !i_bs.isZero() ) setString( i_bs );

    return *this;
}

//------------------------------------------------------------------------------

BitStringBuffer & BitStringBuffer::operator=( const BitStringBuffer & i_bsb )
{
    if ( this != &i_bsb ) // Check for assignment to self
    {
        // The initBuffer() function will deallocate the buffer as well, however
        // we also need to deallocate the buffer here before we set the length.
        delete [] getBufAddr();
        setBufAddr( nullptr );

        setBitLen( i_bsb.getBitLen() );
        initBuffer();
        if ( !i_bsb.isZero() ) setString( i_bsb );
    }

    return *this;
}

//------------------------------------------------------------------------------

void BitStringBuffer::initBuffer()
{
    // Deallocate the current buffer.
    delete [] getBufAddr();

    // Allocate the new buffer.
    setBufAddr( new uint8_t[ getMinBytes(getBitLen()) ] );

    // Clear the new buffer.
    if ( !isZero() ) clearAll();
}

} // end namespace libhei
