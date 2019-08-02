
#include <hei_bit_string.hpp>

#include <hei_user_defines.hpp>

#include <algorithm>

namespace libhei
{

//##############################################################################
//                             BitString class
//##############################################################################

// number of bits in a uint64_t
const uint32_t BitString::UINT64_BIT_LEN = sizeof(uint64_t) * 8;

// number of bits in a uint8_t
const uint32_t BitString::UINT8_BIT_LEN = sizeof(uint8_t) * 8;

// uint8_t with all bits set
const uint8_t BitString::UINT8_BIT_MASK = static_cast<uint8_t>(-1);

//------------------------------------------------------------------------------

uint64_t BitString::getFieldLeft( uint32_t i_pos, uint32_t i_len ) const
{
    HEI_ASSERT( nullptr != getBufAddr() );      // must to have a valid address
    HEI_ASSERT( 0 < i_len );                    // must have at least one bit
    HEI_ASSERT( i_len <= UINT64_BIT_LEN );      // i_len length must be valid
    HEI_ASSERT( i_pos + i_len <= getBitLen() ); // field must be within range

    // The returned value.
    uint64_t o_val = 0;

    // Get the relative address and position of the field.
    uint32_t relPos = 0;
    uint8_t * relAddr = (uint8_t *)getRelativePosition( relPos, i_pos );

    // The desired bitstring may be larger than one byte so retrieve each
    // byte recursively. Each byte that is to be retrieved may cross a byte
    // boundary. In this case retrieve each byte as a low chunk and a high
    // chunk masking and shifting each chunk and combining them to form one.

    // Assume the low  chunk contains the entire value
    uint32_t len0 = i_len;
    uint32_t len1 = 0;

    // Remaining length and next position used for recursion
    uint32_t remainLen = 0;
    uint32_t nextPos = 0;

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

    // Masks will be the size of the chunk length and right justified
    uint8_t mask0 = UINT8_BIT_MASK >> ( UINT8_BIT_LEN - len0 ); // low chunk
    uint8_t mask1 = UINT8_BIT_MASK >> ( UINT8_BIT_LEN - len1 ); // high chunk

    // Align the low chunk to bit position 0 and mask the bits of interest
    o_val = ((*relAddr >> relPos) & mask0);

    // Get the high chunk if needed
    if (len1)
    {
        ++relAddr; // high chunk is in the next array element

        // Get the high chunk, mask the bits of interes and
        // align it to bit position 0 plus the length
        o_val |= ((*relAddr & mask1) << len0);
    }

    // Prep for another recursive call
    remainLen = (i_len - (len0 + len1));
    nextPos = (i_pos + (len0 + len1));

    // Get next high order byte of bitstring, if needed (recursively)
    if (remainLen)
    {
        // Shift the next highest set of bits to the next highest byte

        // Right-justified return value
        //o_val |= ( getFieldLeft( nextPos, remainLen ) << UINT8_BIT_LEN );

        // Left-justified return value
        uint64_t tempval = (getFieldLeft( nextPos, remainLen ) >> (UINT64_BIT_LEN - remainLen));
        o_val |= tempval << UINT8_BIT_LEN;

    }

    // Right-justified return value
    //return o_val;

    // Left-justified return value
    return (o_val << (UINT64_BIT_LEN - i_len));
}

//------------------------------------------------------------------------------

void BitString::setFieldLeft( uint32_t i_pos, uint32_t i_len, uint64_t i_val )
{
    HEI_ASSERT( nullptr != getBufAddr() );      // must to have a valid address
    HEI_ASSERT( 0 < i_len );                    // must have at least one bit
    HEI_ASSERT( i_len <= UINT64_BIT_LEN );      // i_len length must be valid
    HEI_ASSERT( i_pos + i_len <= getBitLen() ); // field must be within range

    // Left-justified input value
    i_val = i_val >> (UINT64_BIT_LEN - i_len);

    // The set value.
    uint8_t val = i_val; // one byte set per pass - need explicit cast?

    // Get the relative address and position of the field.
    uint32_t relPos = 0;
    uint8_t * relAddr = (uint8_t *)getRelativePosition( relPos, i_pos );

    // The bitstring may be larger than one byte so set one byte at a time
    // recursively. Each byte that is to be set may cross a byte boundary.
    // In this case set each byte as a low chunk and a high chunk masking
    // and shifting each chunk before writing them.

    // Assume the low  chunk contains the entire value
    uint32_t len0 = i_len;
    uint32_t len1 = 0;

    // Remaining length and next position used for recursion
    uint32_t remainLen = 0;
    uint32_t nextPos = 0;

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

    // Masks will be the size of the chunk length and right justified
    uint8_t mask0 = UINT8_BIT_MASK >> ( UINT8_BIT_LEN - len0 ); // low chunk
    uint8_t mask1 = UINT8_BIT_MASK >> ( UINT8_BIT_LEN - len1 ); // high chunk

    val &= mask0; // mask source bits to set
    *relAddr &= ~(mask0 >> relPos); // clear destination bits
    *relAddr |= (val >> relPos); // set destination bits

    // Get the hight chunk if needed
    if (len1)
    {
        ++relAddr; //high chunk is in the next array element

        val &= mask1; // mask source bits
        *relAddr &= ~mask1; // clear destination bits
        *relAddr |= val; // set destination bits
    }

    // Prep for another recursive call
    remainLen = (i_len - (len0 + len1));
    nextPos = (i_pos + (len0 + len1));

    // Set the next high order byte of bitstring, if needed (recurively)
    if (remainLen)
    {
        // Left-justified input value
        i_val = i_val << (UINT64_BIT_LEN - remainLen);

        // Shift the next highest bits into position to set next
        i_val = (i_val >> UINT8_BIT_LEN);

        setFieldLeft( nextPos, remainLen, i_val);
    }

    return;
}

//------------------------------------------------------------------------------

void BitString::setPattern( uint32_t i_sPos, uint32_t i_sLen,
                            uint8_t i_pattern, uint32_t i_pLen )
{
    HEI_ASSERT(nullptr != getBufAddr());        // must to have a valid address
    HEI_ASSERT(0 < i_sLen);                     // must have at least one bit
    HEI_ASSERT(i_sPos + i_sLen <= getBitLen()); // field must be within range
    HEI_ASSERT(0 < i_pLen);                     // must have at least one bit
    HEI_ASSERT(i_pLen <= UINT64_BIT_LEN);       // i_pLen length must be valid

    // Get a bit string for the pattern subset (right justified).
    BitString bso ( i_pLen, &i_pattern, UINT8_BIT_LEN - i_pLen );

    // Iterate the range in chunks the size of i_pLen.
    uint32_t endPos = i_sPos + i_sLen;
    for ( uint32_t pos = i_sPos; pos < endPos; pos += i_pLen )
    {
        // The true chunk size is either i_pLen or the leftovers at the end.
        uint32_t len = std::min( i_pLen, endPos - pos );

        // Get this chunk's pattern value, truncate (left justified) if needed.
        uint8_t pattern = bso.getFieldLeft( 0, len );

        // Set the pattern in this string.
        setFieldLeft( pos, len, pattern );
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
    uint8_t * sRelAddr = (uint8_t *)i_sStr.getRelativePosition( sRelOffset, i_sPos );
    uint8_t * dRelAddr = (uint8_t *)       getRelativePosition( dRelOffset, i_dPos );

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

            uint64_t value = i_sStr.getFieldLeft( i_sPos + pos, len );
            setFieldLeft( i_dPos + pos, len, value );
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

            uint64_t value = i_sStr.getFieldLeft( i_sPos + pos, len );
            setFieldLeft( i_dPos + pos, len, value );
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

        uint64_t dVal =        getFieldLeft( pos, len );
        uint64_t sVal = i_mask.getFieldLeft( pos, len );

        setFieldLeft( pos, len, dVal & ~sVal );
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

        if ( getFieldLeft(pos, len) != i_str.getFieldLeft(pos, len) )
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

        if ( 0 != getFieldLeft(pos, len) )
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

        uint64_t dVal = getFieldLeft( pos, len );

        bsb.setFieldLeft( pos, len, ~dVal );
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

        uint64_t dVal =      getFieldLeft( pos, len );
        uint64_t sVal = i_bs.getFieldLeft( pos, len );

        bsb.setFieldLeft( pos, len, dVal & sVal );
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

        uint64_t dVal =      getFieldLeft( pos, len );
        uint64_t sVal = i_bs.getFieldLeft( pos, len );

        bsb.setFieldLeft( pos, len, dVal | sVal );
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
    setBufAddr( new uint8_t[ getNumBufBytes(getBitLen()) ] );

    // Clear the new buffer.
    if ( !isZero() ) clearAll();
}


} // end namespace libhei
