#pragma once

/**
@file hei_register.hpp
@brief Description:  The Register class.

Class Specification

Name:             Register
Title:            Scan Communication Register
Purpose:          "Register" provides the representation and access to a
                  physical register.
Usage:            This is an abstract base class.
Side-effects:     Memory is allocated.
Dependencies:     None.
Notes:            The Register class is a model of an actual physical register.
                  The bits in the register are represented by the bit_string
                  data member which is modified dynamically as operations are
                  preformed.  It acts as a temporarily cached value of the
                  register.  When a read is performed, the bit values are
                  updated in the bit string. When a write is performed, the
                  current value of the bits are used as the value to write.
                  The current value of this cached bit string can be accessed
                  or modified by other objects via the public interface.  The
                  physical address and bit length of the hardware register are
                  set during initialization and used on all acceses.

                  The basic Read() and Write() functions are virtual.  The
                  actual implemenations are dependent on the actual hardware
                  and the software Hardware Manual Ops Scan Control Routines.
                  These function specifications describe a common behaviour
                  that every derived class must follow.  Additional,
                  information may also be specified.

                  A Read() function is also provided that has a Bit String mask
                  parameter.  This function calls the virtual Read() and then
                  applies the mask so that the internal Bit String contains the
                  hardware register contents with certain bits ignored (masked
                  off).

Cardinality:      0

Space Complexity: Linear
                  K + Mn where K and M are constants and n is the
                  number of bits in the register.
*/

#include <stdio.h>
#include <register/hei_attn_types.hpp>
#include <util/hei_bit_string.hpp>
#include <chip_data/hei_chip_data.hpp>
#include <hei_includes.hpp>

namespace libhei
{

/**
Register class
Register provides the representation and access to a physical register.
*/
class Register
{
  private:
    

  public:

    /* The register access level */
    enum AccessLevel
    {
        ACCESS_NONE = 0x0, //< No access
        ACCESS_RO   = 0x1, //< Read-only access
        ACCESS_WO   = 0x2, //< Write-only access
        ACCESS_RW   = 0x3, //< Read/Write access
    };

  public:

    /* Constructor */
    Register() {};

    /* Disable the copy and move semantics
       Doing this with copy also does it for moves */
    Register(const Register&) = delete;                 // copy
    Register& operator=(const Register&) = delete;      // assignment

    /* Destructor */
    virtual ~Register(void);

    /**
    @brief Read hardware register (virtual)
    @param        None
    @return       [SUCCESS | MOPs return code]
    Requirements: None.
    Promises:     Internal bit string represents the value of the
                  hardware register (if rc == SUCCESS).
    Sideaffects:  Value guaranteed to be read from hardware.
    Exceptions:   None.
    Notes:        Default is to call Read().  If a child class cannot
                  guarantee hardware access every time Read() is
                  called then the function ForceRead() should be
                  overridden.
    */
    virtual uint32_t forceRead(void) const { return read(); }

    /**
    @brief Read hardware register (pure virtual)
    @param        None
    @return       [SUCCESS | MOPs return code]
    Requirements: None.
    Promises:     Internal bit string represents the value of the
                  hardware register (if rc == SUCCESS)
    Sideaffects:  The bit string value may or may not be retrieved
                  from hardware; a buffered copy may be used.
    Exceptions:   None.

    */
    virtual uint32_t read(void) const = 0;

    /**
    @brief        Read hardware register and apply a mask
    @param        Mask to apply
    @return       [SUCCESS | MOPs return code]
    Requirements: None.
    Promises:     Internal bit string represents the value of the hardware
                  register with the bits turned off as specified by the mask.
    Sideaffects:  The bit string value may or may not be retrieved
                  from hardware. a buffered copy may be used.
    Exceptions:   None.
    Notes:        if bits read from hardware = '00110100'
                  and mask =                   '01110000'
                  then internal bit sting =    '00000100'

                  if mask.Length() < GetBitString()->Length()
                  then mask is right extended with 0's

                  if mask.Length() > GetBitString()->Length()
                  then extra mask bits are ignored.
    */
    uint32_t read(BitString & mask);

    /**
    @brief        Write hardware register (pure virtual)
    @param        None
    @return       [SUCCESS | MOPs return code]
    Requirements: None.
    Promises:     Internal bit string value written to hardware
    Exceptions:   None.
    Notes:        If internal bitstring was never read/set/modified then
                  zeros are written to corresponding hardware register.
    */
    virtual uint32_t write(void) = 0;

    /**
    @brief Access a copy of the scan comm address
    @param        None
    @return       Returns scom address
    Requirements: None.
    Promises:     None.
    Exceptions:   None.
    */
    virtual uint64_t getAddress(void) const {return 0 ;}

    /**
    @brief Access a copy of the short id for signatures.
    @param        None
    @return       ID.
    Requirements: None.
    Promises:     None.
    Exceptions:   None.
    */
    virtual uint16_t getId(void) const = 0;

    /**
    @brief Set the short id for signatures.
    @param        ID.
    @return       None.
    Requirements: None.
    Promises:     None.
    Exceptions:   For virtual registers, this is not required to have
                  any effect.
    */
    virtual void setId(uint16_t) = 0;


    /**
    @brief Access the bit length of the register
    @param        None
    @return       bit length of the register
    Requirements: None.
    Promises:     None.
    Exceptions:   None.
    */
    virtual uint32_t getByteLength(void) const
    {
        return DEFAULT_BYTE_LENGTH;
    }

    /**
    @brief Access the internal bit string (pure virtual)
    @param        None
    @return       ptr to the internal bit string (const)
    Requirements: None.
    Promises:     None.
    Exceptions:   None.
    Notes:        If the internal bit string was never read/modified then
                  all bits are zero
    */
    virtual const BitString * getBitString(ATTENTION_TYPE
                  i_type = INVALID_ATTENTION_TYPE) const = 0;

    /**
    @brief Modify the internal bit string (pure virtual)
    @param        a bit string
    @return       Nothing
    Requirements: None.
    Promises:     Internal bit string == *bs for first len bits where
                  len is the smaller of the two lengths.  Memory may
                  be (re)allocated
    Exceptions:   None.
    Notes:        The hardware register value is not modified until
                  Write() is called
    */
    virtual void setBitString(const BitString * bs) = 0;

    /**
    @brief        SetBit
    @param        Position of bit to set (= 1)
    @return       None.
    Requirements: bit position < GetBitString()->Length()
    Promises:     GetBitString()->isBitSet(bit_position) == true
    Exceptions:   None.
    Notes:        Register value is not reflected in hardware until
                  Write() is called
    */
    void setBit(uint32_t bit_position);

    /**
    @brief        ClearBit (reset bit)
    @param        Position of bit to clear (= 0)
    @return       None.
    Requirements: bit position < GetBitString()->Length()
    Promises:     GetBitString()->isBitSet(bit_position) == false
    Exceptions:   None.
     Notes:       Register value is not reflected in hardware until
                  Write() is called
    */
    void clearBit(uint32_t bit_position);

    /**
    @brief        Will query if a bit is set.
    @param        i_bitPos The bit position to query.
                  Bit position must be less than GetBitString()->Length()
    @return       TRUE if the bit is set, FALSE otherwise.
    */
    bool isBitSet( uint32_t i_bitPos )
    {
        return getBitString()->isBitSet(i_bitPos);
    }

    /**
    @brief        Flushes all bits to 0.
    */
    void clearAllBits();

    /**
    @brief        Flushes all bits to 1.
    */
    void setAllBits();

    /**
    @brief        Returns target value from the BitString (right justified).
    @param        i_pos   Starting position in the bit string.
    @param        i_len   Length of target value.
                  i_pos + i_len must be less than or equal 64 bits.
    @return       The target value (right justified).
    */
    uint64_t getBitFieldJustified( uint32_t i_pos, uint32_t i_len ) const;

    /**
    @brief        Set a field within the BitString with a value (right
                  justified).
    @param        i_pos   Starting position in the bit string.
    @param        i_len   Length of target value.
    @param        i_value Value to add to BitString.
                  i_pos + i_len must be less than or equal 64 bits.
    */
    void setBitFieldJustified( uint32_t i_pos, uint32_t i_len,
                               uint64_t i_value );

    /**
    @brief        Query if bit string is all zeros
    @param        None.
    @return       [true | false]
    Requirements: None.
    Promises:     None.
    Exceptions:   None.
    */
    bool bitStringIsZero()
    {
        return getBitString()->isZero();
    }

    /**
    @brief        Returns DEFAULT_CHIP_TYPE (0) as type of Target associated
                  with class "register".  Actual implementation is expected in
                  derived class. 
    */
    virtual ChipType_t getChipType(void)const
    {
        return DEFAULT_CHIP_TYPE;
    }

    /**
    @brief        The register access level (see enum AccessLevel).
    */
    virtual AccessLevel getAccessLevel() const
    {
        return ACCESS_RW;
    }

    /**
    @brief        Sets the register access level (see enum AccessLevel).
    */
    virtual void setAccessLevel( AccessLevel i_op ) {}

  protected:

    /**
    @brief        Get modifiable reference to internal bit string
    @param        None.
    @return       Reference to the internal bit string
    Requirements: None.
    Promises:     None.
    */
  virtual BitString & accessBitString(void) = 0;

  private:
    static const int DEFAULT_BYTE_LENGTH = 8;
    static const int SUCCESS = 0;

};

}//namespace libhei
