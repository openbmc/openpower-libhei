#include <hei_includes.hpp>
#include <register/hei_register_cache.hpp>

namespace libhei
{

#if 0
//------------------------------------------------------------------------------

RegDataCache & RegDataCache::getCachedRegisters()
{
    return PRDF_GET_SINGLETON( ReadCache );
}

//------------------------------------------------------------------------------

RegDataCache::~RegDataCache()
{
    flush();
}

//------------------------------------------------------------------------------

BitString & RegDataCache::read( ExtensibleChip * i_chip,
                                       const Register * i_reg )
{
    ScomRegisterAccess l_scomAccessKey ( *i_reg, i_chip );
    BitString * l_pBitString = queryCache( l_scomAccessKey );

    if ( nullptr == l_pBitString )
    {
        // Creating new entry
        l_pBitString = new BitStringBuffer( i_reg->GetBitLength() );
        // Adding register in the cache
        iv_cachedRead[l_scomAccessKey] = l_pBitString;
    }

    return *l_pBitString;
}

//------------------------------------------------------------------------------

void RegDataCache::flush()
{
    for ( CacheDump::iterator it = iv_cachedRead.begin();
          it != iv_cachedRead.end(); it++ )
    {
        // Freeing up the bit string memory reserved on heap
        delete it->second;
    }

    // Deleting all the entry from the cache
    iv_cachedRead.clear();
}

//------------------------------------------------------------------------------

void RegDataCache::flush( ExtensibleChip* i_pChip,
                          const Register * i_pRegister )
{
    ScomRegisterAccess l_scomAccessKey ( *i_pRegister,i_pChip );
    // Find the entries associated with the given target in the map
    CacheDump::iterator it = iv_cachedRead.find( l_scomAccessKey );

    // If entry exists delete the entry for given scom address
    if ( it !=iv_cachedRead.end() )
    {
        delete it->second;
        iv_cachedRead.erase( it );
    }
}

//------------------------------------------------------------------------------

BitString * RegDataCache::queryCache(
                            ExtensibleChip* i_pChip,
                            const Register * i_pRegister )const
{
    ScomRegisterAccess l_scomAccessKey ( *i_pRegister,i_pChip );
    return queryCache( l_scomAccessKey );
}

//------------------------------------------------------------------------------

BitString * RegDataCache::queryCache(
                        const ScomRegisterAccess & i_scomAccessKey ) const
{
    BitString * l_pBitString = nullptr;
    CacheDump::const_iterator itDump = iv_cachedRead.find( i_scomAccessKey );
    if( iv_cachedRead.end() != itDump )
    {
        l_pBitString = itDump->second ;
    }

    return l_pBitString;
}

//------------------------------------------------------------------------------
#endif

} // end namespace libhei

