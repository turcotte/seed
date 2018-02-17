/*                               -*- Mode: C -*- 
 * tests.c --- tests driver
 * Author          : Truong Nguyen and Marcel Turcotte
 * Created On      : Thu Jun 30 12:12:44 2005
 * Last Modified By: Marcel Turcotte
 * Last Modified On: Wed Jul 13 15:45:39 2005
 *
 * This copyrighted source code is freely distributed under the terms
 * of the GNU General Public License. 
 * See the files COPYRIGHT and LICENSE for details.
 */

#include "libdev.h"
#include "seq.h"

/*****************************************************************
 * banner - displays the name of the library                     *
 *****************************************************************/

static void
banner()
{
  printf( "* libbio - tests driver *\n\n" );
}

/*****************************************************************
 * main - main program                                           *
 *****************************************************************/

int
main( void )
{

  dev_init();

  banner();

  printf( "bio_nuc_cmp( a, b) ::\n\n" );
  printf( "  " );
  for ( int i=1; i<bio_nuc_alphabet.size; i++ )
    printf( "%c", bio_nuc_tochar( i ) );
  printf( "\n");

  for ( int i=1; i<bio_nuc_alphabet.size; i++ ) {
    printf( "%c ", bio_nuc_tochar( i ) );
    for ( int j=1; j<bio_nuc_alphabet.size; j++ )
      if ( bio_nuc_cmp( i, j ) )
	printf( "*" );
      else
	printf( " " );
    printf( "\n");
  }
  printf( "\n");

  exit( EXIT_SUCCESS );
}
