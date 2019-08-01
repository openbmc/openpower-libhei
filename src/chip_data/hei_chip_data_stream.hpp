#pragma once

/**
@file <hei_chip_data_stream.hpp>
@brief Description:  Declaration for the ChipDataStream class.
*/

/*! \mainpage Module Description

ChipDataStream makes it possible to read a buffer of binary data
using the stream operators, ">>".

Instantiate ChipDataStream class with a pointer to a data buffer
and its size in bytes as below:

    ChipDataStream cds(streamData,552);

Use the ">>" stream operator then to read the major C++ types
(bool, char, uint8_t, int8_t, uint16_t, int16_t, uint32_t,
int32_t, uint64_t, and int64_t).

 End Module Description */

#include <assert.h>
#include <endian.h>
#include <hei_user_defines.hpp>
#include <stdint.h>
#include <string.h>

namespace libhei
{

/**
* @class ChipDataStream
* ChipDataStream offers convenient access to binary data.
*/
class ChipDataStream
{

  private:

      size_t iv_asyncOffset;
      uint8_t* iv_buffer;
      size_t iv_bufferSize;

  public:

      //Constructors

      //When instantiating the ChipDataStream object
      //a pointer to a data buffer containing all
      //the data and its size is passed into the class.
      ChipDataStream(uint8_t* i_buffer, size_t i_bufferSize);

      //Eliminate copy and assignment operator constructors
      ChipDataStream ( const ChipDataStream & ) = delete;
      ChipDataStream & operator=( const ChipDataStream & ) = delete;

      //Destructor

      ~ChipDataStream();

      /**
      //@brief setOffset allows you to jump to any location in the buffer
      *
      * @param i_offset   Offset into the buffer
      * @return           None\n\n
      * Note:             An assert will trigger if offset >= buffersize
      */
      void setOffset(size_t i_offset);

      /**
      *@brief read does the actual copy from the buffer
      *@param o_buf       a pointer to the location to copy to
      *@param i_size      the size (in bytes) to copy 
      *@return            None\n\n
      */
      void read( void * o_buf, size_t i_size )
      {
          // Ensure memory is not accessed outside i_buffer
          HEI_ASSERT((iv_asyncOffset + i_size) <= iv_bufferSize);
          // Copy appropriate bytes from i_buffer to o_buff
          memcpy(o_buf,iv_buffer+iv_asyncOffset,i_size);
          // Increment asynchronous offset to next piece of data
          iv_asyncOffset = iv_asyncOffset + i_size;
      }

      /**
      *@brief Default case for "operator>>" template
      *@param             None
      *@return            A pointer to "this" object\n\n
      * Notes:            Not actually used (see .cpp file for
      *                   implementation by type)
      */                  
      template <class D>
          ChipDataStream & operator>>( D & o_right )
          {
              read( &o_right, sizeof(D) );
              return *this;
          }
};
}//namespace libhei
