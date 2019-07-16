#include <hei_user_defines.hpp>
#include <bitstring.hpp>
#include "gtest/gtest.h"

// test case parameters
#define num_fields 2
#define field_size ( sizeof( uint64_t ) * 8 )
#define bitstring_size ( field_size * num_fields )

// userful test patterns
#define max_field_size ( sizeof( uint64_t) * 8 )
#define pattern_55 ( 0x5555555555555555ULL >> ( max_field_size - field_size ) )
#define pattern_aa ( 0xaaaaaaaaaaaaaaaaULL >> ( max_field_size - field_size ) )
#define pattern_ff ( 0xffffffffffffffffULL >> ( max_field_size - field_size ) )
#define pattern_00 ( 0x0000000000000000ULL >> ( max_field_size - field_size ) )

using namespace libhei;

//
// BitString and BitStringBuffer tests
//
TEST( BitStringTest, BitStringBufferConstructorFromLength )
{

    // test BitStringBuffer constructor
    BitStringBuffer bsb ( bitstring_size );
    ASSERT_NE( nullptr, bsb.getBufAddr() );
    ASSERT_EQ( bitstring_size, bsb.getBitLen() );
    ASSERT_TRUE( bsb.isZero() );
}

TEST( BitStringTest, CopyConstructorFromBitString )
{

    // test with un-initialized buffer
    BitString bs1 ( bitstring_size, nullptr, 0 );
    BitStringBuffer bsb1 = bs1;
    ASSERT_NE( nullptr, bsb1.getBufAddr() );
    ASSERT_EQ( bs1.getBitLen(), bsb1.getBitLen() );
    ASSERT_TRUE( bsb1.isZero() );

    // test with initialized buffer
    uint64_t buffer2 [ num_fields ] = { 0 };
    BitString bs2 ( bitstring_size, buffer2, 0 );
    BitStringBuffer bsb2 = bs2;
    ASSERT_NE( nullptr, bsb2.getBufAddr() );
    ASSERT_EQ( bs2.getBitLen() , bsb2.getBitLen() );
    ASSERT_TRUE( bsb2.isEqual(bs2) );

    // test with initialized buffer
    uint64_t buffer3 [] = { pattern_aa, pattern_55 };
    BitString bs3 ( bitstring_size, buffer3, 0 );
    BitStringBuffer bsb3 = bs3;
    ASSERT_NE( nullptr, bsb3.getBufAddr() );
    ASSERT_EQ( bs3.getBitLen(), bsb3.getBitLen() );
    ASSERT_FALSE( bsb3.isZero() );
    ASSERT_TRUE( bsb3.isEqual(bs3) );
}

TEST( BitStringTest, CopyConstructorFromBitStringBuffer )
{

    // test with default bitstringbuffer
    BitStringBuffer bsb1 ( bitstring_size );
    BitStringBuffer bsb2 = bsb1;
    ASSERT_NE( nullptr, bsb2.getBufAddr() );
    ASSERT_EQ( bsb1.getBitLen(), bsb2.getBitLen() );
    ASSERT_TRUE( bsb2.isZero() );

    // test with un-initialized buffer -> bitstringbuffer
    BitString bs3 ( bitstring_size, nullptr, 0 );
    BitStringBuffer bsb3 = bs3;
    BitStringBuffer bsb4 = bsb3;
    ASSERT_NE( nullptr, bsb4.getBufAddr() );
    ASSERT_EQ( bsb3.getBitLen(), bsb4.getBitLen() );
    ASSERT_TRUE( bsb4.isZero() );

    // test with initialized buffer -> bitstringbuffer
    uint64_t buffer4 [ num_fields ] = { 0 };
    BitString bs4 ( bitstring_size, buffer4, 0 );
    BitStringBuffer bsb5 = bs4;
    BitStringBuffer bsb6 = bsb5;
    ASSERT_NE( nullptr, bsb6.getBufAddr() );
    ASSERT_EQ( bsb5.getBitLen(), bsb6.getBitLen() );
    ASSERT_TRUE( bsb6.isEqual( bsb5 ) );

    // test with initialized buffer -> bitstringbuffer
    uint64_t buffer6 [] = { pattern_aa, pattern_55 };
    BitString bs6 ( bitstring_size, buffer6, 0 );
    BitStringBuffer bsb7 = bs6;
    BitStringBuffer bsb8 = bsb7;
    ASSERT_NE( nullptr, bsb8.getBufAddr() );
    ASSERT_EQ( bsb7.getBitLen(), bsb8.getBitLen() );
    ASSERT_FALSE( bsb8.isZero() );
    ASSERT_TRUE( bsb8.isEqual( bsb7 ) );
}

TEST( BitStringTest, AssignmentFromBitString )
{

    // test with un-initialized buffer
    BitString bs1 ( bitstring_size, nullptr, 0 );
    BitStringBuffer bsb1 ( bitstring_size );
    bsb1 = bs1;
    ASSERT_NE( nullptr, bsb1.getBufAddr() );
    ASSERT_EQ( bs1.getBitLen(), bsb1.getBitLen() );
    ASSERT_TRUE( bsb1.isZero() );

    // test with fields cleared
    uint64_t buffer2 [ num_fields ] = { 0 };
    BitString bs2 ( bitstring_size, buffer2, 0 );
    BitStringBuffer bsb2 ( bitstring_size );
    bsb2 = bs2;
    ASSERT_NE( nullptr, bsb2.getBufAddr() );
    ASSERT_EQ( bs2.getBitLen(), bsb2.getBitLen() );
    ASSERT_TRUE( bsb2.isEqual(bs2) );

    // test with fields set
    uint64_t buffer3 [] = { pattern_aa, pattern_55 };
    BitString bs3 ( bitstring_size, buffer3, 0 );
    BitStringBuffer bsb3 ( bitstring_size );
    bsb3 = bs3;
    ASSERT_NE( nullptr, bsb3.getBufAddr() );
    ASSERT_EQ( bs3.getBitLen(), bsb3.getBitLen() );
    ASSERT_FALSE( bsb3.isZero() );
    ASSERT_TRUE( bsb3.isEqual(bs3) );
}

TEST( BitStringTest, AssignmentFromBitStringBuffer )
{

    // test with fields cleared bitstringbuffer -> bitstringbuffer
    BitStringBuffer bsb1 ( bitstring_size );
    BitStringBuffer bsb2 ( bitstring_size );
    bsb2 = bsb1;
    ASSERT_NE( nullptr, bsb2.getBufAddr() );
    ASSERT_EQ( bsb1.getBitLen(), bsb2.getBitLen() );
    ASSERT_TRUE( bsb2.isZero() );

    // test with un-initialized buffer -> bitstringbuffer
    BitString bs3 ( bitstring_size, nullptr, 0 );
    BitStringBuffer bsb3 = bs3;
    BitStringBuffer bsb4 ( bitstring_size );
    bsb4 = bsb3;
    ASSERT_NE( nullptr, bsb4.getBufAddr() );
    ASSERT_EQ( bsb3.getBitLen(), bsb4.getBitLen() );
    ASSERT_TRUE( bsb4.isZero() );

    // test with initialized buffer -> bitstringbuffer
    uint64_t buffer4 [ num_fields ] = { 0 };
    BitString bs4 ( bitstring_size, buffer4, 0 );
    BitStringBuffer bsb5 = bs4;
    BitStringBuffer bsb6 ( bitstring_size );
    bsb6 = bsb5;
    ASSERT_NE( nullptr, bsb6.getBufAddr() );
    ASSERT_EQ( bsb5.getBitLen(), bsb6.getBitLen() );
    ASSERT_TRUE( bsb6.isEqual( bsb5 ) );

    // test with initialized buffer -> bitstringbuffer
    uint64_t buffer6 [] = { pattern_aa, pattern_55 };
    BitString bs6( bitstring_size, buffer6, 0 );
    BitStringBuffer bsb7 = bs6;
    BitStringBuffer bsb8 ( bitstring_size ) ;
    bsb8 = bsb7;
    ASSERT_NE( nullptr, bsb8.getBufAddr() );
    ASSERT_EQ( bsb7.getBitLen(), bsb8.getBitLen() );
    ASSERT_FALSE( bsb8.isZero() );
    ASSERT_TRUE( bsb8.isEqual( bsb7 ) );
}

TEST( BitStringTest, BitStringBufferOperatorOverloadNot )
{

    // tests with fields set
    uint64_t buffer1 [ num_fields ] = { pattern_aa, pattern_aa };
    BitString bs1( bitstring_size, buffer1 );
    BitStringBuffer bsb1 = bs1;
    uint64_t buffer2 [ num_fields ] = { pattern_55, pattern_55 };
    BitString bs2( bitstring_size, buffer2 );
    BitStringBuffer bsb2 = bs2;
    // ~(0x55) = 0xaa
    bsb2 = ~( bsb2 );
    ASSERT_TRUE( bsb1.isEqual( bsb2 ) );
}

TEST( BitStringTest, BitStringBufferOperatorOverloadAnd )
{

    // test with fields set
    uint64_t buffer1 [ num_fields ] = { pattern_aa, pattern_aa };
    BitString bs1( bitstring_size, buffer1 );
    BitStringBuffer bsb1 = bs1;
    uint64_t buffer2 [ num_fields ] = { pattern_55, pattern_55 };
    BitString bs2( bitstring_size, buffer2 );
    BitStringBuffer bsb2 = bs2;
    // 0xaa & 0x55 = 0xaa
    bsb1 = ( bsb1 & bsb2 );
    ASSERT_TRUE( bsb1.isZero() );
}

TEST( BitStringTest, BitStringBufferOperatorOverloadOr )
{

    // test with fields set
    uint64_t buffer1 [ num_fields ] = { pattern_aa, pattern_aa };
    BitString bs1( bitstring_size, buffer1 );
    BitStringBuffer bsb1 = bs1;
    uint64_t buffer2 [ num_fields ] = { pattern_55, pattern_55 };
    BitString bs2( bitstring_size, buffer2 );
    BitStringBuffer bsb2 = bs2;
    // 0x55 | 0xaa = 0xff
    bsb1 = ~( bsb1 | bsb2 ); // ~(0xff) = 0x00
    ASSERT_TRUE( bsb1.isZero() );
}

TEST( BitStringTest, BitStringBufferOperatorOverloadShiftRight )
{

    // test with fields set
    uint64_t buffer1 [ num_fields ] = { pattern_aa, pattern_aa };
    BitString bs1( bitstring_size, buffer1 );
    BitStringBuffer bsb1 = bs1;
    uint64_t buffer2 [ num_fields ] = { pattern_55, pattern_55 };
    BitString bs2( bitstring_size, buffer2 );
    BitStringBuffer bsb2 = bs2;
    // 0xaa >> 1 = 0x55
    bsb1 = ( bsb1 >> 1 );
    ASSERT_TRUE( bsb1.isEqual( bsb2 ) );
    // clear with shift
    bsb1 = ( bsb1 >> bsb1.getBitLen() );
    ASSERT_TRUE( bsb1.isZero() );
}

TEST( BitStringTest, BitStringBufferOperatorOverloadShiftLeft )
{

    // test with fields set
    uint64_t buffer1 [ num_fields ] = { pattern_aa, pattern_aa };
    BitString bs1( bitstring_size, buffer1 );
    BitStringBuffer bsb1 = bs1;
    uint64_t buffer2 [ num_fields ] = { pattern_55, pattern_55 };
    BitString bs2( bitstring_size, buffer2 );
    BitStringBuffer bsb2 = bs2;
    // 0x55 << 1 = 0xaa
    bsb2 = ( bsb2 << 1 );
    ASSERT_TRUE( bsb2.isEqual( bsb1 ) );
    // clear with shift
    bsb2 = ( bsb2 <<  bsb2.getBitLen() );
    ASSERT_TRUE( bsb2.isZero() );
}

TEST( BitStringTest, BitStringConstructor )
{

    // test with unitialized buffer
    BitString bs1( bitstring_size, nullptr );
    ASSERT_EQ( nullptr, bs1.getBufAddr() );
    ASSERT_EQ( bitstring_size, bs1.getBitLen() );

    // test with initialized bufffer
    uint64_t buffer2 [ num_fields ] = {};
    BitString bs2( bitstring_size, buffer2 );
    ASSERT_NE( nullptr, bs2.getBufAddr() );
    ASSERT_EQ( bitstring_size, bs2.getBitLen() );
}

TEST( BitStringTest, BitStringGetBitLen )
{

    // test with unitialized buffer
    BitString bs1( bitstring_size, nullptr );
    ASSERT_EQ( bitstring_size, bs1.getBitLen() );
}

TEST( BitStringTest, BitStringGetBufAddr )
{

    // test with uninitialized buffer
    BitString bs1( bitstring_size, nullptr );
    ASSERT_EQ( nullptr, bs1.getBufAddr() );

    // test initialized buffer
    uint64_t buffer2 [ num_fields ] = {};
    BitString bs2( bitstring_size, buffer2 );
    ASSERT_NE( nullptr, bs2.getBufAddr() );
}

TEST( BitStringTest, BitStringGetField )
{

    // test with initialized bufffer
    uint64_t buffer1 [ num_fields ] = { pattern_aa, pattern_55 };
    BitString bs1( bitstring_size, buffer1 );
    ASSERT_EQ( pattern_aa, bs1.getField( 0, field_size ) );
    ASSERT_EQ( pattern_55, bs1.getField( field_size, field_size ) );
    // todo, test 0x55aa (verified works, needs test case)
}

TEST( BitStringTest, BitStringGetFieldJustify )
{
    // todo, need test case
}

TEST( BitStringTest, BitStringSetField )
{

    // start with empty field
    uint64_t buffer1 [ num_fields ] = {};
    BitString bs1( bitstring_size, buffer1 );
    ASSERT_TRUE( bs1.isZero() );
    // set pattern
    bs1.setField( 0, field_size, pattern_aa );
    ASSERT_EQ( pattern_aa, bs1.getField( 0, field_size ) );
}

TEST( BitStringTest, BitStringSetFieldJustify )
{
    // todo, need test case
}

TEST( BitStringTest, BitStringIsBitSet )
{

    // start with bit set
    uint64_t buffer1 [ num_fields ] = { 1 };
    // test bit
    BitString bs1( bitstring_size, buffer1 );
    ASSERT_TRUE( bs1.isBitSet( 0 ) );
    //  todo, add more test patterns
}

TEST( BitStringTest, BitStringSetBit )
{

    // start with empty fields
    uint64_t buffer1 [ num_fields ] = { 0 };
    BitString bs1( bitstring_size, buffer1 );
    ASSERT_FALSE( bs1.isBitSet( 0 ) );
    // set bit
    bs1.setBit( 0 );
    ASSERT_TRUE( bs1.isBitSet( 0 ) );
    // todo, add more test patterns
}

TEST( BitStringTest, BitStringSetAll )
{

    // start with clear fields
    uint64_t buffer1 [ num_fields ] = { 0 };
    BitString bs1( bitstring_size, buffer1 );
    ASSERT_TRUE( bs1.isZero() );
    // set bits
    bs1.setAll();
    ASSERT_EQ( pattern_ff, bs1.getField( 0, field_size ) );
}

TEST( BitStringTest, BitStringClearBit )
{

    // start with fields set
    uint64_t buffer1 [ num_fields ] = { 1 };
    BitString bs1( bitstring_size, buffer1 );
    ASSERT_TRUE( bs1.isBitSet( 0 ) );
    // clear bit
    bs1.clearBit( 0 );
    ASSERT_FALSE( bs1.isBitSet( 0 ) );
    // todo, add more test patterns
}

TEST( BitStringTest, BitStringClearAll )
{

    // test with fields set
    uint64_t buffer1 [ num_fields ] = { pattern_ff, pattern_ff };
    BitString bs1( bitstring_size, buffer1 );
    ASSERT_FALSE( bs1.isZero() );
    // clear bits
    bs1.clearAll();
    ASSERT_TRUE( bs1.isZero() );
}

TEST( BitStringTest, BitStringSetString )
{

    // start with fields set
    uint64_t buffer1 [ num_fields ] = { pattern_ff, pattern_ff };
    BitString bs1( bitstring_size, buffer1 );
    uint64_t buffer2 [ num_fields ] = { 0 };
    BitString bs2( bitstring_size, buffer2 );
    ASSERT_TRUE( bs2.isZero() );
    // set bits
    bs2.setString( bs1, 0, bitstring_size, 0 );
    ASSERT_EQ( pattern_ff, bs1.getField( 0, field_size ) );
    ASSERT_EQ( pattern_ff, bs1.getField( field_size, field_size ) );
    // todo, add more test patterns
}

TEST( BitStringTest, BitStringMaskString )
{

    // test with fields set
    uint64_t buffer1 [ num_fields ] = { pattern_ff, pattern_ff };
    BitString bs1( bitstring_size, buffer1 );
    uint64_t buffer2 [ num_fields ] = { pattern_55, pattern_55 };
    BitString bs2( bitstring_size, buffer2 );
    // 0xff & 0xff = 0xaa
    bs1.maskString( bs2 );
    ASSERT_EQ( pattern_aa, bs1.getField( 0, field_size ) );
    ASSERT_EQ( pattern_aa, bs1.getField( field_size, field_size ) );
}

TEST( BitStringTest, BitStringIsEqual )
{

    // test with fields set
    uint64_t buffer1 [ num_fields ] = { pattern_55, pattern_55 };
    BitString bs1( bitstring_size, buffer1 );
    uint64_t buffer2 [ num_fields ] = { pattern_55, pattern_55 };
    BitString bs2( bitstring_size, buffer2 );
    ASSERT_TRUE( bs1.isEqual( bs2 ) );
}

TEST( BitStringTest, BitStringIsZero )
{

    // test with fields clear
    uint64_t buffer1 [ num_fields ] = { 0 };
    BitString bs1( bitstring_size, buffer1 );
    ASSERT_TRUE( bs1.isZero() );
    // test with fields set
    uint64_t buffer2 [ num_fields ] = { pattern_55, pattern_55 };
    BitString bs2( bitstring_size, buffer2 );
    ASSERT_FALSE( bs2.isZero() );
}

TEST( BitStringTest, BitStringGetSetCount )
{

    // test with no clear field
    uint64_t buffer1 [ num_fields ] = { 0 };
    BitString bs1( bitstring_size, buffer1 );
    ASSERT_EQ( 0, bs1.getSetCount() );
    // test with bits set
    uint64_t buffer2 [ num_fields ] = { pattern_ff, pattern_ff };
    BitString bs2( bitstring_size, buffer2 );
    ASSERT_EQ( bitstring_size, bs2.getSetCount() );
}

TEST( BitStringTest, BitStringOperatorOverloadEqEq )
{

    // test == buffers
    uint64_t buffer1 [ num_fields ] = { pattern_55, pattern_55 };
    BitString bs1( bitstring_size, buffer1 );
    uint64_t buffer2 [ num_fields ] = { pattern_55, pattern_55 };
    BitString bs2( bitstring_size, buffer2 );
    ASSERT_TRUE( bs1 == bs2 );
    // test with != buffers
    uint64_t buffer3 [ num_fields ] = { 0 };
    BitString bs3( bitstring_size, buffer3 );
    ASSERT_FALSE( bs2 == bs3 );
}

TEST( BitStringTest, ExerciseBitString )
{

    // start with unititialized buffer
    uint64_t buffer1 [ num_fields ] = { 0 };
    BitString bs1( bitstring_size, buffer1 );

    // initialize test parameters
    uint64_t allbits = -1;
    uint64_t val1 = 0;
    uint64_t val2 = 0;
    uint64_t pos = 0;
    uint64_t len = 1;

    // test set/get ...
    // for every valid field position
    for (pos = 0; pos <= bs1.getBitLen(); pos++) {
        // For every valid field length
        for (len = 1;  ( ( len <= field_size ) && ( ( len + pos ) \
                        <= bs1.getBitLen() ) ); len++) {

            bs1.setField( pos, len, allbits );

            // Get the bit field
            val1 = bs1.getField( pos, len );

            // Test the bit field
            val2 = ( allbits >> ( field_size - len ) );

            ASSERT_EQ( val1, val2 );
        }
    }
}
