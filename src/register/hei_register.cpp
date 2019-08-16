/**
@file hei_register.cpp
@brief Description:  Implementation of the Register class.
Notes:               Unless stated otherwise, assume that each function
                     specification has no side-effects, no dependencies,
                     and constant time complexity.
*/

#include <register/hei_register.hpp>

using namespace libhei;

/**
Title:               ~Register (Virtual destructor)
Purpose:             This destructor deallocates the Bit String.
Side-effects:        Memory is deallocated.
Dependencies:        None.
*/
Register::~Register (void)
{
}

/*
Title:               Read
Purpose:             This function reads the actual hardware register and
                     sets the Bit String data member values.  The specified
                     Bit String is then used to mask the Bit String data
                     member.  If an error occur, then the error is reported
                     and the Bit String values are undefined.
Side-effects:        Hardware register is read.
                     Bit String data member is modified.
                     Memory is reallocated.
*/
uint32_t Register::read (BitString & mask)
{
    uint32_t rc = read();
    if(rc == SUCCESS)
    {
        BitString & bitString = accessBitString();
        bitString.maskString(mask);
    }
return(rc);
}

/*
Title:              Set Bit
Purpose:            This function sets(1) the specified bit position in
                    the Bit String.  If the Bit String is NULL, then a
                    new Bit String is allocated and cleared to all zero
                    before setting the bit.
Side-effects:       Bit String is modified.
                    Memory may be allocated.
Dependencies:       bit_position must be in the string
*/
void Register::setBit (uint32_t bit_position)
{
    BitString & bitString = accessBitString();
    bitString.setBit(bit_position);
}

/*
Title:              Clear Bit
Purpose:            This function clears(0) the specified bit position in
                    the Bit String.  If the Bit String is NULL, then a
                    new Bit String is allocated and cleared to all zeros.
Side-effects:       Bit String is modified.
                    Memory may be allocated.
Dependencies:       bit_position must be in the string
*/
void Register::clearBit (uint32_t bit_position)
{
    BitString & bitString = accessBitString();
    bitString.clearBit(bit_position);
}

/*
Title:             Clear Bit String
Purpose:           This function clears the Bit String.  If the data
                   member is NULL, then a new Bit String is allocated.
                   Upon return, the state of the Bit String is all zero.
Side-effects:      Bit String data member is modified.
                   Memory is allocated or reallocated.
*/
void Register::clearAllBits()
{
    BitString & bitString = accessBitString();
    bitString.clearAll();
}

/*
Title:             Set Bit String
Purpose:           This function sets all bits in the Bit String.  If the data
                   member is NULL, then a new Bit String is allocated.
                   Upon return, the state of the Bit String is all ones.
Side-effects:      Bit String data member is modified.
                   Memory is allocated or reallocated.
*/
void Register::setAllBits()
{
    BitString & bitString = accessBitString();
    bitString.setAll();
}

/*
Title:             Get Bit Field Justified
Purpose:           Returns target value from the BitString (right justified).
*/
uint64_t Register::getBitFieldJustified(uint32_t i_pos,
                                        uint32_t i_len) const
{
    uint64_t o_value = 0;

    const uint32_t len_cpu_word = sizeof(uint64_t) * 8;
    const uint32_t len_uint64   = sizeof(uint64_t) * 8;
    const uint32_t pos_end      = i_pos + i_len;

    HEI_ASSERT(pos_end <= len_uint64);

    const BitString * bs = getBitString();

    for (uint32_t pos = i_pos; pos < pos_end; pos += len_cpu_word)
    {
        // Calculate chunk length.
        uint32_t len_chunk = len_cpu_word;
        if (len_chunk > pos_end - pos)
            len_chunk = pos_end - pos;

        o_value <<= len_chunk; // Make room for new chunk.

        // Get chunk.
        o_value |= static_cast<uint64_t>(bs->getFieldRight(pos, len_chunk));
    }

    return o_value;
}

/*
Title:            Set Bit Field Justified
Purpose:          Set a field within the BitString with a value
                  (right justified).
*/
void Register::setBitFieldJustified(uint32_t i_pos,
                                    uint32_t i_len,
                                    uint64_t i_value)
{
    const uint32_t len_cpu_word = sizeof(uint64_t) * 8;
    const uint32_t len_uint64   = sizeof(uint64_t) * 8;

    HEI_ASSERT(i_pos + i_len <= len_uint64);

    BitString & bs = accessBitString();

    for (uint32_t offset = 0; offset < i_len; offset += len_cpu_word)
    {
        // Calculate chunk length.
        uint32_t len_chunk = len_cpu_word;
        if (len_chunk > i_len - offset)
            len_chunk = i_len - offset;

        uint64_t value = i_value;
        value >>= i_len - (offset + len_chunk); // right justify

        // Set chunk.
        bs.setFieldRight( i_pos + offset, len_chunk,
                            static_cast<uint64_t>(value) );
    }
}
