// Module Description **************************************************
//
// Description: This module provides the implementation for the PRD Scan
//              Comm Register Chip class.
//
// End Module Description **********************************************

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------

#include <hei_includes.hpp>
#include <hei_user_interface.hpp>
#include <register/hei_hardware_register.hpp>
#include <util/hei_bit_string.hpp>

#if 0
#include <iipchip.h>
#include <prdfMain.H>
#include <prdfRasServices.H>
#include <prdfRegisterCache.H>
#include <prdfPlatServices.H>
#include <prdfExtensibleChip.H>

//----------------------------------------------------------------------
//  User Types
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//  Macros
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//  Internal Function Prototypes
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//  Global Variables
//----------------------------------------------------------------------

//---------------------------------------------------------------------
// Member Function Specifications
//---------------------------------------------------------------------

// --------------------------------------------------------------------
#endif

namespace libhei
{

#if 0
// ---------------------------------------------------------------------

void HardwareRegister::SetBitString( const BitString *bs )
{
    BitString & l_string  = AccessBitString();
    l_string.setString(*bs);
}


//------------------------------------------------------------------------------

const BitString * HardwareRegister::GetBitString(ATTENTION_TYPE i_type) const
{
    // Calling Read() will ensure that an entry exists in the cache and the
    // entry has at been synched with hardware at least once. Note that we
    // cannot read hardware for write-only registers. In this case, an entry
    // will be created in the cache, if it does not exist, when readCache() is
    // called below.
    if ( ( ACCESS_NONE != iv_operationType ) &&
            ( ACCESS_WO != iv_operationType ) )
    {
        Read();
    }
    return &(readCache());
}

//------------------------------------------------------------------------------

BitString & HardwareRegister::AccessBitString()
{
    // Calling Read() will ensure that an entry exists in the cache and the
    // entry has at been synched with hardware at least once. Note that we
    // cannot read hardware for write-only registers. In this case, an entry
    // will be created in the cache, if it does not exist, when readCache() is
    // called below.
    if ( ( ACCESS_NONE != iv_operationType ) &&
            ( ACCESS_WO != iv_operationType ) )
    {
        Read();
    }

    return readCache();
}

//------------------------------------------------------------------------------

uint32_t HardwareRegister::Read() const
{
    uint32_t o_rc = SUCCESS;

    // First query the cache for an existing entry.
    if ( !queryCache() )
    {
        // There was not a previous entry in the cache, so do a ForceRead() to
        // sync the cache with hardware.
        o_rc = ForceRead();
    }

    return o_rc;
}

//------------------------------------------------------------------------------

uint32_t HardwareRegister::ForceRead() const
{
    #define PRDF_FUNC "[HardwareRegister::ForceRead] "

    uint32_t o_rc = FAIL;

    do
    {
        // No read allowed if register access attribute is write-only or no
        // access.
        if ( ( ACCESS_NONE == iv_operationType ) &&
                ( ACCESS_WO == iv_operationType ) )
        {
            HEI_ERR( PRDF_FUNC "Write-only register: 0x%08x 0x%016llx",
                      getChip()->GetId(), iv_scomAddress );
            break;
        }

        // Read hardware.
        HEI_ASSERT( nullptr != cv_accessor );
        o_rc = cv_accessor->read( this, readCache() );
        if ( SUCCESS != o_rc )
        {
            // The read failed. Remove the entry from the cache so a subsequent
            // Read() will attempt to read from hardware again.
            flushCache( getChip() );
        }

    } while (0);

    return o_rc;

    #undef PRDF_FUNC
}

//------------------------------------------------------------------------------

uint32_t HardwareRegister::Write()
{
    #define PRDF_FUNC "[HardwareRegister::Write] "

    uint32_t o_rc = FAIL;

    do
    {
        // No write allowed if register access attribute is read-only or no
        // access.
        if ( ( ACCESS_NONE == iv_operationType ) &&
                 ( ACCESS_RO == iv_operationType ) )
        {
            HEI_ERR( PRDF_FUNC "Read-only register: 0x%08x 0x%016llx",
                      getChip()->GetId(), iv_scomAddress );
            break;
        }

        // Query the cache for an existing entry.
        if ( !queryCache() )
        {
            // Something bad happened and there was nothing in the cache to
            // write to hardware.
            HEI_ERR( PRDF_FUNC "No entry found in cache: 0x%08x 0x%016llx",
                      getChip()->GetId(), iv_scomAddress );
            break;
        }

        // Write hardware.
        HEI_ASSERT( nullptr != cv_accessor );
        o_rc = cv_accessor->write( this, readCache() );

    } while (0);

    return o_rc;

    #undef PRDF_FUNC
}

//-----------------------------------------------------------------------------
ExtensibleChip* HardwareRegister::getChip( )const
{
    ExtensibleChip* l_pchip = nullptr;
    l_pchip = ServiceDataCollector::getChipAnalyzed();
    TARGETING::TYPE l_type = PlatServices::getTargetType(
                                                l_pchip->GetChipHandle() );
    HEI_ASSERT( iv_chipType == l_type )
    return l_pchip;
}

//------------------------------------------------------------------------------

bool HardwareRegister::queryCache() const
{
    RegDataCache & cache = RegDataCache::getCachedRegisters();
    BitString * bs = cache.queryCache( getChip(), this );
    return ( nullptr != bs );
}

//------------------------------------------------------------------------------

BitString & HardwareRegister::readCache() const
{
    RegDataCache & cache = RegDataCache::getCachedRegisters();
    return cache.read( getChip(), this );
}

//------------------------------------------------------------------------------

void HardwareRegister::flushCache( ExtensibleChip *i_pChip ) const
{
     RegDataCache & regDump = RegDataCache::getCachedRegisters();
    if( nullptr == i_pChip )
    {
        regDump.flush();
    }
    else
    {
        regDump.flush( i_pChip ,this );
    }
}

//-----------------------------------------------------------------------------

bool HardwareRegister::operator == ( const HardwareRegister & i_rightRegister ) const
{
    if( iv_scomAddress == i_rightRegister.GetAddress() )
    {
        return ( iv_chipType == i_rightRegister.getChipType() );
    }
    else
    {
        return false ;
    }

}

//-----------------------------------------------------------------------------
bool HardwareRegister::operator < ( const HardwareRegister & i_rightRegister  ) const
{
    if( iv_scomAddress == i_rightRegister.GetAddress() )
    {
        return ( iv_chipType < i_rightRegister.getChipType() );
    }
    else
    {
        return( iv_scomAddress  < i_rightRegister.GetAddress() );
    }


}
//-----------------------------------------------------------------------------
bool HardwareRegister::operator >= ( const HardwareRegister & i_rightRegister  ) const
{
    return !( *this < i_rightRegister );
}
#endif

//------------------------------------------------------------------------------

HardwareRegister::Accessor * HardwareRegister::cv_accessor = nullptr;

//------------------------------------------------------------------------------

ReturnCode HardwareRegister::Accessor::read( const HardwareRegister * i_hwReg,
                                             BitString & o_bs ) const
{
    HEI_ASSERT( nullptr != i_hwReg );
    HEI_ASSERT( nullptr != iv_chip );

    // Get the byte size of the buffer.
    size_t sz_buffer = BitString::getMinBytes( o_bs.getBitLen() );

    // Read the register.
    return registerRead( iv_chip, i_hwReg->getType(), i_hwReg->getAddress(),
                         o_bs.getBufAddr(), sz_buffer );
}

//------------------------------------------------------------------------------

#ifndef __HEI_READ_ONLY

ReturnCode HardwareRegister::Accessor::write( const HardwareRegister * i_hwReg,
                                              const BitString & i_bs ) const
{
    HEI_ASSERT( nullptr != i_hwReg );
    HEI_ASSERT( nullptr != iv_chip );

    // Get the byte size of the buffer.
    size_t sz_buffer = BitString::getMinBytes( i_bs.getBitLen() );

    // Write the register.
    return registerWrite( iv_chip, i_hwReg->getType(), i_hwReg->getAddress(),
                          i_bs.getBufAddr(), sz_buffer );
}

#endif // __HEI_READ_ONLY

//------------------------------------------------------------------------------

} // end namespace libhei

