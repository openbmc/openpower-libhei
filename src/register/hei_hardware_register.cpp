// Module Description **************************************************
//
// Description: This module provides the implementation for the Hardware
//              Register Chip class.
//
// End Module Description **********************************************

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------

#include <hei_hardware_register.hpp>

namespace libhei
{

// ---------------------------------------------------------------------

void HardwareRegister::setBitString( const BitString *bs )
{
    BitString & l_string  = AccessBitString();
    l_string.setString(*bs);
}


//------------------------------------------------------------------------------

const BitString * HardwareRegister::getBitString() const
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

BitString & HardwareRegister::accessBitString()
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

uint32_t HardwareRegister::read() const
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

uint32_t HardwareRegister::forceRead() const
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
            PRDF_ERR( PRDF_FUNC "Write-only register: 0x%08x 0x%016llx",
                      getChip()->GetId(), iv_scomAddress );
            break;
        }

        // Read hardware.
        o_rc = access( readCache(), RegisterAccess::READ );
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

uint32_t HardwareRegister::write()
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
            PRDF_ERR( PRDF_FUNC "Read-only register: 0x%08x 0x%016llx",
                      getChip()->GetId(), iv_scomAddress );
            break;
        }

        // Query the cache for an existing entry.
        if ( !queryCache() )
        {
            // Something bad happened and there was nothing in the cache to
            // write to hardware.
            PRDF_ERR( PRDF_FUNC "No entry found in cache: 0x%08x 0x%016llx",
                      getChip()->GetId(), iv_scomAddress );
            break;
        }

        // Write hardware.
        o_rc = access( readCache(), RegisterAccess::WRITE );

    } while (0);

    return o_rc;

    #undef PRDF_FUNC
}

//------------------------------------------------------------------------------

uint32_t HardwareRegister::access( BitString & bs,
                               RegisterAccess::Operation op ) const
{
    int32_t l_rc = SCR_ACCESS_FAILED;
    TARGETING::TargetHandle_t i_pchipTarget = getChip()->GetChipHandle();
    l_rc = getScomService().Access( i_pchipTarget,bs,iv_scomAddress,op );

    return(l_rc);
}
//-----------------------------------------------------------------------------
ExtensibleChip* HardwareRegister::getChip( )const
{
    ExtensibleChip* l_pchip = nullptr;
    l_pchip = ServiceDataCollector::getChipAnalyzed();
    ChipType_t l_type = PlatServices::getTargetType(
                                                l_pchip->GetChipHandle() );
    PRDF_ASSERT( iv_chipType == l_type )
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
}//namespace libhei ends
