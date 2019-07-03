#ifndef CHIP_DATA_STREAM
#define CHIP_DATA_STREAM

#include <endian.h>
#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

//Remove
//Temporarily adding assert macro until it makes its way into the project.
#define HEI_ASSERT( expression ) \
      assert( expression );

using namespace std;

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

	  //setOffset allows you to jump to any location in the buffer
      //An assert will trigger if offset >= buffersize 
      void setOffset(size_t i_offset);

      void read( void * o_buf, size_t i_size )
      {
          // some sort of bounds checking
          HEI_ASSERT((iv_asyncOffset + i_size) <= iv_bufferSize);
          // a memcpy() into o_buf with i_size
          memcpy(o_buf,iv_buffer+iv_asyncOffset,i_size);
          // increment the internal offset.
          iv_asyncOffset = iv_asyncOffset + i_size;
      }

      // Default case for shift operator.
      template <class D>
          ChipDataStream & operator>>( D & o_right )
          {
              read( &o_right, sizeof(D) );
              return *this;
          }

};
#endif
