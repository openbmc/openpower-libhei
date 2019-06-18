#include <stdio.h>
#include <assert.h>

#define HEI_INF( format, ... ) printf( "I> " format "\n", ##__VA_ARGS__ );

#define HEI_ERR( format, ... ) printf( "E> " format "\n", ##__VA_ARGS__ );

#define HEI_ASSERT( expression ) assert( expression );

