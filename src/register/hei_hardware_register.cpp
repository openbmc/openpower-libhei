// Module Description **************************************************
//
// Description: This module provides the implementation for the Hardware
//              Register Chip class.
//
// End Module Description **********************************************

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------

#include <hei_includes.hpp>
#include <hei_user_interface.hpp>
#include <register/hei_hardware_register.hpp>
#include <util/hei_bit_string.hpp>

namespace libhei
{

// ---------------------------------------------------------------------

void HardwareRegister::setBitString( const BitString *bs )
{
    BitString & l_string  = accessBitString();
    l_string.setString(*bs);
}

//------------------------------------------------------------------------------

const BitString * HardwareRegister::getBitString() const
{
    // Calling Read() will ensure that an entry exists in the cache and the
    // entry has at been synched with hardware at least once. Note that we
    // cannot read hardware for write-only registers. In this case, an entry
    // will be created in the cache, if it does not exist, when the cache is
    // read below.

    if ( ( ACCESS_NONE != iv_operationType ) &&
         ( ACCESS_WO   != iv_operationType ) )
    {
        read();
    }

    return &( accessCache() );
}

//------------------------------------------------------------------------------

BitString & HardwareRegister::accessBitString()
{
    // Calling Read() will ensure that an entry exists in the cache and the
    // entry has at been synched with hardware at least once. Note that we
    // cannot read hardware for write-only registers. In this case, an entry
    // will be created in the cache, if it does not exist, when the cache is
    // read below.

    if ( ( ACCESS_NONE != iv_operationType ) &&
         ( ACCESS_WO   != iv_operationType ) )
    {
        read();
    }

    return accessCache();
}

//------------------------------------------------------------------------------

ReturnCode HardwareRegister::read( bool i_force ) const
{
    ReturnCode rc;

    // Read from hardware only if the read is forced or the entry for this
    // instance does not exist in the cache.
    if ( i_force || !queryCache() )
    {
        // This register must be readable.
        HEI_ASSERT( ( ACCESS_NONE != iv_operationType ) &&
                    ( ACCESS_WO   != iv_operationType ) );

        // Get the buffer from the register cache.
        BitString & bs = accessCache();

        // Get the byte size of the buffer.
        size_t sz_buffer = BitString::getMinBytes( bs.getBitLen() );

        // Read this register from hardware.
        rc = registerRead( getAccessorChip().getChip(), bs.getBufAddr(),
                           sz_buffer, getRegisterType(), getAddress() );
        if ( RC_SUCCESS != rc )
        {
            // The read failed and we can't trust what was put in the register
            // cache. So remove this instance's entry from the cache.
            cv_cache.flush( getAccessorChip(), this );
        }
        else
        {
            // Sanity check. The returned size of the data written to the buffer
            // should match the register size.
            HEI_ASSERT( getByteSize() == sz_buffer );
        }
    }

    return rc;
}

//------------------------------------------------------------------------------

#ifndef __HEI_READ_ONLY

ReturnCode HardwareRegister::write() const
{
    ReturnCode rc;

    // This register must be writable.
    HEI_ASSERT( ( ACCESS_NONE != iv_operationType ) &&
                ( ACCESS_RO   != iv_operationType ) );

    // An entry for this register must exist in the cache.
    HEI_ASSERT( queryCache() );

    // Get the buffer from the register cache.
    BitString & bs = accessCache();

    // Get the byte size of the buffer.
    size_t sz_buffer = BitString::getMinBytes( bs.getBitLen() );

    // Write to this register to hardware.
    rc = registerWrite( getAccessorChip().getChip(), bs.getBufAddr(),
                        sz_buffer, getRegisterType(), getAddress() );

    if ( RC_SUCCESS == rc )
    {
        // Sanity check. The returned size of the data written to the buffer
        // should match the register size.
        HEI_ASSERT( getByteSize() == sz_buffer );
    }

    return rc;
}

#endif // __HEI_READ_ONLY

//------------------------------------------------------------------------------

bool HardwareRegister::operator == ( const HardwareRegister & i_rightRegister ) const
{
    if( iv_address == i_rightRegister.getAddress() )
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
    if( iv_address == i_rightRegister.getAddress() )
    {
        return ( iv_chipType < i_rightRegister.getChipType() );
    }
    else
    {
        return( iv_address  < i_rightRegister.getAddress() );
    }


}
//-----------------------------------------------------------------------------
bool HardwareRegister::operator >= ( const HardwareRegister & i_rightRegister  ) const
{
    return !( *this < i_rightRegister );
}

//------------------------------------------------------------------------------

HardwareRegister::Accessor * HardwareRegister::cv_accessor = nullptr;

//------------------------------------------------------------------------------

HardwareRegister::Cache HardwareRegister::cv_cache {};

//------------------------------------------------------------------------------

bool HardwareRegister::Cache::query( const Chip & i_chip,
                                     const HardwareRegister * i_hwReg ) const
{
    // Does i_chip exist in the cache?
    auto chipPairItr = iv_cache.find( i_chip );
    if ( iv_cache.end() != chipPairItr )
    {
        auto & hwRegMap = chipPairItr->second; // for ease of use

        // Does i_hwReg exist in the cache?
        auto hwRegPairItr = hwRegMap.find( i_hwReg );
        if ( hwRegMap.end() != hwRegPairItr )
        {
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------

BitString & HardwareRegister::Cache::access( const Chip & i_chip,
                                             const HardwareRegister * i_hwReg )
{
    // If the entry does not exist, create a new entry.
    if ( !query(i_chip, i_hwReg) )
    {
        BitString * bs = new BitStringBuffer { i_hwReg->getByteSize() * 8 };
        iv_cache[i_chip][i_hwReg] = bs;
    }

    // Return a reference to the target entry.
    return *(iv_cache[i_chip][i_hwReg]);
}

//------------------------------------------------------------------------------

void HardwareRegister::Cache::flush()
{
    // Delete all of the BitStrings.
    for ( auto & chipPair : iv_cache )
    {
        for ( auto & hwRegPair : chipPair.second )
        {
            delete hwRegPair.second;
        }
    }

    // !!! Do not delete the HardwareRegisters !!!
    // Those are deleted when the main uninitialize() API is called.

    // Flush the rest of the cache.
    iv_cache.clear();
}

//------------------------------------------------------------------------------

void HardwareRegister::Cache::flush( const Chip & i_chip,
                                     const HardwareRegister * i_hwReg )
{
    // Does i_chip exist in the cache?
    auto chipPairItr = iv_cache.find( i_chip );
    if ( iv_cache.end() != chipPairItr )
    {
        auto & hwRegMap = chipPairItr->second; // for ease of use

        // Does i_hwReg exist in the cache?
        auto hwRegPairItr = hwRegMap.find( i_hwReg );
        if ( hwRegMap.end() != hwRegPairItr )
        {
            delete hwRegPairItr->second; // delete the BitString
            hwRegMap.erase(i_hwReg);     // remove the entry for this register
        }

        // If i_hwReg was the only entry for i_chip, we can remove i_chip from
        // the cache.
        if ( hwRegMap.empty() )
        {
            iv_cache.erase(i_chip);
        }
    }
}

//------------------------------------------------------------------------------

} // end namespace libhei

