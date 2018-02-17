/*                               -*- Mode: C -*- 
 * seq.c --- sequence related functions
 * Author          : Truong Nguyen and Marcel Turcotte
 * Created On      : Wed Jun  8 14:04:26 2005
 * Last Modified By: Marcel Turcotte
 * Last Modified On: Fri Aug  5 13:17:04 2005
 *
 * This copyrighted source code is freely distributed under the terms
 * of the GNU General Public License. 
 * See the files COPYRIGHT and LICENSE for details.
 */

#include "libdev.h"
#include "seq.h"

#include <ctype.h>
#include <string.h>

/*****************************************************************
 * Constants                                                     *
 *****************************************************************/

#define BUFFER_SIZE 256

/*****************************************************************
 * Definitions for digitalizing an input sequence                *
 *****************************************************************/

static code_t codes[] =
  { { '-', SYM_GAP   },
    { 'A', SYM_NUC_A },
    { 'C', SYM_NUC_C },
    { 'M', SYM_NUC_M },
    { 'G', SYM_NUC_G },
    { 'R', SYM_NUC_R },
    { 'S', SYM_NUC_S },
    { 'V', SYM_NUC_V },
    { 'T', SYM_NUC_T },
    { 'U', SYM_NUC_U },
    { 'W', SYM_NUC_W },
    { 'Y', SYM_NUC_Y },
    { 'H', SYM_NUC_H },
    { 'K', SYM_NUC_K },
    { 'D', SYM_NUC_D },
    { 'B', SYM_NUC_B },
    { 'N', SYM_NUC_N } };

alphabet_t bio_nuc_alphabet = { codes, NUM_SYMBOLS, ALPHABET_SIZE };

int
bio_nuc_isbp( symbol_t a, symbol_t b, int gu_allowed )
{
  switch ( a ) {
  case SYM_NUC_A: return b == SYM_NUC_U;
  case SYM_NUC_C: return b == SYM_NUC_G;
  case SYM_NUC_M: return b == SYM_NUC_K;
  case SYM_NUC_G: return b == SYM_NUC_C || ( gu_allowed && b == SYM_NUC_U );
  case SYM_NUC_R: return b == SYM_NUC_Y;
  case SYM_NUC_S: return b == SYM_NUC_S;
  case SYM_NUC_V: return b == SYM_NUC_B;
  case SYM_NUC_U: return b == SYM_NUC_A || ( gu_allowed && b == SYM_NUC_G );
  case SYM_NUC_W: return b == SYM_NUC_W;
  case SYM_NUC_Y: return b == SYM_NUC_R;
  case SYM_NUC_H: return b == SYM_NUC_D;
  case SYM_NUC_K: return b == SYM_NUC_M;
  case SYM_NUC_D: return b == SYM_NUC_H;
  case SYM_NUC_B: return b == SYM_NUC_V;
  case SYM_NUC_N: return b == SYM_NUC_N;
  default:
    dev_die( "unknown symbol %d\n", a );
  }
  return FALSE; /* never reached */
}

/*****************************************************************
 * bio_nuc_complement - returns the complement                   *
 * a : input symbol                                              *
 *****************************************************************/

symbol_t
bio_nuc_complement( symbol_t a )
{
  switch ( a ) {
  case SYM_NUC_A: return SYM_NUC_U;
  case SYM_NUC_C: return SYM_NUC_G;
  case SYM_NUC_M: return SYM_NUC_K;
  case SYM_NUC_G: return SYM_NUC_C;
  case SYM_NUC_R: return SYM_NUC_Y;
  case SYM_NUC_S: return SYM_NUC_S;
  case SYM_NUC_V: return SYM_NUC_B;
  case SYM_NUC_U: return SYM_NUC_A;
  case SYM_NUC_W: return SYM_NUC_W;
  case SYM_NUC_Y: return SYM_NUC_R;
  case SYM_NUC_H: return SYM_NUC_D;
  case SYM_NUC_K: return SYM_NUC_M;
  case SYM_NUC_D: return SYM_NUC_H;
  case SYM_NUC_B: return SYM_NUC_V;
  case SYM_NUC_N: return SYM_NUC_N;
  default:
    dev_die( "unknown symbol %d\n", a );
  }

  return -1; /* never reached */
}

/*****************************************************************
 * bio_nuc_revcomp -                                             *
 *****************************************************************/

dstring_t *
bio_nuc_revcomp( dstring_t *forward )
{
  int n = forward->length;
  dstring_t *result = dev_new_dstring( &bio_nuc_alphabet, n, 0 );

  for ( int i=0; i<n-1; i++ ) {
    result->text[ n - i - 2 ] = bio_nuc_complement( forward->text[ i ] );
  }

  result->text[ n-1 ] = bio_nuc_alphabet.size;

  return result;
}

/*****************************************************************
 * isnuc - predicate returning true if its input is a nucleotide *
 * c : input character                                           *
 * return true if c designates a nucleotide                      *
 *****************************************************************/

int
isnuc( char c )
{
  
  int i;

  for ( i=0; i<NUM_SYMBOLS; i++ )
    if ( codes[ i ].sym == c )
      return TRUE;
  
  return FALSE;
}

/*****************************************************************
 * is_char_class - returns true if the input symbol is generic,  *
 * matches more than one specific nucleotide.                    *
 *****************************************************************/

int
bio_is_char_class( symbol_t a )
{
  int result;
  switch ( a ) {
  case SYM_NUC_M: /* A|C     aMino group at common position */
  case SYM_NUC_R: /* A|G     puRines */
  case SYM_NUC_S: /* C|G     Strong hydrogen bonding */
  case SYM_NUC_V: /* A|C|G   not T */
  case SYM_NUC_W: /* A|T     Weak hydrogen bonding */
  case SYM_NUC_Y: /* C|T     pYrimidines */
  case SYM_NUC_H: /* A|C|T   not G */
  case SYM_NUC_K: /* G|T     Keto group at common position */
  case SYM_NUC_D: /* A|G|T   not C */
  case SYM_NUC_B: /* C|G|T   not A */
  case SYM_NUC_N: /* A|C|G|T aNy */
    result = TRUE;
    break;
  default:
    result = FALSE;
  }
  return result;
}

/*****************************************************************
 * bio_nuc_cmp - comparator                                      *
 * c,d : input symbols                                           *
 * return true if c and d match                                  *
 *****************************************************************/

int
bio_nuc_cmp( symbol_t c, symbol_t d )
{

  assert( c > SYM_GAP && c < SYM_TER );

  assert( d > SYM_GAP && d < SYM_TER );

  return c & d;
}

/*****************************************************************
 * bio_nuc_tochar - get a printable representation of a symbol   *
 * c : input symbol                                              *
 *****************************************************************/

char
bio_nuc_tochar( symbol_t s )
{
  assert( s >= SYM_GAP && s <= SYM_TER );

  for ( int i=0; i<NUM_SYMBOLS; i++ )
    if ( codes[ i ].code == s )
      return codes[ i ].sym;
  
  dev_die( "internal error, symbol %d unknown", s );
  
  return -1;
}

/*****************************************************************
 * BUFFERED_FILE - used for reading one sequence at a time       *
 *****************************************************************/

typedef struct {

  FILE *fh;
  char *name;
  char *buffer;
  int offset;
  int length;
  int line;

} BUFFERED_FILE;

/*****************************************************************
 * bio_fopen - open a file and creates its BUFFERED_FILE struct  *
 *****************************************************************/

static BUFFERED_FILE *
bio_fopen( const char *name )
{
  BUFFERED_FILE *fh;

  fh = dev_malloc( sizeof( BUFFERED_FILE ) );

  fh->fh = dev_fopen( name, "r" );

  fh->buffer = dev_malloc( BUFFER_SIZE );
  fh->name = dev_strcpy( name );
  fh->offset = -1;
  fh->length = 0;
  fh->line = 1;

  return fh;
}

/*****************************************************************
 * bio_close - close a file and frees its BUFFERED_FILE struct   *
 *****************************************************************/

int
bio_fclose( BUFFERED_FILE *fh )
{
  int status;

  status = fclose( fh->fh );

  dev_free( fh->buffer );
  dev_free( fh->name );
  dev_free( fh );

  return status;
}

/*****************************************************************
 * bio_fasta_has_next -                                          *
 * fh : file handle                                              *
 * return true if the input seems to have at least one more entry*
 *****************************************************************/

static int
bio_fasta_has_next( BUFFERED_FILE *fh )
{

  /* characters in the buffer? skip blanks */

  while ( ( fh->length > 0 ) && isspace( (int) fh->buffer[ fh->offset ] ) ) {

    if ( dev_isnewline( fh->buffer[ fh->offset ] ) )
      fh->line++;

    fh->offset++;
    fh->length--;

  }

  if ( fh->length > 0 ) {

    if ( fh->buffer[ fh->offset ] == '>' )
      return TRUE;
    else
      dev_die( "not a fasta file, %s line %d", fh->name, fh->line );

  }  

  /* let's look ahead */

  if ( fgets( fh->buffer, BUFFER_SIZE, fh->fh ) == NULL )
    return FALSE;

  fh->offset = 0;
  fh->length = strlen( fh->buffer );

  return bio_fasta_has_next( fh );
}

/*****************************************************************
 * bio_fasta_has_more_chars -                                    *
 * fh : file handle                                              *
 * return true if the input has more characters to read          *
 *****************************************************************/

static int
bio_fasta_has_more_chars( BUFFERED_FILE *fh )
{

  /* characters in the buffer? skip blanks */

  while ( ( fh->length > 0 ) && isspace( (int) fh->buffer[ fh->offset ] ) ) {

    if ( dev_isnewline( fh->buffer[ fh->offset ] ) )
      fh->line++;

    fh->offset++;
    fh->length--;

  }

  if ( fh->length > 0 ) {

    if ( fh->buffer[ fh->offset ] == '>' )
      return FALSE;
    else
      return TRUE;

  }  

  /* let's look ahead */

  if ( fgets( fh->buffer, BUFFER_SIZE, fh->fh ) == NULL )
    return FALSE;

  fh->offset = 0;
  fh->length = strlen( fh->buffer );

  return bio_fasta_has_more_chars( fh );
}

/*****************************************************************
 * bio_read_seq_fasta - reads a sequence in fasta format         *
 * fh : file handle                                              *
 * seq : sequence data                                           *
 * desc : description                                            *
 *****************************************************************/

static void
bio_read_seq_fasta( BUFFERED_FILE *fh, char **seq_out, char **desc_out, int ( *isvalid )( char c ) )
{
  char *seq, *desc;
  size_t offset, size;
  
  if ( ! bio_fasta_has_next( fh ) )
    dev_die( "not a fasta file, %s line %d", fh->name, fh->line );

  assert( fh->buffer[ fh->offset ] == '>' );

  /* reading description line */

  offset = 0;
  size = BUFFER_SIZE;
  desc = dev_malloc( size );

  while ( ( fh->length > 0 ) && ( ! dev_isnewline( fh->buffer[ fh->offset ] ) ) ) {

    if ( offset >= ( size - 2 ) ) {
      size = 2 * size;
      desc = dev_realloc( desc, size );
    }

    desc[ offset++ ] = fh->buffer[ fh->offset ];

    fh->offset++;
    fh->length--;

    if ( fh->length == 0 ) {
      if ( fgets( fh->buffer, BUFFER_SIZE, fh->fh ) == NULL )
	dev_die( "not a fasta file, %s line %d", fh->name, fh->line );

      fh->offset = 0;
      fh->length = strlen( fh->buffer );
    }

  }

  fh->line++;
  fh->offset++;
  fh->length--;

  desc[ offset ] = '\0';

  *desc_out = dev_strcpy( desc );

  dev_free( desc );

  /* reading sequence data */

  offset = 0;
  size = BUFFER_SIZE;
  seq = dev_malloc( size );

  if ( ! bio_fasta_has_more_chars( fh ) )
    dev_die( "empty sequence, %s line %d", fh->name, fh->line );

  while ( bio_fasta_has_more_chars( fh ) ) {

    char c = fh->buffer[ fh->offset ];

    if ( offset >= ( size - 2 ) ) {
      size = 2 * size;
      seq = dev_realloc( seq, size );
    }

    c = toupper( c );

    if ( ! isvalid( c ) )
      dev_die( "not a valid character %c, %s line %d", c, fh->name, fh->line );

    seq[ offset++ ] = c;

    fh->offset++;
    fh->length--;

  }

  seq[ offset ] = '\0';

  *seq_out = dev_strcpy( seq );

  dev_free( seq );

}

/*****************************************************************
 * bio_read_fasta - reads multiple sequences in fasta format     *
 * name : file name                                              *
 * seqs_out : sequence data                                      *
 * descs_out : descriptions                                      *
 * return a list of sequences or NULL                            *
 *****************************************************************/

int
bio_read_fasta( char *name, char ***seqs_out, char ***descs_out, int ( *isvalid )( char c ) )
{
  char **seqs_buffer, **descs_buffer;
  size_t num_seqs = 0;
  BUFFERED_FILE *fh;

  fh = bio_fopen( name );

  while ( bio_fasta_has_next( fh ) ) {

    char *seq, *desc;

    bio_read_seq_fasta( fh, &seq, &desc, isvalid );

    if ( num_seqs == 0 ) {

      seqs_buffer = dev_malloc( sizeof( char * ) );
      descs_buffer = dev_malloc( sizeof( char * ) );

    } else {
    
      seqs_buffer = dev_realloc( seqs_buffer, ( num_seqs + 1 ) * sizeof( char * ) );
      descs_buffer = dev_realloc( descs_buffer, ( num_seqs + 1) * sizeof( char * ) );

    }

    seqs_buffer[ num_seqs ] = seq;
    descs_buffer[ num_seqs ] = desc;
    
    num_seqs++;

  }

  *seqs_out = seqs_buffer;
  *descs_out = descs_buffer;

  bio_fclose( fh );

  return num_seqs;
}

