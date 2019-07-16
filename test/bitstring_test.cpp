/**
 * Copyright © 2019 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <hei_user_defines.hpp>
#include <bitstring.hpp>

using namespace libhei;

/**
 * @brief Exercise BitString and BitString buffer classes
 * @param bsb BitStringBuffer to test with.
 */
int test_BitStringClass( BitStringBuffer & bsb )
{

    int retval = 0;         // test result

    BitStringBuffer bstest( 128 );

    bsb.clearAll();

    // Test case parameters
    uint32_t len = 1;
    uint32_t pos = 0;

    // Test values
    uint64_t val1 = 0, val2 = 0;

    //
    // test setField() and getField()
    //

    // For every valid field position
    for (pos = 0; pos <= bsb.getBitLen(); pos++)
    {
        // For every valid field length
        for (len = 1;  ( ( len <=  MAXBITS ) && ( ( len + pos ) <= bsb.getBitLen() ) ); len++)
        {

            // Set the bit field
            bsb.setField( pos, len, ALLBITS );

            // Get the bit field
            val1 = bsb.getField( pos, len );

            // Test the bit field
            val2 = ( ALLBITS >> ( MAXBITS - len ) );

            if ( val1 != val2 )
            {
                retval++;
                HEI_INF("mismatch detected !\n");
            }
        }
    }

    //
    // test setAll()
    //
    bsb.setAll();

    if ( ALLBITS != ( bsb.getField( 0, MAXBITS ) ) )
    {
        retval++;
        HEI_INF("setAll() failed!\n");
    }

    //
    // test clearAll()
    //
    bsb.clearAll();

    if ( ( 0 != bsb.getField( 0, MAXBITS ) ) )
    {
        retval++;
        HEI_INF("clearAll() failed!\n");
    }

    //
    // test setBit();
    //
    for ( pos = 0; pos <= MAXBITS; pos++ )
    {
        bsb.setBit(pos);
    }

    if ( ALLBITS != bsb.getField( 0, MAXBITS ) )
    {
        retval++;
        HEI_INF("setBit() failed!\n");
    }

    //
    // test clearBit();
    //
    for ( pos = 0; pos <= MAXBITS; pos++ )
    {
        bsb.clearBit(pos);
    }

    if ( 0 != bsb.getField( 0, MAXBITS ) )
    {
        retval++;
        HEI_INF("clearBit() failed!\n");
    }

    //
    // test isBitSet()
    //
    bsb.setAll();
    for ( pos = 0; pos < bsb.getBitLen(); pos++ )
    {
        if ( 1 != bsb.isBitSet(pos) )
        {
            retval++;
            HEI_INF("isBitSet(1) failed!\n");
        }
    }

    bsb.clearAll();
    for ( pos = 0; pos < bsb.getBitLen(); pos++ )
    {
        if ( 0 != bsb.isBitSet(pos) )
        {
            retval++;
            HEI_INF("isBitSet(0) failed!\n");
        }
    }

    //
    // test isZero()
    //
    bsb.clearAll();
    if ( true != bsb.isZero() )
    {
        retval++;
        HEI_INF("isZero(0) failed!\n");
    }

    bsb.setAll();
    if ( false != bsb.isZero() )
    {
        retval++;
        HEI_INF("isZero(1) failed!\n");
    }

    //
    // test isEqual()
    //
    bsb.clearAll();
    bstest.clearAll();
    if ( !( bsb.isEqual( bstest ) ) )
    {
        retval++;
        HEI_INF("isEqual(0) failed!\n");
    }
    bstest.setAll();
    if ( bsb.isEqual( bstest ) )
    {
        retval++;
        HEI_INF("isEqual(1) failed!\n");
    }

    //
    // test getSetCount()
    //
    bsb.setAll();
    if ( bsb.getBitLen() != bsb.getSetCount() )
    {
        retval++;
        HEI_INF("getSetCount(1) failed!\n");
    }

    bsb.clearAll();
    if ( 0 != bsb.getSetCount() )
    {
        retval++;
        HEI_INF("getSetCount(0) failed!\n");
    }

    for( pos = 0; pos < MAXBITS; pos += 2 ) { bsb.setBit(pos); }
    if ( ( MAXBITS / 2 )  != bsb.getSetCount(0, MAXBITS) )
    {
        retval++;
        HEI_INF("getSetCount(MAXBITS) failed!\n");
    }

    //
    // test setString()
    //
    bsb.clearAll();
    bstest.setAll();

    bsb.setString( bstest );
    if( ! ( bsb.isEqual( bstest ) ) )
    {
        retval++;
        HEI_INF("setString() failed!\n");
    }

    // clear both strings
    bsb.clearAll();
    bstest.clearAll();

    // set bottom half of one string and top half of other string
    bsb.setField( 0, MAXBITS, ALLBITS );
    bstest.setField( MAXBITS, MAXBITS, ALLBITS );

    // copy top half of one string to bottom half of other string (vice versa)
    bsb.setString( bstest, MAXBITS, MAXBITS, MAXBITS );
    bstest.setString( bsb, 0, MAXBITS, 0 );
    if ( ! ( bsb.isEqual( bstest ) ) )
    {
        retval++;
        HEI_INF("setString(0) failed!\n");
    }

    //
    // test maskString()
    //
    bsb.setAll();

    // create mask string
    bstest.clearAll();
    for ( pos = 0; pos < bstest.getBitLen(); pos += 2 )
    {
        bstest.setBit( pos );
    }

    //  apply mask
    bsb.maskString( bstest );

    val1 = bsb.getField( 0, MAXBITS );
    val2 = bstest.getField( 0, MAXBITS );
    val1 ^= val2;

    if ( ALLBITS != val1 )
    {
        retval++;
        HEI_INF("maskString() failed!\n");
    }

    //
    // test bitwise operators (==, not, and, or, shr, shl)
    //
    BitStringBuffer bsbtest1( MAXBITS * 2 );
    BitStringBuffer bsbtest2( MAXBITS * 2 );
    BitStringBuffer bsbzero( MAXBITS * 2 );
    BitStringBuffer bsbones( MAXBITS * 2 );

    bsbtest1.clearAll();
    bsbtest2.clearAll();

    // test == operator
    bsb.setAll();
    if ( ! ( bsbtest1 == bsbtest2 ) )
    {
        retval++;
        HEI_INF("'==' operator failed!\n");
    }

    bsbtest1.setField( 0, MAXBITS , ALLBITS );      // set bottom half
    bsbtest2.setField( MAXBITS, MAXBITS, ALLBITS ); // set top haf

    // test ~
    if ( ! ( ~bsbtest1 == bsbtest2 ) )
    {
        retval++;
        HEI_INF("operator ~ failed!\n");
    }

    // test <<
    if ( ! ( ( bsbtest1 << MAXBITS ) == bsbtest2 ) )
    {
        retval++;
        HEI_INF("operator << failed!\n");
    }

    // test >>
    if ( ! ( ( bsbtest2 >> MAXBITS ) == bsbtest1 ) )
    {
        retval++;
        HEI_INF("operator >>  failed!\n");
    }

    bsbzero.clearAll(); // bit-string of all ones
    bsbones.setAll();   // bit-string of all zeros

    // test &
    if ( ! ( bsbzero == ( bsbtest1 & bsbtest2 ) ) )
    {
        retval++;
        HEI_INF("operator & failed!\n");
    }

    // test |
    if ( ! ( bsbones == ( bsbtest1 | bsbtest2 ) ) )
    {
        retval++;
        HEI_INF("operator | failed!\n");
    }

    //
    // test BitString and BitStringBuffers constructors and assignments
    //

    // BitString constructor
    uint64_t buffer [MAXBITS] = { 0 };
    BitString bs1( ( sizeof( buffer ) * 8 ), buffer );

    if ( ( nullptr == bs1.getBufAddr() ) || \
           MAXBITS == bs1.getBitLen() || \
            ( ! ( bs1.isZero() ) ) )
    {
        retval++;
        HEI_INF("BitString constructor failed!\n");
    }

    // BitstringBuffer constructor
    BitStringBuffer bsb1( 128 );

    if ( ( nullptr == bsb1.getBufAddr() ) || \
           MAXBITS == bsb1.getBitLen() || \
            ( ! ( bsb1.isZero() ) ) )
    {
        retval++;
        HEI_INF("BitStringBuffer constructor failed!\n");
    }

    // BitStringBuffer copy constructor from BitStringBuffer
    BitStringBuffer bsb2 = bsb1;

    if ( ! ( bsb2 == bsb1 ) )
    {
        retval++;
        HEI_INF("BitStringBuffer copy constructor =BitStringBuffer failed!\n");
    }


    // BitStringBuffer constructor from BitString
    BitStringBuffer bsb3 = bs1;

    if ( ! ( bsb3 == bs1 ) )
    {
        retval++;
        HEI_INF("BitStringBuffer copy constructor =BitString failed!\n");
    }


    // assignment operator BitStringBuffer from BitstringBuffer
    bsb2 = bsb3;

    if ( ! ( bsb2 == bsb3 ) )
    {
        retval++;
        HEI_INF("BitStringBuffer assignment =BitStringBuffer failed!\n");
    }


    // assignment operator BitStringBuffer from BitString
    bsb3 = bs1;

    if ( ! ( bsb3 == bs1 ) )
    {
        retval++;
        HEI_INF("BitStringBuffer assignment =BitString failed!\n");
    }


    // BitStringBuffer constructor with BitStringBuffer initializer
    BitStringBuffer bsb4( bsb1 );

    if ( ! ( bsb4 == bsb1 ) )
    {
        retval++;
        HEI_INF("BitStringBuffer constructor (BitStringBuffer) failed!\n");
    }

    // BitStringBuffer constructor with BitString initializer
    BitStringBuffer bsb5( bs1 );

    if ( ! ( bsb5 == bs1 ) )
    {
        retval++;
        HEI_INF("BitStringBuffer constructor (BitString) failed!\n");
    }

    // copy constructor BitString from BitString
    BitString bs2 = bs1;

    if ( ! ( bs2 == bs1 ) )
    {
        retval++;
        HEI_INF("BitString copy constructor =BitString failed!\n");
    }

    // copy constructor BitString from BitStringBuffer - not supported
    //BitString bs3 = bsb1;
    bs1.setAll();

    // assignment operator BitString from BitString
    bs2 = bs1;

    if ( ! ( bs2 == bs1 ) )
    {
        retval++;
        HEI_INF("BitString assignment =BitString failed!\n");
    }

    // assignment operator BitString from BitStringBuffer - not supported
    //bs3 = bsb1;

    // BitString constructor with BitString initializer
    BitString bs4( bs1 );

    if ( ! ( bs4 == bs1 ) )
    {
        retval++;
        HEI_INF("BitString constructor (BitString) failed!\n");
    }

    // BitString constructor with BitStringBuffer initializer - not supported
    //BitString bs5( bsb1);

    // BitStringBuffer destructor - todo need test case
    ~bsb2;

    // BitString destructor - todo need test case
    ~bs2;

    return retval;
}

/**
 * @brief Test the BitString based classes.
 * @return Number of errors detected
 */
int main()
{
    uint32_t retval = 0;

    // test with 8-bit fields
    BitStringBuffer bsb(128);
    retval += test_BitStringClass( bsb );

    if (retval)
    {
        HEI_INF("errors: %u\n", retval);
    }
    else
    {
        HEI_INF("all bitstring tests passed\n");
    }

    return retval;
}
