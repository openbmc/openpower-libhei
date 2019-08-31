#pragma once

/**
 * @brief Models register.It does not contain target.
 *
 * This class stores the hash id and bit length of scom registers It models
 * registers without maintaining target information. Instances of this class
 * are shared across rule chip objects of same type.Once prd object model is
 * built, instances of this register are saved in flyweight.These instances
 * persist as long as prd object model survives.
 */

#include <hei_includes.hpp>
#include <register/hei_register.hpp>
#include <util/hei_bit_string.hpp>

#include <prdfHomRegisterAccess.H>

namespace libhei
{

// Forward References
class CHIP_CLASS;
class MopsRegisterAccess;
class ExtensibleChip;

class ScomRegister : public Register
{
  public:

    /**
     * @brief     constructor
     * @param     i_address        address of the register
     * @param     i_bitLength      bit length of register
     * @param     i_targetType     target type associated with register
     */
    ScomRegister( uint64_t i_address, uint32_t i_bitLength,
                  TARGETING::TYPE i_targetType, AccessLevel i_access ) :
        Register(),
        iv_bitLength( i_bitLength ),
        iv_chipType( i_targetType ),
        iv_scomAddress( i_address ),
        iv_operationType( i_access )
    {}

    /**
     * @brief     constructor .Added this because we save object of this type in
     * @          FlyweightS
     */
    ScomRegister():
        Register(),
        iv_bitLength( 0 ),
        iv_chipType( TARGETING::TYPE_NA ),
        iv_scomAddress( 0 ),
        iv_operationType( ACCESS_NONE )
    {}

    /**
     * @brief     Returns the pointer to bit string
     * @param     i_type               attention type
     * @return    BitString *   pointer to bit string
     */

    virtual const BitString * GetBitString(ATTENTION_TYPE i_type =
                                                  INVALID_ATTENTION_TYPE) const;
    /**
     * @brief     Updates bit string contents associated with register
     * @param     i_bs               poiner to bit string
     * @return    Nil
     */

    virtual void SetBitString(const BitString * i_bs) ;

    /**
     * @brief     Returns length of the bits string associated with register
     * @return    length of bit string
     */
    uint32_t GetBitLength(void) const { return iv_bitLength ;}

    /**
     * @brief     Directly reads from hardware register
     * @return    SUCCESS|FAIL
     */
    virtual uint32_t ForceRead() const;

    /**
     * @brief     Returns contents of register.If entry does not exist in cache
     *            a  fresh entry is created and hardware is read.
     * @return    SUCCESS|FAIL
    */
    virtual uint32_t Read() const;

    /**
     * @brief     Writes cache contents to register.
     * @return    SUCCESS|FAIL
     */
    virtual uint32_t Write();

    /**
     * @brief     Returns the hash id of register
     * @return    returns  hash id of register
     * @pre       None
     * @post      None
     * @note
     */
    virtual uint16_t GetId(void) const { return iv_shortId; };

    /**
     * @brief     Sets the hash id of register
     * @param     i_id     hash id of register
     * @return    Nil
     */
    virtual void SetId(uint16_t i_id) { iv_shortId = i_id; };

   /**
    * @brief    Returns type of Target associated with register.
    * @return   Refer to function description
    */
   TARGETING::TYPE getChipType()const{ return iv_chipType ;} ;
   /**
    * @brief    Returns scom address of register
    * @return   Refer to function description
    */
   uint64_t GetAddress( ) const {return iv_scomAddress ;};
   /**
    * @brief     compares two ScomRegisterAccess register for equality
    * @param     i_rightRegister   register to be compared against
    * @return    Returns true if registers are equal false otherwise
    */
   bool operator == ( const ScomRegister & i_rightRegister ) const ;
   /**
    * @brief     defines < operation for ScomRegisterAccess
    * @param     i_rightRegister   register to be compared against
    * @return    Returns false if i_rightRegisters is less and true otherwise
    */
   bool operator < ( const ScomRegister & i_rightRegister  ) const ;
   /**
    * @brief     defines >= operation for ScomRegisterAccess
    * @param     i_rightRegister   register to be compared against
    * @return    Returns true if registers is >= i_rightRegister false
    *            otherwise
    */
   bool operator >= ( const ScomRegister & i_rightRegister  ) const;

    /** @return The register access level (see enum AccessLevel). */
    virtual AccessLevel getAccessLevel() const { return iv_operationType; }

    /** @brief Sets the register access level (see enum AccessLevel). */
    virtual void setAccessLevel( AccessLevel i_op ) { iv_operationType = i_op; }

  protected: // Functions

    /**
     * @brief     copy constructor
     * @param     i_scomRegister   scomRegister instance to be copied
     */
    ScomRegister( const Register & i_scomRegister ):
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
    virtual BitString & AccessBitString( );
    /**
     * @brief     Gets the register read and write done by calling access
     *            function of scom  accessor service.
     * @param     reference to bit string maintained in caller  class
     * @param     Read or write operation
     * @return    [SUCCESS|FAIL]
     */
    uint32_t Access( BitString & bs,
                     RegisterAccess::Operation op )const;

    /**
     * @brief  Returns rulechip pointer associated with the register
     * @return Refer to function description
     */
    virtual ExtensibleChip * getChip() const;

private: // functions

  friend class CaptureData;

    /** @return TRUE if entry for this register exist in this cache. */
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
    void flushCache( ExtensibleChip *i_pChip = NULL ) const;

  private: // Data

    uint32_t        iv_bitLength;     // bit length of scom
    uint16_t        iv_shortId;       // unique hash id of register
    TARGETING::TYPE iv_chipType;      // type of target associated with register
    uint64_t        iv_scomAddress;   // scom address associated with regiser
    AccessLevel     iv_operationType; // Operation supported (RO, WO, or RW)

};

} // end namespace libhei
