/*                               -*- Mode: C -*- 
 * sizeof_info.c --- know thy machine (displays information about sizes)
 * Author          : Truong Nguyen and Marcel Turcotte
 * Created On      : Tue Jun  7 12:51:04 2005
 * Last Modified By: Marcel Turcotte
 * Last Modified On: Wed Jul 13 15:46:40 2005
 *
 * This copyrighted source code is freely distributed under the terms
 * of the GNU General Public License. 
 * See the files COPYRIGHT and LICENSE for details.
 *
 * Usage: [ sample session on Solaris 9 SunBlade 150 workstation ]
 *
 *   $ gcc -m32 -o sizeof_info sizeof_info.c
 *
 *   $ ./sizeof_info
 *
 *   sizeof( off_t ) == 4
 *   sizeof( size_t ) == 4
 *   sizeof( void * ) == 4
 *   sizeof( char ) == 1
 *   sizeof( short ) == 2
 *   sizeof( int ) == 4
 *   sizeof( long ) == 4
 *   sizeof( long long ) == 8
 *   sizeof( float ) == 4
 *   sizeof( double ) == 8
 *
 *   $ gcc -m64 -o sizeof_info sizeof_info.c
 *
 *   $ ./sizeof_info
 *
 *   sizeof( off_t ) == 8
 *   sizeof( size_t ) == 8
 *   sizeof( void * ) == 8
 *   sizeof( char ) == 1
 *   sizeof( short ) == 2
 *   sizeof( int ) == 4
 *   sizeof( long ) == 8
 *   sizeof( long long ) == 8
 *   sizeof( float ) == 4
 *   sizeof( double ) == 8
 */

#include <stdio.h>
#include <stdlib.h>

int
main( void )
{

  printf( "sizeof( off_t ) == %d\n", sizeof( off_t ) );
  printf( "sizeof( size_t ) == %d\n", sizeof( size_t ) );
  printf( "sizeof( void * ) == %d\n", sizeof( void * ) );
  printf( "sizeof( char ) == %d\n", sizeof( char ) );
  printf( "sizeof( short ) == %d\n", sizeof( short ) );
  printf( "sizeof( int ) == %d\n", sizeof( int ) );
  printf( "sizeof( long ) == %d\n", sizeof( long ) );
  printf( "sizeof( long long ) == %d\n", sizeof( long long ) );
  printf( "sizeof( float ) == %d\n", sizeof( float ) );
  printf( "sizeof( double ) == %d\n", sizeof( double ) );

  exit( EXIT_SUCCESS );
}
