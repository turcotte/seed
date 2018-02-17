/*                               -*- Mode: C -*- 
 * debug.c --- mainly trace functions
 * Author          : Truong Nguyen and Marcel Turcotte
 * Created On      : Mon Jul  4 09:28:32 2005
 * Last Modified By: Marcel Turcotte
 * Last Modified On: Wed Jul 13 15:48:00 2005
 *
 * This copyrighted source code is freely distributed under the terms
 * of the GNU General Public License. 
 * See the files COPYRIGHT and LICENSE for details.
 */

#include "libdev.h"
#include "libvtree.h"

/*****************************************************************
 * vtree_print_tables -                                          *
 *****************************************************************/

void
vtree_print_tables( alphabet_t *alphabet, vtree_t *v )
{
  printf( "* enhanced suffix array *\n\n" );
  printf( "        s   ds   sa   ra  lcp   bw    ^    v    > \n" );

  for ( int i=0; i<v->length; i++ ) {

    char c;

    if  ( dev_isspecial( alphabet, v->text[ i ] ) )
      c = '$';
    else
      c = dev_decode( alphabet, v->text[ i ] );

    printf( " %3d ", i );
    printf( "[%3c]", c );
    printf( "[%3d]", v->text[ i ] );
    printf( "[%3d]", v->suftab[ i ] );
    printf( "[%3d]", v->isuftab[ i ] );
    printf( "[%3d]", v->lcptab[ i ] );
    printf( "[%3d]", v->bwtab[ i ] );
    printf( "[%3d]", v->childtab[ i ].up );
    printf( "[%3d]", v->childtab[ i ].down );
    printf( "[%3d]", v->childtab[ i ].next );

    printf( " " );

    for ( int j=v->suftab[ i ]; j<v->length; j++ ) {
      if  ( dev_isspecial( alphabet, v->text[ j ] ) )
	c = '$';
      else
	c = dev_decode( alphabet, v->text[ j ] );
      printf( "%c", c );
    }
    
    printf( "\n" );
  }
}
