#pragma once

/**
@file hei_hardware_register.hpp
@brief Models Register.  It does not contain target.

Class Specification

Name:     Hardware Register
Purpose:  This class stores the hash ID and bit length of scom registers.
Notes:    It models registers without maintaining target information.
          Instances of this class are shared across rule chip objects of same
          type.  Once prd object model is built, instances of this register
          are saved in flyweight.  These instances persist as long as prd
          object model survives.
*/

#include <hei_includes.hpp>
#include <register/hei_register.hpp>
#include <util/hei_bit_string.hpp>

#include <prdfHomRegisterAccess.H>

namespace libhei
{

/* Forward References */
class CHIP_CLASS;
class MopsRegisterAccess;
class ExtensibleChip;

class HardwareRegister : public Register
{
  public:

    /* Disable the copy and move semantics\n
        Deleting the following also keeps implicit moves from being created */
    HardwareRegister(const HardwareRegister&) = delete;            // copy
    HardwareRegister& operator=(const HardwareRegister&) = delete; // assignment

    /* The register access level */
    enum AccessLevel
    {   
        ACCESS_NONE = 0x0, //< No access
        ACCESS_RO   = 0x1, //< Read-only access
        ACCESS_WO   = 0x2, //< Write-only access
        ACCESS_RW   = 0x3, //< Read/Write access
    };

    /**
    @brief     Constructor
    @param     i_address        address of the register
    @param     i_bitLength      bit length of register
    @param     i_targetType     target type associated with register
    */
    HardwareRegister( uint64_t i_address, uint32_t i_bitLength,
                  ChipType_t i_targetType, AccessLevel i_access ) :
        Register(),
        iv_bitLength( i_bitLength ),
        iv_chipType( i_targetType ),
        iv_scomAddress( i_address ),
        iv_operationType( i_access )
    {}

    /**
     * @brief     Constructor, added because we save object of this type in
     * @          FlyweightS
     */
    HardwareRegister():
        Register(),
        iv_bitLength( 0 ),
        iv_chipType( TARGETING::TYPE_NA ),
        iv_scomAddress( 0 ),
        iv_operationType( ACCESS_NONE )
    {}

    /* The register access level */
    enum AccessLevel_t
    {
        ACCESS_NONE = 0x0, //< No access
        ACCESS_RO   = 0x1, //< Read-only access
        ACCESS_WO   = 0x2, //< Write-only access
        ACCESS_RW   = 0x3, //< Read/Write access
    };
    const AccessLevel_t iv_accessLevel;

    /*
     *Instance variables
     */
//These will get their types from  hei_chip_data.hpp but they aren't there yet.
    /** A unique ID for tihs register. */
    const uint16_t iv_id;

    /** A register may have multiple instances.  All of those registers will
        have the same iv_id and this instance variable will be used to
        distinguish between each instance. */
    const uint8_t iv_instance;

    /** The 1, 2, 4, or 8 byte address (right justified) of this register. */
    const uint64_t iv_address;

    /** The byte length of this register. */
    const size_t iv_size;

    /** This is given to the user application to indicate what type of
        register access is needed. */
    const uint16_t iv_type;

    /*
     *Class variables
     */

    /* This allows all HardwareRegister objects access to hardware via the
       user interface APIs. */
    static Accessor * cv_accessor;

    /*
     * Public Member functions.
     */

    /**
     * @brief     Returns the pointer to bit string
     * @param     i_type               attention type
     * @return    BitString *   pointer to bit string
     */
    virtual const BitString * getBitString() const = 0;

    /**
     * @brief     Returns contents of register.  If entry does not exist in
                  cache a  fresh entry is created and hardware is read.
     * @return    SUCCESS|FAIL
    */
    ReturnCode read(void) const = 0;

    /**
     * @brief     Updates bit string contents associated with register
     * @param     i_bs               poiner to bit string
     * @return    Nil
     */
    virtual void setBitString(const BitString * i_bs) ;

    /**
     * @brief     Returns length of the bits string associated with register
     * @return    length of bit string
     */
    uint32_t getBitLength(void) const { return iv_bitLength ;}

    /**
     * @brief     Directly reads from hardware register
     * @return    SUCCESS|FAIL
     */
    virtual uint32_t forceRead() const;

    /**
     * @brief     Writes cache contents to register.
     * @return    SUCCESS|FAIL
     */
    virtual uint32_t write();

    /**
     * @brief     Returns the hash id of register
     * @return    returns  hash id of register
     */
    virtual uint16_t getId(void) const { return iv_shortId; };

    /**
     * @brief     Sets the hash id of register
     * @param     i_id     hash id of register
     * @return    Nil
     */
    virtual void setId(uint16_t i_id) { iv_shortId = i_id; };

    /**
    * @brief    Returns type of Target associated with register.
    * @return   Refer to function description
    */
   ChipType_t getChipType()const{ return iv_chipType ;} ;

    /**
    * @brief    Returns scom address of register
    * @return   Refer to function description
    */
   uint64_t getAddress( ) const {return iv_scomAddress ;};

    /**
    * @brief     compares two ScomRegisterAccess register for equality
    * @param     i_rightRegister   register to be compared against
    * @return    Returns true if registers are equal false otherwise
    */
   bool operator == ( const HardwareRegister & i_r) const ;

    /**
    * @brief     defines < operation for ScomRegisterAccess
    * @param     i_rightRegister   register to be compared against
    * @return    Returns false if i_rightRegisters is less and true otherwise
    */
   bool operator < ( const HardwareRegister & i_r) const ;

    /**
    * @brief     defines >= operation for ScomRegisterAccess
    * @param     i_rightRegister   register to be compared against
    * @return    Returns true if registers is >= i_rightRegister false
    *            otherwise
    */
   bool operator >= ( const HardwareRegister & i_rightRegister  ) const;

    /** @return The register access level (see enum AccessLevel). */
    virtual AccessLevel getAccessLevel() const { return iv_operationType; }

    /** @brief Sets the register access level (see enum AccessLevel). */
    virtual void setAccessLevel( AccessLevel i_op ) { iv_operationType = i_op; }

  protected: // Functions

    /**
     * @brief     copy constructor
     * @param     i_scomRegister   scomRegister instance to be copied
     */
    HardwareRegister( const Register & i_scomRegister ):
        Register(),
        iv_bitLength(     i_scomRegister.GetBitLength()   ),
        iv_shortId(       i_scomRegister.GetId()          ),
        iv_chipType(      i_scomRegister.getChipType()    ),
        iv_scomAddress(   i_scomRegister.GetAddress()     ),
        iv_operationType( i_scomRegister.getAccessLevel() )
    {}

    /**
    * @brief    Returns reference to bit string associated with register
    * @return   Refer to function description
    */
    virtual BitString & accessBitString( );

    /**
     * @brief     Gets the register read and write done by calling access
     *            function of scom  accessor service.
     * @param     reference to bit string maintained in caller class
     * @param     Read or write operation
     * @return    [SUCCESS|FAIL]
     */
    uint32_t access( BitString & bs,
                     RegisterAccess::Operation op )const;

    /**
     * @brief  Returns rulechip pointer associated with the register
     * @return Refer to function description
     */
    virtual ExtensibleChip * getChip() const;

  private: // functions

  friend class CaptureData;

    /**
     * @brief Checks if entry for this register exists in cache.
     * @return TRUE if exists
     */
    bool queryCache() const;

    /**
     * @brief  Reads register contents from cache.
     * @return Reference to bit string buffer maintained in cache.
     */
    BitString & readCache() const;

    /**
     * @brief     Deletes one or all entry in the cache
     * @param     RuleChip pointer associated with register
     * @return    Nil
     */
    void flushCache( ExtensibleChip *i_pChip = nullptr ) const;

  private: // Data

    uint32_t        iv_bitLength;     // bit length of scom
    uint16_t        iv_shortId;       // unique hash id of register
    ChipType_t      iv_chipType;      // type of target associated with register
    uint64_t        iv_scomAddress;   // scom address associated with regiser
    AccessLevel     iv_operationType; // Operation supported (RO, WO, or RW)

};

}//namespace libhei ends
