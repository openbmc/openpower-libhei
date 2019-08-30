#include <hei_user_interface.hpp>

#include <hei_includes.hpp>

namespace libhei
{

//------------------------------------------------------------------------------

ReturnCode registerRead( void * i_chip, uint64_t i_regType, uint64_t i_address,
                         void * o_buffer, size_t & io_bufSize )
{
    ReturnCode rc;

    HEI_ASSERT( nullptr != i_chip );
    HEI_ASSERT( nullptr != o_buffer );
    HEI_ASSERT( 0 != io_bufSize );

    switch ( i_regType )
    {
        default:
            rc = RC_REG_ACCESS_UNSUPPORTED;
            HEI_ERR( "registerRead(%p,%lx,%lx,%p,%lx)", i_chip, i_regType,
                     i_address, o_buffer, io_bufSize );
    }

    return rc;
}

//------------------------------------------------------------------------------

#ifndef __HEI_READ_ONLY

ReturnCode registerWrite( void * i_chip, uint64_t i_regType, uint64_t i_address,
                          void * i_buffer, size_t & io_bufSize )
{
    ReturnCode rc;

    HEI_ASSERT( nullptr != i_chip );
    HEI_ASSERT( nullptr != i_buffer );
    HEI_ASSERT( 0 != io_bufSize );

    switch ( i_regType )
    {
        default:
            rc = RC_REG_ACCESS_UNSUPPORTED;
            HEI_ERR( "registerWrite(%p,%lx,%lx,%p,%lx)", i_chip, i_regType,
                     i_address, i_buffer, io_bufSize );
    }

    return rc;
}

#endif

//------------------------------------------------------------------------------

} // end namespace libhei

