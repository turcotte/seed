/*                               -*- Mode: C -*- 
 * find.c --- find an exact match
 * Author          : Truong Nguyen and Marcel Turcotte
 * Created On      : Fri Jun 24 13:17:17 2005
 * Last Modified By: Marcel Turcotte
 * Last Modified On: Thu Jul 21 16:21:22 2005
 *
 * This copyrighted source code is freely distributed under the terms
 * of the GNU General Public License. 
 * See the files COPYRIGHT and LICENSE for details.
 *
 * This program is actually not part of Seed.  It was created for
 * testing the suffix arrays library. 
 */

#include "libdev.h"
#include "seq.h"
#include "libvtree.h"

/*****************************************************************
 * display_usage - general instructions for running the program  *
 *****************************************************************/

static void
display_usage_and_exit()
{
  printf( "Usage: find file pattern\n" );
  exit( EXIT_SUCCESS );
}

/*****************************************************************
 * main -                                                        *
 *****************************************************************/

int
main( int argc, char *argv[] )
{

  int num_seqs;
  char **seqs;
  char **descs;

  dstring_t *db, *pattern;
  vtree_t *v;

  if ( argc != 3 )
    display_usage_and_exit();

  /* initialisations */

  dev_init();

  /* reading data */

  num_seqs = bio_read_fasta( argv[ 1 ], &seqs, &descs, isnuc );

  db = dev_digitalize( &bio_nuc_alphabet, seqs[ 0 ] );

  v = vtree_create( db );

  pattern = dev_digitalize( &bio_nuc_alphabet, argv[ 2 ] );
  pattern->length--; /* no terminator */

  vtree_find_exact_match( v, pattern );

  /* post-processings */

  vtree_free( v );
  dev_free_dstring( db );
  dev_free_dstring( pattern );
  dev_free_array( (void **) descs, num_seqs );
  dev_free_array( (void **) seqs, num_seqs );

  exit( EXIT_SUCCESS );
}
